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


#include "oc3_menu.h"
#include "oc3_pushbutton.h"
#include "picture.hpp"
#include "sdl_facade.hpp"
#include "pic_loader.hpp"
#include "oc3_resourcegroup.h"
#include "oc3_event.h"
#include "oc3_buildmenu.h"
#include "oc3_guienv.h"
#include "gui_paneling.hpp"
#include "oc3_widgetpositionanimator.h"
#include "oc3_label.h"

static const int REMOVE_TOOL_ID = B_MAX + 1; 
static const int MAXIMIZE_ID = REMOVE_TOOL_ID + 1;

class Menu::Impl
{
public:
    Picture* bgPicture;

    Widget* lastPressed;
    PushButton* menuButton;
    PushButton* minimizeButton;
    PushButton* senateButton;
    PushButton* empireButton;
    PushButton* missionButton;
    PushButton* northButton;
    PushButton* rotateLeftButton;
    PushButton* rotateRightButton;
    PushButton* messageButton;
    PushButton* disasterButton;
    PushButton* houseButton;
    PushButton* waterButton;
    PushButton* clearButton;
    PushButton* roadButton;
    PushButton* administrationButton;
    PushButton* entertainmentButton;
    PushButton* educationButton;
    PushButton* templeButton;
    PushButton* commerceButton;
    PushButton* securityButton;
    PushButton* healthButton;
    PushButton* engineerButton;
    PushButton* cancelButton;
    Label* middleLabel;

oc3_signals public:
    Signal1< int > onCreateConstructionSignal;
    Signal0<> onRemoveToolSignal;
    Signal0<> onMaximizeSignal;
};

Signal1< int >& Menu::onCreateConstruction()
{
    return _d->onCreateConstructionSignal;
}

Signal0<>& Menu::onRemoveTool()
{
    return _d->onRemoveToolSignal;
}

class MenuButton : public PushButton
{
public:
    MenuButton( Widget* parent, const Rect& rectangle, const std::string& caption, int id, int midIconId )
        : PushButton( parent, rectangle, caption, id )
    {
        _midIconId = midIconId;
    }

    int getMidPicId() const { return _midIconId; }
    void setMidPicId( int id ) { _midIconId = id; }
private:
    int _midIconId;
};

Menu::Menu( Widget* parent, int id, const Rect& rectangle ) : Widget( parent, id, rectangle ), _d( new Impl )
{
    _d->lastPressed = 0;
    // // top of menu
    //_menuButton.setText("Menu");
    //_menuButton.setEvent(WidgetEvent::InGameMenuEvent());
    //_menuButton.setNormalPicture(PicLoader::instance().get_picture("paneling", 234));
    //_menuButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 234+1));
    //_menuButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 234+2));
    //_menuButton.setUnavailablePicture(PicLoader::instance().get_picture("paneling", 234+3));
    //_menuButton.init_pictures();
    //add_widget(_menuButton);

    const bool haveSubMenu = true;
    _d->minimizeButton = _addButton( ResourceMenu::maximizeBtnPicId, false, 0, MAXIMIZE_ID, !haveSubMenu, ResourceMenu::emptyMidPicId );
    _d->minimizeButton->setGeometry( Rect( Point( 6, 4 ), Size( 31, 20 ) ) );

    _d->houseButton = _addButton( ResourceMenu::houseBtnPicId, true, 0, B_HOUSE, !haveSubMenu, ResourceMenu::houseMidPicId );
    _d->clearButton = _addButton( 131, true, 1, REMOVE_TOOL_ID, !haveSubMenu, ResourceMenu::clearMidPicId );
    _d->roadButton = _addButton( 135, true, 2, B_ROAD, !haveSubMenu, ResourceMenu::roadMidPicId );
    _d->waterButton = _addButton( 127, true, 3, BM_WATER, haveSubMenu, ResourceMenu::waterMidPicId );
    _d->healthButton = _addButton( 163, true, 4, BM_HEALTH, haveSubMenu, ResourceMenu::healthMidPicId );
    _d->templeButton = _addButton( 151, true, 5, BM_RELIGION, haveSubMenu, ResourceMenu::religionMidPicId );
    _d->educationButton = _addButton( 147, true, 6, BM_EDUCATION, haveSubMenu, ResourceMenu::educationMidPicId );
    _d->entertainmentButton = _addButton( 143, true, 7, BM_ENTERTAINMENT, haveSubMenu, ResourceMenu::entertainmentMidPicId );
    _d->administrationButton = _addButton( 139, true, 8, BM_ADMINISTRATION, haveSubMenu, ResourceMenu::administrationMidPicId );
    _d->engineerButton = _addButton( 167, true, 9, BM_ENGINEERING, haveSubMenu, ResourceMenu::engineerMidPicId );
    _d->securityButton = _addButton( 159, true, 10, BM_SECURITY, haveSubMenu, ResourceMenu::securityMidPicId );
    _d->commerceButton = _addButton( 155, true, 11, BM_COMMERCE, haveSubMenu, ResourceMenu::comerceMidPicId );
}

