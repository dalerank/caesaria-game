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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "game.hpp"
#include "scene/logo.hpp"
#include "city/build_options.hpp"
#include "core/stringhelper.hpp"
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "gfx/picture.hpp"
#include "gfx/gl_engine.hpp"
#include "sound/engine.hpp"
#include "gfx/picture_bank.hpp"
#include "scene/menu.hpp"
#include "scene/level.hpp"
#include "gui/environment.hpp"
#include "settings.hpp"
#include "vfs/filesystem.hpp"
#include "enums.hpp"
#include "gfx/animation_bank.hpp"
#include "vfs/entries.hpp"
#include "world/empire.hpp"
#include "core/exception.hpp"
#include "loader.hpp"
#include "gamedate.hpp"
#include "saver.hpp"
#include "resourceloader.hpp"
#include "core/saveadapter.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "core/locale.hpp"
#include "pathway/astarpathfinding.hpp"
#include "objects/house_level.hpp"
#include "walker/name_generator.hpp"
#include "walker/walker.hpp"
#include "core/foreach.hpp"
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
#include "gfx/tileoverlay.hpp"
#include "gamestate.hpp"

#include <list>

using namespace gfx;

class Game::Impl
{
public:
  ScreenType nextScreen;
  std::string nextFilename;
  gamestate::BaseState* currentScreen;
  gfx::Engine* engine;
  gui::Ui* gui;

  world::EmpirePtr empire;
  PlayerCityPtr city;
  PlayerPtr player;

  int pauseCounter;
  unsigned int manualTicksCounterX10;
  std::string restartFile;

  unsigned int saveTime; // last action time
  unsigned int timeX10; // time (ticks) multiplied by 10;
  unsigned int timeMultiplier; // 100 = 1x speed

  void initLocale( std::string localePath );
  void initVideo();
  void initSound();
  void initPictures();
  void initGuiEnvironment();
  void initArchiveLoaders();
  void initPantheon( vfs::Path filename );
  void initFontCollection( vfs::Path resourcePath );
  void mountArchives( ResourceLoader& loader );
  void createSaveDir();

  Impl(): nextScreen(SCREEN_NONE),
      currentScreen(0), engine(0), gui(0)
  {
	  saveTime = 0;
	  timeX10 = 0;
	  timeMultiplier = 100;
	  pauseCounter = 0;
	  manualTicksCounterX10 = 0;
  }
};

void Game::Impl::initLocale( std::string localePath )
{
  //init translator
  Logger::warning( "Game: initialize localization folder" );
  Locale::setDirectory( localePath );

  Logger::warning( "Game: load default language" );
  Locale::setLanguage( SETTINGS_VALUE( language ).toString() );
}

void Game::Impl::initVideo()
{
  Logger::warning( "GraficEngine: create" );

  //std::string render = GameSettings::get( GameSettings::render ).toString();

  engine = new gfx::SdlEngine();

  Logger::warning( "GraficEngine: set size" );
  engine->setScreenSize(Size(640,480));

  bool fullscreen = SETTINGS_VALUE( fullscreen );
  if( fullscreen )
  {
    Logger::warning( "GraficEngine: try set fullscreen mode" );
    engine->setFlag( gfx::Engine::fullscreen, fullscreen ? 1 : 0 );
  }

  engine->init();
}

void Game::Impl::initSound()
{
  Logger::warning( "init sound engine" );
  audio::Engine& ae = audio::Engine::instance();

  ae.init();
  Logger::warning( "Game: load volumes" );
  ae.setVolume( audio::ambientSound, SETTINGS_VALUE( ambientVolume ) );
  ae.setVolume( audio::themeSound, SETTINGS_VALUE( musicVolume ) );
  ae.setVolume( audio::gameSound, SETTINGS_VALUE( soundVolume ) );

  Logger::warning( "Game: load talks archive" );
  audio::Helper::initTalksArchive( SETTINGS_RC_PATH( talksArchive ) );
}

