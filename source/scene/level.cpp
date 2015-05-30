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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "level.hpp"
#include "gfx/engine.hpp"
#include "city/victoryconditions.hpp"
#include "core/exception.hpp"
#include "gui/rightpanel.hpp"
#include "gui/loadfiledialog.hpp"
#include "game/resourcegroup.hpp"
#include "gui/environment.hpp"
#include "gui/topmenu.hpp"
#include "gui/menu.hpp"
#include "events/changespeed.hpp"
#include "core/event.hpp"
#include "gui/dialogbox.hpp"
#include "game/infoboxmanager.hpp"
#include "objects/objects_factory.hpp"
#include "gfx/renderermode.hpp"
#include "layers/constants.hpp"
#include "gui/message_stack_widget.hpp"
#include "core/time.hpp"
#include "core/utils.hpp"
#include "gui/empiremap_window.hpp"
#include "gui/advisors_window.hpp"
#include "game/alarm_event_holder.hpp"
#include "gfx/city_renderer.hpp"
#include "game/game.hpp"
#include "gui/senate_popup_info.hpp"
#include "game/funds.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "game/settings.hpp"
#include "gui/mission_target_window.hpp"
#include "gui/label.hpp"
#include "gfx/helper.hpp"
#include "core/gettext.hpp"
#include "gui/minimap_window.hpp"
#include "gui/window_gamespeed_options.hpp"
#include "events/setvideooptions.hpp"
#include "core/logger.hpp"
#include "game/patrolpointeventhandler.hpp"
#include "gfx/tilemap_camera.hpp"
#include "city/ambientsound.hpp"
#include "gui/win_mission_window.hpp"
#include "events/showempiremapwindow.hpp"
#include "events/showadvisorwindow.hpp"
#include "gui/texturedbutton.hpp"
#include "sound/engine.hpp"
#include "events/showtileinfo.hpp"
#include "gui/androidactions.hpp"
#include "events/setsoundoptions.hpp"
#include "gui/widgetescapecloser.hpp"
#include "gui/scribesmessages.hpp"
#include "core/foreach.hpp"
#include "world/romechastenerarmy.hpp"
#include "events/warningmessage.hpp"
#include "events/postpone.hpp"
#include "gfx/pictureconverter.hpp"
#include "gui/city_options_window.hpp"
#include "gui/widget_helper.hpp"
#include "core/timer.hpp"
#include "city/cityservice_military.hpp"
#include "city/cityservice_info.hpp"
#include "layers/layer.hpp"
#include "game/debug_handler.hpp"
#include "game/hotkey_manager.hpp"
#include "city/build_options.hpp"
#include "events/movecamera.hpp"
#include "events/missionwin.hpp"
#include "events/savegame.hpp"
#include "core/tilerect.hpp"
#include "city/active_points.hpp"
#include "city/statistic.hpp"
#include "events/loadgame.hpp"
#include "city/states.hpp"

using namespace gui;
using namespace events;
using namespace gfx;
using namespace city;

namespace scene
{
typedef std::vector< EventHandlerPtr > EventHandlers;
const int topMenuHeight = 23;

class Level::Impl
{
public:
  EventHandlers eventHandlers;
  Minimap* mmap;
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
  DebugHandler dhandler;