PushButton* Menu::_addButton( int startPic, bool pushBtn, int yMul, int id, bool haveSubmenu, int midPic )
{
    Point offset( 1, 32 );
    int dy = 35;

    MenuButton* ret = new MenuButton( this, Rect( 0, 0, 39, 26), "", -1, -1 );
    ret->setID( id | ( haveSubmenu ? BuildMenu::subMenuCreateIdHigh : 0 ) );
    GuiPaneling::configureTexturedButton( ret, ResourceGroup::panelBackground, startPic, pushBtn );
    ret->setPosition( offset + Point( 0, dy * yMul ) );

    if( MenuButton* btn = safety_cast< MenuButton* >( ret ) )
    {
        btn->setMidPicId( midPic );
    }

    return ret;
}

void Menu::draw( GfxEngine& painter )
{
    if( !isVisible() )
        return;

    painter.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );
    
    Widget::draw( painter );
}

bool Menu::onEvent(const NEvent& event)
{
    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
    {
        if( !event.GuiEvent.Caller )
            return false;

        int id = event.GuiEvent.Caller->getID();
        switch( id )
        {
        case MAXIMIZE_ID:
            _d->lastPressed = 0;
            _createBuildMenu( -1, this );
            _d->onMaximizeSignal.emit();
        break;

        case B_HOUSE:
        case B_ROAD:
            _d->lastPressed = event.GuiEvent.Caller;
            _d->onCreateConstructionSignal.emit( id );
            _createBuildMenu( -1, this );
        break;

        case REMOVE_TOOL_ID:
            _d->lastPressed = event.GuiEvent.Caller;
            _d->onRemoveToolSignal.emit();
            _createBuildMenu( -1, this );
        break;
        
        default:
            if( _d->lastPressed != event.GuiEvent.Caller )
            {
                if( event.GuiEvent.Caller->getParent() == this )
                    _d->lastPressed = event.GuiEvent.Caller;
                
                if( PushButton* btn = safety_cast< PushButton* >( event.GuiEvent.Caller ) )
                {
                    int id = btn->getID();
                    if( id & BuildMenu::subMenuCreateIdHigh )
                    {
                        _createBuildMenu( id & 0xff, event.GuiEvent.Caller );        
                    }
                    else
                    {
                        _d->onCreateConstructionSignal.emit( id );
                        _createBuildMenu( -1, this );
                        setFocus();
                        //_d->lastPressed = 0;
                    }
                }
            }

        break;
        }

        unselectAll();
        if( PushButton* btn = safety_cast< PushButton* >( _d->lastPressed ) )
        {
            btn->setPressed( true && btn->isPushButton() );
        }
        return true;
    }

    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_ELEMENT_FOCUS_LOST )
    {
        unselectAll();
        _d->lastPressed = 0;
    }

    if( event.EventType == OC3_MOUSE_EVENT )
    {
        switch( event.MouseEvent.Event )
        {
        case OC3_RMOUSE_LEFT_UP:
            _createBuildMenu( -1, this );
            unselectAll();
            _d->lastPressed = 0;
        return true;

        case OC3_LMOUSE_PRESSED_DOWN:
        case OC3_LMOUSE_LEFT_UP:
            {
                //lock movement for tilemap
                if( findChildren<BuildMenu*>().size() > 0 )
                    return true;
            }
        break;
        }
    }

    return Widget::onEvent( event );
}

