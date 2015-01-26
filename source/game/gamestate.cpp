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
#include "scene/menu.hpp"
#include "scene/level.hpp"
#include "gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "events/dispatcher.hpp"
#include "player.hpp"
#include "core/time.hpp"
#include "world/empire.hpp"
#include "freeplay_finalizer.hpp"
#include "steam.hpp"

using namespace scene;

namespace gamestate
{

void BaseState::_initialize(scene::Base* screen, ScreenType screenType) {
  this->_screen = screen;
  this->_screenType = screenType;
  this->_screen->initialize();
}

BaseState::BaseState(Game* game): _game(game), _screen(0), _screenType(SCREEN_NONE)
{
}

BaseState::~BaseState()
{
  delete _screen;
}

bool BaseState::update(gfx::Engine* engine)
{
  if (_screen->isStopped())
  {
    return false;
  }

#ifdef CAESARIA_USE_STEAM
  //steamapi::Handler::update();
#endif
  _screen->update(*engine);
  return true;
}

ScreenType BaseState::getScreenType()
{
  return _screenType;
}

scene::Base* BaseState::toBase() {  return _screen; }


MissionSelect::MissionSelect(Game* game, gfx::Engine* engine, const std::string& file):
  BaseState(game),
  _briefing(new scene::Briefing( *game, *engine, file ))
{
  _initialize(_briefing, SCREEN_BRIEFING);
}

MissionSelect::~MissionSelect()
{
  switch( _screen->result() )
  {
  case scene::Briefing::loadMission:
  {
    bool loadOk = _game->load( _briefing->getMapName() );
    Logger::warning( (loadOk ? "Briefing: end loading file" : "Briefing: cant load file") + _briefing->getMapName() );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF( "Briefing: unexpected result event" );
  }
}


ShowMainMenu::ShowMainMenu(Game* game, gfx::Engine* engine):
  BaseState(game),
  startMenu(new scene::StartMenu( *game, *engine ))
{
  _initialize(startMenu, SCREEN_MENU);
}

ShowMainMenu::~ShowMainMenu()
{
  _game->reset();

  switch( _screen->result() )
  {
  case scene::StartMenu::startNewGame:
  {
    std::srand( DateTime::elapsedTime() );
    std::string startMission = ":/missions/tutorial.mission";

    bool loadOk = _game->load( startMission );
    _game->player()->setName( startMenu->playerName() );

    Logger::warning( (loadOk ? "Career: start mission " : "Career: cant load mission") + startMission );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case scene::StartMenu::reloadScreen:
    _game->setNextScreen( SCREEN_MENU );
  break;

  case scene::StartMenu::loadSavedGame:
  case scene::StartMenu::loadMission:
  {
    bool loadOk = _game->load( startMenu->mapName() );
    Logger::warning( (loadOk ? "ScreenMenu: end loading mission/sav" : "ScreenMenu: cant load file") + startMenu->mapName()  );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case scene::StartMenu::loadMap:
  {
    bool loadOk = _game->load( startMenu->mapName() );
    Logger::warning( (loadOk ? "ScreenMenu: end loading map" : "ScreenMenu: cant load map") + startMenu->mapName() );

    game::freeplay::addPopulationMilestones( _game->city() );
    game::freeplay::initBuildOptions(_game->city() );
    game::freeplay::addEvents( _game->city());
    game::freeplay::resetFavour( _game->city() );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case scene::StartMenu::closeApplication:
  {
    _game->setNextScreen( SCREEN_QUIT );
  }
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF( "Unexpected result event" );
  }
}

GameLoop::GameLoop(Game* game, gfx::Engine* engine,
                                   unsigned int& saveTime,
                                   unsigned int& timeX10,
                                   unsigned int& timeMultiplier,
                                   unsigned int& manualTicksCounterX10,
                                   std::string& nextFilename,
                                   std::string& restartFilename) :
  BaseState(game),
  _level(new scene::Level( *game, *engine )),
  _saveTime( saveTime ),
  _timeX10( timeX10 ),
  _timeMultiplier( timeMultiplier ),
  _manualTicksCounterX10( manualTicksCounterX10 ),
  _nextFilename( nextFilename ),
  _restartFilename( restartFilename )
{
  _initialize(_level, SCREEN_GAME);

  Logger::warning( "game: prepare for game loop" );
}

bool GameLoop::update(gfx::Engine* engine)
{
  if (_screen->isStopped())
  {
    return false;
  }

  game::Date& cdate = game::Date::instance();

  _screen->update( *engine );

  if( _game->city()->tilemap().direction() == constants::north )
  {
    if( !_game->isPaused() )
    {
      _timeX10 += _timeMultiplier / 10;
    }
    else if ( _manualTicksCounterX10 > 0 )
    {
      unsigned int add = math::min( _timeMultiplier / 10, _manualTicksCounterX10 );
      _timeX10 += add;
      _manualTicksCounterX10 -= add;
    }

    while( _timeX10 > _saveTime * 10 + 1 )
    {
      _saveTime++;

      cdate.timeStep( _saveTime );
      _game->empire()->timeStep( _saveTime );

      _level->animate( _saveTime );
    }
  }

  events::Dispatcher::instance().update( *_game, _saveTime );
  return true;
}

GameLoop::~GameLoop()
{
  _game->clear();

  _nextFilename = _level->nextFilename();
  switch( _screen->result() )
  {
  case scene::Level::mainMenu: _game->setNextScreen( SCREEN_MENU );  break;
  case scene::Level::loadGame: _game->setNextScreen( SCREEN_GAME );  _game->load( _level->nextFilename() ); break;

  case scene::Level::restart:
  {
    Logger::warning( "ScreenGame: restart game " + _restartFilename );
    bool loadOk = _game->load( _restartFilename );
    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );

    Logger::warning( (loadOk ? "ScreenGame: end loading file " : "ScreenGame: cant load file " )+ _restartFilename );

    if( loadOk )
    {
      std::string ext = vfs::Path( _restartFilename ).extension();
      if( ext == ".map" || ext == ".sav" )
      {
        game::freeplay::addPopulationMilestones( _game->city() );
        game::freeplay::initBuildOptions( _game->city() );
        game::freeplay::addEvents( _game->city() );
      }
    }
  }
  break;

  case scene::Level::loadBriefing: _game->setNextScreen( SCREEN_BRIEFING ); break;
  case scene::Level::quitGame: _game->setNextScreen( SCREEN_QUIT );  break;
  default: _game->setNextScreen( SCREEN_QUIT ); break;
  }
}

}//end namespace gamestate
