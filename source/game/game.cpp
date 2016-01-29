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
#include "scene/logo.hpp"
#include "city/build_options.hpp"
#include "core/utils.hpp"
#include "objects/construction.hpp"
#include "game/player.hpp"
#include "gfx/picture.hpp"
#include "gfx/gl_engine.hpp"
#include "sound/engine.hpp"
#include "core/variant_map.hpp"
#include "gfx/picture_bank.hpp"
#include "scene/lobby.hpp"
#include "scene/level.hpp"
#include "gui/environment.hpp"
#include "settings.hpp"
#include "vfs/filesystem.hpp"
#include "gfx/animation_bank.hpp"
#include "vfs/entries.hpp"
#include "world/empire.hpp"
#include "core/exception.hpp"
#include "loader.hpp"
#include "objects/infodb.hpp"
#include "gamedate.hpp"
#include "saver.hpp"
#include "resourceloader.hpp"
#include "core/saveadapter.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "core/locale.hpp"
#include "pathway/astarpathfinding.hpp"
#include "objects/house_spec.hpp"
#include "walker/name_generator.hpp"
#include "religion/pantheon.hpp"
#include "vfs/archive_sg2.hpp"
#include "vfs/archive_zip.hpp"
#include "scene/briefing.hpp"
#include "gfx/logo.hpp"
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
#include "video_config.hpp"
#include "config.hpp"
#include "world/emperor.hpp"
#include "core/metric.hpp"
#include "city/build_options.hpp"
#include "roman_celebrates.hpp"
#include "gameloop.hpp"

#include <list>

using namespace gfx;
using namespace scene;
using namespace events;

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
  ScopedPtr<scene::SplashScreen> splash;

  int pauseCounter;
  std::string restartFile;

  Simulation simulation;

  void initLocale(bool& isOk , std::string& result);
  void initVideo(bool& isOk, std::string& result);
  void initSound(bool& isOk, std::string& result);
  void initPictures(bool& isOk, std::string& result);
  void initGameConfigs(bool& isOk, std::string& result);
  void initAddons(bool& isOk, std::string& result);
  void initHotkeys(bool& isOk, std::string& result);
  void initMovie(bool& isOk, std::string& result);
  void initMetrics(bool& isOk, std::string& result);
  void initCelebrations(bool& isOk, std::string& result);
  void initUI(bool& isOk, std::string& result);
  void initVfsSettings(bool& isOk, std::string& result);
  void initTilemapSettings(bool& isOk, std::string& result);
  void initArchiveLoaders(bool& isOk, std::string& result);
  void initFontCollection(bool& isOk , std::string& result);
  void mountArchives( ResourceLoader& loader );
  void createSaveDir(bool& isOk , std::string& result);
  void showSplashScreen(bool& isOk , std::string& result);
  void loadResources(bool& isOk , std::string& result);
  void updateSplashText(std::string text);
  void loadPicInfo(bool& isOk , std::string& result);
  void initNameGenerator(bool& isOk , std::string& result);
  void loadHouseSpecs(bool& isOk , std::string& result);
  void loadObjectsMetadata(bool& isOk, std::string& result);
  void loadWalkersMetadata(bool& isOk, std::string& result);
  void loadReligionConfig(bool& isOk, std::string& result);
  void fadeSplash(bool& isOk, std::string& result);

  Impl(): nextScreen(SCREEN_NONE),
      currentScreen(nullptr), engine(nullptr),
      gui(nullptr), splash(nullptr)
  {}
};

void Game::Impl::loadPicInfo(bool& isOk, std::string& result)
{
  Logger::warning( "Game: initialize offsets" );
  result = "##loading_offsets##";
  PictureInfoBank::instance().initialize( SETTINGS_RC_PATH( pic_offsets ) );
}

void Game::Impl::initNameGenerator(bool& isOk, std::string& result)
{
  result = "##initialize_names##";
  NameGenerator::instance().initialize( SETTINGS_RC_PATH( ctNamesModel ) );
  NameGenerator::instance().setLanguage( SETTINGS_STR( language ) );
}

