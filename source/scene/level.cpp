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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "level.hpp"

#include <algorithm>

#include "gfx/engine.hpp"
#include "city/victoryconditions.hpp"
#include "core/exception.hpp"
#include "gui/rightpanel.hpp"
#include "gui/loadfiledialog.hpp"
#include "game/resourcegroup.hpp"
#include "gui/environment.hpp"
#include "gui/topmenu.hpp"
#include "gui/menu.hpp"
#include "core/event.hpp"
#include "gui/dialogbox.hpp"
#include "game/infoboxmanager.hpp"
#include "objects/objects_factory.hpp"
#include "gfx/renderermode.hpp"
#include "gui/message_stack_widget.hpp"
#include "core/time.hpp"
#include "core/stringhelper.hpp"
#include "gui/empiremap_window.hpp"
#include "gui/save_dialog.hpp"
#include "gui/advisors_window.hpp"
#include "game/alarm_event_holder.hpp"
#include "gfx/city_renderer.hpp"
#include "game/game.hpp"
#include "gui/senate_popup_info.hpp"
#include "city/funds.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "game/settings.hpp"
#include "gui/mission_target_window.hpp"
#include "gui/label.hpp"
#include "core/gettext.hpp"
#include "gui/minimap_window.hpp"
#include "gui/window_gamespeed_options.hpp"
#include "events/setvideooptions.hpp"
#include "core/logger.hpp"
#include "walker/enemysoldier.hpp"
#include "game/patrolpointeventhandler.hpp"
#include "city/city.hpp"
#include "gfx/tilemap_camera.hpp"
#include "game/ambientsound.hpp"
#include "gui/win_mission_window.hpp"
#include "events/showempiremapwindow.hpp"
#include "events/showadvisorwindow.hpp"
#include "gui/texturedbutton.hpp"
#include "sound/engine.hpp"
#include "events/showtileinfo.hpp"
#include "gui/androidactions.hpp"
#include "events/setsoundoptions.hpp"
#include "gui/widgetescapecloser.hpp"
#include "walker/walkers_factory.hpp"
#include "gui/scribesmessages.hpp"
#include "core/foreach.hpp"
#include "events/random_animals.hpp"
#include "gfx/layerconstants.hpp"
#include "world/romechastenerarmy.hpp"
#include "events/warningmessage.hpp"
#include "religion/pantheon.hpp"
#include "core/saveadapter.hpp"
#include "events/postpone.hpp"
#include "gfx/pictureconverter.hpp"
#include "gui/city_options_window.hpp"

using namespace gui;
using namespace constants;
using namespace gfx;

namespace scene
{

class Level::Impl
{
public:
  typedef std::vector< EventHandlerPtr > EventHandlers;

  EventHandlers eventHandlers;
  gui::MenuRigthPanel* rightPanel;
  gui::TopMenu* topMenu;
  gui::Menu* menu;
  Engine* engine;
  gui::ExtentMenu* extMenu;
  CityRenderer renderer;
  Game* game; // current game
  AlarmEventHolder alarmsHolder;
  std::string mapToLoad;
  TilePos selectedTilePos;
  citylayer::Type lastLayerId;

  int result;

  void showSaveDialog();
  void showEmpireMapWindow();
  void showAdvisorsWindow(const advisor::Type advType );
  void showAdvisorsWindow();
  void showMissionTaretsWindow();
  void showTradeAdvisorWindow();
  void resolveCreateConstruction( int type );
  void resolveSelectLayer( int type );
  void resolveRemoveTool();
  void makeScreenShot();
  void setVideoOptions();
  void showGameSpeedOptionsDialog();
  void showCityOptionsDialog();
  void resolveWarningMessage( std::string );
  void saveCameraPos(Point p);
  void showSoundOptionsWindow();
  void makeEnemy();
  void makeFastSave();
  void showTileHelp();
  void showMessagesWindow();
  void setAutosaveInterval( int value );
  void layerChanged( int layer );
  void makeFullScreenshot();

