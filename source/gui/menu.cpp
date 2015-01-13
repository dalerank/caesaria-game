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

#include "gfx/layerconstants.hpp"
#include "menu.hpp"
#include "texturedbutton.hpp"
#include "gfx/picture.hpp"
#include "core/color.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "buildmenu.hpp"
#include "environment.hpp"
#include "gfx/decorator.hpp"
#include "widgetpositionanimator.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "game/minimap_colours.hpp"
#include "gfx/city_renderer.hpp"
#include "gfx/tile.hpp"
#include "gfx/engine.hpp"
#include "overlays_menu.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "city/city.hpp"
#include "events/playsound.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

namespace gui
{

static const int REMOVE_TOOL_ID = 0xff00 + 1;
static const int MAXIMIZE_ID = REMOVE_TOOL_ID + 1;

class Menu::Impl
{
public:
  Pictures background;

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
  PushButton* overlaysButton;
  Label* middleLabel;
  OverlaysMenu* overlaysMenu; 
  PlayerCityPtr city;


  void initActionButton( PushButton* btn, Point pos );
  void playSound();
  void updateBuildingOptions();

signals public:
  Signal1<int> onCreateConstructionSignal;
  Signal0<> onRemoveToolSignal;
  Signal0<> onHideSignal;
};

Signal1<int>& Menu::onCreateConstruction(){  return _d->onCreateConstructionSignal;}
Signal0<>& Menu::onRemoveTool(){  return _d->onRemoveToolSignal;}

class MenuButton : public TexturedButton
{
public:
  MenuButton( Widget* parent, const Point& pos, int id, int midIconId, int startPic, bool pushBtn )
    : TexturedButton( parent, pos, Size( 39, 26 ), id, startPic )
  {
    _midIconId = midIconId;
    setIsPushButton( pushBtn );
  }

  int midPicId() const { return _midIconId; }
  void setMidPicId( int id ) { _midIconId = id; }
private:
  int _midIconId;
};

Menu::Menu( Widget* parent, int id, const Rect& rectangle )
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  setupUI( ":/gui/shortmenu.gui" );
  _d->lastPressed = 0;
  _d->overlaysMenu = 0;

  const bool haveSubMenu = true;
  _d->minimizeButton = _addButton( ResourceMenu::maximizeBtn, false, 0, MAXIMIZE_ID,
                                   !haveSubMenu, ResourceMenu::emptyMidPicId, _("##show_bigpanel##") );

  _d->minimizeButton->setGeometry( Rect( Point( 6, 4 ), Size( 31, 20 ) ) );

  _d->houseButton = _addButton( ResourceMenu::houseBtnPicId, true, 0, objects::house,
                                !haveSubMenu, ResourceMenu::houseMidPicId, _("##build_housing##") );

  _d->clearButton = _addButton( 131, true, 1, REMOVE_TOOL_ID,
                                !haveSubMenu, ResourceMenu::clearMidPicId, _("##clear_land##") );

  _d->roadButton = _addButton( 135, true, 2, objects::road, !haveSubMenu, ResourceMenu::roadMidPicId, _("##build_road_tlp##") );
  _d->waterButton = _addButton( 127, true, 3, development::water, haveSubMenu, ResourceMenu::waterMidPicId, _("##water_build_tlp##") );
  _d->healthButton = _addButton( 163, true, 4, development::health, haveSubMenu, ResourceMenu::healthMidPicId, _("##healthBtnTooltip##") );
  _d->templeButton = _addButton( 151, true, 5, development::religion, haveSubMenu, ResourceMenu::religionMidPicId, _("##templeBtnTooltip##") );
  _d->educationButton = _addButton( 147, true, 6, development::education, haveSubMenu, ResourceMenu::educationMidPicId, _("##education_objects##") );

  _d->entertainmentButton = _addButton( 143, true, 7, development::entertainment, haveSubMenu,
                                        ResourceMenu::entertainmentMidPicId, _("##entertainment##") );

  _d->administrationButton = _addButton( 139, true, 8, development::administration, haveSubMenu,
                                         ResourceMenu::administrationMidPicId, _("##administration_building##") );

  _d->engineerButton = _addButton( 167, true, 9, development::engineering, haveSubMenu,
                                   ResourceMenu::engineerMidPicId, _("##engineering_structures##") );

  _d->securityButton = _addButton( 159, true, 10, development::security, haveSubMenu,
                                   ResourceMenu::securityMidPicId, _("##securityBtnTooltip##") );

  _d->commerceButton = _addButton( 155, true, 11, development::commerce, haveSubMenu,
                                   ResourceMenu::comerceMidPicId, _("##comerceBtnTooltip##") );

