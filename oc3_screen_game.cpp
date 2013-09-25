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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "oc3_screen_game.hpp"

#include <algorithm>

#include "oc3_gfx_engine.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_rightpanel.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_guienv.hpp"
#include "oc3_topmenu.hpp"
#include "oc3_gui_menu.hpp"
#include "oc3_event.hpp"
#include "oc3_infoboxmanager.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_tilemapchangecommand.hpp"
#include "oc3_message_stack_widget.hpp"
#include "oc3_time.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_window_empiremap.hpp"
#include "oc3_save_dialog.hpp"
#include "oc3_advisors_window.hpp"
#include "oc3_alarm_event_holder.hpp"
#include "oc3_city_renderer.hpp"
#include "oc3_game.hpp"
#include "oc3_senate_popup_info.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_empire.hpp"
#include "oc3_game_settings.hpp"
#include "oc3_window_mission_target.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_gettext.hpp"
#include "oc3_window_minimap.hpp"
#include "oc3_game_event_mgr.hpp"
#include "oc3_window_video_options.hpp"

class ScreenGame::Impl
{
public:
  MenuRigthPanel* rightPanel;
  TopMenu* topMenu;
  Menu* menu;
  GfxEngine* engine;
  ExtentMenu* extMenu;
  InfoBoxManagerPtr infoBoxMgr;
  CityRenderer renderer;
  WindowMessageStack* wndStackMsgs;
  Game* game; // current game scenario
  AlarmEventHolder alarmsHolder;

  int result;

  void showSaveDialog();
  void showEmpireMapWindow();
  void showAdvisorsWindow( const int advType );
  void showAdvisorsWindow();
  void showMissionTaretsWindow();
  void showTradeAdvisorWindow();
  void resolveCreateConstruction( int type );
  void resolveSelectOverlayView( int type );
  void resolveRemoveTool();
  void showTileInfo( const Tile& tile );
  void makeScreenShot();
  void showScreenOptionsDialog();
};

ScreenGame::ScreenGame(Game& game , GfxEngine& engine ) : _d( new Impl )
{
  _d->topMenu = NULL;
  _d->game = &game;
  _d->engine = &engine;
}

ScreenGame::~ScreenGame() {}

