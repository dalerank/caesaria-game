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


#include "oc3_buildmenu.h"
#include "sdl_facade.hpp"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <string>
#include <iosfwd>

#include "gettext.hpp"
#include "oc3_pushbutton.h"
#include "gui_paneling.hpp"
#include "oc3_event.h"

#include "building_data.hpp"

static const int subMenuCreateIdHigh = 0x1000;
static BuildMenu* buildMenuInstance = 0;

// used to display the building name and its cost
class BuildButton : public PushButton
{
public:
    BuildButton( Widget* parent, const std::string& caption, const Rect& rectangle, int id )
        : PushButton( parent, rectangle, caption, id )
    {
        _cost = 0;

        //Font& font = FontCollection::instance().getFont(FONT_2);
        Font& fontRed = FontCollection::instance().getFont(FONT_2_RED);

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
            //itoa( _cost, buffer, 10 ); - no itoa in linux :-(
	    sprintf(buffer, "%i", _cost);
            Rect textRect = font.calculateTextRect( buffer, Rect( 0, 0, getWidth(), getHeight() ),
                                                    alignLowerRight, getVerticalTextAlign() );
            SdlFacade::instance().drawText( *_getPicture( state ), buffer, textRect.getLeft(), textRect.getTop(), 
                                            font );
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

class BuildMenu::Impl
{
oc3_signals public:
    Signal2< int, Widget* > onCreateBuildMenuSignal;
    Signal1< int > onCreateConstructionSignal;
};

BuildMenu::BuildMenu( Widget* parent, const Rect& rectangle, int id )
    : Widget( parent, id, rectangle ), _d( new Impl )
{
}

Signal2<int, Widget*>& BuildMenu::onCreateBuildMenu()
{
    return _d->onCreateBuildMenuSignal;
}

Signal1< int >& BuildMenu::onCreateConstruction()
{
    return _d->onCreateConstructionSignal;
}


bool BuildMenu::onEvent(const NEvent& event)
{
    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
    {
        buildMenuInstance = 0;

        if( BuildButton* btn = safety_cast< BuildButton* >( event.GuiEvent.Caller ) )
        {
            int id = btn->getID();
            if( id & subMenuCreateIdHigh )
            {
                _d->onCreateBuildMenuSignal.emit( id & 0xff, this );
            }
            else
            {
                _d->onCreateConstructionSignal.emit( id );
            }
        }
     
        deleteLater();
        return true;
    }

    return Widget::onEvent( event );
}

void BuildMenu::init()
{
    addButtons();

    // compute the necessary width
    SdlFacade &sdlFacade = SdlFacade::instance();
    int max_text_width = 0;
    int max_cost_width = 0;
    int text_width;
    int text_height;
    Font &font = FontCollection::instance().getFont(FONT_2);
    for (Widget::ConstChildIterator itWidget = getChildren().begin(); itWidget != getChildren().end(); ++itWidget)
    {
        if( BuildButton *button = dynamic_cast< BuildButton* >( *itWidget ) )
        {
            sdlFacade.getTextSize(font, button->getText(), text_width, text_height);
            max_text_width = std::max(max_text_width, text_width);

            char buffer[32];
	    sprintf(buffer, "%i", button->getCost());
            sdlFacade.getTextSize(font, buffer, text_width, text_height);
            max_cost_width = std::max(max_cost_width, text_width);
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


void BuildMenu::addBuildButton(const BuildingType buildingType)
{
    BuildingData &buildingData = BuildingDataHolder::instance().getData(buildingType);

    int cost = buildingData.getCost();
    if( cost != -1 )
    {
        // building can be built
        BuildButton* button = new BuildButton( this, buildingData.getPrettyName(), Rect( 0, getHeight(), getWidth(), getHeight() + 25 ), -1 );
        button->setCost(cost);
        button->setID( buildingType );

        setHeight( getHeight() + 30 );
    }
}

BuildMenu *BuildMenu::getMenuInstance(const BuildMenuType menuType, Widget* parent )
{
    if( buildMenuInstance )
    {
        buildMenuInstance->deleteLater();
        buildMenuInstance = 0;
    }

    switch (menuType)
    {
    case BM_WATER:          buildMenuInstance = new BuildMenu_water( parent, Rect( 0, 0, 60, 1 ) ); break;
    case BM_HEALTH:         buildMenuInstance = new BuildMenu_health( parent, Rect( 0, 0, 60, 1 ) ); break;
    case BM_SECURITY:       buildMenuInstance = new BuildMenu_security( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_EDUCATION:      buildMenuInstance = new BuildMenu_education( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_ENGINEERING:    buildMenuInstance = new BuildMenu_engineering( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_ADMINISTRATION: buildMenuInstance = new BuildMenu_administration( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_ENTERTAINMENT:  buildMenuInstance = new BuildMenu_entertainment( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_COMMERCE:       buildMenuInstance = new BuildMenu_commerce( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_FARM:           buildMenuInstance = new BuildMenu_farm( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_RAW_MATERIAL:   buildMenuInstance = new BuildMenu_raw_factory( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_FACTORY:        buildMenuInstance = new BuildMenu_factory( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_RELIGION:       buildMenuInstance = new BuildMenu_religion( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_TEMPLE:         buildMenuInstance = new BuildMenu_temple( parent, Rect( 0, 0, 60, 1 )); break;
    case BM_BIGTEMPLE:      buildMenuInstance = new BuildMenu_bigtemple( parent, Rect( 0, 0, 60, 1 ) ); break;
    default:       break; // DO NOTHING 
    };

    return buildMenuInstance;
}

void BuildMenu_water::addButtons()
{
    addBuildButton(B_FOUNTAIN);
    addBuildButton(B_WELL);
    addBuildButton(B_AQUEDUCT);
    addBuildButton(B_RESERVOIR);
}

BuildMenu_water::BuildMenu_water( Widget* parent, const Rect& rectangle )
	: BuildMenu( parent, rectangle, -1 )	 
{
	
}

void BuildMenu_security::addButtons()
{
    addBuildButton(B_PREFECT);
}

BuildMenu_security::BuildMenu_security( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_education::addButtons()
{
    addBuildButton(B_SCHOOL);
    addBuildButton(B_LIBRARY);
    addBuildButton(B_COLLEGE);
}

BuildMenu_education::BuildMenu_education( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_health::addButtons()
{
    addBuildButton(B_DOCTOR);
    addBuildButton(B_BARBER);
    addBuildButton(B_BATHS);
    addBuildButton(B_HOSPITAL);
}

BuildMenu_health::BuildMenu_health( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_engineering::addButtons()
{
    addBuildButton(B_ENGINEER);
    addBuildButton(B_LOW_BRIDGE);
    addBuildButton(B_HIGH_BRIDGE);
    addBuildButton(B_DOCK);
    addBuildButton(B_SHIPYARD);
    addBuildButton(B_WHARF);
    addBuildButton(B_TRIUMPHAL_ARCH);
}

BuildMenu_engineering::BuildMenu_engineering( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_administration::addButtons()
{
    addBuildButton(B_FORUM);
    addBuildButton(B_SENATE);

    addBuildButton(B_GOVERNOR_HOUSE);
    addBuildButton(B_GOVERNOR_VILLA);
    addBuildButton(B_GOVERNOR_PALACE);
}

BuildMenu_administration::BuildMenu_administration( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_entertainment::addButtons()
{
    addBuildButton(B_THEATER);
    addBuildButton(B_AMPHITHEATER);
    addBuildButton(B_COLLOSSEUM);
    addBuildButton(B_HIPPODROME);
    addBuildButton(B_ACTOR);
    addBuildButton(B_GLADIATOR);
    addBuildButton(B_LION);
    addBuildButton(B_CHARIOT);
}

BuildMenu_entertainment::BuildMenu_entertainment( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_commerce::addButtons()
{
    addSubmenuButton(BM_FARM, _("Farm") );
    addSubmenuButton(BM_RAW_MATERIAL, _("Raw materials") );
    addSubmenuButton(BM_FACTORY, _("Factory") );

    addBuildButton(B_MARKET);
    addBuildButton(B_GRANARY);
    addBuildButton(B_WAREHOUSE);
}

BuildMenu_commerce::BuildMenu_commerce( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_farm::addButtons()
{
    addBuildButton(B_WHEAT);
    addBuildButton(B_FRUIT);
    addBuildButton(B_OLIVE);
    addBuildButton(B_GRAPE);
    addBuildButton(B_MEAT);
    addBuildButton(B_VEGETABLE);
}

BuildMenu_farm::BuildMenu_farm( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_raw_factory::addButtons()
{
    addBuildButton(B_MARBLE);
    addBuildButton(B_IRON);
    addBuildButton(B_TIMBER);
    addBuildButton(B_CLAY);
}

BuildMenu_raw_factory::BuildMenu_raw_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_factory::addButtons()
{
    addBuildButton(B_WINE);
    addBuildButton(B_OIL);
    addBuildButton(B_WEAPON);
    addBuildButton(B_FURNITURE);
    addBuildButton(B_POTTERY);
}

BuildMenu_factory::BuildMenu_factory( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_religion::addButtons()
{
    addSubmenuButton(BM_TEMPLE , _("Small temples") );   
    addSubmenuButton(BM_BIGTEMPLE , _("Large temples") );
    addBuildButton(B_TEMPLE_ORACLE);   
}

BuildMenu_religion::BuildMenu_religion( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}

void BuildMenu_temple::addButtons()
{
    addBuildButton(B_TEMPLE_CERES);
    addBuildButton(B_TEMPLE_NEPTUNE);
    addBuildButton(B_TEMPLE_MARS);
    addBuildButton(B_TEMPLE_VENUS);
    addBuildButton(B_TEMPLE_MERCURE);
}

BuildMenu_temple::BuildMenu_temple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}
void BuildMenu_bigtemple::addButtons()
{
    addBuildButton(B_BIG_TEMPLE_CERES);
    addBuildButton(B_BIG_TEMPLE_NEPTUNE);
    addBuildButton(B_BIG_TEMPLE_MARS);
    addBuildButton(B_BIG_TEMPLE_VENUS);
    addBuildButton(B_BIG_TEMPLE_MERCURE);
}

BuildMenu_bigtemple::BuildMenu_bigtemple( Widget* parent, const Rect& rectangle )
: BuildMenu( parent, rectangle, -1 )	 
{

}