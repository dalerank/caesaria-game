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

#include "oc3_game.hpp"
#include "oc3_screen_wait.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_city.hpp"
#include "oc3_picture.hpp"
#include "oc3_gfx_sdl_engine.hpp"
#include "oc3_gfx_gl_engine.hpp"
#include "oc3_sound_engine.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_building_data.hpp"
#include "oc3_picture_bank.hpp"
#include "oc3_screen_menu.hpp"
#include "oc3_screen_game.hpp"
#include "oc3_house_level.hpp"
#include "oc3_guienv.hpp"
#include "oc3_game_settings.hpp"
#include "oc3_divinity.hpp"
#include "oc3_filesystem.hpp"
#include "oc3_enums.hpp"
#include "oc3_animation_bank.hpp"
#include "oc3_filesystem_filelist.hpp"
#include "oc3_empire.hpp"
#include "oc3_exception.hpp"
#include "oc3_name_generator.hpp"
#include "oc3_scenario_loader.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_game_event_mgr.hpp"

#include <libintl.h>
#include <list>

#if defined(OC3_PLATFORM_WIN)
  #undef main
#endif

class Game::Impl
{
public:
  ScreenType nextScreen;
  GfxEngine* engine;
  GuiEnv* gui;

  EmpirePtr empire;
  CityPtr city;
  Player* player;
  CityWinTargets targets;

  bool loadOk;
  bool paused;

  unsigned int time, saveTime;
  float timeMultiplier;
  
  void initLocale(const std::string & localePath);
  void initVideo();
  void initPictures(const io::FilePath& resourcePath);
  void initGuiEnvironment();
};

void Game::Impl::initLocale(const std::string & localePath)
{
  // init the internationalization library (gettext)
  setlocale(LC_ALL, "");
  bindtextdomain( "caesar", localePath.data() );
  textdomain( "caesar" );
}

void Game::Impl::initVideo()
{
  StringHelper::debug( 0xff, "init graphic engine" );
  engine = new GfxSdlEngine();
   
  /* Typical resolutions:
   * 640 x 480; 800 x 600; 1024 x 768; 1400 x 1050; 1600 x 1200
   */
  GfxEngine::instance().setScreenSize(1024, 768);
  GfxEngine::instance().init();
}

void Game::initSound()
{
  StringHelper::debug( 0xff, "init sound engine" );
  new SoundEngine();
  SoundEngine::instance().init();
}

void Game::mountArchives()
{
  StringHelper::debug( 0xff, "mount archives begin" );

  io::FileSystem& fs = io::FileSystem::instance();

  fs.mountArchive( GameSettings::rcpath( "/pics/pics_wait.zip" ) );
  fs.mountArchive( GameSettings::rcpath( "/pics/pics.zip" ) );
  fs.mountArchive( GameSettings::rcpath( "/pics/pics_oc3.zip" ) );
}

void Game::Impl::initGuiEnvironment()
{
  GuiEnv::instance().initialize( *engine );
}

void Game::Impl::initPictures(const io::FilePath& resourcePath)
{
  AnimationBank::loadCarts();
  AnimationBank::loadWalkers();
  
  StringHelper::debug( 0xff, "Load fonts" );
  FontCollection::instance().initialize( resourcePath.toString() );

  StringHelper::debug( 0xff, "Create runtime pictures" );
  PictureBank::instance().createResources();
}

void Game::setScreenWait()
{
   ScreenWait screen;
   screen.initialize();
   screen.drawFrame();
}