void Game::Impl::loadHouseSpecs(bool& isOk, std::string& result)
{
  result = "##initialize_house_specification##";
  HouseSpecHelper::instance().initialize( SETTINGS_RC_PATH( houseModel ) );
}

void Game::Impl::loadObjectsMetadata(bool& isOk, std::string& result)
{
  result = "##initialize_constructions##";
  object::InfoDB::instance().initialize( SETTINGS_RC_PATH( constructionModel ) );
}

void Game::Impl::loadWalkersMetadata(bool& isOk, std::string& result)
{
  result = "##initialize_walkers##";
  WalkerHelper::instance().load( SETTINGS_RC_PATH( walkerModel ) );
}

void Game::Impl::loadReligionConfig(bool& isOk, std::string& result)
{
  result = "##initialize_religion##";
  vfs::Path filename = SETTINGS_RC_PATH( pantheonModel );
  VariantMap pantheon = config::load( filename );
  religion::rome::Pantheon::instance().load( pantheon );
}

void Game::Impl::fadeSplash(bool& isOk, std::string& result)
{
  result = "##ready_to_game##";

  if( game::Settings::get( "no-fade" ).isNull() && splash )
      splash->exitScene( scene::SplashScreen::showDevText );

  splash.reset( 0 );
}

void Game::Impl::initMovie(bool& isOk, std::string& result)
{
  movie::Config& config = movie::Config::instance();

  config.loadAlias( SETTINGS_RC_PATH( videoAlias ) );
  std::string c3videoFile = SETTINGS_STR( c3video );

  if( !c3videoFile.empty() )
  {
    config.addFolder( c3videoFile );
  }
}

void Game::Impl::initMetrics(bool& isOk, std::string& result)
{
  int value = SETTINGS_VALUE( metricSystem );
  metric::Measure::setMode( (metric::Measure::Mode)value );
}

void Game::Impl::initCelebrations(bool& isOk, std::string& result)
{
  vfs::Path value = SETTINGS_RC_PATH( celebratesConfig );
  game::Celebrates::instance().load( value );
}

void Game::Impl::initLocale( bool& isOk, std::string& result )
{
  //init translator
  Logger::warning( "Game: initialize localization folder" );
  Locale::setDirectory( SETTINGS_STR( localePath ) );

  Logger::warning( "Game: load default language" );
  Locale::setLanguage( SETTINGS_STR( language ) );
}

void Game::Impl::initVideo(bool& isOk, std::string& result)
{
  Logger::warning( "GraficEngine: create" );

  bool batchTexures = SETTINGS_VALUE( batchTextures );

  engine = new SdlEngine();

  Size size = SETTINGS_VALUE( resolution );
  Logger::warning( "GraficEngine: set size [{}x{}]", size.width(), size.height() );
  engine->setScreenSize( size );
  engine->setFlag( Engine::batching, batchTexures ? 1 : 0 );

  bool fullscreen = KILLSWITCH( fullscreen );
  if( fullscreen )
  {
    Logger::warning( "GraficEngine: try set fullscreen mode" );
    engine->setFlag( Engine::fullscreen, fullscreen ? 1 : 0 );
  }

  engine->init();
}

void Game::Impl::initSound(bool& isOk, std::string& result)
{
  Logger::warning( "init sound engine" );
  audio::Engine& ae = audio::Engine::instance();

  ae.init();
  Logger::warning( "Game: load volumes" );
  ae.setVolume( audio::ambient, SETTINGS_VALUE( ambientVolume ) );
  ae.setVolume( audio::theme, SETTINGS_VALUE( musicVolume ) );
  ae.setVolume( audio::game, SETTINGS_VALUE( soundVolume ) );
  ae.loadAlias( SETTINGS_RC_PATH( soundAlias ) );

  std::string c3musicFolder = SETTINGS_STR( c3music );
  if( !c3musicFolder.empty() )
  {
    ae.addFolder( c3musicFolder );
  }

  Logger::warning( "Game: load talks archive" );
  audio::Helper::initTalksArchive( SETTINGS_STR( talksArchive ) );
}

