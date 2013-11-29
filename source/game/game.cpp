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
#include "game/screen_wait.hpp"
#include "core/stringhelper.hpp"
#include "city.hpp"
#include "gfx/picture.hpp"
#include "gfx/sdl_engine.hpp"
#include "sound/oc3_sound_engine.hpp"
#include "astarpathfinding.hpp"
#include "building/metadata.hpp"
#include "gfx/picture_bank.hpp"
#include "screen_menu.hpp"
#include "screen_game.hpp"
#include "house_level.hpp"
#include "gui/environment.hpp"
#include "settings.hpp"
#include "divinity.hpp"
#include "vfs/filesystem.hpp"
#include "enums.hpp"
#include "gfx/animation_bank.hpp"
#include "vfs/entries.hpp"
#include "world/empire.hpp"
#include "core/exception.hpp"
#include "name_generator.hpp"
#include "loader.hpp"
#include "win_targets.hpp"
#include "gamedate.hpp"
#include "saver.hpp"
#include "core/saveadapter.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"

#include <libintl.h>
#include <list>

#if defined(CAESARIA_PLATFORM_WIN)
  #undef main
#endif

class Game::Impl
{
public:
  ScreenType nextScreen;
  GfxEngine* engine;
  gui::GuiEnv* gui;

  world::EmpirePtr empire;
  PlayerCityPtr city;
  PlayerPtr player;

  bool loadOk;
  int pauseCounter;

  float time, saveTime;
  float timeMultiplier;
  
  void initLocale(const std::string & localePath);
  void initVideo();
  void initPictures(const vfs::Path& resourcePath);
  void initGuiEnvironment();
  void loadSettings(const vfs::Path& filename);
  void initPantheon( const vfs::Path& filename );
};

void Game::Impl::initLocale(const std::string & localePath)
{
  // init the internationalization library (gettext)
#ifdef CAESARIA_PLATFORM_WIN
  ByteArray localeData;
  localeData = StringHelper::format( 0xff, "LC_ALL=%s", GameSettings::get( GameSettings::localeName ).toString().c_str() );

  putenv( localeData.data() );
#else
  setlocale(LC_ALL, "");
#endif
  bindtextdomain( "caesar", localePath.data() );
  bind_textdomain_codeset( "caesar", "UTF-8" );
  textdomain( "caesar" );
}

void Game::Impl::initVideo()
{
  Logger::warning( "init graphic engine" );
  engine = new GfxSdlEngine();
   
  /* Typical resolutions:
   * 640 x 480; 800 x 600; 1024 x 768; 1400 x 1050; 1600 x 1200
   */

  engine->setScreenSize( GameSettings::get( GameSettings::resolution ).toSize() );
  engine->setFlag( GfxEngine::fullscreen, GameSettings::get( GameSettings::fullscreen ).toBool() ? 1 : 0 );
  engine->init();
}

void Game::initSound()
{
  Logger::warning( "init sound engine" );
  new SoundEngine();
  SoundEngine::instance().init();
}

void Game::mountArchives()
{
  Logger::warning( "mount archives begin" );

  vfs::FileSystem& fs = vfs::FileSystem::instance();

  fs.mountArchive( GameSettings::rcpath( "/pics/pics_wait.zip" ) );
  fs.mountArchive( GameSettings::rcpath( "/pics/pics.zip" ) );
  fs.mountArchive( GameSettings::rcpath( "/pics/pics_oc3.zip" ) );
  fs.mountArchive( GameSettings::rcpath( "/pics/pics_celts.zip" ) );
}

void Game::Impl::initGuiEnvironment()
{
  gui = new gui::GuiEnv( *engine );
}

void Game::Impl::loadSettings( const vfs::Path& filename )
{
  VariantMap settings = SaveAdapter::load( filename );

  foreach( VariantMap::value_type& v, settings )
  {
    GameSettings::set( v.first, v.second );
  }
}

void Game::Impl::initPantheon(const vfs::Path& filename)
{
  VariantMap pantheon = SaveAdapter::load( filename );
  DivinePantheon::getInstance().load( pantheon );
}

void Game::Impl::initPictures(const vfs::Path& resourcePath)
{
  AnimationBank::loadCarts();
  AnimationBank::loadWalkers();
  
  Logger::warning( "Load fonts" );
  FontCollection::instance().initialize( resourcePath.toString() );

  Logger::warning( "Create runtime pictures" );
  PictureBank::instance().createResources();
}

void Game::setScreenWait()
{
   ScreenWait screen;
   screen.initialize();
   screen.update( *_d->engine );
}