  int result;

public:
  void showSaveDialog();
  void showEmpireMapWindow();
  void showAdvisorsWindow(const advisor::Type advType );
  void showAdvisorsWindow();
  void showMissionTaretsWindow();
  void showTradeAdvisorWindow();
  void resolveCreateConstruction( int type );
  void resolveSelectLayer( int type );
  void checkFailedMission(Level *lvl, bool forceFailed=false);
  void checkWinMission(Level *lvl, bool forceWin=false);
  void resolveRemoveTool();
  void makeScreenShot();
  void setVideoOptions();
  void showGameSpeedOptionsDialog();
  void showCityOptionsDialog();
  void resolveWarningMessage( std::string );
  void saveCameraPos(Point p);
  void showSoundOptionsWindow();
  void makeFastSave();
  void showTileHelp();
  void showMessagesWindow();
  void setAutosaveInterval( int value );
  void layerChanged( int layer );
  void makeFullScreenshot();
  void extendReign( int years );
  void saveScrollSpeed( int speed );
  void changeZoom( int delta );
  void handleDirectionChange( Direction direction );
  void initRender();
  void initMainUI();
  void installHandlers( Base* scene);
  void initSound();
  void initTabletUI(Level* scene);

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

void Level::Impl::initRender()
{
  bool oldGraphics = SETTINGS_VALUE( oldgfx ).toBool() || !SETTINGS_VALUE( c3gfx ).toString().empty();
  renderer.initialize( game->city(), engine, game->gui(), oldGraphics );
  renderer.setViewport( engine->screenSize() );
  renderer.camera()->setScrollSpeed( SETTINGS_VALUE( scrollSpeed ) );
}

void Level::Impl::initMainUI()
{  
  PlayerCityPtr city = game->city();
  gui::Ui& ui = *game->gui();

  ui.clear();

  Picture rPanelPic( ResourceGroup::panelBackground, PicID::rightPanelTx );
  Engine& engine = Engine::instance();

  Rect rPanelRect( engine.virtualSize().width() - rPanelPic.width(), topMenuHeight,
                   engine.virtualSize().width(), engine.virtualSize().height() );

  rightPanel = MenuRigthPanel::create( ui.rootWidget(), rPanelRect, rPanelPic);

  topMenu = new TopMenu( ui.rootWidget(), topMenuHeight, !city->getOption( PlayerCity::c3gameplay ) );
  topMenu->setPopulation( game->city()->states().population );
  topMenu->setFunds( game->city()->treasury().money() );

  menu = Menu::create( ui.rootWidget(), -1, city );
  menu->setPosition( Point( engine.virtualSize().width() - rightPanel->width(),
                                topMenu->height() ) );

  extMenu = ExtentMenu::create( ui.rootWidget(), -1, city );
  extMenu->setPosition( Point( engine.virtualSize().width() - extMenu->width() - rightPanel->width(),
                               topMenu->height() ) );

  mmap = new Minimap( extMenu, Rect( Point( 8, 35), Size( 144, 110 ) ),
                      city,
                      *renderer.camera() );

  WindowMessageStack::create( ui.rootWidget() );

  rightPanel->bringToFront();
}

void Level::Impl::installHandlers( Base* scene )
{
  scene->installEventHandler( PatrolPointEventHandler::create( *game, renderer ) );
}

void Level::Impl::initSound()
{
  SmartPtr<city::AmbientSound> sound = statistic::getService<city::AmbientSound>( game->city() );
  if( sound.isValid() )
    sound->setCamera( renderer.camera() );
}

void Level::Impl::initTabletUI( Level* scene )
{
  //specific tablet actions bar
  tablet::ActionsBar* tabletUi = new tablet::ActionsBar( game->gui()->rootWidget() );
  tablet::ActionsHandler::assignTo( tabletUi, scene );

  tabletUi->setVisible( SETTINGS_VALUE(showTabletMenu) );
}

void Level::initialize()
{
  PlayerCityPtr city = _d->game->city();
  gui::Ui& ui = *_d->game->gui();

  _d->initRender();
  _d->initMainUI();
  _d->installHandlers( this );
  _d->initSound();
  _d->initTabletUI( this );

  //connect elements
  CONNECT( _d->topMenu, onSave(),                 _d.data(),         Impl::showSaveDialog )
  CONNECT( _d->topMenu, onExit(),                 this,              Level::_requestExitGame )
  CONNECT( _d->topMenu, onLoad(),                 this,              Level::_showLoadDialog )
  CONNECT( _d->topMenu, onEnd(),                  this,              Level::exit )
  CONNECT( _d->topMenu, onRestart(),              this,              Level::restart )
  CONNECT( _d->topMenu, onRequestAdvisor(),       _d.data(),         Impl::showAdvisorsWindow )
  CONNECT( _d->topMenu, onShowVideoOptions(),     _d.data(),         Impl::setVideoOptions )
  CONNECT( _d->topMenu, onShowSoundOptions(),     _d.data(),         Impl::showSoundOptionsWindow )
  CONNECT( _d->topMenu, onShowGameSpeedOptions(), _d.data(),         Impl::showGameSpeedOptionsDialog )
  CONNECT( _d->topMenu, onShowCityOptions(),      _d.data(),         Impl::showCityOptionsDialog )
  CONNECT( _d->topMenu, onShowExtentInfo(),       _d->extMenu,       ExtentMenu::showInfo )

  CONNECT( city, onPopulationChanged(),           _d->topMenu,       TopMenu::setPopulation )
  CONNECT( city, onFundsChanged(),                _d->topMenu,       TopMenu::setFunds )
  CONNECT( city, onWarningMessage(),              _d.data(),         Impl::resolveWarningMessage )

  CONNECT( _d->menu, onCreateConstruction(),      _d.data(),         Impl::resolveCreateConstruction )
  CONNECT( _d->menu, onRemoveTool(),              _d.data(),         Impl::resolveRemoveTool )
  CONNECT( _d->menu, onHide(),                    _d->extMenu,       ExtentMenu::maximize )

  CONNECT( _d->extMenu, onHide(),                 _d->menu,          Menu::maximize )
  CONNECT( _d->extMenu, onCreateConstruction(),   _d.data(),         Impl::resolveCreateConstruction )
  CONNECT( _d->extMenu, onRemoveTool(),           _d.data(),         Impl::resolveRemoveTool )
  CONNECT( _d->extMenu, onRotateRight(),          &_d->renderer,     CityRenderer::rotateRight )
  CONNECT( _d->extMenu, onRotateLeft(),           &_d->renderer,     CityRenderer::rotateLeft )
  CONNECT( _d->extMenu, onSelectOverlayType(),    _d.data(),         Impl::resolveSelectLayer )
  CONNECT( _d->extMenu, onEmpireMapShow(),        _d.data(),         Impl::showEmpireMapWindow )
  CONNECT( _d->extMenu, onAdvisorsWindowShow(),   _d.data(),         Impl::showAdvisorsWindow )
  CONNECT( _d->extMenu, onMissionTargetsWindowShow(), _d.data(),     Impl::showMissionTaretsWindow )
  CONNECT( _d->extMenu, onMessagesShow(),         _d.data(),         Impl::showMessagesWindow )
  CONNECT( _d->extMenu, onSwitchAlarm(),          &_d->alarmsHolder, AlarmEventHolder::next )

  CONNECT( city, onDisasterEvent(),               &_d->alarmsHolder, AlarmEventHolder::add )
  CONNECT( &_d->alarmsHolder, onMoveToAlarm(),    _d->renderer.camera(), Camera::setCenter )
  CONNECT( &_d->alarmsHolder, onAlarmChange(),    _d->extMenu,       ExtentMenu::setAlarmEnabled )

  CONNECT( _d->renderer.camera(), onPositionChanged(), _d->mmap,     Minimap::setCenter )
  CONNECT( _d->renderer.camera(), onPositionChanged(), _d.data(),    Impl::saveCameraPos )
  CONNECT( _d->renderer.camera(), onDirectionChanged(), _d.data(),   Impl::handleDirectionChange )
  CONNECT( _d->mmap, onCenterChange(), _d->renderer.camera(),        Camera::setCenter )
  CONNECT( _d->mmap, onZoomChange(), _d->renderer.camera(),          gfx::Camera::changeZoom )
  CONNECT( &_d->renderer, onLayerSwitch(), _d->extMenu,              ExtentMenu::changeOverlay )
  CONNECT( &_d->renderer, onLayerSwitch(), _d.data(),                Impl::layerChanged )

  _d->showMissionTaretsWindow();
  _d->renderer.camera()->setCenter( city->cameraPos() );

  _d->dhandler.insertTo( _d->game, _d->topMenu );
  _d->dhandler.setVisible( false );

  CONNECT( &_d->dhandler, onWinMission(), _d.data(), Impl::checkWinMission )
  CONNECT( &_d->dhandler, onFailedMission(), _d.data(), Impl::checkFailedMission )
#ifdef DEBUG  
  _d->dhandler.setVisible( true );
#endif

#ifdef CAESARIA_USE_STEAM
  dialog::Information( &ui, "Please note", "Black object are not done yet and will be added as soon as finished." );
#endif
}

std::string Level::nextFilename() const{  return _d->mapToLoad;}

void Level::Impl::showSaveDialog() {  GameEventPtr e = ShowSaveDialog::create();   e->dispatch(); }
void Level::Impl::setVideoOptions(){  GameEventPtr e = SetVideoSettings::create(); e->dispatch(); }

void Level::Impl::showGameSpeedOptionsDialog()
{
  dialog::SpeedOptions* dialog = new dialog::SpeedOptions( game->gui()->rootWidget(),
                                                           game->timeMultiplier(),
                                                           SETTINGS_VALUE( scrollSpeed ),
                                                           SETTINGS_VALUE( autosaveInterval ) );

  CONNECT( dialog, onGameSpeedChange(), game, Game::setTimeMultiplier );
  CONNECT( dialog, onScrollSpeedChange(), renderer.camera(), Camera::setScrollSpeed );
  CONNECT( dialog, onScrollSpeedChange(), this, Impl::saveScrollSpeed );
  CONNECT( dialog, onAutosaveIntervalChange(), this, Impl::setAutosaveInterval );
}

void Level::Impl::showCityOptionsDialog()
{
  dialog::CityOptions* wnd = new dialog::CityOptions( game->gui()->rootWidget(),
                                                      game->city() );
  wnd->show();
}

void Level::Impl::resolveWarningMessage(std::string text)
{
  GameEventPtr e = WarningMessage::create( text, WarningMessage::neitral );
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
  GameEventPtr e = ChangeSoundOptions::create();
  e->dispatch();
}

void Level::Impl::makeFastSave() { game->save( createFastSaveName().toString() ); }

void Level::Impl::showMessagesWindow()
{
  unsigned int id = Hash( CAESARIA_STR_A(dialog::ScribesMessages) );
  Widget* wnd = game->gui()->findWidget( id );

  if( wnd == 0 )
  {
    wnd = new dialog::ScribesMessages( game->gui()->rootWidget(), game->city() );
  }
  else
  {
    wnd->bringToFront();
  }
}

void Level::Impl::setAutosaveInterval(int value)
{
  SETTINGS_SET_VALUE( autosaveInterval, value );
  game::Settings::save();
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
  Point lastRightPos = tile::tilepos2screen( lastRightTile.pos() );
  Point lastBottomPos = tile::tilepos2screen( lastBottomTile.pos() );
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

  Picture fullPic = Picture( fullPicSize, 0, true );
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
    //fullPic->draw( tpic, srcRect, t->mappos() + doffset - tpic.offset() );
  }

