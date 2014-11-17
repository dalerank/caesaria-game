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
#include "core/stringhelper.hpp"
#include "objects/metadata.hpp"
#include "city/build_options.hpp"
#include "core/foreach.hpp"
#include "objects/constants.hpp"
#include "events/playsound.hpp"
#include "core/logger.hpp"

using namespace constants;
using namespace gfx;

// used to display the building name and its cost
namespace gui
{

CAESARIA_LITERALCONST(cost)

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

  void setCost(const int cost)
  {
    _cost = cost;
  }

  void _resizeEvent()
  {
    for( int i=0; i < StateCount; i++ )
        _updateBackground( ElementState(i) );
  }

  int getCost() const
  {
    return _cost;
  }

private:
    int _cost;   // cost of the building
};

BuildMenu::BuildMenu( Widget* parent, const Rect& rectangle, int id )
    : Widget( parent, id, rectangle )
{
}

void BuildMenu::initialize()
{
  // compute the necessary width
  int max_text_width = 0;
  int max_cost_width = 0;
  Size textSize;
  Font font = Font::create( FONT_2 );
  Widget::Widgets rchildren = children();
  foreach( widget, rchildren )
  {
    BuildButton *button = dynamic_cast< BuildButton* >( *widget );
    if( button )
    {
        textSize = font.getTextSize( button->text());
        max_text_width = std::max(max_text_width, textSize.width() );

        std::string text = StringHelper::format( 0xff, "%i", button->getCost() );
        textSize = font.getTextSize( text );
        max_cost_width = std::max(max_cost_width, textSize.width());
    }
  }

  setWidth( std::max(150, max_text_width + max_cost_width + 20) );

  // set the same size for all buttons
  foreach( widget, rchildren )
  {
    BuildButton *button = dynamic_cast< BuildButton* >( *widget );
    if( button )
    {
      button->setWidth( width() );
    }
  }
}

BuildMenu::~BuildMenu()
{
}

void BuildMenu::addSubmenuButton(const BuildMenuType menuType, const std::string &text)
{
  if( !_options.isGroupAvailable( menuType ) )
    return;

  BuildButton* button = new BuildButton( this, text, Rect( Point( 0, height() ), Size( width(), 25 ) ), -1 );
  button->setID( menuType | subMenuCreateIdHigh );
  button->setCost(-1);  // no display

  setHeight( height() + 30 );
}