Menu* Menu::create( Widget* parent, int id )
{
	Menu* ret = new Menu( parent, id, Rect( 0, 0, 1, 1 ) );

    SdlFacade &sdlFacade = SdlFacade::instance();

    const Picture& bground = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 16 );
    const Picture& bottom = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 21 );

    ret->_d->bgPicture = &sdlFacade.createPicture( bground.get_width(), bground.get_height() + bottom.get_height() );
    sdlFacade.drawPicture( bground, *ret->_d->bgPicture , 0, 0);
    sdlFacade.drawPicture( bottom, *ret->_d->bgPicture , 0, bground.get_height() );

    ret->setGeometry( Rect( 0, 0, bground.get_width(), ret->_d->bgPicture->get_height() ) );

	return ret;
}

bool Menu::unselectAll()
{
    bool anyPressed = false;
    for( ConstChildIterator it=getChildren().begin(); it != getChildren().end(); it++ )
    {
        if( PushButton* btn = safety_cast< PushButton* >( *it ) )
        {
            anyPressed |= btn->isPressed();
            btn->setPressed( false );
        }
    }

    return anyPressed;
}

void Menu::_createBuildMenu( int type, Widget* parent )
{
    List< BuildMenu* > menus = findChildren<BuildMenu*>();
    for( List< BuildMenu* >::iterator it=menus.begin(); it != menus.end(); it++ )
        (*it)->deleteLater();

    BuildMenu* buildMenu = BuildMenu::getMenuInstance( (BuildMenuType)type, this );

    if( buildMenu != NULL )
    {
        buildMenu->setNotClipped( true );

        buildMenu->init();
       
        int y = math::clamp< int >( parent->getScreenTop() - getScreenTop(), 0, _environment->getRootWidget()->getHeight() - buildMenu->getHeight() );
        buildMenu->setPosition( Point( -(int)buildMenu->getWidth() - 5, y ) );
    }
}

Signal0<>& Menu::onMaximize()
{
    return _d->onMaximizeSignal;
}

ExtentMenu* ExtentMenu::create( Widget* parent, int id )
{
    ExtentMenu* ret = new ExtentMenu( parent, id, Rect( 0, 0, 1, 1 ) );

    SdlFacade &sdlFacade = SdlFacade::instance();

    const Picture& bground = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 17 );
    const Picture& bottom = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 20 );

    ret->_d->bgPicture = &sdlFacade.createPicture( bground.get_width(), bground.get_height() + bottom.get_height() );
    sdlFacade.drawPicture( bground, *ret->_d->bgPicture, 0, 0);
    sdlFacade.drawPicture( bottom, *ret->_d->bgPicture, 0, bground.get_height() );

    ret->setGeometry( Rect( 0, 0, bground.get_width(), ret->_d->bgPicture->get_height() ) );

    return ret;
}

void ExtentMenu::minimize()
{
    _d->lastPressed = 0;
    _createBuildMenu( -1, this );
    Point stopPos = getRelativeRect().UpperLeftCorner + Point( getWidth(), 0 );
    PositionAnimator* anim = new PositionAnimator( this, WidgetAnimator::removeSelf, 
                                                   stopPos, 300 );
}

void ExtentMenu::maximize()
{
    Point stopPos = getRelativeRect().UpperLeftCorner - Point( getWidth(), 0 );
    show();
    PositionAnimator* anim = new PositionAnimator( this, WidgetAnimator::showParent | WidgetAnimator::removeSelf, 
                                                   stopPos, 300 );
}