void Game::Impl::mountArchives(ResourceLoader &loader)
{
  Logger::warning( "Game: mount archives begin" );

  std::string errorStr;
  std::string c3res = SETTINGS_STR( c3gfx );
  if( !c3res.empty() )
  {    
    vfs::Directory gfxDir( c3res );
    vfs::Path c3path = gfxDir/"c3.sg2";

    if( !c3path.exist( vfs::Path::ignoreCase ) )
    {
      errorStr = "This game use resources files (.sg2, .map) from Caesar III(c), but "
                 "original game archive c3.sg2 not found in folder " + c3res +
                 "!!!.\nBe sure that you copy all .sg2, .map and .smk files placed to resource folder";
      SETTINGS_SET_VALUE( c3gfx, std::string( "" ) );
      game::Settings::save();
    }

    loader.loadFromModel( SETTINGS_RC_PATH( sg2model ), gfxDir );
    engine->setFlag( Engine::batching, false );
  }
  else
  {
    vfs::Path testPics = SETTINGS_RC_PATH( picsArchive );
    if( !testPics.exist() )
    {
      SETTINGS_SET_VALUE( resourcePath, Variant("") );
      game::Settings::save();
      errorStr = "Not found graphics package. Use precompiled CaesarIA archive or use\n"
                 "-c3gfx flag to set absolute path to Caesar III(r) installation folder,\n"
                 "forexample, \"-c3gfx c:/games/caesar3/\"";
    }

    loader.loadFromModel( SETTINGS_RC_PATH( remakeModel ) );
  }

  if( !errorStr.empty() )
  {
    OSystem::error( "Resources error", errorStr );
    Logger::warning( "CRITICAL: not found original resources in " + c3res );
    exit( -1 ); //kill application
  }

  loader.loadFromModel( SETTINGS_RC_PATH( archivesModel ) );
}

void Game::Impl::createSaveDir(bool& isOk , std::string& result)
{
  Logger::warning( "Game: initialize save directory" );
  vfs::Directory saveDir = SETTINGS_STR( savedir );

  bool dirCreated = true;
  if( !saveDir.exist() )
  {
    dirCreated = vfs::Directory::createByPath( saveDir );
  }

  Logger::warningIf( !dirCreated, "Game: can't create save dir" );
}

void Game::Impl::showSplashScreen(bool& isOk, std::string& result)
{
  if( !splash )
  {
    splash.createInstance();
    splash->initialize();
    splash->setImage( SETTINGS_STR( logoImageRc ),
                      SETTINGS_VALUE( logoImageIndex ) );
    splash->update( *engine );
  }
}

void Game::Impl::loadResources(bool& isOk, std::string& result)
{
  Logger::warning( "Game: initialize resource loader" );
  ResourceLoader rcLoader;
  rcLoader.loadFiles( SETTINGS_RC_PATH( logoArchive ) );
  rcLoader.onStartLoading().connect( this, &Impl::updateSplashText );

  Logger::warning( "Game: initialize resources" );
  mountArchives( rcLoader );  // init some quick pictures for screenWait
}

void Game::Impl::updateSplashText(std::string text)
{
  if( splash )
  {
    splash->setText( text );
  }
}

void Game::Impl::initUI(bool& isOk, std::string& result)
{
  Logger::warning( "Game: initialize gui" );

  gui = new gui::Ui( *engine );
  gui::infobox::Manager::instance().setBoxLock( KILLSWITCH( lockInfobox ) );
}