  CONNECT( _d->minimizeButton, onClicked(), this, Menu::minimize );
}

PushButton* Menu::_addButton( int startPic, bool pushBtn, int yMul, 
                             int id, bool haveSubmenu, int midPic, const std::string& tooltip )
{
  Point offset( 1, 32 );
  int dy = 35;

  MenuButton* ret = new MenuButton( this, Point( 0, 0 ), -1, -1, startPic, pushBtn );
  ret->setID( id | ( haveSubmenu ? BuildMenu::subMenuCreateIdHigh : 0 ) );
  ret->setPosition( offset + Point( 0, dy * yMul ) );
  ret->setTooltipText( tooltip );

  if( MenuButton* btn = safety_cast< MenuButton* >( ret ) )
  {
    btn->setMidPicId( midPic );
  }

  return ret;
}

/* here will be helper functions for minimap generation */
void Menu::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  painter.draw( _d->background, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
    
  Widget::draw( painter );
}

bool Menu::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    if( !event.gui.caller )
        return false;

    int id = event.gui.caller->ID();
    switch( id )
    {
    case objects::house:
    case objects::road:
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit _d->onCreateConstructionSignal( id );
    break;

    case REMOVE_TOOL_ID:
      _d->lastPressed = event.gui.caller;
      _createBuildMenu( -1, this );
      emit _d->onRemoveToolSignal();
    break;

    default:
      if( _d->lastPressed != event.gui.caller )
      {
        if( event.gui.caller->parent() == this )
            _d->lastPressed = event.gui.caller;

        if( PushButton* btn = safety_cast< PushButton* >( event.gui.caller ) )
        {
          int id = btn->ID();
          if( id & BuildMenu::subMenuCreateIdHigh )
          {
            _createBuildMenu( id & 0xff, event.gui.caller );
          }
          else
          {
            emit _d->onCreateConstructionSignal( id );
            _createBuildMenu( -1, this );
            setFocus();
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

  if( event.EventType == sEventGui && event.gui.type == guiElementFocusLost )
  {
    unselectAll();
    _d->lastPressed = 0;
  }

  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type )
    {
    case mouseRbtnRelease:
      _createBuildMenu( -1, this );
      unselectAll();
      _d->lastPressed = 0;
    return true;

    case mouseLbtnPressed:
    case mouseLbtnRelease:
    {
      //lock movement for tilemap
      if( findChildren<BuildMenu*>().size() > 0 )
        return true;
    }
    break;

    default: break;
    }
  }

  return Widget::onEvent( event );
}

Menu* Menu::create(Widget* parent, int id, PlayerCityPtr city )
{
  const Picture& bground = Picture::load( ResourceGroup::panelBackground, 16 );

  Menu* ret = new Menu( parent, id, Rect( 0, 0, bground.width(), parent->height() ) );

  const Picture& bottom  = Picture::load( ResourceGroup::panelBackground, 21 );

  ret->_d->background.clear();

  ret->_d->background.append( bground, Point( 0, 0 ) );
  unsigned int y = bground.height();
  while( y < parent->height() )
  {
    ret->_d->background.append( bottom, Point( 0, -y ) );
    y += bottom.height() - 5;
  }

  ret->_d->city = city;  
  ret->_d->updateBuildingOptions();

  CONNECT( city, onChangeBuildingOptions(), ret->_d.data(), Impl::updateBuildingOptions );

  return ret;
}

void Menu::minimize()
{
  _d->lastPressed = 0;
  _createBuildMenu( -1, this );
  Point stopPos = lefttop() + Point( width(), 0 );
  PositionAnimator* anim = new PositionAnimator( this, WidgetAnimator::removeSelf, stopPos, 300 );
  CONNECT( anim, onFinish(), &_d->onHideSignal, Signal0<>::_emit );

  events::GameEventPtr e = events::PlaySound::create( "panel", 3, 100 );
  e->dispatch();
}

void Menu::maximize()
{
  Point stopPos = lefttop() - Point( width(), 0 );
  show();
  new PositionAnimator( this, WidgetAnimator::showParent | WidgetAnimator::removeSelf, stopPos, 300 );

  events::GameEventPtr e = events::PlaySound::create( "panel", 3, 100 );
  e->dispatch();
}

bool Menu::unselectAll()
{
  bool anyPressed = false;
  foreach( it, children() )
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
   foreach( item, menus ) { (*item)->deleteLater(); }

   BuildMenu* buildMenu = BuildMenu::create( (development::Branch)type, this );

   if( buildMenu != NULL )
   {
     buildMenu->setNotClipped( true );

     buildMenu->setBuildOptions( _d->city->buildOptions() );
     buildMenu->initialize();

     int y = math::clamp< int >( parent->screenTop() - screenTop(), 0, _environment->rootWidget()->height() - buildMenu->height() );
     buildMenu->setPosition( Point( -(int)buildMenu->width() - 5, y ) );
   }
}

Signal0<>& Menu::onHide() { return _d->onHideSignal; }

void Menu::Impl::initActionButton(PushButton* btn, Point pos)
{
  btn->setPosition( pos );
  CONNECT( btn, onClicked(), this, Impl::playSound );
}

void Menu::Impl::playSound()
{
  events::GameEventPtr e = events::PlaySound::create( "panel", rand() % 2 + 1, 100 );
  e->dispatch();
}

void Menu::Impl::updateBuildingOptions()
{
  const development::Options& options = city->buildOptions();
  waterButton->setEnabled( options.isGroupAvailable( development::water ));
  administrationButton->setEnabled( options.isGroupAvailable( development::administration ));
  entertainmentButton->setEnabled( options.isGroupAvailable( development::entertainment ));
  educationButton->setEnabled( options.isGroupAvailable( development::education ));
  templeButton->setEnabled( options.isGroupAvailable( development::religion ));
  commerceButton->setEnabled( options.isGroupAvailable( development::commerce ));
  securityButton->setEnabled( options.isGroupAvailable( development::security ));
  healthButton->setEnabled( options.isGroupAvailable( development::health ));
  engineerButton->setEnabled( options.isGroupAvailable( development::engineering ));
}

ExtentMenu* ExtentMenu::create(Widget* parent, int id, PlayerCityPtr city )
{
  const Picture& bground = Picture::load( ResourceGroup::panelBackground, 17 );

  ExtentMenu* ret = new ExtentMenu( parent, id, Rect( 0, 0, bground.width(), parent->height() ) );

  const Picture& bottom = Picture::load( ResourceGroup::panelBackground, 20 );

  ret->_d->background.clear();

  ret->_d->background.append( bground, Point( 0, 0 ) );
  unsigned int y = bground.height();
  while( y < parent->height() )
  {
    ret->_d->background.append( bottom, Point( 0, -y ) );
    y += bottom.height() - 5;
  }

  ret->_d->city = city;
  ret->_d->updateBuildingOptions();

  CONNECT( city, onChangeBuildingOptions(), ret->_d.data(), Impl::updateBuildingOptions );

  return ret;
}

ExtentMenu::ExtentMenu(Widget* p, int id, const Rect& rectangle )
    : Menu( p, id, rectangle )
{
  setupUI( ":/gui/fullmenu.gui" );

  _d->minimizeButton->deleteLater();
  _d->minimizeButton = _addButton( 97, false, 0, MAXIMIZE_ID, false, ResourceMenu::emptyMidPicId, _("##hide_bigpanel##") );
  _d->minimizeButton->setGeometry( Rect( Point( 127, 5 ), Size( 31, 20 ) ) );
  CONNECT( _d->minimizeButton, onClicked(), this, ExtentMenu::minimize );  

  _d->initActionButton( _d->houseButton, Point( 13, 277 ) );
  _d->houseButton->setIsPushButton( false );
  _d->initActionButton( _d->clearButton, Point( 63, 277 ) );
  _d->clearButton->setIsPushButton( false );
  _d->initActionButton( _d->roadButton, Point( 113, 277 ) );
  _d->roadButton->setIsPushButton( false );

  _d->initActionButton( _d->waterButton, Point( 13, 313 ) );
  _d->initActionButton( _d->healthButton, Point( 63, 313 ) );
  _d->initActionButton( _d->templeButton, Point( 113, 313 ) );
  _d->initActionButton( _d->educationButton, Point(13, 349 ));
  _d->initActionButton( _d->entertainmentButton, Point(63, 349 ) );
  _d->initActionButton( _d->administrationButton, Point( 113, 349) );
  _d->initActionButton( _d->engineerButton, Point( 13, 385 ) );
  _d->initActionButton( _d->securityButton, Point( 63, 385 ) );
  _d->initActionButton( _d->commerceButton, Point( 113, 385) );

  //header
  _d->senateButton = _addButton( 79, false, 0, -1, false, -1, _("##senate##") );
  _d->senateButton->setGeometry( Rect( Point( 7, 155 ), Size( 71, 23 ) ) );
  _d->empireButton = _addButton( 82, false, 0, -1, false, -1, _("##empireBtnTooltip##") );
  _d->empireButton->setGeometry( Rect( Point( 84, 155 ), Size( 71, 23 ) ) );
  
  _d->missionButton = _addButton( 85, false, 0, -1, false, -1, _("##missionBtnTooltip##") );
  _d->missionButton->setGeometry( Rect( Point( 7, 184 ), Size( 33, 22 ) ) );
  _d->northButton = _addButton( 88, false, 0, -1, false, -1, _("##reorient_map_to_north##") );
  _d->northButton->setGeometry( Rect( Point( 46, 184 ), Size( 33, 22 ) ) );
  _d->rotateLeftButton = _addButton( 91, false, 0, -1, false, -1, _("##rotate_map_counter-clockwise##") );
  _d->rotateLeftButton->setGeometry( Rect( Point( 84, 184 ), Size( 33, 22 ) ) );
  _d->rotateRightButton = _addButton( 94, false, 0, -1, false, -1, _("##rotate_map_clockwise##") );
  _d->rotateRightButton->setGeometry( Rect( Point( 123, 184 ), Size( 33, 22 ) ) );

  _d->cancelButton = _addButton( 171, false, 0, -1, false, -1, _("##cancelBtnTooltip##") );
  _d->cancelButton->setGeometry( Rect( Point( 13, 421 ), Size( 39, 22 ) ) );
  _d->cancelButton->setEnabled( false );

  _d->messageButton = _addButton( 115, false, 0, -1, false, -1, _("##messageBtnTooltip##") );
  _d->messageButton->setGeometry( Rect( Point( 63, 421 ), Size( 39, 22 ) ) );
  _d->disasterButton = _addButton( 119, false, 0, -1, false, -1, _("##disasterBtnTooltip##") );
  _d->disasterButton->setGeometry( Rect( Point( 113, 421 ), Size( 39, 22 ) ) );
  _d->disasterButton->setEnabled( false );
  _d->disasterButton->setTooltipText( "##show_spots_of_city_troubles_tip##");

  _d->middleLabel = new Label(this, Rect( Point( 7, 216 ), Size( 148, 52 )) );
  _d->middleLabel->setBackgroundPicture( Picture::load( ResourceGroup::menuMiddleIcons, ResourceMenu::emptyMidPicId ) );

  _d->overlaysMenu = new OverlaysMenu( parent(), Rect( 0, 0, 160, 1 ), -1 );
  _d->overlaysMenu->hide();

  _d->overlaysButton = new PushButton( this, Rect( 4, 3, 122, 28 ), _("##ovrm_text##") );
  _d->overlaysButton->setTooltipText( _("##select_city_layer##") );
  
  CONNECT( _d->overlaysButton, onClicked(), this, ExtentMenu::toggleOverlayMenuVisible );
  CONNECT( _d->overlaysMenu, onSelectOverlayType(), this, ExtentMenu::changeOverlay );
}

bool ExtentMenu::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    if( MenuButton* btn = safety_cast< MenuButton* >( event.gui.caller ) )
    {
      int picId = btn->midPicId() > 0 ? btn->midPicId() : ResourceMenu::emptyMidPicId;
      _d->middleLabel->setBackgroundPicture( Picture::load( ResourceGroup::menuMiddleIcons, picId ) );
    }
  }

  return Menu::onEvent( event );
}