void Game::setScreenMenu()
{
  ScreenMenu screen;
  screen.initialize();

  int result = screen.run();
  reset();

  switch( result )
  {
    case ScreenMenu::startNewGame:
    {
      /* temporary*/     
      io::FileList::Items maps = io::FileDir( GameSettings::rcpath( "/maps/" ) ).getEntries().filter( io::FileList::file, "" ).getItems();
      std::srand( DateTime::getElapsedTime() );
      std::string file = maps.at( std::rand() % maps.size() ).fullName.toString();
      StringHelper::debug( 0xff, "Loading map:%s", file.c_str() );

      load( file );

      _d->nextScreen = _d->loadOk ? SCREEN_GAME : SCREEN_MENU;
    }
    break;
   
    case ScreenMenu::loadSavedGame:
    {  
      std::cout<<"Loading map:" << "lepcismagna.sav" << std::endl;
      load(  GameSettings::rcpath( "/savs/timgad.sav" ).toString() );

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
      _OC3_DEBUG_BREAK_IF( "Unexpected result event" );
   }
}

void Game::setScreenGame()
{
  ScreenGame screen( *this );
  screen.initialize();

  CONNECT( &screen, onFrameRenderFinished(), this, Game::timeStep );

  int result = screen.run();

  switch( result )
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

void Game::timeStep()
{
  if( _d->paused )
    return;

  _d->time += _d->timeMultiplier / 100.f;

  while( (_d->time - _d->saveTime) > 1 )
  {
    _d->empire->timeStep( _d->time );

    GameDate::timeStep( _d->time );

    _d->saveTime += 1;

    GameEventMgr::update( _d->time );
  }
}

Player* Game::getPlayer() const
{
  return _d->player;
}

CityPtr Game::getCity() const
{
  return _d->city;
}

EmpirePtr Game::getEmpire() const
{
  return _d->empire;
}

void Game::setWinTargets(const CityWinTargets& targets)
{
  _d->targets = targets;
}

void Game::setPaused(bool value)
{
  _d->paused = value;
}

bool Game::isPaused() const
{
  return _d->paused;
}

Game::Game() : _d( new Impl )
{
  _d->nextScreen = SCREEN_NONE;
  _d->paused = false;
  _d->time = 0;
  _d->saveTime = 0;
  _d->timeMultiplier = 100;

  CONNECT( &GameEventMgr::instance(), onEvent(), this, Game::resolveEvent );
}

void Game::changeTimeMultiplier(int percent)
{
  _d->timeMultiplier = math::clamp<int>( _d->timeMultiplier + percent, 10, 300 );
}

void Game::resolveEvent(GameEventPtr event)
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

}

void Game::load(std::string filename)
{
  StringHelper::debug( 0xff, "Load game begin" );

  _d->empire->initialize( GameSettings::rcpath( GameSettings::citiesModel ) );

  GameLoader loader;
  _d->loadOk = loader.load( filename, *this);

  if( !_d->loadOk )
  {
    StringHelper::debug( 0xff, "LOADING ERROR: can't load game from %s", filename.c_str() );
    return;
  }

  _d->empire->initPlayerCity( _d->city.as<EmpireCity>() );

  LandOverlayList& llo = _d->city->getOverlayList();
  foreach( LandOverlayPtr overlay, llo )
  {
    ConstructionPtr construction = overlay.as<Construction>();
    if( construction.isValid() )
    {
      construction->computeAccessRoads();
    }
  }

  Pathfinder::getInstance().update( _d->city->getTilemap() );

  StringHelper::debug( 0xff, "Load game end" );
  return;
}

void Game::initialize()
{
  StringHelper::redirectCout2( "stdout.log" );

  _d->initLocale(GameSettings::get( GameSettings::localePath ).toString());
  _d->initVideo();
  _d->initGuiEnvironment();
  initSound();
  //SoundEngine::instance().play_music("resources/sound/drums.wav");
  mountArchives();  // init some quick pictures for screenWait
  setScreenWait();

  _d->initPictures( GameSettings::rcpath() );
  NameGenerator::initialize( GameSettings::rcpath( GameSettings::ctNamesModel ) );
  HouseSpecHelper::getInstance().initialize( GameSettings::rcpath( GameSettings::houseModel ) );
  DivinePantheon::getInstance().initialize(  GameSettings::rcpath( GameSettings::pantheonModel ) );
  BuildingDataHolder::instance().initialize( GameSettings::rcpath( GameSettings::constructionModel ) );
}

void Game::exec()
{
  _d->nextScreen = SCREEN_MENU;
  _d->engine->setFlag( 0, 1 );

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
        _OC3_DEBUG_BREAK_IF( "Unexpected next screen type" );
        StringHelper::debug( 0xff, "Unexpected next screen type %d", _d->nextScreen );
     }
  }
}

void Game::reset()
{
  _d->empire = Empire::create();
  _d->player = new Player();
  _d->city = City::create( _d->empire, _d->player );
}
