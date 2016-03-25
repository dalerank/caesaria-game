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
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#include "game.hpp"
#include <GameScene>
#include <GameCore>
#include <GameGfx>
#include <GameCity>
#include <GameLogger>
#include <GameEvents>
#include <GameVfs>
#include <GameObjects>
#include <GameApp>
#include <GameScene>

#include "scripting/core.hpp"
#include "sound/engine.hpp"

#include "gui/environment.hpp"
#include "settings.hpp"
#include "vfs/entries.hpp"
#include "world/empire.hpp"
#include "core/exception.hpp"
#include "loader.hpp"
#include "gamedate.hpp"
#include "font/font_collection.hpp"
#include "saver.hpp"
#include "pathway/astarpathfinding.hpp"
#include "walker/name_generator.hpp"
#include "religion/pantheon.hpp"
#include "scene/briefing.hpp"
#include "walker/helper.hpp"
#include "core/osystem.hpp"
#include "freeplay_finalizer.hpp"
#include "events/warningmessage.hpp"
#include "gfx/picture_info_bank.hpp"
#include "gfx/sdl_engine.hpp"
#include "objects/overlay.hpp"
#include "gfx/tilemap_config.hpp"
#include "gamestate.hpp"
#include "infoboxmanager.hpp"
#include "hotkey_manager.hpp"
#include "addon_manager.hpp"
#include "config.hpp"
#include "world/emperor.hpp"
#include "core/metric.hpp"
#include "city/build_options.hpp"
#include "roman_celebrates.hpp"
#include "gui/widget_factory.hpp"
#include "gameloop.hpp"

#include <list>

using namespace gfx;
using namespace scene;
using namespace events;

class ScriptWidgetFinalizer : public gui::WidgetFinalizer
{
public:
  virtual void destroyed(gui::Widget* w)
  {
    VariantMap vm = w->properties();
    for( const auto& item : vm )
    {
      if( item.first.find( "js_" ) == 0 )
        script::Core::unref( item.second.toString() );
    }
  }
};

class Game::Impl
{
public:
  ScreenType nextScreen;
  std::string nextFilename;
  gamestate::State* currentScreen;
  gfx::Engine* engine;
  gui::Ui* gui;

  world::EmpirePtr empire;
  PlayerCityPtr city;
  PlayerPtr player;

  int pauseCounter;
  std::string restartFile;

  Simulation simulation;

  void initLocale(bool& isOk , std::string& result);
  void initVideo(bool& isOk, std::string& result);
  void initSound(bool& isOk, std::string& result);
  void initPictures(bool& isOk, std::string& result);
  void initGameConfigs(bool& isOk, std::string& result);
  void initHotkeys(bool& isOk, std::string& result);
  void initMetrics(bool& isOk, std::string& result);
  void initUI(bool& isOk, std::string& result);
  void initVfsSettings(bool& isOk, std::string& result);
  void initTilemapSettings(bool& isOk, std::string& result);
  void initArchiveLoaders(bool& isOk, std::string& result);
  void initFontCollection(bool& isOk , std::string& result);
  void createSaveDir(bool& isOk , std::string& result);
  void themeFinished();
  void resolveHotkey(std::string actionName);

  Impl(): nextScreen(SCREEN_NONE),
      currentScreen(nullptr), engine(nullptr),
      gui(nullptr)
  {}
};

void Game::Impl::themeFinished()
{
  events::dispatch<events::ScriptFunc>("OnThemePlayFinished");
}

void Game::Impl::resolveHotkey(std::string actionName)
{
  VariantList vl; vl << Variant(actionName);
  events::dispatch<events::ScriptFunc>("OnExecHotkey", vl);
}

void Game::Impl::initMetrics(bool& isOk, std::string& result)
{
  int value = SETTINGS_VALUE( metricSystem );
  metric::Measure::setMode( (metric::Measure::Mode)value );
}

void Game::Impl::initLocale( bool& isOk, std::string& result )
{
  //init translator
  Logger::debug( "Game: initialize localization folder" );
  Locale::setDirectory( SETTINGS_STR( localePath ) );

  Logger::debug( "Game: load default language" );
  Locale::setLanguage( SETTINGS_STR( language ) );
}