void Game::setScreenMenu()
{
  ScreenMenu screen( *this, *_d->engine );
  screen.initialize();

  while( !screen.isStopped() )
  {
    screen.update( *_d->engine );
  }

  reset();

  switch( screen.getResult() )
  {
    case ScreenMenu::startNewGame:
    {
      /* temporary*/     
      vfs::Entries::Items maps = vfs::Directory( GameSettings::rcpath( "/maps/" ) ).getEntries().filter( vfs::Entries::file, "" ).getItems();
      std::srand( DateTime::getElapsedTime() );
      std::string file = maps.at( std::rand() % maps.size() ).fullName.toString();
      Logger::warning( "Loading map:%s", file.c_str() );

      load( file );

      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;
   
    case ScreenMenu::loadSavedGame:
    {  
      std::cout<<"Loading map:" << "lepcismagna.sav" << std::endl;
      load( GameSettings::rcpath( "/savs/timgad.sav" ).toString() );

      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;

    case ScreenMenu::loadMap:
    {
      load( screen.getMapName() );
      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;
   
    case ScreenMenu::closeApplication:
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
  ScreenGame screen( *this, *_d->engine );
  screen.initialize();

  while( !screen.isStopped() )
  {
    screen.update( *_d->engine );

    if( !_d->pauseCounter )
    {
      _d->time += _d->timeMultiplier / 100.f;

      while( (_d->time - _d->saveTime) > 1 )
      {
        _d->empire->timeStep( _d->time );

        GameDate::timeStep( _d->time );

        _d->saveTime += 1;

        screen.animate( _d->saveTime );
      }
    }

    events::Dispatcher::update( _d->time );
  }

  switch( screen.getResult() )
  {
    case ScreenGame::mainMenu:
      _d->nextScreen = SCREEN_MENU;
    break;

    case ScreenGame::quitGame:
      _d->nextScreen = SCREEN_QUIT;
    break;

    default:
      _d->nextScreen = SCREEN_QUIT;
  }
}

PlayerPtr Game::getPlayer() const { return _d->player; }
PlayerCityPtr Game::getCity() const { return _d->city; }
world::EmpirePtr Game::getEmpire() const { return _d->empire; }
gui::GuiEnv* Game::getGui() const { return _d->gui; }
GfxEngine*Game::getEngine() const { return _d->engine; }
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

  CONNECT( &events::Dispatcher::instance(), onEvent(), this, Game::resolveEvent );
}

void Game::changeTimeMultiplier(int percent)
{
  setTimeMultiplier( _d->timeMultiplier + percent );
}

void Game::setTimeMultiplier(int percent)
{
  _d->timeMultiplier = math::clamp<int>( percent, 10, 300 );
}

int Game::getTimeMultiplier() const
{
  return _d->timeMultiplier;
}

void Game::resolveEvent( events::GameEventPtr event )
{
  if( event.isValid() )
  {
    event->exec( *this );
  }
}


Game::~Game()
{

}

void Game::save(std::string filename) const
{
  GameSaver saver;
  saver.save( filename, *this );
}

void Game::load(std::string filename)
{
  Logger::warning( "Load game begin" );

  _d->empire->initialize( GameSettings::rcpath( GameSettings::citiesModel ) );

  GameLoader loader;
  _d->loadOk = loader.load( filename, *this);

  if( !_d->loadOk )
  {
    Logger::warning( "LOADING ERROR: can't load game from %s", filename.c_str() );
    return;
  }

  _d->empire->initPlayerCity( _d->city.as<world::City>() );

  TileOverlayList& llo = _d->city->getOverlays();
  foreach( TileOverlayPtr overlay, llo )
  {
    ConstructionPtr construction = overlay.as<Construction>();
    if( construction.isValid() )
    {
      construction->computeAccessRoads();
    }
  }

  Pathfinder::getInstance().update( _d->city->getTilemap() );

  Logger::warning( "Load game end" );
  return;
}

void Game::initialize()
{
  Logger::registerWriter( Logger::consolelog );
  Logger::registerWriter( Logger::filelog );

  _d->loadSettings( GameSettings::rcpath( GameSettings::settingsPath ) );
  _d->initLocale( GameSettings::get( GameSettings::localePath ).toString() );
  _d->initVideo();
  _d->initGuiEnvironment();
  initSound();
  //SoundEngine::instance().play_music("resources/sound/drums.wav");
  mountArchives();  // init some quick pictures for screenWait
  setScreenWait();

  _d->initPictures( GameSettings::rcpath() );
  NameGenerator::initialize( GameSettings::rcpath( GameSettings::ctNamesModel ) );
  HouseSpecHelper::getInstance().initialize( GameSettings::rcpath( GameSettings::houseModel ) );
  MetaDataHolder::instance().initialize( GameSettings::rcpath( GameSettings::constructionModel ) );
  _d->initPantheon( GameSettings::rcpath( GameSettings::pantheonModel ) );
}

void Game::exec()
{
  _d->nextScreen = SCREEN_MENU;
  _d->engine->setFlag( GfxEngine::debugInfo, 1 );

  while(_d->nextScreen != SCREEN_QUIT)
  {
     switch(_d->nextScreen)
     {
     case SCREEN_MENU:
        setScreenMenu();
     break;

     case SCREEN_GAME:
        setScreenGame();
     break;

     default:
        Logger::warning( "Unexpected next screen type %d", _d->nextScreen );
        _CAESARIA_DEBUG_BREAK_IF( "Unexpected next screen type" );
     }
  }
}

void Game::reset()
{
  _d->empire = world::Empire::create();
  _d->player = Player::create();
  _d->pauseCounter = 0;
  _d->city = PlayerCity::create( _d->empire, _d->player );
}