void Game::Impl::mountArchives(ResourceLoader &loader)
{
  Logger::warning( "Game: mount archives begin" );

  std::string errorStr;
  Variant c3res = SETTINGS_VALUE( c3gfx );
  if( c3res.isValid() )
  {
    vfs::Directory gfxDir( c3res.toString() );
    vfs::Path c3sg2( "c3.sg2" );
    vfs::Path c3path = gfxDir/c3sg2;

    if( !c3path.exist( vfs::Path::ignoreCase ) )
    {
      errorStr = "This game use resources files (.sg2, .map) from Caesar III(c), but "
                 "original game archive c3.sg2 not found in folder " + c3res.toString() +
                 "!!!.\nBe sure that you copy all .sg2, .map and .smk files placed to resource folder";
    }

    loader.loadFromModel( SETTINGS_RC_PATH( sg2model ), gfxDir );
  }
  else
  {
    vfs::Path testPics = SETTINGS_RC_PATH( picsArchive );
    if( !testPics.exist() )
    {
      errorStr = "Not found graphics set. Use precompiled CaesarIA archive or use\n"
                 "-c3gfx flag to set absolute path to Caesar III(r) installation folder,\n"
                 "forexample, \"-c3gfx c:/games/caesar3/\"";
    }

    loader.loadFromModel( SETTINGS_RC_PATH( remakeModel ) );
  }

  if( !errorStr.empty() )
  {
    OSystem::error( "Resources error", errorStr );
    Logger::warning( "CRITICAL: not found original resources in " + c3res.toString() );
    exit( -1 ); //kill application
  }

  loader.loadFromModel( SETTINGS_RC_PATH( archivesModel ) );
}

void Game::Impl::createSaveDir()
{
  Logger::warning( "Game: initialize save directory" );
  vfs::Directory saveDir = SETTINGS_VALUE( savedir ).toString();

  bool dirCreated = true;
  if( !saveDir.exist() )
  {
    dirCreated = vfs::Directory::createByPath( saveDir );
  }

  Logger::warningIf( !dirCreated, "Game: can't create save dir" );
}

void Game::Impl::initGuiEnvironment()
{
  Logger::warning( "Game: initialize gui" );
  gui = new gui::Ui( *engine );
}

void Game::Impl::initPantheon( vfs::Path filename)
{
  VariantMap pantheon = SaveAdapter::load( filename );
  religion::rome::Pantheon::instance().load( pantheon );
}

void Game::Impl::initFontCollection( vfs::Path resourcePath )
{
  Logger::warning( "Game: load fonts" );
  FontCollection::instance().initialize( resourcePath.toString() );
}

void Game::Impl::initPictures()
{
  AnimationBank::instance().loadCarts();
  AnimationBank::instance().loadAnimation( SETTINGS_RC_PATH( animationsModel ) );
}

PlayerPtr Game::player() const { return _d->player; }
PlayerCityPtr Game::city() const { return _d->city; }
world::EmpirePtr Game::empire() const { return _d->empire; }
gui::Ui* Game::gui() const { return _d->gui; }
gfx::Engine* Game::engine() const { return _d->engine; }
scene::Base* Game::scene() const { return _d->currentScreen->toBase(); }
bool Game::isPaused() const { return _d->pauseCounter>0; }
void Game::play() { setPaused( false ); }
void Game::pause() { setPaused( true ); }

void Game::setPaused(bool value)
{
  _d->pauseCounter = math::clamp( _d->pauseCounter + (value ? 1 : -1 ), 0, 99 );
}

void Game::step(unsigned int count)
{
  _d->manualTicksCounterX10 += count * 10;
}

Game::Game() : _d( new Impl )
{
  _d->nextScreen = SCREEN_NONE;
  _d->pauseCounter = 0;
  _d->manualTicksCounterX10 = 0;
  _d->timeX10 = 0;
  _d->saveTime = 0;
  _d->timeMultiplier = 70;
}

