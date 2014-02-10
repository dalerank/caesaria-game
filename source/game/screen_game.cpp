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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "screen_game.hpp"

#include <algorithm>

#include "gfx/engine.hpp"
#include "city/win_targets.hpp"
#include "core/exception.hpp"
#include "gui/rightpanel.hpp"
#include "resourcegroup.hpp"
#include "gui/environment.hpp"
#include "gui/topmenu.hpp"
#include "gui/menu.hpp"
#include "core/event.hpp"
#include "infoboxmanager.hpp"
#include "objects/objects_factory.hpp"
#include "gfx/renderermode.hpp"
#include "gui/message_stack_widget.hpp"
#include "core/time.hpp"
#include "core/stringhelper.hpp"
#include "gui/empiremap_window.hpp"
#include "gui/save_dialog.hpp"
#include "gui/advisors_window.hpp"
#include "alarm_event_holder.hpp"
#include "gfx/city_renderer.hpp"
#include "game.hpp"
#include "gui/senate_popup_info.hpp"
#include "city/funds.hpp"
#include "gamedate.hpp"
#include "world/empire.hpp"
#include "settings.hpp"
#include "gui/mission_target_window.hpp"
#include "gui/label.hpp"
#include "core/gettext.hpp"
#include "gui/minimap_window.hpp"
#include "gui/window_gamespeed_options.hpp"
#include "events/setvideooptions.hpp"
#include "core/logger.hpp"
#include "walker/enemysoldier.hpp"
#include "patrolpointeventhandler.hpp"
#include "city/city.hpp"
#include "gfx/tilemap_camera.hpp"
#include "ambientsound.hpp"
#include "gui/win_mission_window.hpp"

using namespace gui;

class ScreenGame::Impl
{
public:
  typedef std::vector< EventHandlerPtr > EventHandlers;

  EventHandlers eventHandlers;
  gui::MenuRigthPanel* rightPanel;
  gui::TopMenu* topMenu;
  gui::Menu* menu;
  GfxEngine* engine;
  gui::ExtentMenu* extMenu;
  CityRenderer renderer;
  Game* game; // current game
  AlarmEventHolder alarmsHolder;
  DateTime lastDate;
  std::string mapToLoad;
  bool isPaused;

  int result;

  void showSaveDialog();
  void showEmpireMapWindow();
  void showAdvisorsWindow( const int advType );
  void showAdvisorsWindow();
  void showMissionTaretsWindow();
  void showTradeAdvisorWindow();
  void resolveCreateConstruction( int type );
  void resolveSelectLayer( int type );
  void resolveRemoveTool();
  void makeScreenShot();
  void setVideoOptions();
  void showGameSpeedOptionsDialog();
  void resolveWarningMessage( std::string );
  void saveCameraPos(Point p);
  void makeEnemy();
  void makeFastSave();
  vfs::Path getFastSaveName();
};

ScreenGame::ScreenGame(Game& game , GfxEngine& engine ) : _d( new Impl )
{
  _d->topMenu = NULL;
  _d->game = &game;
  _d->engine = &engine;
  _d->isPaused = false;
}

ScreenGame::~ScreenGame() {}