void Game::Impl::initVfsSettings(bool& isOk, std::string& result)
{
  //mount default rcpath folder
  Logger::warning( "Game: set resource folder as {}", game::Settings::rcpath().toString() );
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
  vfs::Path resourcePath = game::Settings::rcpath();
  Logger::warning( "Game: load fonts" );
  std::string fontname = SETTINGS_STR( font );
  FontCollection::instance().initialize( resourcePath.toString(), fontname );
}

void Game::Impl::initPictures(bool& isOk , std::string& result)
{
  result = "##initialize_animations##";

  AnimationBank::instance().loadCarts( SETTINGS_RC_PATH( cartsModel ) );
  AnimationBank::instance().loadAnimation( SETTINGS_RC_PATH( animationsModel ),
                                           SETTINGS_RC_PATH( simpleAnimationModel ) );
}

void Game::Impl::initGameConfigs(bool& isOk, std::string& result)
{
  city::development::loadBranchOptions( SETTINGS_RC_PATH( cntrGroupsModel ) );
}

void Game::Impl::initAddons(bool& isOk, std::string& result)
{
  addon::Manager& am = addon::Manager::instance();
  am.load( vfs::Directory( std::string( ":/addons" ) ) );
}

void Game::Impl::initHotkeys(bool& isOk, std::string& result)
{
  game::HotkeyManager& hkMgr = game::HotkeyManager::instance();
  hkMgr.load( SETTINGS_RC_PATH( hotkeysModel ) );

  CONNECT( &hkMgr, onHotkey(), &events::Dispatcher::instance(), events::Dispatcher::load );
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
  Logger::warning( "Game: try load from " + filename );

  Logger::warning( "Game: reseting variables" );
  reset();

  scene::SplashScreen screen;

  screen.initialize();
  bool usingOldgfx = KILLSWITCH( oldgfx ) || !SETTINGS_STR( c3gfx ).empty();
  screen.setImage( usingOldgfx ? "load4" : "freska", 1 );
  screen.update( *_dfunc()->engine );

  vfs::Path fPath( filename );
  if( !fPath.exist() )
  {
    Logger::warning( "Game: Cannot find file " + fPath.toString() );
    fPath = game::Settings::rpath( filename );

    if( !fPath.exist() )
    {
      Logger::warning( "Game: Cannot find file " + fPath.toString() );
      Logger::warning( "Game: Try find file in resource's folder " );

      fPath = game::Settings::rcpath( filename ).absolutePath();
      if( !fPath.exist() )
      {
        Logger::warning( "Game: Cannot find file " + fPath.toString() );
        return false;
      }
    }
  }

  Logger::warning( "Game: init empire start options" );
  events::Dispatcher::instance().reset();
  d.empire->initialize( SETTINGS_RC_PATH( citiesModel ),
                        SETTINGS_RC_PATH( empireObjectsModel ),
                        SETTINGS_RC_PATH( worldModel ) );

  Logger::warning( "Game: try find loader" );
  game::Loader loader;
  loader.onUpdate() += makeDelegate( &screen, &scene::SplashScreen::setText );

  bool loadOk = loader.load( fPath, *this );

  if( !loadOk )
  {
    Logger::warning( "LOADING ERROR: can't load game from " + filename );
    return false;
  }

  d.restartFile = loader.restartFile();
  Logger::warning( "Game: init player city" );
  world::CityPtr city = d.empire->initPlayerCity( ptr_cast<world::City>( d.city ) );
  if( city.isNull() )
  {
    Logger::warning( "INIT ERROR: can't initalize city {} in empire" + d.city->name() );
    return false;
  }
  d.empire->emperor().checkCities();

  Logger::warning( "Game: calculate road access for buildings" );
  const OverlayList& llo = d.city->overlays();
  for( auto overlay : llo )
  {
    ConstructionPtr construction = overlay.as<Construction>();
    if( construction.isValid() )
    {
      construction->computeRoadside();
    }
  }

  Logger::warning( "Game: initialize local pathfinder" );
  Pathfinder::instance().update( d.city->tilemap() );

  Logger::warning( "Game: load finished" );

  screen.exitScene( scene::SplashScreen::hideDevText );
  return true;
}