  std::string getScreenshotName();
  vfs::Path createFastSaveName( const std::string& type="", const std::string& postfix="");
};

Level::Level(Game& game, gfx::Engine& engine ) : _d( new Impl )
{
  _d->topMenu = NULL;
  _d->game = &game;
  _d->engine = &engine;
}

Level::~Level() {}

void Level::initialize()
{
  PlayerCityPtr city = _d->game->city();
  _d->renderer.initialize( city, _d->engine, _d->game->gui() );
  _d->game->gui()->clear();

  const int topMenuHeight = 23;
  Picture rPanelPic = Picture::load( ResourceGroup::panelBackground, PicID::rightPanelTx );

  Engine& engine = Engine::instance();
  gui::GuiEnv& gui = *_d->game->gui();

  installEventHandler( PatrolPointEventHandler::create( *_d->game, _d->renderer ) );

  Rect rPanelRect( engine.screenSize().width() - rPanelPic.width(), topMenuHeight,
                   engine.screenSize().width(), engine.screenSize().height() );

  _d->rightPanel = MenuRigthPanel::create( gui.rootWidget(), rPanelRect, rPanelPic);

  _d->topMenu = new TopMenu( gui.rootWidget(), topMenuHeight );
  _d->topMenu->setPopulation( _d->game->city()->population() );
  _d->topMenu->setFunds( _d->game->city()->funds().treasury() );

  _d->menu = Menu::create( gui.rootWidget(), -1, city );
  _d->menu->setPosition( Point( engine.screenSize().width() - _d->rightPanel->width(),
                                _d->topMenu->height() ) );

  _d->extMenu = ExtentMenu::create( gui.rootWidget(), -1, city );
  _d->extMenu->setPosition( Point( engine.screenSize().width() - _d->extMenu->width() - _d->rightPanel->width(),
                                     _d->topMenu->height() ) );

  Minimap* mmap = new Minimap( _d->extMenu, Rect( 8, 35, 8 + 144, 35 + 110 ),
                               city,
                               *_d->renderer.camera() );

  WindowMessageStack::create( gui.rootWidget() );

  _d->rightPanel->bringToFront();
  _d->renderer.setViewport( engine.screenSize() );

  new SenatePopupInfo( gui.rootWidget(), _d->renderer );

  _d->game->city()->addService( city::AmbientSound::create( _d->game->city(), _d->renderer.camera() ) );

  //specific android actions bar
#ifdef CAESARIA_PLATFORM_ANDROID
  AndroidActionsBar* androidBar = new AndroidActionsBar( _d->game->gui()->rootWidget() );

  CONNECT( androidBar, onRequestTileHelp(), _d.data(), Impl::showTileHelp );
  CONNECT( androidBar, onEscapeClicked(), this, Level::_resolveEscapeButton );
  CONNECT( androidBar, onEnterClicked(), this, Level::_resolveEnterButton );
  CONNECT( androidBar, onRequestMenu(), this, Level::_showIngameMenu );
#endif

  //connect elements
  CONNECT( _d->topMenu, onSave(), _d.data(), Impl::showSaveDialog );
  CONNECT( _d->topMenu, onExit(), this, Level::_resolveExitGame );
  CONNECT( _d->topMenu, onLoad(), this, Level::_resolveShowLoadGameWnd );
  CONNECT( _d->topMenu, onEnd(), this, Level::_resolveEndGame );
  CONNECT( _d->topMenu, onRestart(), this, Level::_resolveRestart );
  CONNECT( _d->topMenu, onRequestAdvisor(), _d.data(), Impl::showAdvisorsWindow );
  CONNECT( _d->topMenu, onShowVideoOptions(), _d.data(), Impl::setVideoOptions );
  CONNECT( _d->topMenu, onShowSoundOptions(), _d.data(), Impl::showSoundOptionsWindow );
  CONNECT( _d->topMenu, onShowGameSpeedOptions(), _d.data(), Impl::showGameSpeedOptionsDialog );
  CONNECT( _d->topMenu, onShowCityOptions(), _d.data(), Impl::showCityOptionsDialog );

  CONNECT( _d->menu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->menu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );
  CONNECT( _d->menu, onHide(), _d->extMenu, ExtentMenu::maximize );

  CONNECT( _d->extMenu, onHide(), _d->menu, Menu::maximize );
  CONNECT( _d->extMenu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->extMenu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );
  CONNECT( _d->extMenu, onRotateRight(), &_d->renderer, CityRenderer::rotateRight );
  CONNECT( _d->extMenu, onRotateLeft(), &_d->renderer, CityRenderer::rotateLeft );

  CONNECT( city, onPopulationChanged(), _d->topMenu, TopMenu::setPopulation );
  CONNECT( city, onFundsChanged(), _d->topMenu, TopMenu::setFunds );
  CONNECT( city, onWarningMessage(), _d.data(), Impl::resolveWarningMessage );

  CONNECT( _d->extMenu, onSelectOverlayType(), _d.data(), Impl::resolveSelectLayer );
  CONNECT( _d->extMenu, onEmpireMapShow(), _d.data(), Impl::showEmpireMapWindow );
  CONNECT( _d->extMenu, onAdvisorsWindowShow(), _d.data(), Impl::showAdvisorsWindow );
  CONNECT( _d->extMenu, onMissionTargetsWindowShow(), _d.data(), Impl::showMissionTaretsWindow );
  CONNECT( _d->extMenu, onMessagesShow(), _d.data(), Impl::showMessagesWindow );

  CONNECT( city, onDisasterEvent(), &_d->alarmsHolder, AlarmEventHolder::add );
  CONNECT( _d->extMenu, onSwitchAlarm(), &_d->alarmsHolder, AlarmEventHolder::next );
  CONNECT( &_d->alarmsHolder, onMoveToAlarm(), _d->renderer.camera(), Camera::setCenter );
  CONNECT( &_d->alarmsHolder, onAlarmChange(), _d->extMenu, ExtentMenu::setAlarmEnabled );

  CONNECT( _d->renderer.camera(), onPositionChanged(), mmap, Minimap::setCenter );
  CONNECT( _d->renderer.camera(), onPositionChanged(), _d.data(), Impl::saveCameraPos );
  CONNECT( mmap, onCenterChange(), _d->renderer.camera(), Camera::setCenter );
  CONNECT( &_d->renderer, onLayerSwitch(), _d->extMenu, ExtentMenu::changeOverlay );
  CONNECT( &_d->renderer, onLayerSwitch(), _d.data(), Impl::layerChanged );

  _d->showMissionTaretsWindow();
  _d->renderer.camera()->setCenter( city->cameraPos() );
}

std::string Level::nextFilename() const{  return _d->mapToLoad;}

void Level::Impl::showSaveDialog()
{
  vfs::Directory saveDir = GameSettings::get( GameSettings::savedir ).toString();
  std::string defaultExt = GameSettings::get( GameSettings::saveExt ).toString();

  if( !saveDir.exist() )
  {
    gui::DialogBox* dialog = new gui::DialogBox( game->gui()->rootWidget(), Rect(), "##warning##",
                                                 "##save_directory_not_exist##", gui::DialogBox::btnOk );
    dialog->show();
    return;
  }

  SaveDialog* dialog = new SaveDialog( game->gui()->rootWidget(), saveDir, defaultExt, -1 );
  CONNECT( dialog, onFileSelected(), game, Game::save );
}

void Level::Impl::setVideoOptions()
{
  events::GameEventPtr event = events::SetVideoSettings::create();
  event->dispatch();
}

void Level::Impl::showGameSpeedOptionsDialog()
{
  GameSpeedOptionsWindow* dialog = new GameSpeedOptionsWindow( game->gui()->rootWidget(),
                                                               game->timeMultiplier(),                                                               
                                                               0,
                                                               GameSettings::get( GameSettings::autosaveInterval ) );

  CONNECT( dialog, onGameSpeedChange(), game, Game::setTimeMultiplier );
  CONNECT( dialog, onScrollSpeedChange(), renderer.camera(), Camera::setScrollSpeed );
  CONNECT( dialog, onAutosaveIntervalChange(), this, Impl::setAutosaveInterval );
}

void Level::Impl::showCityOptionsDialog()
{
  CityOptionsWindow* wnd = new CityOptionsWindow( game->gui()->rootWidget(),
                                                  game->city() );
  wnd->show();
}

void Level::Impl::resolveWarningMessage(std::string text )
{
  events::GameEventPtr e = events::WarningMessageEvent::create( text );
  e->dispatch();
}

void Level::Impl::saveCameraPos(Point p)
{
  Size scrSize = engine->screenSize();
  Tile* tile = renderer.camera()->at( Point( scrSize.width()/2, scrSize.height()/2 ), false );

  if( tile )
  {
    game->city()->setCameraPos( tile->pos() );
  }
}

void Level::Impl::showSoundOptionsWindow()
{
  events::GameEventPtr e = events::SetSoundOptions::create();
  e->dispatch();
}

void Level::Impl::makeEnemy()
{
  WalkerPtr wlk = WalkerManager::instance().create( constants::walker::etruscanArcher, game->city() );
  EnemySoldierPtr enemy = ptr_cast<EnemySoldier>( wlk );
  if( enemy.isValid() )
  {
    enemy->send2City( game->city()->borderInfo().roadEntry );
  }
}

void Level::Impl::makeFastSave() { game->save( createFastSaveName().toString() ); }

void Level::Impl::showTileHelp()
{
  const Tile& tile = game->city()->tilemap().at( selectedTilePos );  // tile under the cursor (or NULL)
  events::GameEventPtr e = events::ShowTileInfo::create( tile.pos() );
  e->dispatch();
}

void Level::Impl::showMessagesWindow()
{
  new ScribesMessagestWindow( game->gui()->rootWidget(), game->city() );
}

void Level::Impl::setAutosaveInterval(int value)
{
  GameSettings::set( GameSettings::autosaveInterval, value );
  GameSettings::save();
}

void Level::Impl::layerChanged(int layer)
{
  if( layer > citylayer::simple && layer < citylayer::build )
  {
    lastLayerId = (citylayer::Type)layer;
  }
}

void Level::Impl::makeFullScreenshot()
{
  Tilemap& tmap = game->city()->tilemap();
  int mapSize = tmap.size();
  Tile& lastRightTile = tmap.at( mapSize-1, mapSize-1 );
  Tile& lastBottomTile = tmap.at( mapSize-1, 0 );
  Point lastRightPos = TileHelper::tilepos2screen( lastRightTile.pos() );
  Point lastBottomPos = TileHelper::tilepos2screen( lastBottomTile.pos() );
  Size fullPicSize( lastRightPos.x(), abs( lastBottomPos.y() ) * 2 );

  TilesArray ret;
  for( int y=0; y < mapSize; y++ )
  {
    for( int t=0; t <= y; t++ )
    {
      ret.push_back( &tmap.at( t, mapSize - 1 - ( y - t ) ) );
    }
  }

  for( int x=1; x < mapSize; x++ )
  {
    for( int t=0; t < mapSize-x; t++ )
    {
      ret.push_back( &tmap.at( x + t, t ) );
    }
  }

  PictureRef fullPic;
  fullPic.init( fullPicSize );
  Point doffset( 0, fullPicSize.height() / 2 );
  foreach( tile, ret )
  {
    Tile* t = *tile;
    if( t->masterTile() )
      t = t->masterTile();       

    const Picture& tpic = t->overlay().isValid()
                            ? t->overlay()->picture()
                            : t->picture();

    Rect srcRect( 0, 0, tpic.width(), tpic.height() );
    fullPic->draw( tpic, srcRect, t->mappos() + doffset - tpic.offset() );
  }

  std::string filename = getScreenshotName();
  PictureConverter::save( *fullPic, filename, "PNG" );
}

std::string Level::Impl::getScreenshotName()
{
  DateTime time = DateTime::getCurrenTime();
  return StringHelper::format( 0xff, "oc3_[%04d_%02d_%02d_%02d_%02d_%02d].png",
                               time.year(), time.month(), time.day(),
                               time.hour(), time.minutes(), time.seconds() );
}

void Level::_resolveLoadGame( std::string filename )
{
  _d->mapToLoad = filename.empty()
                      ? _d->createFastSaveName().toString()
                      : filename;
  _resolveSwitchMap();
}

void Level::_resolveEscapeButton()
{
  NEvent e;

  e.EventType = sEventKeyboard;
  e.keyboard.key = KEY_ESCAPE;
  e.keyboard.pressed = false;
  e.keyboard.shift = false;
  e.keyboard.control = false;
  e.keyboard.symbol = 0;

  handleEvent( e );
}

void Level::_resolveEnterButton()
{
  NEvent e;

  e.EventType = sEventKeyboard;
  e.keyboard.key = KEY_RETURN;
  e.keyboard.pressed = false;
  e.keyboard.shift = false;
  e.keyboard.control = false;
  e.keyboard.symbol = 0;

  handleEvent( e );
}


void Level::_showIngameMenu()
{
  gui::Widget* p = _d->game->gui()->rootWidget();
  gui::Widget* menu = new Label( p, Rect( 0, 0, 500, 450 ), "", false, Label::bgWhiteFrame );

  menu->setCenter( p->center() );
  menu->setupUI( GameSettings::rcpath( "/gui/ingamemenu_android.gui") );
  WidgetEscapeCloser::insertTo( menu );

  PushButton* btnContinue = findChildA<PushButton*>( "btnContinue", true, menu );
  PushButton* btnSave = findChildA<PushButton*>( "btnSave", true, menu );
  PushButton* btnLoad = findChildA<PushButton*>( "btnLoad", true, menu );
  PushButton* btnRestart = findChildA<PushButton*>( "btnRestart", true, menu );
  PushButton* btnMainMenu = findChildA<PushButton*>( "btnMainMenu", true, menu );
  PushButton* btnExit = findChildA<PushButton*>( "btnExit", true, menu );

  CONNECT( btnContinue, onClicked(), menu, Label::deleteLater );
  CONNECT( btnExit, onClicked(), this, Level::_resolveExitGame );
  CONNECT( btnSave, onClicked(), this, Level::_resolveShowLoadGameWnd );
  CONNECT( btnLoad, onClicked(), _d.data(), Impl::showSaveDialog  );
  CONNECT( btnRestart, onClicked(), this, Level::_resolveRestart );
  CONNECT( btnMainMenu, onClicked(), this, Level::_resolveEndGame );
}

vfs::Path Level::Impl::createFastSaveName(const std::string& type, const std::string& postfix )
{
  std::string typesave = type.empty() ? SETTINGS_VALUE( fastsavePostfix ).toString() : type;
  vfs::Path filename = game->city()->name()
                       + typesave
                       + postfix
                       + SETTINGS_VALUE( saveExt ).toString();

  vfs::Directory saveDir = SETTINGS_VALUE( savedir ).toString();

  return saveDir/filename;
}

void Level::_resolveSwitchMap()
{
  bool isNextBriefing = vfs::Path( _d->mapToLoad ).isMyExtension( ".briefing" );
  _d->result = isNextBriefing ? Level::loadBriefing : Level::loadGame;
  stop();
}

void Level::Impl::showEmpireMapWindow()
{
  events::GameEventPtr e;
  if( game->empire()->isAvailable() ) { e = events::ShowEmpireMapWindow::create( true ); }
  else {  e = events::WarningMessageEvent::create( "##not_available##" ); }

  if( e.isValid() ) e->dispatch();
}

void Level::draw()
{ 
  _d->renderer.render();

  _d->game->gui()->beforeDraw();
  _d->game->gui()->draw();
}

void Level::animate( unsigned int time )
{
  _d->renderer.animate( time );

  if( GameDate::isMonthChanged() )
  {
    PlayerCityPtr city = _d->game->city();
    const city::VictoryConditions& wt = city->victoryConditions();

    int culture = city->culture();
    int prosperity = city->prosperity();
    int favour = city->favour();
    int peace = city->favour();
    int population = city->population();
    bool success = wt.isSuccess( culture, prosperity, favour, peace, population );

    if( success )
    {
      std::string newTitle = wt.getNewTitle();
      gui::WinMissionWindow* wnd = new gui::WinMissionWindow( _d->game->gui()->rootWidget(), newTitle, false );

      _d->mapToLoad = wt.getNextMission();

      CONNECT( wnd, onAcceptAssign(), this, Level::_resolveSwitchMap );
    }

    int autosaveInterval = GameSettings::get( GameSettings::autosaveInterval );
    if( GameDate::current().month() % autosaveInterval == 0 )
    {
      static int rotate = 0;
      rotate = (rotate + 1) % 3;
      vfs::Path filename = _d->createFastSaveName( "autosave", StringHelper::i2str( rotate ) );
      _d->game->save( filename.toString() );
    }
  }
}

void Level::afterFrame()
{
}

void Level::handleEvent( NEvent& event )
{
  //After MouseDown events are send to the same target till MouseUp
  GuiEnv& gui = *_d->game->gui();

  if (event.EventType == sEventQuit)
  {
    _resolveExitGame();
    return;
  }

  static enum _MouseEventTarget
  {
    _MET_NONE,
    _MET_GUI,
    _MET_TILES
  } _mouseEventTarget = _MET_NONE;

  if( event.EventType == sEventKeyboard && !event.keyboard.pressed)
  {
    if( event.keyboard.control && !event.keyboard.shift )
    {
      bool handled = true;
      switch( event.keyboard.key )
      {
      case KEY_SPACE:
      {
        int newLayer = _d->renderer.layerType() == citylayer::simple
                          ? _d->lastLayerId : citylayer::simple;
        _d->renderer.setLayer( newLayer );
      }
      break;

      case KEY_KEY_F: _d->renderer.setLayer( citylayer::fire ); break;
      case KEY_KEY_D: _d->renderer.setLayer( citylayer::damage ); break;
      case KEY_KEY_C: _d->renderer.setLayer( citylayer::crime ); break;
      case KEY_KEY_T: _d->renderer.setLayer( citylayer::troubles ); break;
      case KEY_KEY_W: _d->renderer.setLayer( citylayer::water ); break;
      case KEY_F1:    _d->showAdvisorsWindow( advisor::employers ); break;
      case KEY_F2:    _d->showAdvisorsWindow( advisor::military ); break;
      case KEY_F3:    _d->showAdvisorsWindow( advisor::empire ); break;
      case KEY_F4:    _d->showAdvisorsWindow( advisor::ratings ); break;
      case KEY_F5:    _d->showAdvisorsWindow( advisor::trading ); break;
      case KEY_F6:    _d->showAdvisorsWindow( advisor::population ); break;
      case KEY_F7:    _d->showAdvisorsWindow( advisor::health ); break;
      case KEY_F8:    _d->showAdvisorsWindow( advisor::education ); break;
      case KEY_F9:    _d->showAdvisorsWindow( advisor::entertainment ); break;
      case KEY_F10:   _d->showAdvisorsWindow( advisor::religion ); break;
      case KEY_F11:   _d->showAdvisorsWindow( advisor::finance ); break;
      case KEY_F12:   _d->showAdvisorsWindow( advisor::main ); break;
      default:
        handled = false;
      break;
      }

      if( handled )
        return;
    }

    switch( event.keyboard.key )
    {
    case KEY_MINUS:
    case KEY_PLUS:
    case KEY_SUBTRACT:
    case KEY_ADD:
    {
      events::GameEventPtr e = events::ChangeSpeed::create( (event.keyboard.key == KEY_MINUS || event.keyboard.key == KEY_SUBTRACT)
                                                            ? -10 : +10 );
      e->dispatch();
    }
    break;

    case KEY_KEY_P:
    {
      events::GameEventPtr e = events::Pause::create( events::Pause::toggle );
      e->dispatch();
    }
    break;

    case KEY_COMMA:
    case KEY_PERIOD:
    {
      events::GameEventPtr e = events::Step::create( event.keyboard.key == KEY_COMMA ? 1 : 25);
      e->dispatch();
    }
    break;

    case KEY_F5: _d->makeFastSave(); break;
    case KEY_F9: _resolveLoadGame( "" ); break;

    case KEY_F10:
      if( !event.keyboard.shift )
        _d->makeScreenShot();
      else
        _d->makeFullScreenshot();
    break;

    case KEY_KEY_R:
    {
      if( event.keyboard.shift )
      {
        VariantMap rqvm = SaveAdapter::load( GameSettings::rcpath( "test_request.model" ) );
        events::GameEventPtr e = events::PostponeEvent::create( "", rqvm );
        e->dispatch();
        return;
      }
    }
    break;

    case KEY_KEY_I:
    {
      if( event.keyboard.shift )
      {
        world::CityPtr rome = _d->game->empire()->rome();
        PlayerCityPtr plCity = _d->game->city();

        world::RomeChastenerArmyPtr army = world::RomeChastenerArmy::create( _d->game->empire() );
        army->setBase( rome );
        army->attack( ptr_cast<world::Object>( plCity ) );
      }
    }
    break;

    case KEY_KEY_Y:
    {
      if( event.keyboard.shift )
      {
        _d->game->player()->appendMoney( 1000 );
      }
    }
    break;

    case KEY_KEY_M:
    {
      if( event.keyboard.shift )
      {
        religion::rome::Pantheon::mars()->updateRelation( -101.f, _d->game->city() );
        return;
      }
    }
    break;

    case KEY_F11:
      if( event.keyboard.shift )
      {
        events::GameEventPtr e = events::RandomAnimals::create( walker::wolf, 10 );
        e->dispatch();
      }
      else { _d->makeEnemy(); }
    break;    

    case KEY_ESCAPE:
    {
      Widget::Widgets children = _d->game->gui()->rootWidget()->children();
      foreach( it, children )
      {
        bool handled = (*it)->onEvent( event );
        if( handled )
            break;
      }
    }
    break;

    default:
    break;
    }
  }

  for( Impl::EventHandlers::iterator it=_d->eventHandlers.begin(); it != _d->eventHandlers.end(); )
  {
    (*it)->handleEvent( event );
    if( (*it)->finished() ) { it = _d->eventHandlers.erase( it ); }
    else{ ++it; }
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
      _d->selectedTilePos = _d->renderer.getTilePos( event.mouse.pos() );
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

void Level::Impl::makeScreenShot()
{
  std::string filename = getScreenshotName();
  Logger::warning( "Level: create screenshot %s", filename.c_str() );

  Engine::instance().createScreenshot( filename );
  events::GameEventPtr e = events::WarningMessageEvent::create( "Screenshot save to " + filename );
  e->dispatch();
}

int Level::result() const {  return _d->result; }
bool Level::installEventHandler(EventHandlerPtr handler) { _d->eventHandlers.push_back( handler ); return true; }
void Level::Impl::resolveCreateConstruction( int type ){  renderer.setMode( BuildMode::create( TileOverlay::Type( type ) ) );}
void Level::Impl::resolveRemoveTool(){  renderer.setMode( DestroyMode::create() );}
void Level::Impl::resolveSelectLayer( int type ){  renderer.setMode( LayerMode::create( type ) );}
void Level::Impl::showAdvisorsWindow(){  showAdvisorsWindow( advisor::employers ); }
void Level::Impl::showTradeAdvisorWindow(){  showAdvisorsWindow( advisor::trading ); }
void Level::Impl::showMissionTaretsWindow(){  MissionTargetsWindow::create( game->gui()->rootWidget(), game->city() ); }
void Level::_resolveEndGame(){  _d->result = Level::mainMenu;  stop();}
void Level::_resolveRestart() { _d->result = Level::restart;  stop();}
void Level::setCameraPos(TilePos pos) {  _d->renderer.camera()->setCenter( pos ); }
void Level::_exitGame(){ _d->result = Level::quitGame;  stop();}

void Level::_resolveExitGame()
{
  DialogBox* dlg = new DialogBox( _d->game->gui()->rootWidget(), Rect(), "", _("##exit_without_saving_question##"), DialogBox::btnOkCancel );
  CONNECT( dlg, onOk(), this, Level::_exitGame );
  CONNECT( dlg, onCancel(), dlg, DialogBox::deleteLater );
}

void Level::Impl::showAdvisorsWindow( const advisor::Type advType )
{  
  events::GameEventPtr e = events::ShowAdvisorWindow::create( true, advType );
  e->dispatch();
}

void Level::_resolveShowLoadGameWnd()
{
  gui::Widget* parent = _d->game->gui()->rootWidget();

  vfs::Path savesPath = GameSettings::get( GameSettings::savedir ).toString();
  std::string defaultExt = GameSettings::get( GameSettings::saveExt ).toString();
  gui::LoadFileDialog* wnd = new gui::LoadFileDialog( parent, Rect(), savesPath, defaultExt,-1 );

  CONNECT( wnd, onSelectFile(), this, Level::_resolveLoadGame );
  wnd->setTitle( _("##mainmenu_loadgame##") );
}

}//end namespace scene
