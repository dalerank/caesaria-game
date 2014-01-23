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

#include "patrolpointeventhandler.hpp"
#include "core/event.hpp"
#include "gfx/city_renderer.hpp"
#include "gfx/tilemap_camera.hpp"
#include "game.hpp"
#include "city/city.hpp"
#include "objects/military.hpp"
#include "walker/patrolpoint.hpp"

using namespace constants;

class PatrolPointEventHandler::Impl
{
public:
  Game* game;
  CityRenderer* renderer;
  PatrolPointPtr patrolPoint;
  TilePos savePatrolPos;
};

bool PatrolPointEventHandler::finished() const{  return false;}
PatrolPointEventHandler::~PatrolPointEventHandler(){}

PatrolPointEventHandler::PatrolPointEventHandler(Game& game, CityRenderer& renderer) : _d( new Impl )
{
  _d->game = &game;
  _d->renderer = &renderer;
}

EventHandlerPtr PatrolPointEventHandler::create( Game& game, CityRenderer& renderer )
{
  EventHandlerPtr handler( new PatrolPointEventHandler( game, renderer ) );
  handler->drop();

  return handler;
}

void PatrolPointEventHandler::handleEvent( NEvent& event )
{
  switch( event.EventType )
  {
  case sEventMouse:
    switch( event.mouse.type )
    {
    case mouseLbtnRelease:
    {
      Tile* tile = _d->renderer->getCamera().at( event.mouse.getPosition(), true );
      if( tile )
      {
        if( _d->patrolPoint.isNull() )
        {
          PlayerCityPtr city = _d->game->getCity();
          WalkerList walkers = city->getWalkers( walker::patrolPoint, tile->getIJ() );
          if( !walkers.empty() )
          {            
            _d->patrolPoint = ptr_cast<PatrolPoint>( walkers.front() );
            _d->savePatrolPos = _d->patrolPoint->getIJ();
          }
        }
        else
        {
          _d->patrolPoint->acceptPosition();
          _d->patrolPoint = PatrolPointPtr();
        }
      }
    }
    break;

    case mouseRbtnRelease:
      if( _d->patrolPoint.isValid() )
      {
        _d->patrolPoint->setIJ( _d->savePatrolPos );
        _d->patrolPoint = PatrolPointPtr();
      }
    break;

    case mouseMoved:
    {
      if( _d->patrolPoint.isValid() )
      {
        Tile* tile = _d->renderer->getCamera().at( event.mouse.getPosition(), true );
        if( tile )
        {
          _d->patrolPoint->setIJ( tile->getIJ() );
        }
      }
    }
    break;

    default: break;
    }
  break;

  default: break;
  }
}
