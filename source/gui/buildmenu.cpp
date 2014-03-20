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
    setTextAlignment( alignUpperLeft, alignCenter );
    setTextOffset( Point( 15, 0 ) );
  }

  void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    Font& font = getFont( state );

    if( font.isValid() && _cost >= 0)
    {           
        char buffer[32];
        sprintf( buffer, "%d", _cost );
        Rect textRect = font.calculateTextRect( buffer, Rect( 5, 0, width()-10, height() ),
                                                alignLowerRight, getVerticalTextAlign() );
        font.draw( *_getTextPicture( state ), buffer, textRect.left(), textRect.top() );
    }
  }

  void setCost(const int cost)
  {
    _cost = cost;
  }

  void _resizeEvent()
  {
    for( int i=0; i < StateCount; i++ )
        _updateTexture( ElementState(i) );
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
  Widget::Widgets children = getChildren();
  foreach( widget, children )
  {
    BuildButton *button = dynamic_cast< BuildButton* >( *widget );
    if( button )
    {
        textSize = font.getSize( button->text());
        max_text_width = std::max(max_text_width, textSize.width() );

        std::string text = StringHelper::format( 0xff, "%i", button->getCost() );
        textSize = font.getSize( text );
        max_cost_width = std::max(max_cost_width, textSize.width());
    }
  }

  setWidth( std::max(150, max_text_width + max_cost_width + 20) );

  // set the same size for all buttons
  foreach( widget, children )
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

  int cost = buildingData.getOption( "cost" );
  bool mayBuildInCity = _options.isBuildingAvailble( buildingType );
  if( cost > 0 && mayBuildInCity )
  {
    // building can be built
    BuildButton* button = new BuildButton( this, _(buildingData.getPrettyName().c_str()),
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
  clickedRect.LowerRightCorner = Point( getParent()->screenLeft(), _environment->rootWidget()->height() );
  return clickedRect.isPointInside( point );
}

void BuildMenu::setBuildOptions( const CityBuildOptions& options )
{
  _options = options;
}

void BuildMenu::_resolveButtonClick()
{
  events::GameEventPtr e = events::PlaySound::create( "icon", 1, 100 );
  e->dispatch();
}

void BuildMenu_water::initialize()
{
  addBuildButton(building::fountain);
  addBuildButton(building::well);
  addBuildButton(building::aqueduct);
  addBuildButton(building::reservoir);

  BuildMenu::initialize();
}

BuildMenu_water::BuildMenu_water( Widget* parent, const Rect& rectangle )
	: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_security::initialize()
{
  addBuildButton(building::prefecture);
  addBuildButton(building::wall);
  addBuildButton(building::fortification);
  addBuildButton(building::fortJavelin);
  addBuildButton(building::fortLegionaire);
  addBuildButton(building::fortMounted);
  addBuildButton(building::barracks);
  addBuildButton(building::gatehouse);
  addBuildButton(building::tower);

  BuildMenu::initialize();
}

BuildMenu_security::BuildMenu_security( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_education::initialize()
{
  addBuildButton(building::school);
  addBuildButton(building::library);
  addBuildButton(building::academy);

  BuildMenu::initialize();
}

BuildMenu_education::BuildMenu_education( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_health::initialize()
{
  addBuildButton(building::doctor);
  addBuildButton(building::barber);
  addBuildButton(building::baths);
  addBuildButton(building::hospital);

  BuildMenu::initialize();
}

BuildMenu_health::BuildMenu_health( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_engineering::initialize()
{
  addBuildButton(building::engineerPost);
  addBuildButton(building::lowBridge);
  addBuildButton(building::highBridge);
  addBuildButton(building::dock);
  addBuildButton(building::shipyard);
  addBuildButton(building::wharf);
  addBuildButton(building::triumphalArch);
  addBuildButton(construction::garden);
  addBuildButton(construction::plaza);

  BuildMenu::initialize();
}

BuildMenu_engineering::BuildMenu_engineering( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_administration::initialize()
{
  addBuildButton(building::forum);
  addBuildButton(building::senate);

  addBuildButton(building::governorHouse);
  addBuildButton(building::governorVilla);
  addBuildButton(building::governorPalace);

  addBuildButton(building::smallStatue);
  addBuildButton(building::middleStatue);
  addBuildButton(building::bigStatue);

  BuildMenu::initialize();
}

BuildMenu_administration::BuildMenu_administration( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_entertainment::initialize()
{
  addBuildButton(building::theater);
  addBuildButton(building::amphitheater);
  addBuildButton(building::colloseum);
  addBuildButton(building::hippodrome);
  addBuildButton(building::actorColony);
  addBuildButton(building::gladiatorSchool);
  addBuildButton(building::lionsNursery);
  addBuildButton(building::chariotSchool);

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

  addBuildButton(building::market);
  addBuildButton(building::granary);
  addBuildButton(building::warehouse);

  BuildMenu::initialize();
}

BuildMenu_commerce::BuildMenu_commerce( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_farm::initialize()
{
  addBuildButton(building::wheatFarm);
  addBuildButton(building::fruitFarm);
  addBuildButton(building::oliveFarm);
  addBuildButton(building::grapeFarm);
  addBuildButton(building::pigFarm);
  addBuildButton(building::vegetableFarm);

  BuildMenu::initialize();
}

BuildMenu_farm::BuildMenu_farm( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_raw_factory::initialize()
{
  addBuildButton(building::marbleQuarry);
  addBuildButton(building::ironMine);
  addBuildButton(building::timberLogger);
  addBuildButton(building::clayPit);

  BuildMenu::initialize();
}

BuildMenu_raw_factory::BuildMenu_raw_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_factory::initialize()
{
  addBuildButton(building::winery);
  addBuildButton(building::creamery);
  addBuildButton(building::weaponsWorkshop);
  addBuildButton(building::furnitureWorkshop);
  addBuildButton(building::pottery);

  BuildMenu::initialize();
}

BuildMenu_factory::BuildMenu_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{}

void BuildMenu_religion::initialize()
{
  addSubmenuButton(BM_TEMPLE , _("##small_temples##") );
  addSubmenuButton(BM_BIGTEMPLE , _("##large_temples##") );

  addBuildButton(building::oracle);

  BuildMenu::initialize();
}

BuildMenu_religion::BuildMenu_religion( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{}

void BuildMenu_temple::initialize()
{
  addBuildButton(building::templeCeres);
  addBuildButton(building::templeNeptune);
  addBuildButton(building::templeMars);
  addBuildButton(building::templeVenus);
  addBuildButton(building::templeMercury);

  BuildMenu::initialize();
}

BuildMenu_temple::BuildMenu_temple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}
void BuildMenu_bigtemple::initialize()
{
  addBuildButton(building::cathedralCeres);
  addBuildButton(building::cathedralNeptune);
  addBuildButton(building::cathedralMars);
  addBuildButton(building::cathedralVenus);
  addBuildButton(building::cathedralMercury);

  BuildMenu::initialize();
}

BuildMenu_bigtemple::BuildMenu_bigtemple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{
}

}//end namespace gui
