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

static const int REMOVE_TOOL_ID = B_MAX + 1; 

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
};

Signal1< int >& Menu::onCreateConstruction()
{
    return _d->onCreateConstructionSignal;
}

Signal0<>& Menu::onRemoveTool()
{
    return _d->onRemoveToolSignal;
}

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
    _d->minimizeButton = new PushButton( this, Rect( 0, 0, 31, 20) );
    GuiPaneling::configureTexturedButton( _d->minimizeButton, ResourceGroup::panelBackground, ResourceMenu::maximazeBtnPicId, false );
    _d->minimizeButton->setPosition( Point( 6, 4 ));

    _d->houseButton = new PushButton( this, Rect( 0, 0, 39, 26), "", B_HOUSE );
    GuiPaneling::configureTexturedButton( _d->houseButton, ResourceGroup::panelBackground, ResourceMenu::houseBtnPicId, true );
    _d->houseButton->setPosition( offset + Point( 0, dy * 0 ) );

    _d->clearButton = new PushButton( this, Rect( 0, 0, 39, 26), "", REMOVE_TOOL_ID );
    GuiPaneling::configureTexturedButton(_d->clearButton, ResourceGroup::panelBackground, 131, true );
    _d->clearButton->setPosition( offset + Point( 0, dy * 1 ) );

    _d->roadButton = new PushButton( this, Rect( 0, 0, 39, 26), "", B_ROAD );
    GuiPaneling::configureTexturedButton(_d->roadButton, ResourceGroup::panelBackground, 135, true );
    _d->roadButton->setPosition( offset + Point( 0, dy * 2 ) );

    _d->waterButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_WATER | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->waterButton,  ResourceGroup::panelBackground, 127, true );
    _d->waterButton->setPosition( offset + Point( 0, dy * 3 ));

    _d->healthButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_HEALTH | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->healthButton, ResourceGroup::panelBackground, 163, true );
    _d->healthButton->setPosition( offset + Point( 0, dy * 4 ) );

    _d->templeButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_RELIGION | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->templeButton, ResourceGroup::panelBackground, 151, true);
    _d->templeButton->setPosition( offset + Point( 0, dy * 5 ) );

    _d->educationButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_EDUCATION | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->educationButton, ResourceGroup::panelBackground, 147, true );
    _d->educationButton->setPosition( offset + Point( 0, dy * 6 ) );

    _d->entertainmentButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_ENTERTAINMENT | BuildMenu::subMenuCreateIdHigh );
    GuiPaneling::configureTexturedButton(_d->entertainmentButton, ResourceGroup::panelBackground, 143, true );
    _d->entertainmentButton->setPosition( offset + Point( 0, dy * 7 ) );

    _d->administrationButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_ADMINISTRATION | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->administrationButton, ResourceGroup::panelBackground, 139, true );
    _d->administrationButton->setPosition( offset + Point( 0, dy * 8 ) );

    _d->engineerButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_ENGINEERING | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->engineerButton, ResourceGroup::panelBackground, 167, true );
    _d->engineerButton->setPosition( offset + Point( 0, dy * 9 ) );

    _d->securityButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_SECURITY | BuildMenu::subMenuCreateIdHigh);
    GuiPaneling::configureTexturedButton(_d->securityButton, ResourceGroup::panelBackground, 159, true );
    _d->securityButton->setPosition( offset + Point( 0, dy * 10 ) );

    _d->commerceButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_COMMERCE | BuildMenu::subMenuCreateIdHigh );
    GuiPaneling::configureTexturedButton(_d->commerceButton, ResourceGroup::panelBackground, 155, true );
    _d->commerceButton->setPosition( offset + Point( 0, dy * 11 ) );
    // //
    // _midIcon.setPicture(PicLoader::instance().get_picture("panelwindows", 1));
    // _midIcon.setPosition(8, 217);
    // add_widget(_midIcon);

    //_bottomIcon.setPicture(PicLoader::instance().get_picture("paneling", 20));
    //_bottomIcon.setPosition(0, _bgPicture->get_surface()->h);
    //add_widget(_bottomIcon);

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
    // _minimizeButton.setPosition(127, 5);
    // header
    // _senateButton.setPosition(7, 155);
    // _empireButton.setPosition(84, 155);
    // _missionButton.setPosition(7, 184);
    // _northButton.setPosition(46, 184);
    // _rotateLeftButton.setPosition(84, 184);
    // _rotateRightButton.setPosition(123, 184);



    //   // 5th row
    //   _cancelButton.setPosition(x0, y0+dy*12);
    //   _messageButton.setPosition(x0, y0+dy*13);
    //   _disasterButton.setPosition(x0, y0+dy*14);

    //   _bgPicture = &PicLoader::instance().get_picture("paneling", 17);
    //   setSize(_bgPicture->get_surface()->w, _bgPicture->get_surface()->h);
    //
    //   // top of menu
    //   _menuButton.setText("Menu");
    //   _menuButton.setEvent(WidgetEvent::InGameMenuEvent());
    //   _menuButton.setNormalPicture(PicLoader::instance().get_picture("paneling", 234));
    //   _menuButton.setHoverPicture(PicLoader::instance().get_picture("paneling", 234+1));
    //   _menuButton.setSelectedPicture(PicLoader::instance().get_picture("paneling", 234+2));
    //   _menuButton.setUnavailablePicture(PicLoader::instance().get_picture("paneling", 234+3));
    //   _menuButton.init_pictures();
    //   add_widget(_menuButton);
    //
    //   set4Button(_minimizeButton, WidgetEvent(), 97);
    //
    //   //
    //   _midIcon.setPicture(PicLoader::instance().get_picture("panelwindows", 1));
    //   _midIcon.setPosition(8, 217);
    //   add_widget(_midIcon);
    //
    //   //_bottomIcon.setPicture(PicLoader::instance().get_picture("paneling", 20));
    //   //_bottomIcon.setPosition(0, _bgPicture->get_surface()->h);
    //   //add_widget(_bottomIcon);
    //
    //   // header
    //   set3Button(_senateButton, WidgetEvent(), 79);
    //   set3Button(_empireButton, WidgetEvent(), 82);
    //   set3Button(_missionButton, WidgetEvent(), 85);
    //   set3Button(_northButton, WidgetEvent(), 88);
    //   set3Button(_rotateLeftButton, WidgetEvent(), 91);
    //   set3Button(_rotateRightButton, WidgetEvent(), 94);
    //
    //   set4Button(_houseButton, WidgetEvent::BuildingEvent(B_HOUSE), 123);
    //   set4Button(_clearButton, WidgetEvent::ClearLandEvent(), 131);
    //   set4Button(_roadButton, WidgetEvent::BuildingEvent(B_ROAD), 135);
    //   // second row
    //   set4Button(_waterButton, WidgetEvent::BuildMenuEvent(BM_WATER), 127);
    //   set4Button(_healthButton, WidgetEvent::BuildMenuEvent(BM_HEALTH), 163);
    //   set4Button(_templeButton, WidgetEvent::BuildMenuEvent(BM_TEMPLE), 151);
    //   // third row
    //   set4Button(_educationButton, WidgetEvent::BuildMenuEvent(BM_EDUCATION), 147);
    //   set4Button(_entertainmentButton, WidgetEvent::BuildMenuEvent(BM_ENTERTAINMENT), 143);
    //   set4Button(_administrationButton, WidgetEvent::BuildMenuEvent(BM_ADMINISTRATION), 139);
    //   // 4th row
    //   set4Button(_engineerButton, WidgetEvent::BuildMenuEvent(BM_ENGINEERING), 167);
    //   set4Button(_securityButton, WidgetEvent::BuildMenuEvent(BM_SECURITY), 159);
    //   set4Button(_commerceButton, WidgetEvent::BuildMenuEvent(BM_COMMERCE), 155);
    //   // 5th row
    //   set4Button(_cancelButton, WidgetEvent(), 171);
    //   set4Button(_messageButton, WidgetEvent(), 115);
    //   set4Button(_disasterButton, WidgetEvent(), 119);
    //
    //   // set button position
    //   _menuButton.setPosition(4, 3);
    //   _minimizeButton.setPosition(127, 5);
    //   // header
    //   _senateButton.setPosition(7, 155);
    //   _empireButton.setPosition(84, 155);
    //   _missionButton.setPosition(7, 184);
    //   _northButton.setPosition(46, 184);
    //   _rotateLeftButton.setPosition(84, 184);
    //   _rotateRightButton.setPosition(123, 184);
    //   // first row
    //   _houseButton.setPosition(13, 277);
    //   _clearButton.setPosition(63, 277);
    //   _roadButton.setPosition(113, 277);
    //   // second row
    //   _waterButton.setPosition(13, 313);
    //   _healthButton.setPosition(63, 313);
    //   _templeButton.setPosition(113, 313);
    //   // third row
    //   _educationButton.setPosition(13, 349);
    //   _entertainmentButton.setPosition(63, 349);
    //   _administrationButton.setPosition(113, 349);
    //   // 4th row
    //   _engineerButton.setPosition(13, 385);
    //   _securityButton.setPosition(63, 385);
    //   _commerceButton.setPosition(113, 385);
    //   // 5th row
    //   _cancelButton.setPosition(13, 421);
    //   _messageButton.setPosition(63, 421);
    //   _disasterButton.setPosition(113, 421);
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