void Game::changeTimeMultiplier(int percent){  setTimeMultiplier( _d->timeMultiplier + percent );}
void Game::setTimeMultiplier(int percent){  _d->timeMultiplier = math::clamp<unsigned int>( percent, 10, 300 );}
int Game::timeMultiplier() const{  return _d->timeMultiplier;}

Game::~Game(){}

void Game::save(std::string filename) const
{
  GameSaver saver;
  saver.setRestartFile( _d->restartFile );
  saver.save( filename, *this );

  events::GameEventPtr e = events::WarningMessageEvent::create( "Game saved to " + vfs::Path( filename ).baseName().toString() );
  e->dispatch();
}

bool Game::load(std::string filename)
{
  Logger::warning( "Game: try load from " + filename );

  Logger::warning( "Game: reseting variables" );
  reset();

  vfs::Path fPath( filename );
  if( !fPath.exist() )
  {
    Logger::warning( "Game: Cannot find file " + fPath.toString() );
    fPath = GameSettings::rpath( filename );

    if( !fPath.exist() )
    {
      Logger::warning( "Game: Cannot find file " + fPath.toString() );
      Logger::warning( "Game: Try find file in resource's folder " );

      fPath = GameSettings::rcpath( filename ).absolutePath();
      if( !fPath.exist() )
      {
        Logger::warning( "Game: Cannot find file " + fPath.toString() );
        return false;
      }
    }
  }

  Logger::warning( "Game: init empire start options" );
  events::Dispatcher::instance().reset();
  _d->empire->initialize( SETTINGS_RC_PATH( citiesModel ),
                          SETTINGS_RC_PATH( empireObjectsModel ),
                          SETTINGS_RC_PATH( worldModel ) );

  Logger::warning( "Game: try find loader" );
  GameLoader loader;
  bool loadOk = loader.load( fPath, *this );

  if( !loadOk )
  {
    Logger::warning( "LOADING ERROR: can't load game from " + filename );
    return false;
  }

  _d->restartFile = loader.restartFile();
  Logger::warning( "Game: init player city" );
  world::CityPtr city = _d->empire->initPlayerCity( ptr_cast<world::City>( _d->city ) );
  if( city.isNull() )
  {
    Logger::warning( "INIT ERROR: can't initalize city %s in empire" + _d->city->name() );
    return false;
  }

  Logger::warning( "Game: calculate road access for buildings" );
  TileOverlayList& llo = _d->city->overlays();
  foreach( overlay, llo )
  {
    ConstructionPtr construction = ptr_cast<Construction>( *overlay );
    if( construction.isValid() )
    {
      construction->computeAccessRoads();
    }
  }

  Logger::warning( "Game: initialize local pathfinder" );
  Pathfinder::instance().update( _d->city->tilemap() );

  Logger::warning( "Game: load finished" );
  return true;
}

void Game::Impl::initArchiveLoaders()
{
  Logger::warning( "Game: initialize sg2/zip archive loaders" );
  vfs::FileSystem& fs = vfs::FileSystem::instance();
  fs.addArchiveLoader( new vfs::Sg2ArchiveLoader( &fs ) );
  fs.addArchiveLoader( new vfs::ZipArchiveLoader( &fs ) );
}