void ExtentMenu::draw(Engine& painter )
{
  if( !visible() )
    return;

  Menu::draw( painter );
}

void ExtentMenu::toggleOverlayMenuVisible()
{
  _d->overlaysMenu->setPosition( Point( screenLeft() - 170, 74 ) );
  _d->overlaysMenu->setVisible( !_d->overlaysMenu->visible() );
}

void ExtentMenu::changeOverlay(int ovType)
{
  std::string layerName = citylayer::Helper::prettyName( (citylayer::Type)ovType );
  _d->overlaysButton->setText( _( layerName ) );
}

Signal1<int>& ExtentMenu::onSelectOverlayType() {  return _d->overlaysMenu->onSelectOverlayType(); }
Signal0<>& ExtentMenu::onEmpireMapShow(){  return _d->empireButton->onClicked(); }
Signal0<>& ExtentMenu::onAdvisorsWindowShow(){  return _d->senateButton->onClicked(); }
Signal0<>& ExtentMenu::onSwitchAlarm(){  return _d->disasterButton->onClicked(); }
Signal0<>& ExtentMenu::onMessagesShow()  { return _d->messageButton->onClicked(); }
Signal0<>& ExtentMenu::onRotateRight() { return _d->rotateRightButton->onClicked(); }
Signal0<>& ExtentMenu::onRotateLeft() { return _d->rotateLeftButton->onClicked(); }
Signal0<>& ExtentMenu::onMissionTargetsWindowShow(){  return _d->missionButton->onClicked(); }

void ExtentMenu::setAlarmEnabled( bool enabled ){  _d->disasterButton->setEnabled( enabled );}
}
