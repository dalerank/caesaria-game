// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdio>
#include <cstring>

#include "oc3_buildmenu.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_event.hpp"
#include "oc3_guienv.hpp"
#include "oc3_time.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_building_data.hpp"
#include "oc3_build_options.hpp"

// used to display the building name and its cost
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
  }

  void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    Font& font = getFont( state );

    if( font.isValid() && _cost >= 0)
    {           
        char buffer[32];
        sprintf( buffer, "%d", _cost );
        Rect textRect = font.calculateTextRect( buffer, Rect( 0, 0, getWidth(), getHeight() ),
                                                alignLowerRight, getVerticalTextAlign() );
        font.draw( *_getPicture( state ), buffer, textRect.getLeft(), textRect.getTop() );
    }
  }

  void setCost(const int cost)
  {
    _cost = cost;
  }

  void resizeEvent_()
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
    : Widget( parent, id, rectangle ), _options( 0 )
{
}

void BuildMenu::initialize()
{
  // compute the necessary width
  int max_text_width = 0;
  int max_cost_width = 0;
  Size textSize;
  Font font = Font::create( FONT_2 );
  for (Widget::ConstChildIterator itWidget = getChildren().begin(); itWidget != getChildren().end(); ++itWidget)
  {
      if( BuildButton *button = dynamic_cast< BuildButton* >( *itWidget ) )
      {
          textSize = font.getSize( button->getText());
          max_text_width = std::max(max_text_width, textSize.getWidth() );

          std::string text = StringHelper::format( 0xff, "%i", button->getCost() );
          textSize = font.getSize( text );
          max_cost_width = std::max(max_cost_width, textSize.getWidth());
      }
  }

  setWidth( std::max(150, max_text_width + max_cost_width + 20) );

  // set the same size for all buttons
  for (Widget::ConstChildIterator itWidget = getChildren().begin(); itWidget != getChildren().end(); ++itWidget)
  {
      if( BuildButton *button = dynamic_cast< BuildButton* >( *itWidget ) )
      {
          button->setWidth( getWidth() );
      }
  }
}

BuildMenu::~BuildMenu()
{
}

void BuildMenu::addSubmenuButton(const BuildMenuType menuType, const std::string &text)
{
  BuildButton* button = new BuildButton( this, text, Rect( Point( 0, getHeight() ), Size( getWidth(), 25 ) ), -1 );
  button->setID( menuType | subMenuCreateIdHigh );
  button->setCost(-1);  // no display

  setHeight( getHeight() + 30 );
}


