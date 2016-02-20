// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "buildmenu.hpp"

#include <cstdio>
#include <cstring>

#include <GameCore>
#include <GameLogger>
#include <GameObjects>
#include <GameGui>

#include "gfx/decorator.hpp"
#include "city/build_options.hpp"
#include "events/playsound.hpp"

//using namespace constants;
using namespace gfx;
using namespace city;
using namespace events;

// used to display the building name and its cost
namespace gui
{

class BuildMenu::Impl
{
public:
  city::development::Options options;
  city::development::Branch branch;
  std::string menuModel;
  bool c3gameplay;
};

class BuildButton : public PushButton
{
public:
  BuildButton( Widget* parent, const std::string& caption, const Rect& rectangle, int id )
      : PushButton( parent, rectangle, caption, id )
  {
    Font fontRed = Font::create( FONT_2_RED );

    setFont( fontRed, stHovered );
    setTextAlignment( align::upperLeft, align::center );
    setTextOffset( Point( 15, 0 ) );

    auto& btn = add<HelpButton>( Point(width()-24,0), "");
    btn.setAlignment( align::lowerRight, align::lowerRight, align::lowerRight, align::lowerRight );
  }

  HelpButton* helpButton() const
  {
    auto items = findChildren<HelpButton*>();
    if( !items.empty() )
      return items.front();

    return nullptr;
  }

  void setSound(const std::string& name)
  {
    addProperty( "sound", name );
  }

  void setHelpType(const std::string& id)
  {
    auto btn = helpButton();
    if( btn )
      btn->setHelpId( id );
  }

  void _updateTexture()
  {
    ElementState state = _state();
    PushButton::_updateTexture();

    Font f = font( state );

    if( f.isValid() && cost() >= 0 )
    {           
      std::string text = utils::i2str( cost() );
      Rect textRect = f.getTextRect( text, Rect( 5, 0, width()-27, height() ),
                                     align::lowerRight, verticalTextAlign() );
      canvasDraw( text, textRect.lefttop(), f );
    }
  }

  void _finalizeResize()
  {
    for( int i=0; i < StateCount; i++ )
        _updateBackground( ElementState(i) );

    auto btn = helpButton();
    if( btn )
      btn->setVisible(cost() > 0);
  }

