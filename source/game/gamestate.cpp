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
#include "config.hpp"

using namespace scene;

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
    bool loadOk = _game->load( _briefing->getMapName() );
    Logger::warning( (loadOk ? "Briefing: end loading file" : "Briefing: cant load file") + _briefing->getMapName() );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  default:
    _GAME_DEBUG_BREAK_IF( "Briefing: unexpected result event" );
  }
}

InMainMenu::InMainMenu(Game* game, gfx::Engine* engine):
  State(game),
  startMenu(new StartMenu( *game, *engine ))
{
  _initialize(startMenu, SCREEN_MENU);
}

InMainMenu::~InMainMenu()
{
  _game->reset();

  switch( _screen->result() )
  {
  case StartMenu::startNewGame:
  {
    std::srand( DateTime::elapsedTime() );
    std::string startMission = ":/missions/tutorial.mission";

    bool loadOk = _game->load( startMission );
    _game->player()->setName( startMenu->playerName() );

    Logger::warning( (loadOk ? "Career: start mission " : "Career: cant load mission") + startMission );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case StartMenu::reloadScreen:
    _game->setNextScreen( SCREEN_MENU );
  break;

  case StartMenu::loadSavedGame:
  case StartMenu::loadMission:
  {
    bool loadOk = _game->load( startMenu->mapName() );
    Logger::warning( (loadOk ? "ScreenMenu: end loading mission/sav" : "ScreenMenu: cant load file") + startMenu->mapName()  );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case StartMenu::loadMap:
  case StartMenu::loadConstructor:
  {
    bool loadOk = _game->load( startMenu->mapName() );
    Logger::warning( (loadOk ? "ScreenMenu: end loading map" : "ScreenMenu: cant load map") + startMenu->mapName() );

    game::freeplay::Finalizer finalizer( _game->city() );
    finalizer.addPopulationMilestones( );
    finalizer.initBuildOptions();
    finalizer.addEvents();
    finalizer.resetFavour();

    if( _screen->result() == StartMenu::loadConstructor )
      _game->city()->setOption( PlayerCity::constructorMode, 1 );

    _game->setNextScreen( loadOk ? SCREEN_GAME : SCREEN_MENU );
  }
  break;

  case StartMenu::closeApplication:
  {
    _game->setNextScreen( SCREEN_QUIT );
  }
  break;

  default:
    _GAME_DEBUG_BREAK_IF( "Unexpected result event" );
  }
}

}//end namespace gamestate
