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
// Copyright 2012-2013 dalerank, dalerankn8@gmail.com

#include "game.hpp"
#include "scene/logo.hpp"
#include "city/build_options.hpp"
#include "core/stringhelper.hpp"
#include "city/city.hpp"
#include "gfx/picture.hpp"
#include "gfx/sdl_engine.hpp"
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
#include "core/foreach.hpp"
#include "religion/pantheon.hpp"
#include "vfs/archive_sg2.hpp"
#include "scene/briefing.hpp"
#include "gfx/logo.hpp"
#include "walker/helper.hpp"
#include "core/osystem.hpp"

#include <list>

using namespace gfx;

class Game::Impl
{
public:
  ScreenType nextScreen;
  std::string nextFilename;
  scene::Base* currentScreen;
  gfx::Engine* engine;
  gui::GuiEnv* gui;

  world::EmpirePtr empire;
  PlayerCityPtr city;
  PlayerPtr player;

  bool loadOk;
  int pauseCounter;

  float time, saveTime;
  float timeMultiplier;
  
  void initLocale(std::string localePath);
  void initVideo();
  void initSound();
  void initPictures( vfs::Path resourcePath);
  void initGuiEnvironment();
  void initPantheon(vfs::Path filename );
  void initFontCollection(vfs::Path resourcePath);
  void mountArchives( ResourceLoader& loader );
};

void Game::Impl::initLocale( std::string localePath)
{
  //init translator
  Locale::setDirectory( localePath );
  Locale::setLanguage( GameSettings::get( GameSettings::language ).toString() );
}

void Game::Impl::initVideo()
{
  Logger::warning( "GraficEngine: create" );
  engine = new gfx::SdlEngine();

  Logger::warning( "GraficEngine: set size" );
  engine->setScreenSize( GameSettings::get( GameSettings::resolution ).toSize() );
    
  Logger::warning( "GraficEngine: try set fullscreen mode" );
  engine->setFlag( gfx::Engine::fullscreen, GameSettings::get( GameSettings::fullscreen ).toBool() ? 1 : 0 );
  engine->init();
}

void Game::Impl::initSound()
{
  Logger::warning( "init sound engine" );
  audio::Engine& ae = audio::Engine::instance();

  ae.init();
  ae.setVolume( audio::ambientSound, GameSettings::get( GameSettings::ambientVolume ) );
  ae.setVolume( audio::themeSound, GameSettings::get( GameSettings::musicVolume ) );
  ae.setVolume( audio::gameSound, GameSettings::get( GameSettings::soundVolume ) );
}

void Game::Impl::mountArchives(ResourceLoader &loader)
{
  vfs::FileSystem& fs = vfs::FileSystem::instance();
  Logger::warning( "Game: initialize sg2 archive loader" );
  fs.addArchiveLoader( new vfs::Sg2ArchiveLoader( &fs ) );

  Logger::warning( "Game: mount archives begin" );

  std::string errorStr;
  Variant c3res = GameSettings::get( GameSettings::c3gfx );
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

    loader.loadFromModel( GameSettings::rcpath( GameSettings::sg2model ) );
  }
  else
  {
    vfs::Path testPics = GameSettings::rcpath( GameSettings::testArchive );
    if( !testPics.exist() )
    {
      errorStr = "Not found graphics set. Use precompiled CaesarIA archive or use\n"
                 "-c3gfx flag to set absolute path to Caesar III(r) installation folder,\n"
                 "forexample, \"-c3gfx c:/games/caesar3/\"";
    }
  }

  if( !errorStr.empty() )
  {
    OSystem::error( "Resources error", errorStr );
    Logger::warning( "CRITICAL: not found original resources in " + c3res.toString() );
    exit( -1 ); //kill application    
  }

  loader.loadFromModel( GameSettings::rcpath( GameSettings::archivesModel ) );
}

