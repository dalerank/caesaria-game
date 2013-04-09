#include "oc3_buildmenu.h"
#include "sdl_facade.hpp"

#include <iostream>
#include <cmath>
#include <iterator>
#include <string>
#include <iosfwd>

#include "oc3_pushbutton.h"
#include "gui_paneling.hpp"

#include "building_data.hpp"

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

        // draw cost
        if (_cost >= 0)
        {
            Widget::setText( _originalText + itoa( _cost, 0, 10 ) );            
        }
    }

    void setCost(const int cost)
    {
        _cost = cost;
    }

    int getCost() const
    {
        return _cost;
    }

    void setCostX(const int x)
    {
        _costX = x;
    }

    // cost=-1 => no cost display
    virtual void init_pictures()
    {
        // draw button text
        
    }

    void setText(const std::string &text)
    {
        _originalText = text;
        PushButton::setText( _originalText + itoa( _cost, 0, 10 ) );
    }

private:
    std::string _originalText;
    int _cost;   // cost of the building
    int _costX;  // position of the "cost" text
};

BuildMenu::BuildMenu( Widget* parent, const Rect& rectangle, int id ) : Widget( parent, id, rectangle )
{
    _isDeleted = false;
    _hoverButton = NULL;
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
        Widget* widget = *itWidget;
        if( BuildButton *button = dynamic_cast< BuildButton* >( widget ) )
        {
            sdlFacade.getTextSize(font, button->getText(), text_width, text_height);
            max_text_width = std::max(max_text_width, text_width);

            sdlFacade.getTextSize(font, itoa( button->getCost(), 0, 10 ), text_width, text_height);
            max_cost_width = std::max(max_cost_width, text_width);
        }
    }

    setWidth( std::max(150, max_text_width + max_cost_width + 20) );

    // set the same size for all buttons
    for (Widget::ConstChildIterator itWidget = getChildren().begin(); itWidget != getChildren().end(); ++itWidget)
    {
        Widget *widget = *itWidget;
        if( BuildButton *button = dynamic_cast< BuildButton* >( widget ) )
        {
            button->setWidth( getWidth() );
            button->setCostX( getWidth() - max_cost_width-10);
        }
    }

}


BuildMenu::~BuildMenu()
{
}


bool BuildMenu::isDeleted() const
{
    return _isDeleted;
}

void BuildMenu::setDeleted()
{
    _isDeleted = true;
}


void BuildMenu::addSubmenuButton(const BuildMenuType menuType, const std::string &text)
{
    BuildButton* button = new BuildButton( this, text, Rect( Point( 0, getHeight() ), Size( getWidth(), 25 ) ), -1 );
    //button->setEvent(WidgetEvent::BuildMenuEvent(menuType));
    button->setCost(-1);  // no display

    setHeight( getHeight() + 30 );
}


void BuildMenu::addBuildButton(const BuildingType buildingType)
{
    BuildingData &buildingData = BuildingDataHolder::instance().getData(buildingType);

    int cost = buildingData.getCost();
    if (cost != -1)
    {
        // building can be built
        BuildButton* button = new BuildButton( this, buildingData.getPrettyName(), Rect( 0, getHeight(), getWidth(), getHeight() + 25 ), -1 );
        //button->setEvent(WidgetEvent::BuildingEvent(buildingType));
        button->setCost(cost);

        setHeight( getHeight() + 30 );
    }
}

BuildMenu *BuildMenu::getMenuInstance(const BuildMenuType menuType, Widget* parent )
{
    BuildMenu *res = NULL;

    switch (menuType)
    {
    case BM_WATER:          res = new BuildMenu_water( parent, Rect( 0, 0, 60, 120 ) ); break;
    /*case BM_HEALTH:         res = new BuildMenu_health(); break;
    case BM_SECURITY:       res = new BuildMenu_security(); break;
    case BM_EDUCATION:      res = new BuildMenu_education(); break;
    case BM_ENGINEERING:    res = new BuildMenu_engineering(); break;
    case BM_ADMINISTRATION: res = new BuildMenu_administration(); break;
    case BM_ENTERTAINMENT:  res = new BuildMenu_entertainment(); break;
    case BM_COMMERCE:       res = new BuildMenu_commerce(); break;
    case BM_FARM:           res = new BuildMenu_farm(); break;
    case BM_RAW_MATERIAL:   res = new BuildMenu_raw_factory(); break;
    case BM_FACTORY:        res = new BuildMenu_factory(); break;
    case BM_RELIGION:       res = new BuildMenu_religion(); break;
    case BM_TEMPLE:         res = new BuildMenu_temple(); break;
    case BM_BIGTEMPLE:      res = new BuildMenu_bigtemple(); break;*/
    default:       break; // DO NOTHING 
    };

    return res;
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


void BuildMenu_education::addButtons()
{
    addBuildButton(B_SCHOOL);
    addBuildButton(B_LIBRARY);
    addBuildButton(B_COLLEGE);
}


void BuildMenu_health::addButtons()
{
    addBuildButton(B_DOCTOR);
    addBuildButton(B_BARBER);
    addBuildButton(B_BATHS);
    addBuildButton(B_HOSPITAL);
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


void BuildMenu_administration::addButtons()
{
    addBuildButton(B_FORUM);
    addBuildButton(B_SENATE);

    addBuildButton(B_GOVERNOR_HOUSE);
    addBuildButton(B_GOVERNOR_VILLA);
    addBuildButton(B_GOVERNOR_PALACE);
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


void BuildMenu_commerce::addButtons()
{
    addSubmenuButton(BM_FARM, "##Farm##" );
    addSubmenuButton(BM_RAW_MATERIAL, "##Raw materials##" );
    addSubmenuButton(BM_FACTORY, "##Factory##" );

    addBuildButton(B_MARKET);
    addBuildButton(B_GRANARY);
    addBuildButton(B_WAREHOUSE);
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


void BuildMenu_raw_factory::addButtons()
{
    addBuildButton(B_MARBLE);
    addBuildButton(B_IRON);
    addBuildButton(B_TIMBER);
    addBuildButton(B_CLAY);
}


void BuildMenu_factory::addButtons()
{
    addBuildButton(B_WINE);
    addBuildButton(B_OIL);
    addBuildButton(B_WEAPON);
    addBuildButton(B_FURNITURE);
    addBuildButton(B_POTTERY);
}

void BuildMenu_religion::addButtons()
{
    addSubmenuButton(BM_TEMPLE , "##Small temples##" );   
    addSubmenuButton(BM_BIGTEMPLE , "##Large temples##" );
    addBuildButton(B_TEMPLE_ORACLE);   
}

void BuildMenu_temple::addButtons()
{

    addBuildButton(B_TEMPLE_CERES);
    addBuildButton(B_TEMPLE_NEPTUNE);
    addBuildButton(B_TEMPLE_MARS);
    addBuildButton(B_TEMPLE_VENUS);
    addBuildButton(B_TEMPLE_MERCURE);
}

void BuildMenu_bigtemple::addButtons()
{
    addBuildButton(B_BIG_TEMPLE_CERES);
    addBuildButton(B_BIG_TEMPLE_NEPTUNE);
    addBuildButton(B_BIG_TEMPLE_MARS);
    addBuildButton(B_BIG_TEMPLE_VENUS);
    addBuildButton(B_BIG_TEMPLE_MERCURE);
}