void ScreenGame::initialize()
{
  PlayerCityPtr city = _d->game->getCity();
  _d->renderer.initialize( city, _d->engine );
  _d->game->getGui()->clear();

  const int topMenuHeight = 23;
  const Picture& rPanelPic = Picture::load( ResourceGroup::panelBackground, 14 );

  GfxEngine& engine = GfxEngine::instance();
  gui::GuiEnv& gui = *_d->game->getGui();

  installEventHandler( PatrolPointEventHandler::create( *_d->game, _d->renderer ) );

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

  WindowMessageStack::create( gui.getRootWidget() );

  _d->rightPanel->bringToFront();
  _d->renderer.getCamera().setViewport( engine.getScreenSize() );

  new SenatePopupInfo( gui.getRootWidget(), _d->renderer );

  _d->game->getCity()->addService( AmbientSound::create( _d->game->getCity(), _d->renderer.getCamera() ) );

  //connect elements
  CONNECT( _d->topMenu, onSave(), _d.data(), Impl::showSaveDialog );
  CONNECT( _d->topMenu, onExit(), this, ScreenGame::_resolveExitGame );
  CONNECT( _d->topMenu, onEnd(), this, ScreenGame::_resolveEndGame );
  CONNECT( _d->topMenu, onRequestAdvisor(), _d.data(), Impl::showAdvisorsWindow );
  CONNECT( _d->topMenu, onShowVideoOptions(), _d.data(), Impl::setVideoOptions );
  CONNECT( _d->topMenu, onShowGameSpeedOptions(), _d.data(), Impl::showGameSpeedOptionsDialog );

  CONNECT( _d->menu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->menu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );
  CONNECT( _d->menu, onMaximize(), _d->extMenu, ExtentMenu::maximize );

  CONNECT( _d->extMenu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->extMenu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );

  CONNECT( city, onPopulationChanged(), _d->topMenu, TopMenu::setPopulation );
  CONNECT( city, onFundsChanged(), _d->topMenu, TopMenu::setFunds );
  CONNECT( city, onWarningMessage(), _d.data(), Impl::resolveWarningMessage );

  CONNECT( _d->extMenu, onSelectOverlayType(), _d.data(), Impl::resolveSelectLayer );
  CONNECT( _d->extMenu, onEmpireMapShow(), _d.data(), Impl::showEmpireMapWindow );
  CONNECT( _d->extMenu, onAdvisorsWindowShow(), _d.data(), Impl::showAdvisorsWindow );
  CONNECT( _d->extMenu, onMissionTargetsWindowShow(), _d.data(), Impl::showMissionTaretsWindow );

  CONNECT( city, onDisasterEvent(), &_d->alarmsHolder, AlarmEventHolder::add );
  CONNECT( _d->extMenu, onSwitchAlarm(), &_d->alarmsHolder, AlarmEventHolder::next );
  CONNECT( &_d->alarmsHolder, onMoveToAlarm(), &_d->renderer.getCamera(), TilemapCamera::setCenter );
  CONNECT( &_d->alarmsHolder, onAlarmChange(), _d->extMenu, ExtentMenu::setAlarmEnabled );

  CONNECT( &_d->renderer.getCamera(), onPositionChanged(), mmap, Minimap::setCenter );
  CONNECT( &_d->renderer.getCamera(), onPositionChanged(), _d.data(), Impl::saveCameraPos );
  CONNECT( mmap, onCenterChange(), &_d->renderer.getCamera(), TilemapCamera::setCenter );

  _d->showMissionTaretsWindow();
  _d->renderer.getCamera().setCenter( city->getCameraPos() );
}

std::string ScreenGame::getMapName() const{  return _d->mapToLoad;}

void ScreenGame::Impl::showSaveDialog()
{
  vfs::Directory saveDir = GameSettings::get( GameSettings::savedir ).toString();
  std::string defaultExt = GameSettings::get( GameSettings::saveExt ).toString();

  if( !saveDir.exist() )
  {
    vfs::Directory::createByPath( saveDir );
  }

  SaveDialog* dialog = new SaveDialog( game->getGui()->getRootWidget(), saveDir, defaultExt, -1 );
  CONNECT( dialog, onFileSelected(), game, Game::save );
}

void ScreenGame::Impl::setVideoOptions()
{
  events::GameEventPtr event = events::SetVideoSettings::create();
  event->dispatch();
}

void ScreenGame::Impl::showGameSpeedOptionsDialog()
{
  GameSpeedOptionsWindow* dialog = new GameSpeedOptionsWindow( game->getGui()->getRootWidget(),
                                                               game->getTimeMultiplier(),
                                                               0 );

  CONNECT( dialog, onGameSpeedChange(), game, Game::setTimeMultiplier );
  CONNECT( dialog, onScrollSpeedChange(), &renderer.getCamera(), TilemapCamera::setScrollSpeed );
}

void ScreenGame::Impl::resolveWarningMessage(std::string text )
{
  events::GameEventPtr e = events::WarningMessageEvent::create( text );
  e->dispatch();
}

void ScreenGame::Impl::saveCameraPos(Point p)
{
  Tile* tile = renderer.getCamera().at( Point( engine->getScreenWidth()/2, engine->getScreenHeight()/2 ), false );

  if( tile )
  {
    game->getCity()->setCameraPos( tile->pos() );
  }
}

void ScreenGame::Impl::makeEnemy()
{
  EnemySoldierPtr enemy = EnemySoldier::create( game->getCity(),
                                                constants::walker::britonSoldier );
  enemy->send2City( game->getCity()->getBorderInfo().roadEntry );
}

void ScreenGame::Impl::makeFastSave() {  game->save( getFastSaveName().toString() ); }

void ScreenGame::_resolveFastLoad()
{
  _d->mapToLoad = _d->getFastSaveName().toString();
  _resolveSwitchMap();
}

vfs::Path ScreenGame::Impl::getFastSaveName()
{
  vfs::Path filename = game->getCity()->getName()
                       + GameSettings::get( GameSettings::fastsavePostfix ).toString()
                       + GameSettings::get( GameSettings::saveExt ).toString();

  vfs::Directory saveDir = GameSettings::get( GameSettings::savedir ).toString();

  return saveDir/filename;
}

void ScreenGame::_resolveSwitchMap() { _d->result = ScreenGame::loadGame;  stop(); }