void BuildMenu::addBuildButton(const TileOverlay::Type buildingType )
{
  //int t = DateTime::getElapsedTime();
  const MetaData &buildingData = MetaDataHolder::instance().getData( buildingType );

  int cost = buildingData.getOption( lc_cost );
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

BuildMenu* BuildMenu::create(const BuildMenuType menuType, Widget* parent )
{
  BuildMenu* ret = 0;
  switch (menuType)
  {
  case BM_WATER:          ret = new BuildMenu_water( parent, Rect( 0, 0, 60, 1 ) ); break;
  case BM_HEALTH:         ret = new BuildMenu_health( parent, Rect( 0, 0, 60, 1 ) ); break;
  case BM_SECURITY:       ret = new BuildMenu_security( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_EDUCATION:      ret = new BuildMenu_education( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_ENGINEERING:    ret = new BuildMenu_engineering( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_ADMINISTRATION: ret = new BuildMenu_administration( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_ENTERTAINMENT:  ret = new BuildMenu_entertainment( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_COMMERCE:       ret = new BuildMenu_commerce( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_FARM:           ret = new BuildMenu_farm( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_RAW_MATERIAL:   ret = new BuildMenu_raw_factory( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_FACTORY:        ret = new BuildMenu_factory( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_RELIGION:       ret = new BuildMenu_religion( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_TEMPLE:         ret = new BuildMenu_temple( parent, Rect( 0, 0, 60, 1 )); break;
  case BM_BIGTEMPLE:      ret = new BuildMenu_bigtemple( parent, Rect( 0, 0, 60, 1 ) ); break;
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

void BuildMenu::setBuildOptions( const city::BuildOptions& options ) {  _options = options; }

void BuildMenu::_resolveButtonClick()
{
  events::GameEventPtr e = events::PlaySound::create( "icon", 1, 100 );
  e->dispatch();
}

void BuildMenu_water::initialize()
{
  addBuildButton(objects::fountain);
  addBuildButton(objects::well);
  addBuildButton(objects::aqueduct);
  addBuildButton(objects::reservoir);

  BuildMenu::initialize();
}

BuildMenu_water::BuildMenu_water( Widget* parent, const Rect& rectangle )
	: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_security::initialize()
{
  addBuildButton(objects::prefecture);
  addBuildButton(objects::wall);
  addBuildButton(objects::fortification);
  addBuildButton(objects::fortJavelin);
  addBuildButton(objects::fortLegionaire);
  addBuildButton(objects::fortMounted);
  addBuildButton(objects::barracks);
  addBuildButton(objects::gatehouse);
  addBuildButton(objects::tower);

  BuildMenu::initialize();
}

BuildMenu_security::BuildMenu_security( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_education::initialize()
{
  addBuildButton(objects::school);
  addBuildButton(objects::library);
  addBuildButton(objects::academy);
  addBuildButton(objects::missionaryPost);

  BuildMenu::initialize();
}

BuildMenu_education::BuildMenu_education( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_health::initialize()
{
  addBuildButton(objects::doctor);
  addBuildButton(objects::barber);
  addBuildButton(objects::baths);
  addBuildButton(objects::hospital);

  BuildMenu::initialize();
}

BuildMenu_health::BuildMenu_health( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_engineering::initialize()
{
  addBuildButton(objects::engineerPost);
  addBuildButton(objects::lowBridge);
  addBuildButton(objects::highBridge);
  addBuildButton(objects::dock);
  addBuildButton(objects::shipyard);
  addBuildButton(objects::wharf);
  addBuildButton(objects::triumphalArch);
  addBuildButton(objects::garden);
  addBuildButton(objects::plaza);

  BuildMenu::initialize();
}

BuildMenu_engineering::BuildMenu_engineering( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_administration::initialize()
{
  addBuildButton(objects::forum);
  addBuildButton(objects::senate);

  addBuildButton(objects::governorHouse);
  addBuildButton(objects::governorVilla);
  addBuildButton(objects::governorPalace);

  addBuildButton(objects::smallStatue);
  addBuildButton(objects::middleStatue);
  addBuildButton(objects::bigStatue);

  BuildMenu::initialize();
}

BuildMenu_administration::BuildMenu_administration( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_entertainment::initialize()
{
  addBuildButton(objects::theater);
  addBuildButton(objects::amphitheater);
  addBuildButton(objects::colloseum);
  addBuildButton(objects::hippodrome);
  addBuildButton(objects::actorColony);
  addBuildButton(objects::gladiatorSchool);
  addBuildButton(objects::lionsNursery);
  addBuildButton(objects::chariotSchool);

  BuildMenu::initialize();
}

BuildMenu_entertainment::BuildMenu_entertainment( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_commerce::initialize()
{
  addSubmenuButton(BM_FARM, _("##bldm_farm##") );
  addSubmenuButton(BM_RAW_MATERIAL, _("##bldm_raw##") );
  addSubmenuButton(BM_FACTORY, _("##bldm_factory##") );

  addBuildButton(objects::market);
  addBuildButton(objects::granary);
  addBuildButton(objects::warehouse);

  BuildMenu::initialize();
}

BuildMenu_commerce::BuildMenu_commerce( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_farm::initialize()
{
  addBuildButton(objects::wheatFarm);
  addBuildButton(objects::fruitFarm);
  addBuildButton(objects::oliveFarm);
  addBuildButton(objects::grapeFarm);
  addBuildButton(objects::pigFarm);
  addBuildButton(objects::vegetableFarm);

  BuildMenu::initialize();
}

BuildMenu_farm::BuildMenu_farm( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_raw_factory::initialize()
{
  addBuildButton(objects::marbleQuarry);
  addBuildButton(objects::ironMine);
  addBuildButton(objects::timberLogger);
  addBuildButton(objects::clayPit);

  BuildMenu::initialize();
}

BuildMenu_raw_factory::BuildMenu_raw_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_factory::initialize()
{
  addBuildButton(objects::winery);
  addBuildButton(objects::creamery);
  addBuildButton(objects::weaponsWorkshop);
  addBuildButton(objects::furnitureWorkshop);
  addBuildButton(objects::pottery);

  BuildMenu::initialize();
}

BuildMenu_factory::BuildMenu_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{}

void BuildMenu_religion::initialize()
{
  addSubmenuButton(BM_TEMPLE , _("##small_temples##") );
  addSubmenuButton(BM_BIGTEMPLE , _("##large_temples##") );

  addBuildButton(objects::oracle);

  BuildMenu::initialize();
}

BuildMenu_religion::BuildMenu_religion( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{}

void BuildMenu_temple::initialize()
{
  addBuildButton(objects::templeCeres);
  addBuildButton(objects::templeNeptune);
  addBuildButton(objects::templeMars);
  addBuildButton(objects::templeVenus);
  addBuildButton(objects::templeMercury);

  BuildMenu::initialize();
}

BuildMenu_temple::BuildMenu_temple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}
void BuildMenu_bigtemple::initialize()
{
  addBuildButton(objects::cathedralCeres);
  addBuildButton(objects::cathedralNeptune);
  addBuildButton(objects::cathedralMars);
  addBuildButton(objects::cathedralVenus);
  addBuildButton(objects::cathedralMercury);

  BuildMenu::initialize();
}

BuildMenu_bigtemple::BuildMenu_bigtemple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{
}

}//end namespace gui