void Game::Impl::initGuiEnvironment()
{
  gui = new gui::GuiEnv( *engine );
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

void Game::Impl::initPictures(vfs::Path resourcePath)
{
  AnimationBank::instance().loadCarts();
  AnimationBank::instance().loadAnimation( GameSettings::rcpath( GameSettings::animationsModel ) );
  
  Logger::warning( "Game: create runtime pictures" );
  PictureBank::instance().createResources();
}

void Game::setScreenBriefing()
{
  scene::Briefing screen( *this, *_d->engine, GameSettings::rcpath( _d->nextFilename ).toString() );
  screen.initialize();
  _d->currentScreen = &screen;

  while( !screen.isStopped() )
  {
    screen.update( *_d->engine );
  }

  switch( screen.result() )
  {
    case scene::Briefing::loadMission:
    {
      load( screen.getMapName() );
      Logger::warning( "Briefing: end loading map" );

      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;

    default:
      _CAESARIA_DEBUG_BREAK_IF( "Unexpected result event" );
   }
}

void Game::setScreenMenu()
{
  scene::StartMenu screen( *this, *_d->engine );
  screen.initialize();
  _d->currentScreen = &screen;

  while( !screen.isStopped() )
  {
    screen.update( *_d->engine );
  }

  reset();

  switch( screen.result() )
  {
    case scene::StartMenu::startNewGame:
    {  
      std::srand( DateTime::elapsedTime() );
      std::string startMission = "/missions/tutorial.mission";
      Logger::warning( "Start new career with mission " + startMission );

      load( startMission );
      _d->player->setName( screen.getPlayerName() );
      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;

      /*_d->nextFilename = "/missions/mission3.briefing";
      _d->nextScreen = SCREEN_BRIEFING;*/
    }
    break;

    case scene::StartMenu::reloadScreen:
      _d->nextScreen = SCREEN_MENU;
    break;
   
    case scene::StartMenu::loadSavedGame:
    case scene::StartMenu::loadMission:
    {        
      load( screen.getMapName() );
      Logger::warning( "screen menu: end loading map" );

      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;

    case scene::StartMenu::loadMap:
    {
      load( screen.getMapName() );
      Logger::warning( "screen menu: end loading map" );

      city::BuildOptions bopts;
      bopts = _d->city->buildOptions();
      bopts.setGroupAvailable( BM_MAX, true );
      _d->city->setBuildOptions( bopts );
      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;
   
    case scene::StartMenu::closeApplication:
    {
      _d->nextScreen = SCREEN_QUIT;
    }
    break;
   
    default:
      _CAESARIA_DEBUG_BREAK_IF( "Unexpected result event" );
   }
}

void Game::setScreenGame()
{
  Logger::warning( "game: enter setScreenGame" );
  scene::Level screen( *this, *_d->engine );
  
  Logger::warning( "game: start initialize" );
  screen.initialize();
  _d->currentScreen = &screen;
  GameDate& cdate = GameDate::instance();
  _d->time = cdate.current().day() * GameDate::days2ticks( 30 ) / cdate.current().daysInMonth();

  Logger::warning( "game: prepare for game loop" );
  while( !screen.isStopped() )
  {
    screen.update( *_d->engine );

    if( !_d->pauseCounter )
    {
      _d->time += _d->timeMultiplier / 100.f;

      while( (_d->time - _d->saveTime) > 1 )
      {
        _d->saveTime++;

        cdate.timeStep( _d->saveTime );
        _d->empire->timeStep( _d->saveTime );

        screen.animate( _d->saveTime );
      }
    }

    events::Dispatcher::instance().update( *this, _d->time );
  }

  _d->nextFilename = screen.nextFilename();
  switch( screen.result() )
  {
    case scene::Level::mainMenu: _d->nextScreen = SCREEN_MENU;  break;    
    case scene::Level::loadGame: _d->nextScreen = SCREEN_GAME;  load( screen.nextFilename() ); break;
    case scene::Level::loadBriefing: _d->nextScreen = SCREEN_BRIEFING; break;
    case scene::Level::quitGame: _d->nextScreen = SCREEN_QUIT;  break;
    default: _d->nextScreen = SCREEN_QUIT;
  }
}

PlayerPtr Game::player() const { return _d->player; }
PlayerCityPtr Game::city() const { return _d->city; }
world::EmpirePtr Game::empire() const { return _d->empire; }
gui::GuiEnv* Game::gui() const { return _d->gui; }
gfx::Engine* Game::engine() const { return _d->engine; }
scene::Base* Game::scene() const { return _d->currentScreen; }
bool Game::isPaused() const { return _d->pauseCounter>0; }
void Game::play() { setPaused( false ); }
void Game::pause() { setPaused( true ); }

void Game::setPaused(bool value)
{
  _d->pauseCounter = math::clamp( _d->pauseCounter + (value ? 1 : -1 ), 0, 99 );
}

Game::Game() : _d( new Impl )
{
  _d->nextScreen = SCREEN_NONE;
  _d->pauseCounter = 0;
  _d->time = 0;
  _d->saveTime = 0;
  _d->timeMultiplier = 70;
}

void Game::changeTimeMultiplier(int percent){  setTimeMultiplier( _d->timeMultiplier + percent );}
void Game::setTimeMultiplier(int percent){  _d->timeMultiplier = math::clamp<int>( percent, 10, 300 );}
int Game::timeMultiplier() const{  return _d->timeMultiplier;}

Game::~Game(){}

void Game::save(std::string filename) const
{
  GameSaver saver;
  saver.save( filename, *this );
}

void Game::load(std::string filename)
{  
  Logger::warning( "Game: try load from " + filename );

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
        return;
      }
    }
  }

  Logger::warning( "Game: reseting varialbes" );
  reset();
  
  Logger::warning( "Game: init empire start options" );
  _d->empire->initialize( GameSettings::rcpath( GameSettings::citiesModel ),
                          GameSettings::rcpath( GameSettings::worldModel ) );

  Logger::warning( "Game: try find loader" );
  GameLoader loader;
  _d->loadOk = loader.load( fPath, *this );
  
  if( !_d->loadOk )
  {
    Logger::warning( "LOADING ERROR: can't load game from %s", filename.c_str() );
    return;
  }

  Logger::warning( "Game: init player city" );
  _d->empire->initPlayerCity( ptr_cast<world::City>( _d->city ) );

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
  return;
}

void Game::initialize()
{
  GameSettings::load();
  _d->initLocale( GameSettings::get( GameSettings::localePath ).toString() );
  _d->initVideo();
  _d->initFontCollection( GameSettings::rcpath() );
  _d->initGuiEnvironment();  
  _d->initSound();

  splash::initialize( "logo_00001" );

  scene::SplashScreen screen;
  screen.initialize();
  screen.update( *_d->engine );

  ResourceLoader rcLoader;
  rcLoader.onStartLoading().connect( &screen, &scene::SplashScreen::setText );

  screen.setPrefix( "##loading_resources##" );
  _d->mountArchives( rcLoader );  // init some quick pictures for screenWait

  screen.setPrefix( "" );
  screen.setText( "##initialize_animations##" );
  _d->initPictures( GameSettings::rcpath() );

  screen.setText( "##initialize_names##" );
  NameGenerator::instance().initialize( GameSettings::rcpath( GameSettings::ctNamesModel ) );

  screen.setText( "##initialize_house_specification##" );
  HouseSpecHelper::instance().initialize( GameSettings::rcpath( GameSettings::houseModel ) );

  screen.setText( "##initialize_constructions##" );
  MetaDataHolder::instance().initialize( GameSettings::rcpath( GameSettings::constructionModel ) );

  screen.setText( "##initialize_walkers##" );
  WalkerHelper::instance().initialize( GameSettings::rcpath( GameSettings::walkerModel ) );

  screen.setText( "##initialize_religion##" );
  _d->initPantheon( GameSettings::rcpath( GameSettings::pantheonModel ) );

  screen.setText( "##ready_to_game" );
  if( GameSettings::get( "no-fade" ).isNull() )
    screen.fadeOut();
}

void Game::exec()
{
  _d->nextScreen = SCREEN_MENU;
  _d->engine->setFlag( gfx::Engine::debugInfo, 1 );

  while(_d->nextScreen != SCREEN_QUIT)
  {
    Logger::warning( "game: exec switch to screen %d", _d->nextScreen );
    switch(_d->nextScreen)
    {
      case SCREEN_MENU:        setScreenMenu();     break;
      case SCREEN_GAME:        setScreenGame();     break;
      case SCREEN_BRIEFING:    setScreenBriefing(); break;

      default:
        Logger::warning( "Unexpected next screen type %d", _d->nextScreen );
        //_CAESARIA_DEBUG_BREAK_IF( "Unexpected next screen type" );
    }
  }
}

void Game::reset()
{
  _d->empire = world::Empire::create();
  _d->player = Player::create();
  _d->pauseCounter = 0;
  if( _d->city.isValid() )
  {
    _d->city->clean();
  }
  _d->city = PlayerCity::create( _d->empire, _d->player );
}