void Game::Impl::initVideo(bool& isOk, std::string& result)
{
  Logger::debug( "GraficEngine: create" );

  bool batchTexures = SETTINGS_VALUE( batchTextures );

  engine = new SdlEngine();

  Size size = SETTINGS_VALUE( resolution );
  Logger::debug( "GraficEngine: set size [{}x{}]", size.width(), size.height() );
  engine->setScreenSize( size );
  engine->setFlag( Engine::batching, batchTexures ? 1 : 0 );

  bool fullscreen = KILLSWITCH( fullscreen );
  if( fullscreen )
  {
    Logger::info( "GraficEngine: try set fullscreen mode" );
    engine->setFlag( Engine::fullscreen, fullscreen ? 1 : 0 );
  }

  engine->init();
}

void Game::Impl::initSound(bool& isOk, std::string& result)
{
  Logger::debug( "init sound engine" );
  audio::Engine& ae = audio::Engine::instance();

  ae.init();
  Logger::debug( "Game: load volumes" );
  ae.setVolume( audio::ambient, SETTINGS_VALUE( ambientVolume ) );
  ae.setVolume( audio::theme, SETTINGS_VALUE( musicVolume ) );
  ae.setVolume( audio::game, SETTINGS_VALUE( soundVolume ) );
  ae.loadAlias( SETTINGS_RC_PATH( soundAlias ) );

  std::string c3musicFolder = SETTINGS_STR( c3music );
  if( !c3musicFolder.empty() )
  {
    ae.addFolder( c3musicFolder );
  }

  Logger::debug( "Game: load talks archive" );
  audio::Helper::initTalksArchive( SETTINGS_STR( talksArchive ) );

  ae.onThemeStopped().connect(this, &Impl::themeFinished);
}

void Game::Impl::createSaveDir(bool& isOk , std::string& result)
{
  Logger::debug( "Game: initialize save directory" );
  vfs::Directory saveDir = SETTINGS_STR( savedir );

  bool dirCreated = true;
  if( !saveDir.exist() )
  {
    dirCreated = vfs::Directory::createByPath( saveDir );
  }

  Logger::warningIf( !dirCreated, "Game: can't create save dir" );
}

void Game::Impl::initUI(bool& isOk, std::string& result)
{
  Logger::debug( "Game: initialize gui" );

  gui = new gui::Ui( *engine );
  auto finalizer = SmartPtr<gui::WidgetFinalizer>(new ScriptWidgetFinalizer());
  finalizer->drop();
  gui->installWidgetFinalizer(finalizer);

  gui::infobox::Manager::instance().setBoxLock( KILLSWITCH( lockInfobox ) );
}

void Game::Impl::initVfsSettings(bool& isOk, std::string& result)
{
  //mount default rcpath folder
  Logger::debug( "Game: set resource folder as {}", game::Settings::rcpath().toString() );
  vfs::FileSystem::instance().setRcFolder( game::Settings::rcpath() );
}

void Game::Impl::initTilemapSettings(bool& isOk, std::string& result)
{
  int cellWidth = SETTINGS_VALUE( cellw );
  if( cellWidth != config::tilemap.cell.width.oldw
      && cellWidth != config::tilemap.cell.width.neww )
  {
    cellWidth = config::tilemap.cell.width.oldw;
  }
  config::tilemap.cell.setWidth( cellWidth );
}

void Game::Impl::initFontCollection( bool& isOk, std::string& result )
{
  Logger::warning( "Game: load fonts" );

  vfs::Path resourcePath = SETTINGS_RC_PATH( fontsDirectory );
  std::string fontname = SETTINGS_STR( font );
  FontCollection::instance().initialize( resourcePath, fontname );
}

void Game::Impl::initGameConfigs(bool& isOk, std::string& result)
{
  city::development::loadBranchOptions( SETTINGS_RC_PATH( cntrGroupsModel ) );
}

void Game::Impl::initHotkeys(bool& isOk, std::string& result)
{
  game::HotkeyManager& hkMgr = game::HotkeyManager::instance();
  CONNECT( &hkMgr, onExec(), this, Impl::resolveHotkey );
}

PlayerPtr Game::player() const { return _dfunc()->player; }
PlayerCityPtr Game::city() const { return _dfunc()->city; }
world::EmpirePtr Game::empire() const { return _dfunc()->empire; }
gui::Ui* Game::gui() const { return _dfunc()->gui; }
gfx::Engine* Game::engine() const { return _dfunc()->engine; }
scene::Base* Game::scene() const { return _dfunc()->currentScreen->toBase(); }

const DateTime& Game::date() const { return game::Date::current(); }
bool Game::isPaused() const { return _dfunc()->pauseCounter>0; }
void Game::play() { setPaused( false ); }
void Game::pause() { setPaused( true ); }