void ScreenGame::Impl::showEmpireMapWindow()
{
  events::GameEventPtr e = events::ShowEmpireMapWindow::create( true );
  e->dispatch();

  e = events::ShowAdvisorWindow::create( false, ADV_TRADING );
  e->dispatch();
}

void ScreenGame::draw()
{
  _d->renderer.render();

  _d->game->getGui()->beforeDraw();
  _d->game->getGui()->draw();
}

void ScreenGame::animate( unsigned int time ) {  _d->renderer.animate( time ); }

void ScreenGame::afterFrame()
{
  if( _d->lastDate.month() != GameDate::current().month() )
  {
    _d->lastDate = GameDate::current();
    PlayerCityPtr city = _d->game->getCity();
    const CityWinTargets& wt = city->getWinTargets();

    int culture = city->getCulture();
    int prosperity = city->getProsperity();
    int favour = city->getFavour();
    int peace = city->getFavour();
    int population = city->getPopulation();
    bool success = wt.isSuccess( culture, prosperity, favour, peace, population );

    if( success )
    {
      std::string newTitle = wt.getNewTitle();

      gui::WinMissionWindow* wnd = new gui::WinMissionWindow( _d->game->getGui()->getRootWidget(), newTitle, false );

      _d->mapToLoad = wt.getNextMission();

      CONNECT( wnd, onAcceptAssign(), this, ScreenGame::_resolveSwitchMap );
    }
  }
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

  if( event.EventType == sEventKeyboard )
  {
    switch( event.keyboard.key )
    {
    case KEY_MINUS:
    case KEY_PLUS:
    case KEY_SUBTRACT:
    case KEY_ADD:
    {
      events::GameEventPtr e = events::ChangeSpeed::create( (event.keyboard.key == KEY_MINUS ||
                                                              event.keyboard.key == KEY_SUBTRACT)
                                                            ? -10 : +10 );
      e->dispatch();
    }
    break;

    case KEY_KEY_P:
    {
      if( event.keyboard.pressed )
        break;

      _d->isPaused = !_d->isPaused;

      events::GameEventPtr e = events::Pause::create( _d->isPaused
                                                        ? events::Pause::pause
                                                        : events::Pause::play );
      e->dispatch();      
    }
    break;

    case KEY_F10:	_d->makeScreenShot(); break;

		case KEY_F5: _d->makeFastSave(); break;
		case KEY_F9: _resolveFastLoad(); break;

		case KEY_F11:
			if( event.keyboard.pressed )
				_d->makeEnemy();
		break;

    default:
    break;
    }
  }

  for( Impl::EventHandlers::iterator it=_d->eventHandlers.begin(); it != _d->eventHandlers.end(); )
  {
    (*it)->handleEvent( event );
    if( (*it)->finished() ) { it = _d->eventHandlers.erase( it ); }
    else{ it++; }
  }

  bool eventResolved = false;
  if (event.EventType == sEventMouse)
  {
    if( event.mouse.type == mouseRbtnPressed || event.mouse.type == mouseLbtnPressed )
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

    if( event.mouse.type == mouseRbtnRelease || event.mouse.type == mouseLbtnRelease )
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
                                               time.year(), time.month(), time.day(),
                                               time.hour(), time.minutes(), time.seconds() );
  Logger::warning( "creating screenshot %s", filename.c_str() );

  GfxEngine::instance().createScreenshot( filename );
}

int ScreenGame::getResult() const {  return _d->result; }
bool ScreenGame::installEventHandler(EventHandlerPtr handler) {  _d->eventHandlers.push_back( handler ); return true; }
void ScreenGame::Impl::resolveCreateConstruction( int type ){  renderer.setMode( BuildMode::create( TileOverlay::Type( type ) ) );}
void ScreenGame::Impl::resolveRemoveTool(){  renderer.setMode( DestroyMode::create() );}
void ScreenGame::Impl::resolveSelectLayer( int type ){  renderer.setMode( LayerMode::create( type ) );}
void ScreenGame::Impl::showAdvisorsWindow(){  showAdvisorsWindow( ADV_EMPLOYERS ); }
void ScreenGame::Impl::showTradeAdvisorWindow(){  showAdvisorsWindow( ADV_TRADING ); }
void ScreenGame::Impl::showMissionTaretsWindow(){  MissionTargetsWindow::create( game->getGui()->getRootWidget(), game->getCity() ); }
void ScreenGame::_resolveEndGame(){  _d->result = ScreenGame::mainMenu;  stop();}
void ScreenGame::_resolveExitGame(){  _d->result = ScreenGame::quitGame;  stop();}

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
    /*AdvisorsWindow* advWnd = */AdvisorsWindow::create( game->getGui()->getRootWidget(), -1,
                                                     (AdvisorType)advType, game->getCity() );
  }
}