void Game::Impl::initArchiveLoaders(bool& isOk, std::string& result)
{
  Logger::warning( "Game: initialize sg2/zip archive loaders" );
  vfs::FileSystem& fs = vfs::FileSystem::instance();
  fs.addArchiveLoader( new vfs::Sg2ArchiveLoader( &fs ) );
  fs.addArchiveLoader( new vfs::ZipArchiveLoader( &fs ) );
}


struct InitializeStep
{
  std::string name;
  Delegate2<bool&, std::string&> function;
};

void Game::initialize()
{
  __D_REF(d,Game)
  #define ADD_STEP(obj,functor) { #functor, makeDelegate(obj,&functor) }
  std::vector<InitializeStep> steps = {
    ADD_STEP( &d, Impl::initTilemapSettings ),
    ADD_STEP( &d, Impl::initVfsSettings ),
    ADD_STEP( &d, Impl::initMetrics ),
    ADD_STEP( &d, Impl::initGameConfigs ),
    ADD_STEP( &d, Impl::initAddons ),
    ADD_STEP( &d, Impl::initArchiveLoaders ),
    ADD_STEP( &d, Impl::initLocale ),
    ADD_STEP( &d, Impl::initVideo ),
    ADD_STEP( &d, Impl::initMovie ),
    ADD_STEP( &d, Impl::initFontCollection ),
    ADD_STEP( &d, Impl::initUI ),
    ADD_STEP( &d, Impl::initSound ),
    ADD_STEP( &d, Impl::initHotkeys ),
    ADD_STEP( &d, Impl::createSaveDir ),
    ADD_STEP( &d, Impl::loadResources ),
    ADD_STEP( &d, Impl::showSplashScreen ),
    ADD_STEP( &d, Impl::initCelebrations ),
    ADD_STEP( &d, Impl::loadPicInfo ),
    ADD_STEP( &d, Impl::initPictures ),
    ADD_STEP( &d, Impl::initNameGenerator ),
    ADD_STEP( &d, Impl::loadHouseSpecs ),
    ADD_STEP( &d, Impl::loadObjectsMetadata ),
    ADD_STEP( &d, Impl::loadWalkersMetadata ),
    ADD_STEP( &d, Impl::loadReligionConfig ),
    ADD_STEP( &d, Impl::fadeSplash )
  };

  #undef ADD_STEP

  for( auto&& step : steps )
  {
    bool isOk = true;
    std::string stepText;

    try
    {
      step.function( isOk, stepText );
      d.updateSplashText( stepText );
      if( !isOk )
      {
        Logger::warning( "Game: initialize faild on step {}", step.name );
        OSystem::error( "Game: initialize faild on step", step.name );
        exit( -1 ); //kill application
      }
    }
    catch(...) { exit(-1); }
  }

  d.nextScreen = SCREEN_MENU;
  d.engine->setFlag( gfx::Engine::showMetrics, 1 );
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

  Logger::warning( "game: exec switch to screen {}", d.nextScreen );
  addon::Manager& am = addon::Manager::instance();
  switch(d.nextScreen)
  {
    case SCREEN_MENU:
    {
      d.currentScreen = new gamestate::InMainMenu(this, d.engine);
      am.initAddons4level( addon::mainMenu );
    }
    break;

    case SCREEN_GAME:
    {
      Logger::warning( "game: enter setScreenGame" );
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
      am.initAddons4level( addon::briefing );
    }
    break;

    case SCREEN_QUIT:
      Logger::warning( "game: prepare for quit" );
    break;

    default:
      Logger::warning( "game: unexpected next screen type {}", d.nextScreen );
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
#ifdef DEBUG
  WalkerDebugQueue::print();
  WalkerDebugQueue::instance().clear();

  OverlayDebugQueue::print();
  OverlayDebugQueue::instance().clear();
#endif
}

void Game::destroy()
{
  //audio::Engine::instance().exit();
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