void Game::setPaused(bool value)
{
  __D_REF(d,Game)
  d.pauseCounter = math::clamp( d.pauseCounter + (value ? 1 : -1 ), 0, 99 );
}

void Game::step(unsigned int count)
{
  _dfunc()->simulation.time.manualTicksCounterX10 += count * config::gamespeed::scale;
}

Game::Game() : __INIT_IMPL(Game)
{
  _dfunc()->nextScreen = SCREEN_NONE;
  _dfunc()->pauseCounter = 0;
}

void Game::changeTimeMultiplier(int percent){ setTimeMultiplier( _dfunc()->simulation.time.multiplier + percent );}
void Game::setTimeMultiplier(int percent){ _dfunc()->simulation.time.multiplier = math::clamp<unsigned int>( percent,
                                                                                            config::gamespeed::minimum,
                                                                                            config::gamespeed::maximux );}
int Game::timeMultiplier() const{  return _dfunc()->simulation.time.multiplier;}

Game::~Game(){}

void Game::save(std::string filename) const
{
  game::Saver saver;
  saver.setRestartFile( _dfunc()->restartFile );
  saver.save( filename, *this );

  SETTINGS_SET_VALUE( lastGame, Variant( filename ) );

  events::dispatch<WarningMessage>( "Game saved to " + vfs::Path( filename ).baseName().removeExtension(), WarningMessage::neitral );
}

bool Game::load(std::string filename)
{
  __D_REF(d,Game)
  Logger::debug( "Game: try load from " + filename );

  Logger::debug( "Game: reseting variables" );
  reset();

  scene::SplashScreen screen;

  screen.initialize();
  bool usingOldgfx = KILLSWITCH( oldgfx ) || !SETTINGS_STR( c3gfx ).empty();
  screen.setBackground( usingOldgfx ? "load4" : "freska", 1 );
  screen.update( *_dfunc()->engine );

  vfs::Path fPath( filename );
  if( !fPath.exist() )
  {
    Logger::debug( "Game: Cannot find file " + fPath.toString() );
    fPath = game::Settings::rpath( filename );

    if( !fPath.exist() )
    {
      Logger::debug( "Game: Cannot find file " + fPath.toString() );
      Logger::debug( "Game: Try find file in resource's folder " );

      fPath = game::Settings::rcpath( filename ).absolutePath();
      if( !fPath.exist() )
      {
        Logger::warning( "Game: Cannot find file " + fPath.toString() );
        return false;
      }
    }
  }

  Logger::debug( "Game: init empire start options" );
  events::Dispatcher::instance().reset();
  d.empire->initialize( SETTINGS_RC_PATH( citiesModel ),
                        SETTINGS_RC_PATH( empireObjectsModel ),
                        SETTINGS_RC_PATH( worldModel ) );

  Logger::debug( "Game: try find loader" );
  game::Loader loader;
  loader.onUpdate() += makeDelegate( &screen, &scene::SplashScreen::setText );

  bool loadOk = loader.load( fPath, *this );

  if( !loadOk )
  {
    Logger::error( "can't load game from " + filename );
    return false;
  }

  d.restartFile = loader.restartFile();
  Logger::debug( "Game: init player city" );
  world::CityPtr city = d.empire->initPlayerCity( ptr_cast<world::City>( d.city ) );
  if( city.isNull() )
  {
    Logger::error( "Can't initalize city {} in empire" + d.city->name() );
    return false;
  }
  d.empire->emperor().checkCities();

  Logger::debug( "Game: calculate road access for buildings" );
  const OverlayList& llo = d.city->overlays();
  for( auto overlay : llo )
  {
    ConstructionPtr construction = overlay.as<Construction>();
    if( construction.isValid() )
    {
      construction->computeRoadside();
    }
  }

  Logger::debug( "Game: initialize local pathfinder" );
  Pathfinder::instance().update( d.city->tilemap() );

  Logger::debug( "Game: load finished" );

  screen.exitScene(scene::SplashScreen::hideDevText);
  return true;
}

void Game::Impl::initArchiveLoaders(bool& isOk, std::string& result)
{
  Logger::debug( "Game: initialize sg2/zip archive loaders" );
  vfs::FileSystem& fs = vfs::FileSystem::instance();
  fs.addArchiveLoader(new vfs::Sg2ArchiveLoader(&fs));
  fs.addArchiveLoader(new vfs::ZipArchiveLoader(&fs));
}

