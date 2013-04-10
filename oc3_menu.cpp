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
    Signal2< int, Widget* > onCreateBuildMenuSignal;
    Signal1< int > onCreateConstructionSignal;
    Signal0<> onRemoveToolSignal;
};

Signal2<int, Widget*>& Menu::onCreateBuildMenu()
{
    return _d->onCreateBuildMenuSignal;
}

Signal1< int >& Menu::onCreateConstruction()
{
    return _d->onCreateConstructionSignal;
}

Signal0<>& Menu::onRemoveTool()
{
    return _d->onRemoveToolSignal;
}

void configureButton(PushButton* oButton, const int pic_index, bool pushButton )
{
    PicLoader& loader = PicLoader::instance();
    oButton->setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index), stNormal );
    oButton->setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index+1), stHovered );
    oButton->setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index+2), stPressed );
    oButton->setPicture( &loader.get_picture( ResourceGroup::panelBackground, pic_index+3), stDisabled );
    oButton->setIsPushButton( pushButton );
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
    configureButton( _d->minimizeButton, ResourceMenu::maximazeBtnPicId, false );
    _d->minimizeButton->setPosition( Point( 6, 4 ));

    _d->houseButton = new PushButton( this, Rect( 0, 0, 39, 26), "", B_HOUSE );
    configureButton( _d->houseButton, ResourceMenu::houseBtnPicId, false );
    _d->houseButton->setPosition( offset + Point( 0, dy * 0 ) );

    _d->clearButton = new PushButton( this, Rect( 0, 0, 39, 26), "", REMOVE_TOOL_ID );
    configureButton(_d->clearButton, 131, false );
    _d->clearButton->setPosition( offset + Point( 0, dy * 1 ) );

    _d->roadButton = new PushButton( this, Rect( 0, 0, 39, 26), "", B_ROAD );
    configureButton(_d->roadButton, 135, false );
    _d->roadButton->setPosition( offset + Point( 0, dy * 2 ) );

    _d->waterButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_WATER );
    configureButton(_d->waterButton,  127, true );
    _d->waterButton->setPosition( offset + Point( 0, dy * 3 ));

    _d->healthButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_HEALTH );
    configureButton(_d->healthButton, 163, true );
    _d->healthButton->setPosition( offset + Point( 0, dy * 4 ) );

    _d->templeButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_RELIGION );
    configureButton(_d->templeButton, 151, true);
    _d->templeButton->setPosition( offset + Point( 0, dy * 5 ) );

    _d->educationButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_EDUCATION );
    configureButton(_d->educationButton, 147, true );
    _d->educationButton->setPosition( offset + Point( 0, dy * 6 ) );

    _d->entertainmentButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_ENTERTAINMENT );
    configureButton(_d->entertainmentButton, 143, true );
    _d->entertainmentButton->setPosition( offset + Point( 0, dy * 7 ) );

    _d->administrationButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_ADMINISTRATION );
    configureButton(_d->administrationButton, 139, true );
    _d->administrationButton->setPosition( offset + Point( 0, dy * 8 ) );

    _d->engineerButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_ENGINEERING );
    configureButton(_d->engineerButton, 167, true );
    _d->engineerButton->setPosition( offset + Point( 0, dy * 9 ) );

    _d->securityButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_SECURITY );
    configureButton(_d->securityButton, 159, true );
    _d->securityButton->setPosition( offset + Point( 0, dy * 10 ) );

    _d->commerceButton = new PushButton( this, Rect( 0, 0, 39, 26), "", BM_COMMERCE );
    configureButton(_d->commerceButton, 155, true );
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
            return true;

        int id = event.GuiEvent.Caller->getID();
        switch( id )
        {
        case B_HOUSE:
        case B_ROAD:
            _d->onCreateConstructionSignal.emit( id );
        break;

        case REMOVE_TOOL_ID:
            _d->onRemoveToolSignal.emit();
        break;
        
        default:
            if( _d->lastPressed != event.GuiEvent.Caller )
            {
                _d->lastPressed = event.GuiEvent.Caller;
                _d->onCreateBuildMenuSignal.emit( id, event.GuiEvent.Caller );                
            }

            unselectAll();
            if( PushButton* btn = safety_cast< PushButton* >( _d->lastPressed ) )
                btn->setPressed( true );
        break;
        }
        
        return true;
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