ExtentMenu::ExtentMenu( Widget* parent, int id, const Rect& rectangle )
    : Menu( parent, id, rectangle )
{
    GuiPaneling::configureTexturedButton( _d->minimizeButton, ResourceGroup::panelBackground, 97, false );
    _d->minimizeButton->setPosition( Point( 127, 5 ) );
    CONNECT( _d->minimizeButton, onClicked(), this, ExtentMenu::minimize );

    _d->houseButton->setPosition( Point( 13, 277 ) );
    _d->houseButton->setIsPushButton( false );
    _d->clearButton->setPosition( Point( 63, 277 ) );
    _d->clearButton->setIsPushButton( false );
    _d->roadButton->setPosition( Point( 113, 277 ) );
    _d->roadButton->setIsPushButton( false );

    _d->waterButton->setPosition( Point( 13, 313 ) );
    _d->healthButton->setPosition( Point( 63, 313 ) );
    _d->templeButton->setPosition( Point( 113, 313 ) );
    _d->educationButton->setPosition( Point(13, 349 ));
    _d->entertainmentButton->setPosition( Point(63, 349 ) );
    _d->administrationButton->setPosition( Point( 113, 349) );
    _d->engineerButton->setPosition( Point( 13, 385 ) );
    _d->securityButton->setPosition( Point( 63, 385 ) );
    _d->commerceButton->setPosition( Point( 113, 385) );

    // // header
    _d->senateButton = _addButton( 79, false, 0, -1, false, -1 );
    _d->senateButton->setGeometry( Rect( Point( 7, 155 ), Size( 71, 23 ) ) );
    _d->empireButton = _addButton( 82, false, 0, -1, false, -1 );
    _d->empireButton->setGeometry( Rect( Point( 84, 155 ), Size( 71, 23 ) ) );
   
    _d->missionButton = _addButton( 85, false, 0, -1, false, -1 );
    _d->missionButton->setGeometry( Rect( Point( 7, 184 ), Size( 33, 22 ) ) );
    _d->northButton = _addButton( 88, false, 0, -1, false, -1 );
    _d->northButton->setGeometry( Rect( Point( 46, 184 ), Size( 33, 22 ) ) );
    _d->rotateLeftButton = _addButton( 91, false, 0, -1, false, -1 );
    _d->rotateLeftButton->setGeometry( Rect( Point( 84, 184 ), Size( 33, 22 ) ) );
    _d->rotateRightButton = _addButton( 94, false, 0, -1, false, -1 );
    _d->rotateRightButton->setGeometry( Rect( Point( 123, 184 ), Size( 33, 22 ) ) );

    _d->cancelButton = _addButton( 171, false, 0, -1, false, -1 );
    _d->cancelButton->setGeometry( Rect( Point( 13, 421 ), Size( 39, 22 ) ) );
    _d->messageButton = _addButton( 115, false, 0, -1, false, -1 );
    _d->messageButton->setGeometry( Rect( Point( 63, 421 ), Size( 39, 22 ) ) );
    _d->disasterButton = _addButton( 119, false, 0, -1, false, -1 );
    _d->disasterButton->setGeometry( Rect( Point( 113, 421 ), Size( 39, 22 ) ) );

    _d->middleLabel = new Label(this, Rect( Point( 7, 216 ), Size( 148, 52 )) );
    _d->middleLabel->setBackgroundPicture( PicLoader::instance().get_picture( ResourceGroup::menuMiddleIcons, ResourceMenu::emptyMidPicId ) );
}

bool ExtentMenu::onEvent(const NEvent& event)
{
    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
    {
        if( MenuButton* btn = safety_cast< MenuButton* >( event.GuiEvent.Caller ) )
        {
            int picId = btn->getMidPicId() > 0 ? btn->getMidPicId() : ResourceMenu::emptyMidPicId;
            _d->middleLabel->setBackgroundPicture( PicLoader::instance().get_picture( ResourceGroup::menuMiddleIcons, picId ) );
        }
    }

    return Menu::onEvent( event );
}