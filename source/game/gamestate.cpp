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

#include "gamestate.hpp"
#include "scene/briefing.hpp"
#include "core/logger.hpp"
#include "scene/lobby.hpp"
#include "scene/splashscreen.hpp"
#include "scene/level.hpp"
#include "gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "events/dispatcher.hpp"
#include "player.hpp"
#include "core/time.hpp"
#include "world/empire.hpp"
#include "freeplay_finalizer.hpp"
#include "gfx/animation_bank.hpp"
#include "objects/house_spec.hpp"
#include "gfx/engine.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"
#include "game/roman_celebrates.hpp"
#include "core/osystem.hpp"
#include "resourceloader.hpp"
#include "walker/name_generator.hpp"
#include "walker/helper.hpp"
#include "scripting/core.hpp"
#include "objects/infodb.hpp"
#include "religion/pantheon.hpp"
#include "gfx/picture_info_bank.hpp"
#include "steam.hpp"
#include "config.hpp"

using namespace scene;
using namespace gfx;

namespace gamestate
{

void State::_initialize(scene::Base* screen, ScreenType screenType)
{
  this->_screen = screen;
  this->_screenType = screenType;
  this->_screen->initialize();
}

State::State(Game* game): _game(game), _screen(0), _screenType(SCREEN_NONE)
{
}

State::~State()
{
  delete _screen;
}

bool State::update(gfx::Engine* engine)
{
  if (_screen->isStopped())
  {
    return false;
  }

  _screen->update(*engine);
  return true;
}

ScreenType State::getScreenType()
{
  return _screenType;
}

scene::Base* State::toBase() {  return _screen; }

InBriefing::InBriefing(Game* game, gfx::Engine* engine, const std::string& file)
   : State(game),
  _briefing(new scene::Briefing( *game, *engine, file ))
{
  _initialize(_briefing, SCREEN_BRIEFING);
}

InBriefing::~InBriefing()
{
  switch( _screen->result() )
  {
  case Briefing::loadMission:
  {
    bool loadOk = _game->load(_briefing->getMapName());
    Logger::debug( (loadOk ? "Briefing: end loading file" : "Briefing: cant load file") + _briefing->getMapName() );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  default:
    _GAME_DEBUG_BREAK_IF( "Briefing: unexpected result event" );
  }
}

InMainMenu::InMainMenu(Game* game, gfx::Engine* engine):
  State(game),
  _startMenu(new Lobby( *game, *engine ))
{
  _initialize(_startMenu, SCREEN_MENU);
}

InMainMenu::~InMainMenu()
{
  _game->reset();

  switch( _screen->result() )
  {
  case Lobby::startNewGame:
  {
    std::srand( DateTime::elapsedTime() );
    std::string startMission = ":/missions/tutorial.mission";

    bool loadOk = _game->load( startMission );
    _game->player()->setName( _startMenu->playerName() );

    Logger::debug( (loadOk ? "Career: start mission " : "Career: cant load mission") + startMission );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case Lobby::reloadScreen:
    _game->setNextScreen( SCREEN_MENU );
  break;

  case Lobby::loadSavedGame:
  case Lobby::loadMission:
  {
    bool loadOk = _game->load( _startMenu->mapName() );
    Logger::debug( (loadOk ? "ScreenMenu: end loading mission/sav" : "ScreenMenu: cant load file") + _startMenu->mapName()  );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case Lobby::loadMap:
  case Lobby::loadConstructor:
  {
    bool loadOk = _game->load( _startMenu->mapName() );
    Logger::debug( (loadOk ? "ScreenMenu: end loading map" : "ScreenMenu: cant load map") + _startMenu->mapName() );

    game::freeplay::Finalizer finalizer( _game->city() );
    finalizer.addPopulationMilestones( );
    finalizer.initBuildOptions();
    finalizer.addEvents();
    finalizer.resetFavour();

    if( _screen->result() == Lobby::loadConstructor )
      _game->city()->setOption( PlayerCity::constructorMode, 1 );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case Lobby::res_close:
  {
    _game->setNextScreen( SCREEN_QUIT );
  }
  break;

  default:
    _GAME_DEBUG_BREAK_IF( "Unexpected result event" );
  }
}

InSplash::InSplash(Game* game) :
  State(game),
  _splash(new SplashScreen())
{
  _initialize(_splash,SCREEN_LOGO);
}

void InSplash::initPictures(bool& isOk , std::string& result)
{
  result = "##initialize_animations##";

  AnimationBank::instance().loadCarts( SETTINGS_RC_PATH( cartsModel ) );
  AnimationBank::instance().loadAnimation( SETTINGS_RC_PATH( animationsModel ),
                                           SETTINGS_RC_PATH( simpleAnimationModel ) );
}

void InSplash::initNameGenerator(bool& isOk, std::string& result)
{
  result = "##initialize_names##";
  NameGenerator::instance().initialize( SETTINGS_RC_PATH( ctNamesModel ) );
  NameGenerator::instance().setLanguage( SETTINGS_STR( language ) );
}

void InSplash::loadHouseSpecs(bool& isOk, std::string& result)
{
  result = "##initialize_house_specification##";
  HouseSpecHelper::instance().initialize( SETTINGS_RC_PATH( houseModel ) );
}

void InSplash::loadObjectsMetadata(bool& isOk, std::string& result)
{
  result = "##initialize_constructions##";
  object::InfoDB::instance().initialize( SETTINGS_RC_PATH( constructionModel ) );
}

void InSplash::loadWalkersMetadata(bool& isOk, std::string& result)
{
  result = "##initialize_walkers##";
  WalkerHelper::instance().load( SETTINGS_RC_PATH( walkerModel ) );
}

void InSplash::loadReligionConfig(bool& isOk, std::string& result)
{
  result = "##initialize_religion##";
  script::Core::execFunction("OnInitRomePantheon");  
}

void InSplash::fadeSplash(bool& isOk, std::string& result)
{
  result = "##ready_to_game##";

  if(game::Settings::get("no-fade").isNull())
  {
    _splash->setOption("show-dev-text", scene::SplashScreen::showDevText);
    _splash->setMode(scene::SplashScreen::exit);
  }
}

bool InSplash::update(gfx::Engine* engine)
{
#define ADD_STEP(obj,functor) { #functor, makeDelegate(obj,&functor) }
  std::vector<InitializeStep> steps = {
    ADD_STEP(this, InSplash::loadResources),
    ADD_STEP(this, InSplash::initScripts),
    ADD_STEP(this, InSplash::initSplashScreen ),
    ADD_STEP(this, InSplash::initCelebrations ),
    ADD_STEP(this, InSplash::loadPicInfo ),
    ADD_STEP(this, InSplash::initPictures ),
    ADD_STEP(this, InSplash::initNameGenerator ),
    ADD_STEP(this, InSplash::loadHouseSpecs ),
    ADD_STEP(this, InSplash::loadObjectsMetadata ),
    ADD_STEP(this, InSplash::loadWalkersMetadata ),
    ADD_STEP(this, InSplash::loadReligionConfig ),
    ADD_STEP(this, InSplash::fadeSplash )
  };
#undef ADD_STEP

  for (auto& step : steps)
  {
    bool isOk = true;
    std::string stepText;

    try
    {
      step.function(isOk, stepText);
      _splash->setOption("tooltip", stepText);
      if (!isOk)
      {
        Logger::error( "Game: initialize faild on step {}", step.name );
        OSystem::error( "Game: initialize faild on step", step.name );
        exit(-1); //kill application
      }
    }
    catch(...) { exit(-1); }
  }

  _game->setNextScreen(SCREEN_MENU);
  return false;
}

InSplash::~InSplash() {}

void InSplash::initScripts(bool& isOk, std::string& result)
{
  script::Core::instance();
  script::Core::registerFunctions(*_game);
}

void InSplash::updateSplashText(std::string text)
{
  _splash->setOption("tooltip", text);
}

void InSplash::loadPicInfo(bool& isOk, std::string& result)
{
  Logger::debug( "Game: initialize offsets" );
  result = "##loading_offsets##";
  PictureInfoBank::instance().initialize( SETTINGS_RC_PATH( pic_offsets ) );
}

void InSplash::initCelebrations(bool& isOk, std::string& result)
{
  vfs::Path value = SETTINGS_RC_PATH(celebratesConfig);
  game::Celebrates::instance().load(value);
}

void InSplash::mountArchives(ResourceLoader &loader)
{
  Logger::debug( "Game: mount archives begin" );

  std::string errorStr;
  std::string c3res = SETTINGS_STR(c3gfx);
  if (!c3res.empty())
  {
    vfs::Directory gfxDir( c3res );
    vfs::Path c3path = gfxDir/"c3.sg2";

    if (!c3path.exist(vfs::Path::ignoreCase))
    {
      errorStr = "This game use resources files (.sg2, .map) from Caesar III(c), but "
                 "original game archive c3.sg2 not found in folder " + c3res +
                 "!!!.\nBe sure that you copy all .sg2, .map and .smk files placed to resource folder";
      SETTINGS_SET_VALUE(c3gfx, std::string( "" ) );
      game::Settings::save();
    }

    loader.loadFromModel(SETTINGS_RC_PATH(sg2model), gfxDir);
    _game->engine()->setFlag( Engine::batching, false );
  }
  else
  {
    vfs::Path testPics = SETTINGS_RC_PATH(picsArchive);
    if (!testPics.exist())
    {
      SETTINGS_SET_VALUE( resourcePath, Variant("") );
      game::Settings::save();
      errorStr = "Not found graphics package. Use precompiled CaesarIA archive or use\n"
                 "-c3gfx flag to set absolute path to Caesar III(r) installation folder,\n"
                 "forexample, \"-c3gfx c:/games/caesar3/\"";
    }

    loader.loadFromModel(SETTINGS_RC_PATH(remakeModel));
  }

  if (!errorStr.empty())
  {
    OSystem::error("Resources error", errorStr);
    Logger::debug("CRITICAL: not found original resources in " + c3res);
    exit(-1); //kill application
  }

  loader.loadFromModel( SETTINGS_RC_PATH( archivesModel ) );
}

void InSplash::initSplashScreen(bool& isOk, std::string& result)
{
  script::Core::execFunction("OnStartSplashScreen");
}

void InSplash::loadResources(bool& isOk, std::string& result)
{
  Logger::debug( "Game: initialize resource loader" );
  ResourceLoader rcLoader;
  rcLoader.loadFiles(SETTINGS_RC_PATH(logoArchive));
  //rcLoader.onStartLoading().connect( this, &InSplash::updateSplashText );

  Logger::debug( "Game: initialize resources" );
  mountArchives(rcLoader);  // init some quick pictures for screenWait
}

}//end namespace gamestate