void ScreenGame::initialize()
{
  CityPtr city = _d->game->getCity();
  _d->renderer.initialize( city, _d->engine );

  _d->infoBoxMgr = InfoBoxManager::create( city, _d->game->getGui() );
  _d->game->getGui()->clear();

  const int topMenuHeight = 23;
  const Picture& rPanelPic = Picture::load( ResourceGroup::panelBackground, 14 );

  GfxEngine& engine = GfxEngine::instance();
  GuiEnv& gui = *_d->game->getGui();

  Rect rPanelRect( engine.getScreenWidth() - rPanelPic.getWidth(), topMenuHeight,
                   engine.getScreenWidth(), engine.getScreenHeight() );

  _d->rightPanel = MenuRigthPanel::create( gui.getRootWidget(), rPanelRect, rPanelPic);

  _d->topMenu = new TopMenu( gui.getRootWidget(), topMenuHeight );
  _d->topMenu->setPopulation( _d->game->getCity()->getPopulation() );
  _d->topMenu->setFunds( _d->game->getCity()->getFunds().getValue() );

  _d->menu = Menu::create( gui.getRootWidget(), -1, city );
  _d->menu->setPosition( Point( engine.getScreenWidth() - _d->menu->getWidth() - _d->rightPanel->getWidth(), 
                                 _d->topMenu->getHeight() ) );

  _d->extMenu = ExtentMenu::create( gui.getRootWidget(), -1, city );
  _d->extMenu->setPosition( Point( engine.getScreenWidth() - _d->extMenu->getWidth() - _d->rightPanel->getWidth(), 
                                     _d->topMenu->getHeight() ) );

  Minimap* mmap = new Minimap( _d->extMenu, Rect( 8, 35, 8 + 144, 35 + 110 ),
                               city->getTilemap(),
                               city->getClimate() );

  _d->wndStackMsgs = WindowMessageStack::create( gui.getRootWidget(), -1 );
  _d->wndStackMsgs->setPosition( Point( gui.getRootWidget()->getWidth() / 4, 33 ) );
  _d->wndStackMsgs->sendToBack();
    
  _d->rightPanel->bringToFront();

  // 8*30: used for high buildings (granary...), visible even when not in tilemap_area.
  _d->renderer.getCamera().setViewport( engine.getScreenSize() + Size( 180 ) );

  new SenatePopupInfo( gui.getRootWidget(), _d->renderer );

  //connect elements
  CONNECT( _d->topMenu, onSave(), _d.data(), Impl::showSaveDialog );
  CONNECT( _d->topMenu, onExit(), this, ScreenGame::resolveExitGame );
  CONNECT( _d->topMenu, onEnd(), this, ScreenGame::resolveEndGame );
  CONNECT( _d->topMenu, onRequestAdvisor(), _d.data(), Impl::showAdvisorsWindow );
  CONNECT( _d->topMenu, onShowVideoOptions(), _d.data(), Impl::showScreenOptionsDialog );

  CONNECT( _d->menu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->menu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );
  CONNECT( _d->menu, onMaximize(), _d->extMenu, ExtentMenu::maximize );

  CONNECT( _d->extMenu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->extMenu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );

  CONNECT( city, onPopulationChanged(), _d->topMenu, TopMenu::setPopulation );
  CONNECT( city, onFundsChanged(), _d->topMenu, TopMenu::setFunds );

  CONNECT( &_d->renderer, onShowTileInfo(), _d.data(), Impl::showTileInfo );

  CONNECT( city, onWarningMessage(), _d->wndStackMsgs, WindowMessageStack::addMessage );
  CONNECT( &_d->renderer, onWarningMessage(), _d->wndStackMsgs, WindowMessageStack::addMessage );
  CONNECT( _d->extMenu, onSelectOverlayType(), _d.data(), Impl::resolveSelectOverlayView );
  CONNECT( _d->extMenu, onEmpireMapShow(), _d.data(), Impl::showEmpireMapWindow );
  CONNECT( _d->extMenu, onAdvisorsWindowShow(), _d.data(), Impl::showAdvisorsWindow );
  CONNECT( _d->extMenu, onMissionTargetsWindowShow(), _d.data(), Impl::showMissionTaretsWindow );

  CONNECT( city, onDisasterEvent(), &_d->alarmsHolder, AlarmEventHolder::add );
  CONNECT( _d->extMenu, onSwitchAlarm(), &_d->alarmsHolder, AlarmEventHolder::next );
  CONNECT( &_d->alarmsHolder, onMoveToAlarm(), &_d->renderer.getCamera(), TilemapCamera::setCenter );
  CONNECT( &_d->alarmsHolder, onAlarmChange(), _d->extMenu, ExtentMenu::setAlarmEnabled );
  CONNECT( &_d->renderer.getCamera(), onPositionChanged(), mmap, Minimap::setCenter );

  _d->showMissionTaretsWindow();
  _d->renderer.getCamera().setCenter( city->getCameraPos() );
}

void ScreenGame::Impl::showSaveDialog()
{
  SaveDialog* dialog = new SaveDialog( game->getGui()->getRootWidget(), "saves", ".oc3save", -1 );
  CONNECT( dialog, onFileSelected(), game, Game::save );
}

void ScreenGame::Impl::showScreenOptionsDialog()
{
  VideoOptionsWindow* dialog = new VideoOptionsWindow( game->getGui()->getRootWidget() );
  CONNECT( dialog, onSreenSizeChange(), engine, GfxEngine::setScreenSize );
}

void ScreenGame::Impl::showEmpireMapWindow()
{
  GameEventMgr::append( ShowEmpireMapWindow::create( true ) );
  GameEventMgr::append( ShowAdvisorWindow::create( false, ADV_TRADING ) );
}

void ScreenGame::draw()
{
  _d->renderer.draw();

  _d->game->getGui()->beforeDraw();
  _d->game->getGui()->draw();
}

void ScreenGame::animate( unsigned int time )
{
  _d->renderer.animate( time );
}

void ScreenGame::afterFrame()
{
}

