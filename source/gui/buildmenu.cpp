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

#include <cstdio>
#include <cstring>

#include "buildmenu.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "gfx/decorator.hpp"
#include "core/event.hpp"
#include "environment.hpp"
#include "core/time.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "objects/metadata.hpp"
#include "city/build_options.hpp"
#include "core/foreach.hpp"
#include "objects/constants.hpp"
#include "events/playsound.hpp"
#include "core/logger.hpp"
#include "core/saveadapter.hpp"
#include "game/settings.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

// used to display the building name and its cost
namespace gui
{

class BuildButton : public PushButton
{
public:
  BuildButton( Widget* parent, const std::string& caption, const Rect& rectangle, int id )
      : PushButton( parent, rectangle, caption, id )
  {
    _cost = 0;

    //Font& font = FontCollection::instance().getFont(FONT_2);
    Font fontRed = Font::create( FONT_2_RED );

    setFont( fontRed, stHovered );
    setTextAlignment( align::upperLeft, align::center );
    setTextOffset( Point( 15, 0 ) );
  }

  void _updateTextPic()
  {
    ElementState state = _state();
    PushButton::_updateTextPic();

    Font f = font( state );

    if( f.isValid() && _cost >= 0 )
    {           
      char buffer[32];
      sprintf( buffer, "%d", _cost );
      Rect textRect = f.getTextRect( buffer, Rect( 5, 0, width()-10, height() ),
                                                align::lowerRight, verticalTextAlign() );
      f.draw( *_textPictureRef(), buffer, textRect.left(), textRect.top() );
    }
  }

  void _resizeEvent()
  {
    for( int i=0; i < StateCount; i++ )
        _updateBackground( ElementState(i) );
  }

  void setCost(const int cost)   {    _cost = cost;  }
  int cost() const  {    return _cost;  }

private:
  int _cost;   // cost of the building
};

BuildMenu::BuildMenu( Widget* parent, const Rect& rectangle, int id,
                      city::development::Branch branch )
    : Widget( parent, id, rectangle )
{
  _branch = branch;
}

void BuildMenu::initialize()
{
  // compute the necessary width
  int max_text_width = 0;
  int max_cost_width = 0;
  Size textSize;
  Font font = Font::create( FONT_2 );

  VariantMap allItems = config::load( SETTINGS_RC_PATH( buildMenuModel ) );
  VariantMap config = allItems.get( city::development::toString( _branch ) ).toMap();
  VariantList submenu = config.get( "submenu" ).toList();
  VariantList buildings = config.get( "buildings" ).toList();

  foreach( it, submenu )
  {
    development::Branch branch = development::toBranch( it->toString() );
    if( branch != development::unknown )
    {
      std::string title = utils::format( 0xff, "##bldm_%s##", it->toString().c_str() );
      addSubmenuButton( branch, title );
    }
  }

  foreach( it, buildings )
  {
    TileOverlay::Type bType = MetaDataHolder::findType( it->toString() );
    if( bType != objects::unknown )
    {
      addBuildButton( bType );
    }
  }

  foreach( widget, children() )
  {
    BuildButton *button = dynamic_cast< BuildButton* >( *widget );
    if( button )
    {
      textSize = font.getTextSize( button->text());
      max_text_width = std::max(max_text_width, textSize.width() );

      std::string text = utils::format( 0xff, "%i", button->cost() );
      textSize = font.getTextSize( text );
      max_cost_width = std::max(max_cost_width, textSize.width());
    }
  }

  setWidth( std::max(150, max_text_width + max_cost_width + 20) );

  // set the same size for all buttons
  foreach( widget, children() )
  {
    BuildButton *button = dynamic_cast< BuildButton* >( *widget );
    if( button )
    {
      button->setWidth( width() );
    }
  }
}

BuildMenu::~BuildMenu() {}

void BuildMenu::addSubmenuButton(const city::development::Branch menuType, const std::string &text)
{
  if( !_options.isGroupAvailable( menuType ) )
    return;

  BuildButton* button = new BuildButton( this, _(text), Rect( Point( 0, height() ), Size( width(), 25 ) ), -1 );
  button->setID( menuType | subMenuCreateIdHigh );
  button->setCost(-1);  // no display

  setHeight( height() + 30 );
}

void BuildMenu::addBuildButton(const TileOverlay::Type buildingType )
{
  //int t = DateTime::getElapsedTime();
  const MetaData &buildingData = MetaDataHolder::instance().getData( buildingType );

  int cost = buildingData.getOption( MetaDataOptions::cost );
  bool mayBuildInCity = _options.isBuildingAvailble( buildingType );
  if( cost > 0 && mayBuildInCity )
  {
    // building can be built
    BuildButton* button = new BuildButton( this, _(buildingData.prettyName().c_str()),
                                           Rect( 0, height(), width(), height() + 25 ), -1 );
    button->setCost(cost);
    button->setID( buildingType );

    setHeight( height() + 30 );

    CONNECT( button, onClicked(), this, BuildMenu::_resolveButtonClick );
  }
}

BuildMenu* BuildMenu::create(const city::development::Branch menuType, Widget* parent )
{
  BuildMenu* ret = 0;
  switch (menuType)
  {
  case development::water:          ret = new BuildMenu_water         ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::health:         ret = new BuildMenu_health        ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::security:       ret = new BuildMenu_security      ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::education:      ret = new BuildMenu_education     ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::engineering:    ret = new BuildMenu_engineering   ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::administration: ret = new BuildMenu_administration( parent, Rect( 0, 0, 60, 1 )); break;
  case development::entertainment:  ret = new BuildMenu_entertainment ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::commerce:       ret = new BuildMenu_commerce      ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::farm:           ret = new BuildMenu_farm          ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::raw_material:   ret = new BuildMenu_raw_material   ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::factory:        ret = new BuildMenu_factory       ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::religion:       ret = new BuildMenu_religion      ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::temple:         ret = new BuildMenu_temple        ( parent, Rect( 0, 0, 60, 1 )); break;
  case development::big_temple:     ret = new BuildMenu_bigtemple     ( parent, Rect( 0, 0, 60, 1 )); break;
  default:       break; // DO NOTHING 
  };

  return ret;
}

bool BuildMenu::isPointInside( const Point& point ) const
{
  Rect clickedRect = _environment->rootWidget()->absoluteRect();
  clickedRect.LowerRightCorner = Point( parent()->screenLeft(), _environment->rootWidget()->height() );
  return clickedRect.isPointInside( point );
}

void BuildMenu::setBuildOptions( const development::Options& options ) {  _options = options; }

void BuildMenu::_resolveButtonClick()
{
  events::GameEventPtr e = events::PlaySound::create( "icon", 1, 100 );
  e->dispatch();
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
