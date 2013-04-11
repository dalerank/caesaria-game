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

    int getMidIconId() const { return _midIconId; }

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

    Point offset( 1, 32 );
    int dy = 35;
    _d->minimizeButton = new PushButton( this, Rect( 0, 0, 31, 20), "", MAXIMIZE_ID );
    GuiPaneling::configureTexturedButton( _d->minimizeButton, ResourceGroup::panelBackground, ResourceMenu::maximizeBtnPicId, false );
    _d->minimizeButton->setPosition( Point( 6, 4 ));

    _d->houseButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", B_HOUSE, ResourceMenu::houseMidPicId );
    GuiPaneling::configureTexturedButton( _d->houseButton, ResourceGroup::panelBackground, ResourceMenu::houseBtnPicId, true );
    _d->houseButton->setPosition( offset + Point( 0, dy * 0 ) );

    _d->clearButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                      REMOVE_TOOL_ID, ResourceMenu::clearMidPicId );
    GuiPaneling::configureTexturedButton(_d->clearButton, ResourceGroup::panelBackground, 131, true );
    _d->clearButton->setPosition( offset + Point( 0, dy * 1 ) );

    _d->roadButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                     B_ROAD, ResourceMenu::clearMidPicId  );
    GuiPaneling::configureTexturedButton(_d->roadButton, ResourceGroup::panelBackground, 135, true );
    _d->roadButton->setPosition( offset + Point( 0, dy * 2 ) );

    _d->waterButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                      BM_WATER | BuildMenu::subMenuCreateIdHigh, 
                                      ResourceMenu::waterMidPicId );
    GuiPaneling::configureTexturedButton(_d->waterButton,  ResourceGroup::panelBackground, 127, true );
    _d->waterButton->setPosition( offset + Point( 0, dy * 3 ));

    _d->healthButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                       BM_HEALTH | BuildMenu::subMenuCreateIdHigh, 
                                       ResourceMenu::healthMidPicId );
    GuiPaneling::configureTexturedButton(_d->healthButton, ResourceGroup::panelBackground, 163, true );
    _d->healthButton->setPosition( offset + Point( 0, dy * 4 ) );

    _d->templeButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                       BM_RELIGION | BuildMenu::subMenuCreateIdHigh, 
                                       ResourceMenu::religionMidPicId );
    GuiPaneling::configureTexturedButton(_d->templeButton, ResourceGroup::panelBackground, 151, true);
    _d->templeButton->setPosition( offset + Point( 0, dy * 5 ) );

    _d->educationButton = new MenuButton( this, Rect( 0, 0, 39, 26), "",
                                          BM_EDUCATION | BuildMenu::subMenuCreateIdHigh, 
                                          ResourceMenu::educationMidPicId );
    GuiPaneling::configureTexturedButton(_d->educationButton, ResourceGroup::panelBackground, 147, true );
    _d->educationButton->setPosition( offset + Point( 0, dy * 6 ) );

    _d->entertainmentButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                              BM_ENTERTAINMENT | BuildMenu::subMenuCreateIdHigh,
                                              ResourceMenu::entertainmentMidPicId );
    GuiPaneling::configureTexturedButton(_d->entertainmentButton, ResourceGroup::panelBackground, 143, true );
    _d->entertainmentButton->setPosition( offset + Point( 0, dy * 7 ) );

    _d->administrationButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                               BM_ADMINISTRATION | BuildMenu::subMenuCreateIdHigh, 
                                               ResourceMenu::administrationMidPicId );
    GuiPaneling::configureTexturedButton(_d->administrationButton, ResourceGroup::panelBackground, 139, true );
    _d->administrationButton->setPosition( offset + Point( 0, dy * 8 ) );

    _d->engineerButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                         BM_ENGINEERING | BuildMenu::subMenuCreateIdHigh, 
                                         ResourceMenu::engineerMidPicId );
    GuiPaneling::configureTexturedButton(_d->engineerButton, ResourceGroup::panelBackground, 167, true );
    _d->engineerButton->setPosition( offset + Point( 0, dy * 9 ) );

    _d->securityButton = new MenuButton( this, Rect( 0, 0, 39, 26), "", 
                                         BM_SECURITY | BuildMenu::subMenuCreateIdHigh,
                                         ResourceMenu::securityMidPicId );
    GuiPaneling::configureTexturedButton(_d->securityButton, ResourceGroup::panelBackground, 159, true );
    _d->securityButton->setPosition( offset + Point( 0, dy * 10 ) );

    _d->commerceButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_COMMERCE | BuildMenu::subMenuCreateIdHigh );
    GuiPaneling::configureTexturedButton(_d->commerceButton, ResourceGroup::panelBackground, 155, true );
    _d->commerceButton->setPosition( offset + Point( 0, dy * 11 ) );

    // // header
    // set3Button(_senateButton, WidgetEvent(), 79);
    // set3Button(_empireButton, WidgetEvent(), 82);
    // set3Button(_missionButton, WidgetEvent(), 85);
    // set3Button(_northButton, WidgetEvent(), 88);
    // set3Button(_rotateLeftButton, WidgetEvent(), 91);
    // set3Button(_rotateRightButton, WidgetEvent(), 94);

    //   // 5th row
    //   set4Button(_cancelButton, WidgetEvent(), 171);
    //   set4Button(_messageButton, WidgetEvent(), 115);
    //   set4Button(_disasterButton, WidgetEvent(), 119);

    // set button position
    // _menuButton.setPosition(4, 3);
    // header
    // _senateButton.setPosition(7, 155);
    // _empireButton.setPosition(84, 155);
    // _missionButton.setPosition(7, 184);
    // _northButton.setPosition(46, 184);
    // _rotateLeftButton.setPosition(84, 184);
    // _rotateRightButton.setPosition(123, 184);

    //   // 5th row
    //   _cancelButton.setPosition(13, 421);
    //   _messageButton.setPosition(63, 421);
    //   _disasterButton.setPosition(113, 421);
}

/*PushButton* Menu::addButton( int startPic, bool pushBtn, int yMul, int id, bool haveSubmenu, int midPic )
{
    MenuButton* ret = new MenuButton( this, Rect( 0, 0, 39, 26), "", -1, -1 );
    ret->setID( id | ( haveSubmenu ? BuildMenu::subMenuCreateIdHigh : 0 ) );
    GuiPaneling::configureTexturedButton( ret, ResourceGroup::panelBackground, startPic, pushBtn );
    _d->commerceButton->setPosition( offset + Point( 0, dy * yMul ) );
}*/

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
            unselectAll();
            _createBuildMenu( -1, this );
            _d->onMaximizeSignal.emit();
        break;

        case B_HOUSE:
        case B_ROAD:
            _d->onCreateConstructionSignal.emit( id );
            _createBuildMenu( -1, this );
        break;

        case REMOVE_TOOL_ID:
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
                        _d->lastPressed = 0;
                    }
                }
            }

            unselectAll();
            if( PushButton* btn = safety_cast< PushButton* >( _d->lastPressed ) )
                btn->setPressed( true );
        break;
        }
        
        return true;
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
    //   _cancelButton.setPosition(13, 421);
    //   _messageButton.setPosition(63, 421);
    //   _disasterButton.setPosition(113, 421);
}