void ScreenGame::handleEvent( NEvent& event )
{
  //After MouseDown events are send to the same target till MouseUp
  GuiEnv& gui = *_d->game->getGui();

  static enum _MouseEventTarget
  {
    _MET_NONE,
    _MET_GUI,
    _MET_TILES
  } _mouseEventTarget = _MET_NONE;

  if( event.EventType == OC3_KEYBOARD_EVENT )
  {
    switch( event.KeyboardEvent.Key )
    {
    case KEY_MINUS:
    case KEY_PLUS:
    case KEY_SUBTRACT:
    case KEY_ADD:
      GameEventMgr::append( ChangeSpeed::create( (event.KeyboardEvent.Key == KEY_MINUS || event.KeyboardEvent.Key == KEY_SUBTRACT)
                                                 ? -10 : +10 ) );
    break;

    case KEY_KEY_P:
      if( event.KeyboardEvent.PressedDown )
        break;

      GameEventMgr::append( TogglePause::create() );
    break;

		case KEY_F10:
			_d->makeScreenShot();
		break;

    default:
    break;
    }
  }

  bool eventResolved = false;
  if (event.EventType == OC3_MOUSE_EVENT)
  {
    if (event.EventType & (OC3_RMOUSE_PRESSED_DOWN | OC3_LMOUSE_PRESSED_DOWN))
    {
      eventResolved = gui.handleEvent( event );
      if (eventResolved)
      {
        _mouseEventTarget = _MET_GUI;
      }
      else // eventresolved
      {
        _mouseEventTarget = _MET_TILES;
        _d->renderer.handleEvent( event );
      }
      return;
    }

    switch(_mouseEventTarget)
    {
    case _MET_GUI:
      gui.handleEvent( event );
    break;

    case _MET_TILES:
      _d->renderer.handleEvent( event );
    break;

    default:
       if (!gui.handleEvent( event ))
        _d->renderer.handleEvent( event );
    break;
    }

    if (event.EventType & (OC3_RMOUSE_LEFT_UP | OC3_LMOUSE_LEFT_UP))
    {
      _mouseEventTarget = _MET_NONE;
    }
  }
  else
  {
    eventResolved = gui.handleEvent( event );
   
    if( !eventResolved )
    {
      _d->renderer.handleEvent( event );
    }
  }
}

void ScreenGame::Impl::makeScreenShot()
{
  DateTime time = DateTime::getCurrenTime();

  std::string filename = StringHelper::format( 0xff, "oc3_[%04d_%02d_%02d_%02d_%02d_%02d].png", 
                                               time.getYear(), time.getMonth(), time.getDay(),
                                               time.getHour(), time.getMinutes(), time.getSeconds() );
  StringHelper::debug( 0xff, "creating screenshot %s", filename.c_str() );

  GfxEngine::instance().createScreenshot( filename );
}

int ScreenGame::getResult() const
{
  return _d->result;
}

void ScreenGame::Impl::resolveCreateConstruction( int type )
{
  renderer.setMode( TilemapBuildCommand::create( BuildingType( type ) ) );
}

void ScreenGame::Impl::resolveRemoveTool()
{
  renderer.setMode( TilemapRemoveCommand::create() );
}

void ScreenGame::Impl::showTileInfo( const Tile& tile )
{
  infoBoxMgr->showHelp( tile );
}

void ScreenGame::resolveEndGame()
{
  _d->result = ScreenGame::mainMenu;
  stop();
}

void ScreenGame::resolveExitGame()
{
  _d->result = ScreenGame::quitGame;
  stop();
}

void ScreenGame::Impl::resolveSelectOverlayView( int type )
{
  renderer.setMode( TilemapOverlayCommand::create( OverlayType( type ) ) );
}

void ScreenGame::Impl::showAdvisorsWindow()
{
  showAdvisorsWindow( ADV_EMPLOYERS );
}

void ScreenGame::Impl::showAdvisorsWindow( const int advType )
{  
  List<AdvisorsWindow*> wndList = game->getGui()->getRootWidget()->findChildren<AdvisorsWindow*>();

  if( wndList.size() == 1 )
  {
    wndList.front()->bringToFront();
    wndList.front()->showAdvisor( (AdvisorType)advType ); 
  }
  else
  {
    AdvisorsWindow* advWnd = AdvisorsWindow::create( game->getGui()->getRootWidget(), -1,
                                                     (AdvisorType)advType, game->getCity() );
  }
}

void ScreenGame::Impl::showTradeAdvisorWindow()
{
  showAdvisorsWindow( ADV_TRADING );
}

void ScreenGame::Impl::showMissionTaretsWindow()
{
  MissionTargetsWindow::create( game->getGui()->getRootWidget(), game->getCity() );
}