  std::string filename = getScreenshotName();
  PictureConverter::save( fullPic, filename, "PNG" );
}

void Level::Impl::extendReign(int years)
{
  VictoryConditions vc;
  vc = game->city()->victoryConditions();
  vc.addReignYears( years );

  game->city()->setVictoryConditions( vc );
}

void Level::Impl::handleDirectionChange(Direction direction)
{
  direction::Helper dHelper;

  GameEventPtr e = WarningMessage::create( _("##" + dHelper.findName( direction ) + "##"), 1 );
  e->dispatch();
}

std::string Level::Impl::getScreenshotName()
{
  DateTime time = DateTime::currenTime();
  vfs::Path filename = utils::format( 0xff, "oc3_[%04d_%02d_%02d_%02d_%02d_%02d].png",
                                      time.year(), time.month(), time.day(),
                                      time.hour(), time.minutes(), time.seconds() );
  vfs::Directory screenDir = SETTINGS_VALUE( screenshotDir ).toString();
  return (screenDir/filename).toString();
}

void Level::loadStage( std::string filename )
{
  _d->mapToLoad = filename.empty()
                      ? _d->createFastSaveName().toString()
                      : filename;
  _resolveSwitchMap();
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
  _d->result = isNextBriefing ? Level::res_briefing : Level::res_load;
  stop();
}

void Level::Impl::showEmpireMapWindow()
{
  GameEventPtr e = ShowEmpireMap::create( true );
  e->dispatch();
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

  if( game::Date::isWeekChanged() )
  {
    _d->checkWinMission( this );
    _d->checkFailedMission( this );
  }

  if( game::Date::isMonthChanged() )
  {
    int autosaveInterval = SETTINGS_VALUE( autosaveInterval );
    if( game::Date::current().month() % autosaveInterval == 0 )
    {
      static int rotate = 0;
      rotate = (rotate + 1) % 3;
      vfs::Path filename = _d->createFastSaveName( "autosave", utils::i2str( rotate ) );
      _d->game->save( filename.toString() );
    }
  }
}

void Level::afterFrame() {}

void Level::handleEvent( NEvent& event )
{
  //After MouseDown events are send to the same target till MouseUp
  Ui& gui = *_d->game->gui();

  if( event.EventType == sEventQuit )
  {
    _requestExitGame();
    return;
  }

  for( EventHandlers::iterator it=_d->eventHandlers.begin(); it != _d->eventHandlers.end(); )
  {
    (*it)->handleEvent( event );
    if( (*it)->finished() ) { it = _d->eventHandlers.erase( it ); }
    else{ ++it; }
  }

  bool eventResolved = gui.handleEvent( event );

  if( !eventResolved )
  {
    eventResolved = _tryExecHotkey( event );
  }

  if( !eventResolved )
  {
    _d->renderer.handleEvent( event );
    _d->selectedTilePos = _d->renderer.screen2tilepos( event.mouse.pos() );
  }
}

void Level::Impl::makeScreenShot()
{
  std::string filename = getScreenshotName();
  Logger::warning( "Level: create screenshot " + filename );

  Engine::instance().createScreenshot( filename );
  GameEventPtr e = WarningMessage::create( "Screenshot save to " + filename, WarningMessage::neitral );
  e->dispatch();
}

void Level::Impl::checkFailedMission( Level* lvl, bool forceFailed )
{
  PlayerCityPtr pcity = game->city();

  const city::VictoryConditions& vc = pcity->victoryConditions();
  MilitaryPtr mil = statistic::getService<city::Military>( pcity );
  InfoPtr info = statistic::getService<city::Info>( pcity );

  if( mil.isValid() && info.isValid()  )
  {
    const city::Info::MaxParameters& params = info->maxParams();

    bool failedByDestroy = mil->threatValue() > 0 && params[ Info::population ].value > 0 && !pcity->states().population;
    bool failedByTime = ( !vc.isSuccess() && game::Date::current() > vc.finishDate() );

    if( failedByDestroy || failedByTime || forceFailed )
    {
      game->pause();
      Window* wnd = new Window( game->gui()->rootWidget(),
                                Rect( 0, 0, 400, 220 ), "" );
      Label* lb = new Label( wnd, Rect( 10, 10, 390, 110 ), _("##mission_failed##") );
      lb->setTextAlignment( align::center, align::center );
      lb->setFont( Font::create( FONT_6 ) );

      PushButton* btnRestart = new PushButton( wnd, Rect( 20, 120, 380, 144), _("##restart_mission##") );
      btnRestart->setTooltipText( _("##restart_mission_tip##") );
      PushButton* btnMenu = new PushButton( wnd, Rect( 20, 150, 380, 174), _("##exit_to_main_menu##") );

      wnd->setCenter( game->gui()->rootWidget()->center() );
      wnd->setModal();

      CONNECT( btnRestart, onClicked(), lvl, Level::restart );
      CONNECT( btnMenu, onClicked(), lvl, Level::exit );
    }
  }
}

void Level::Impl::checkWinMission( Level* lvl, bool force )
{
  PlayerCityPtr city = game->city();
  const VictoryConditions& wt = city->victoryConditions();

  int culture = city->culture();
  int prosperity = city->prosperity();
  int favour = city->favour();
  int peace = city->peace();
  int population = city->states().population;
  bool success = wt.isSuccess( culture, prosperity, favour, peace, population );

  if( success || force )
  {
    dialog::WinMission* wnd = new dialog::WinMission( game->gui()->rootWidget(),
                                                      wt.newTitle(), wt.winText(),
                                                      wt.winSpeech(), wt.mayContinue() );

    mapToLoad = wt.nextMission();

    CONNECT( wnd, onAcceptAssign(), lvl, Level::_resolveSwitchMap );
    CONNECT( wnd, onContinueRules(), this, Impl::extendReign )
  }

  if( success )
  {
    GameEventPtr e = MissionWin::create( wt.name() );
    e->dispatch();
  }
}

bool Level::installEventHandler(EventHandlerPtr handler) { _d->eventHandlers.push_back( handler ); return true; }
void Level::Impl::resolveCreateConstruction( int type ){  renderer.setMode( BuildMode::create( object::Type( type ) ) );}
void Level::Impl::resolveRemoveTool(){  renderer.setMode( DestroyMode::create() );}
void Level::Impl::resolveSelectLayer( int type ){  renderer.setMode( LayerMode::create( type ) );}
void Level::Impl::showAdvisorsWindow(){  showAdvisorsWindow( advisor::employers ); }
void Level::Impl::showTradeAdvisorWindow(){  showAdvisorsWindow( advisor::trading ); }
void Level::setCameraPos(TilePos pos) {  _d->renderer.camera()->setCenter( pos ); }
void Level::switch2layer(int layer) { _d->renderer.setLayer( layer ); }
Camera* Level::camera() const { return _d->renderer.camera(); }
void Level::Impl::saveScrollSpeed(int speed) { SETTINGS_SET_VALUE( scrollSpeed, speed ); }

void Level::_quit(){ _d->result = Level::res_quit; stop(); }
void Level::restart() { _d->result = Level::res_restart; stop();}
int  Level::result() const {  return _d->result; }
void Level::exit() { _d->result = Level::res_menu; stop(); }

void Level::_requestExitGame()
{
  dialog::Dialog* dlg = dialog::Confirmation( _d->game->gui(), "", _("##exit_without_saving_question##") );
  CONNECT( dlg, onOk(), this, Level::_quit );
}

bool Level::_tryExecHotkey(NEvent &event)
{
  bool handled = false;
  if( event.EventType == sEventKeyboard && !event.keyboard.pressed)
  {
    if( !event.keyboard.shift )
    {
      handled = true;
      game::HotkeyManager::instance().execute( event.keyboard.key );
      switch( event.keyboard.key )
      {
      case KEY_SPACE:
      {
        int newLayer = _d->renderer.layerType() == citylayer::simple
                          ? _d->lastLayerId : citylayer::simple;
        _d->renderer.setLayer( newLayer );
      }
      break;

      case KEY_KEY_E:
      {
        TilePos center = _d->renderer.camera()->center();
        TileRect trect( center-TilePos(1,1), center+TilePos(1,1));
        const BorderInfo& binfo = _d->game->city()->borderInfo();
        center = (trect.contain(binfo.roadEntry) ? binfo.roadExit : binfo.roadEntry);
        _d->renderer.camera()->setCenter( center, false );
      }
      break;

      default:
        handled = false;
      break;
      }

      if( handled )
        return handled;
    }

    switch( event.keyboard.key )
    {
    case KEY_MINUS:
    case KEY_PLUS:
    case KEY_SUBTRACT:
    case KEY_EQUALS:
    case KEY_ADD:
    {
      GameEventPtr e = ChangeSpeed::create( (event.keyboard.key == KEY_MINUS || event.keyboard.key == KEY_SUBTRACT)
                                                            ? -10 : +10 );
      e->dispatch();
      handled = true;
    }
    break;

    case KEY_KEY_P:
    {
      GameEventPtr e = Pause::create( Pause::toggle );
      e->dispatch();
      handled = true;
    }
    break;

    case KEY_COMMA:
    case KEY_PERIOD:
    {
      GameEventPtr e = Step::create( event.keyboard.key == KEY_COMMA ? 1 : 25);
      e->dispatch();
      handled = true;
    }
    break;

    case KEY_F5:
      _d->makeFastSave();
      handled = true;
    break;

    case KEY_F9:
      loadStage( "" );
      handled = true;
    break;

    case KEY_F1: case KEY_F2:
    case KEY_F3: case KEY_F4:
    {
      if( event.keyboard.control )
      {
        unsigned int index = event.keyboard.key - KEY_KEY_1;
        development::Options bopts;
        bopts = _d->game->city()->buildOptions();
        if( event.keyboard.shift )
        {
          TilePos camPos = _d->renderer.camera()->center();
          _d->game->city()->activePoints().set( index, camPos );
          _d->game->city()->setBuildOptions( bopts );
        }
        else
        {
          TilePos camPos = _d->game->city()->activePoints().get( index );
          _d->renderer.camera()->setCenter( camPos );
        }

        handled = true;
      }
    }
    break;

    case KEY_SNAPSHOT:
      if( !event.keyboard.shift )
        _d->makeScreenShot();
      else
        _d->makeFullScreenshot();
      handled = true;
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

  return handled;
}

void Level::Impl::showMissionTaretsWindow()
{
  unsigned int id = Hash( CAESARIA_STR_EXT(MissionTargetsWindow) );
  Widget* wdg = game->gui()->findWidget( id );
  if( !wdg )
  {
    dialog::MissionTargets* wnd = dialog::MissionTargets::create( game->gui()->rootWidget(), game->city() );
    wnd->show();
    wnd->setID( id );
  }
}

void Level::Impl::showAdvisorsWindow( const advisor::Type advType ) { ShowAdvisorWindow::create( true, advType )->dispatch(); }
void Level::_showLoadDialog() { ShowLoadDialog::create()->dispatch(); }

}//end namespace scene