void Game::initialize()
{
  Logger::warning( "Game: load game settings" );
  GameSettings::load();
  //mount default rcpath folder
  Logger::warning( "Game: set resource folder" );
  vfs::FileSystem::instance().setRcFolder( GameSettings::rcpath() );

  _d->initArchiveLoaders();
  _d->initLocale( SETTINGS_VALUE( localePath ).toString() );
  _d->initVideo();
  _d->initFontCollection( GameSettings::rcpath() );
  _d->initGuiEnvironment();
  _d->initSound();
  _d->createSaveDir();

  Logger::warning( "Game: load splash screen" );
  splash::initialize( "logo_00001" );

  scene::SplashScreen screen;
  screen.initialize();
  screen.update( *_d->engine );

  Logger::warning( "Game: initialize resource loader" );
  ResourceLoader rcLoader;
  rcLoader.onStartLoading().connect( &screen, &scene::SplashScreen::setText );

  Logger::warning( "Game: initialize offsets" );
  screen.setPrefix( "##loading_offsets##" );
  PictureInfoBank::instance().initialize( SETTINGS_RC_PATH( pic_offsets ) );

  Logger::warning( "Game: initialize resources" );
  screen.setPrefix( "##loading_resources##" );
  _d->mountArchives( rcLoader );  // init some quick pictures for screenWait

  screen.setPrefix( "" );
  screen.setText( "##initialize_animations##" );
  _d->initPictures();

  screen.setText( "##initialize_names##" );
  NameGenerator::instance().initialize( SETTINGS_RC_PATH( ctNamesModel ) );

  screen.setText( "##initialize_house_specification##" );
  HouseSpecHelper::instance().initialize( SETTINGS_RC_PATH( houseModel ) );

  screen.setText( "##initialize_constructions##" );
  MetaDataHolder::instance().initialize( SETTINGS_RC_PATH( constructionModel ) );

  screen.setText( "##initialize_walkers##" );
  WalkerHelper::instance().load( SETTINGS_RC_PATH( walkerModel ) );

  screen.setText( "##initialize_religion##" );
  _d->initPantheon( SETTINGS_RC_PATH( pantheonModel ) );

  screen.setText( "##ready_to_game##" );

  if( GameSettings::get( "no-fade" ).isNull() )
    screen.exitScene();

  _d->nextScreen = SCREEN_MENU;
  _d->engine->setFlag( gfx::Engine::debugInfo, 1 );
}

bool Game::exec()
{
  if (_d->currentScreen && _d->currentScreen->getScreenType() == _d->nextScreen)
  {
    if (!_d->currentScreen->update(_d->engine))
    {
      delete _d->currentScreen;
      _d->currentScreen = 0;
    }

    return true;
  }

  Logger::warning( "game: exec switch to screen %d", _d->nextScreen );
  switch(_d->nextScreen)
  {
    case SCREEN_MENU:
    {
      _d->currentScreen = new gamestate::ShowMainMenu(this, _d->engine);
    } break;
    case SCREEN_GAME:
    {
      Logger::warning( "game: enter setScreenGame" );
      _d->timeX10 = 0;
      _d->saveTime = _d->timeX10;
      _d->currentScreen = new gamestate::GameLoop(this, _d->engine,
                                                          _d->saveTime, _d->timeX10,
                                                          _d->timeMultiplier, _d->manualTicksCounterX10,
                                                          _d->nextFilename, _d->restartFile );
    } break;
    case SCREEN_BRIEFING:
    {
      _d->currentScreen = new gamestate::MissionSelect(this, _d->engine, _d->nextFilename );
    } break;

    default:
    Logger::warning( "Unexpected next screen type %d", _d->nextScreen );
    //_CAESARIA_DEBUG_BREAK_IF( "Unexpected next screen type" );
  }

  return _d->nextScreen != SCREEN_QUIT;
}

void Game::reset()
{
  _d->empire = world::Empire::create();

  _d->player = Player::create();
  _d->pauseCounter = 0;
  _d->timeX10 = 0;
  _d->saveTime = 0;
  _d->manualTicksCounterX10 = 0;

  WalkerRelations::instance().clear();
  WalkerRelations::instance().load( SETTINGS_RC_PATH( walkerRelations ) );

  _d->city = PlayerCity::create( _d->empire, _d->player );
}

void Game::clear()
{
  //_d->empire = world::EmpirePtr();
  _d->city->clean();
  _d->city = PlayerCityPtr();
#ifdef DEBUG
  WalkerDebugQueue::print();
  WalkerDebugQueue::instance().clear();

  gfx::OverlayDebugQueue::print();
  gfx::OverlayDebugQueue::instance().clear();
#endif
}

void Game::setNextScreen(ScreenType screen)
{
  _d->nextScreen = screen;
}