void BuildMenu::addBuildButton( const BuildingType buildingType )
{
  //int t = DateTime::getElapsedTime();
  BuildingData &buildingData = BuildingDataHolder::instance().getData( buildingType );

  int cost = buildingData.getCost();
  bool mayBuildInCity = (_options ? _options->isBuildingAvailble( buildingType ) : true);
  if( cost > 0 && mayBuildInCity )
  {
      // building can be built
      BuildButton* button = new BuildButton( this, buildingData.getPrettyName(), Rect( 0, getHeight(), getWidth(), getHeight() + 25 ), -1 );
      button->setCost(cost);
      button->setID( buildingType );

      setHeight( getHeight() + 30 );
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
    Rect clickedRect = _environment->getRootWidget()->getAbsoluteRect();
    clickedRect.LowerRightCorner = Point( getParent()->getScreenLeft(), _environment->getRootWidget()->getHeight() );
    return clickedRect.isPointInside( point );
}

void BuildMenu::setBuildOptions( CityBuildOptions* options )
{
  _options = options;
}

void BuildMenu_water::initialize()
{
  addBuildButton(B_FOUNTAIN);
  addBuildButton(B_WELL);
  addBuildButton(B_AQUEDUCT);
  addBuildButton(B_RESERVOIR);

  BuildMenu::initialize();
}

BuildMenu_water::BuildMenu_water( Widget* parent, const Rect& rectangle )
	: BuildMenu( parent, rectangle, -1 )	 
{
	
}

void BuildMenu_security::initialize()
{
  addBuildButton(B_PREFECTURE);

  BuildMenu::initialize();
}

BuildMenu_security::BuildMenu_security( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_education::initialize()
{
  addBuildButton(B_SCHOOL);
  addBuildButton(B_LIBRARY);
  addBuildButton(B_COLLEGE);

  BuildMenu::initialize();
}

BuildMenu_education::BuildMenu_education( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_health::initialize()
{
  addBuildButton(B_DOCTOR);
  addBuildButton(B_BARBER);
  addBuildButton(B_BATHS);
  addBuildButton(B_HOSPITAL);

  BuildMenu::initialize();
}

BuildMenu_health::BuildMenu_health( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_engineering::initialize()
{
  addBuildButton(B_ENGINEER_POST);
  addBuildButton(B_LOW_BRIDGE);
  addBuildButton(B_HIGH_BRIDGE);
  addBuildButton(B_DOCK);
  addBuildButton(B_SHIPYARD);
  addBuildButton(B_WHARF);
  addBuildButton(B_TRIUMPHAL_ARCH);
  addBuildButton(B_GARDEN);
  addBuildButton(B_PLAZA);

  BuildMenu::initialize();
}

BuildMenu_engineering::BuildMenu_engineering( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_administration::initialize()
{
  addBuildButton(B_FORUM);
  addBuildButton(B_SENATE);

  addBuildButton(B_GOVERNOR_HOUSE);
  addBuildButton(B_GOVERNOR_VILLA);
  addBuildButton(B_GOVERNOR_PALACE);

  addBuildButton(B_STATUE1);
  addBuildButton(B_STATUE2);
  addBuildButton(B_STATUE3);

  BuildMenu::initialize();
}

BuildMenu_administration::BuildMenu_administration( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_entertainment::initialize()
{
  addBuildButton(B_THEATER);
  addBuildButton(B_AMPHITHEATER);
  addBuildButton(B_COLLOSSEUM);
  addBuildButton(B_HIPPODROME);
  addBuildButton(B_ACTOR);
  addBuildButton(B_GLADIATOR);
  addBuildButton(B_LION);
  addBuildButton(B_CHARIOT);

  BuildMenu::initialize();
}

BuildMenu_entertainment::BuildMenu_entertainment( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_commerce::initialize()
{
  addSubmenuButton(BM_FARM, _("Farm") );
  addSubmenuButton(BM_RAW_MATERIAL, _("Raw materials") );
  addSubmenuButton(BM_FACTORY, _("Factory") );

  addBuildButton(B_MARKET);
  addBuildButton(B_GRANARY);
  addBuildButton(B_WAREHOUSE);

  BuildMenu::initialize();
}

BuildMenu_commerce::BuildMenu_commerce( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_farm::initialize()
{
  addBuildButton(B_WHEAT_FARM);
  addBuildButton(B_FRUIT);
  addBuildButton(B_OLIVE);
  addBuildButton(B_GRAPE);
  addBuildButton(B_MEAT);
  addBuildButton(B_VEGETABLE);

  BuildMenu::initialize();
}

BuildMenu_farm::BuildMenu_farm( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_raw_factory::initialize()
{
  addBuildButton(B_MARBLE);
  addBuildButton(B_IRON_MINE);
  addBuildButton(B_TIMBER);
  addBuildButton(B_CLAY_PIT);

  BuildMenu::initialize();
}

BuildMenu_raw_factory::BuildMenu_raw_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_factory::initialize()
{
  addBuildButton(B_WINE);
  addBuildButton(B_OIL);
  addBuildButton(B_WEAPON);
  addBuildButton(B_FURNITURE);
  addBuildButton(B_POTTERY);

  BuildMenu::initialize();
}

BuildMenu_factory::BuildMenu_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_religion::initialize()
{
  addSubmenuButton(BM_TEMPLE , _("Small temples") );   
  addSubmenuButton(BM_BIGTEMPLE , _("Large temples") );
  addBuildButton(B_TEMPLE_ORACLE);   

  BuildMenu::initialize();
}

BuildMenu_religion::BuildMenu_religion( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_temple::initialize()
{
  addBuildButton(B_TEMPLE_CERES);
  addBuildButton(B_TEMPLE_NEPTUNE);
  addBuildButton(B_TEMPLE_MARS);
  addBuildButton(B_TEMPLE_VENUS);
  addBuildButton(B_TEMPLE_MERCURE);

  BuildMenu::initialize();
}

BuildMenu_temple::BuildMenu_temple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}
void BuildMenu_bigtemple::initialize()
{
  addBuildButton(B_BIG_TEMPLE_CERES);
  addBuildButton(B_BIG_TEMPLE_NEPTUNE);
  addBuildButton(B_BIG_TEMPLE_MARS);
  addBuildButton(B_BIG_TEMPLE_VENUS);
  addBuildButton(B_BIG_TEMPLE_MERCURE);

  BuildMenu::initialize();
}

BuildMenu_bigtemple::BuildMenu_bigtemple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{
}