  void setCost(const int cost){ addProperty("cost",cost); }
  int cost() const            { return getProperty("cost").toInt();}
};

BuildMenu::BuildMenu( Widget* parent, const Rect& rectangle, int id,
                      city::development::Branch branch )
    : Widget( parent, id, rectangle ), __INIT_IMPL(BuildMenu)
{
  __D_REF(d,BuildMenu)
  d.c3gameplay = false;
  d.branch = branch;
}

void BuildMenu::initialize()
{
  // compute the necessary width
  __D_IMPL(_d,BuildMenu)
  int max_text_width = 0;
  int max_cost_width = 0;
  Size textSize;
  Font font = Font::create( FONT_2 );

  VariantMap allItems = config::load( _d->menuModel );
  VariantMap config = allItems.get( city::development::toString( _d->branch ) ).toMap();
  VariantList submenu = config.get( "submenu" ).toList();
  VariantList buildings = config.get( "buildings" ).toList();

  for( auto& item : submenu )
  {
    development::Branch branch = development::findBranch( item.toString() );
    if( branch != development::unknown )
    {
      std::string title = fmt::format( "##bldm_{}##", item.toString() );
      addSubmenuButton( branch, title );
    }
  }

  for( auto& item : buildings )
  {
    object::Type bType = object::findType( item.toString() );
    if( bType != object::unknown )
    {
      addBuildButton( bType );
    }
  }

  auto buildButtons = children().select<BuildButton>();
  for( auto bbutton : buildButtons )
  {
    textSize = font.getTextSize( bbutton->text());
    max_text_width = std::max(max_text_width, textSize.width() );
    textSize = font.getTextSize( utils::i2str( bbutton->cost() ) );
    max_cost_width = std::max(max_cost_width, textSize.width());
  }

  setWidth( std::max(150, max_text_width + max_cost_width + 30 + 27) );

  // set the same size for all buttons
  for( auto button : buildButtons )
    button->setWidth( width() );
}

BuildMenu::~BuildMenu() {}

void BuildMenu::addSubmenuButton(const city::development::Branch menuType, const std::string &text)
{  
  if( !_dfunc()->options.isGroupAvailable( menuType ) )
    return;

  BuildButton& button = add<BuildButton>( _(text),
                                          Rect( 0, height(), width(), height()+25 ),
                                          menuType | subMenuCreateIdHigh );
  button.setCost(-1);  // no display cost
  button.setSound( "bmsel_" + development::toString( menuType ) );

  CONNECT_LOCAL( &button, onClickedEx(), BuildMenu::_resolveButtonClick );

  setHeight( height() + 30 );
}

void BuildMenu::addBuildButton(const object::Type buildingType )
{
  __D_IMPL(_d,BuildMenu)
  //int t = DateTime::getElapsedTime();
  auto info = object::Info::find( buildingType );

  int cost = info .cost();
  bool mayBuildInCity = _d->options.isBuildingAvailable( buildingType );
  if( _d->c3gameplay )
  {
    mayBuildInCity &= info .c3logic( true );
  }

  if( cost > 0 && mayBuildInCity )
  {
    // building can be built
    auto& button = add<BuildButton>( _(info.prettyName()),
                                     Rect( 0, height(), width(), height() + 25 ), -1 );
    button.setCost(cost);
    button.setID( buildingType );
    button.setSound( "bmsel_" + info.name() );
    button.setHelpType( info.typeName() );

    setHeight( height() + 30 );

    CONNECT_LOCAL( &button, onClickedEx(), BuildMenu::_resolveButtonClick );
  }
}

void BuildMenu::setModel(const std::string& filename)
{
  _dfunc()->menuModel = filename;
}

void BuildMenu::setC3gameplay(bool enabled)
{
  _dfunc()->c3gameplay = enabled;
}

BuildMenu* BuildMenu::create(const city::development::Branch menuType, Widget* parent, bool c3gameplay )
{
  BuildMenu* ret = 0;
  switch (menuType)
  {
  case development::water:          ret = &parent->add<BuildMenu_water >        ( Rect( 0, 0, 60, 1 )); break;
  case development::health:         ret = &parent->add<BuildMenu_health>        ( Rect( 0, 0, 60, 1 )); break;
  case development::security:       ret = &parent->add<BuildMenu_security>      ( Rect( 0, 0, 60, 1 )); break;
  case development::education:      ret = &parent->add<BuildMenu_education>     ( Rect( 0, 0, 60, 1 )); break;
  case development::engineering:    ret = &parent->add<BuildMenu_engineering>   ( Rect( 0, 0, 60, 1 )); break;
  case development::administration: ret = &parent->add<BuildMenu_administration>( Rect( 0, 0, 60, 1 )); break;
  case development::entertainment:  ret = &parent->add<BuildMenu_entertainment> ( Rect( 0, 0, 60, 1 )); break;
  case development::commerce:       ret = &parent->add<BuildMenu_commerce>      ( Rect( 0, 0, 60, 1 )); break;
  case development::farm:           ret = &parent->add<BuildMenu_farm>          ( Rect( 0, 0, 60, 1 )); break;
  case development::raw_material:   ret = &parent->add<BuildMenu_raw_material>  ( Rect( 0, 0, 60, 1 )); break;
  case development::factory:        ret = &parent->add<BuildMenu_factory>       ( Rect( 0, 0, 60, 1 )); break;
  case development::religion:       ret = &parent->add<BuildMenu_religion>      ( Rect( 0, 0, 60, 1 )); break;
  case development::temple:         ret = &parent->add<BuildMenu_temple>        ( Rect( 0, 0, 60, 1 )); break;
  case development::big_temple:     ret = &parent->add<BuildMenu_bigtemple>     ( Rect( 0, 0, 60, 1 )); break;
  default:       break; // DO NOTHING 
  };

  if( ret )
  {
    ret->setC3gameplay( c3gameplay );
  }

  return ret;
}

bool BuildMenu::isPointInside( const Point& point ) const
{
  Rect clickedRect = ui()->rootWidget()->absoluteRect();
  clickedRect._bottomright = Point( parent()->screenLeft(), ui()->rootWidget()->height() );
  return clickedRect.isPointInside( point );
}

void BuildMenu::setBuildOptions( const development::Options& options ) { _dfunc()->options = options; }

void BuildMenu::_resolveButtonClick( Widget* widget )
{
  std::string sound = widget->getProperty( "sound" ).toString();
  if( sound.empty() )
    sound = "icon";

  events::dispatch<PlaySound>( sound, 1, 100 );
}

BuildMenu_water::BuildMenu_water( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::water )
{
}

BuildMenu_security::BuildMenu_security( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::security )
{
}

BuildMenu_education::BuildMenu_education( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::education )
{

}

BuildMenu_health::BuildMenu_health( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::health )
{
}

BuildMenu_engineering::BuildMenu_engineering( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::engineering )
{
}

BuildMenu_administration::BuildMenu_administration( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::administration )
{
}

BuildMenu_entertainment::BuildMenu_entertainment( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::entertainment )
{
}

BuildMenu_commerce::BuildMenu_commerce( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::commerce )
{
}

BuildMenu_farm::BuildMenu_farm( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::farm )
{
}

BuildMenu_raw_material::BuildMenu_raw_material( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::raw_material )
{
}

BuildMenu_factory::BuildMenu_factory( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::factory )
{}

BuildMenu_religion::BuildMenu_religion( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::religion )
{}

BuildMenu_temple::BuildMenu_temple( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::temple )
{
}

BuildMenu_bigtemple::BuildMenu_bigtemple( Widget* parent, const Rect& rectangle )
  : BuildMenu( parent, rectangle, -1, development::big_temple )
{
}

}//end namespace gui