void Game::initialize()
{
  __D_REF(d, Game)
  #define ADD_STEP(obj,functor) { #functor, makeDelegate(obj,&functor) }
  std::vector<gamestate::InitializeStep> steps = {
    ADD_STEP( &d, Impl::initTilemapSettings ),
    ADD_STEP( &d, Impl::initVfsSettings ),
    ADD_STEP( &d, Impl::initMetrics ),
    ADD_STEP( &d, Impl::initGameConfigs ),
    ADD_STEP( &d, Impl::initArchiveLoaders ),
    ADD_STEP( &d, Impl::initLocale ),
    ADD_STEP( &d, Impl::initVideo ),
    ADD_STEP( &d, Impl::initFontCollection ),
    ADD_STEP( &d, Impl::initUI ),
    ADD_STEP( &d, Impl::initSound ),
    ADD_STEP( &d, Impl::initHotkeys ),
    ADD_STEP( &d, Impl::createSaveDir ),
  };
  #undef ADD_STEP

  for (auto& step : steps)
  {
    bool isOk = true;
    std::string stepText;

    try
    {
      step.function(isOk, stepText);
      if (!isOk)
      {
        Logger::error( "Game: initialize faild on step {}", step.name );
        OSystem::error( "Game: initialize faild on step", step.name );
        exit(-1); //kill application
      }
    }
    catch(...) { exit(-1); }
  }

  d.nextScreen = SCREEN_LOGO;
  d.engine->setFlag(gfx::Engine::showMetrics, 1);
}

bool Game::exec()
{
  __D_REF(d,Game)
  if (d.currentScreen && d.currentScreen->getScreenType() == d.nextScreen)
  {
    if (!d.currentScreen->update(d.engine))
    {
      delete d.currentScreen;
      d.currentScreen = 0;
    }
    return true;
  }

  Logger::debug( "game: exec switch to screen {}", d.nextScreen );
  addon::Manager& am = addon::Manager::instance();
  switch(d.nextScreen)
  {
    case SCREEN_LOGO:
    {
      d.currentScreen = new gamestate::InSplash(this);

    }
    break;

    case SCREEN_MENU:
    {
      d.currentScreen = new gamestate::InMainMenu(this, d.engine);
      am.initAddons4level( addon::mainMenu );
    }
    break;

    case SCREEN_GAME:
    {
      Logger::debug( "game: enter setScreenGame" );
      d.simulation.reset();
      d.currentScreen = new gamestate::InGame( this, d.engine,
                                               d.simulation,
                                               d.nextFilename,
                                               d.restartFile );
      am.initAddons4level( addon::level );
    }
    break;

    case SCREEN_BRIEFING:
    {
      d.currentScreen = new gamestate::InBriefing(this, d.engine, d.nextFilename );
      am.initAddons4level(addon::briefing);
    }
    break;

    case SCREEN_QUIT:
      audio::Engine::instance().exit();
      Logger::debug( "game: prepare for quit" );
    break;

    default:
      Logger::debug( "game: unexpected next screen type {}", d.nextScreen );
  }

  return d.nextScreen != SCREEN_QUIT;
}

void Game::reset()
{
  __D_REF(d,Game)
  d.empire = world::Empire::create();

  d.player = Player::create();
  d.player->setName( SETTINGS_STR( playerName ) );
  d.pauseCounter = 0;

  WalkerRelations::instance().clear();
  WalkerRelations::instance().load( SETTINGS_RC_PATH( walkerRelations ) );

  bool oldGameplay = KILLSWITCH( oldgfx ) || !SETTINGS_STR( c3gfx ).empty();
  d.city = PlayerCity::create( d.empire, d.player );
  d.city->setOption( PlayerCity::c3gameplay, oldGameplay );
}

void Game::clear()
{
  //_d->empire = world::EmpirePtr();
  _dfunc()->city->clean();
  _dfunc()->city = PlayerCityPtr();
}

void Game::destroy()
{
}

void Game::setNextScreen(ScreenType screen) { _dfunc()->nextScreen = screen;}

void Simulation::reset()
{
  time.ticksX10 = 0;
  time.current = time.ticksX10;
}

Simulation::Simulation()
{
  time.manualTicksCounterX10 = 0;
  time.ticksX10 = 0;
  time.current = 0;
  time.multiplier = config::gamespeed::defaultMutltiplier;
}
