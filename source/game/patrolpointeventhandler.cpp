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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "patrolpointeventhandler.hpp"
#include "core/event.hpp"
#include "gfx/city_renderer.hpp"
#include "gfx/tilemap_camera.hpp"
#include "game.hpp"
#include "city/city.hpp"
#include "objects/military.hpp"
#include "walker/patrolpoint.hpp"

using namespace constants;
using namespace gfx;

class PatrolPointEventHandler::Impl
{
public:
  Game* game;
  Renderer* renderer;
  PatrolPointPtr patrolPoint;
  TilePos savePatrolPos;
};

bool PatrolPointEventHandler::finished() const{  return false;}
PatrolPointEventHandler::~PatrolPointEventHandler(){}

PatrolPointEventHandler::PatrolPointEventHandler(Game& game, gfx::Renderer& renderer) : _d( new Impl )
{
  _d->game = &game;
  _d->renderer = &renderer;
}

scene::EventHandlerPtr PatrolPointEventHandler::create( Game& game, gfx::Renderer& renderer )
{
  scene::EventHandlerPtr handler( new PatrolPointEventHandler( game, renderer ) );
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
      Tile* tile = _d->renderer->camera()->at( event.mouse.pos(), true );
      if( tile )
      {
        if( _d->patrolPoint.isNull() )
        {
          PlayerCityPtr city = _d->game->city();
          PatrolPointList ppoints;
          ppoints << city->walkers( tile->pos() );

          if( !ppoints.empty() )
          {            
            _d->patrolPoint = ppoints.front();
            _d->savePatrolPos = _d->patrolPoint->pos();
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
        _d->patrolPoint->setPos( _d->savePatrolPos );
        _d->patrolPoint = PatrolPointPtr();
      }
    break;

    case mouseMoved:
    {
      if( _d->patrolPoint.isValid() )
      {
        Tile* tile = _d->renderer->camera()->at( event.mouse.pos(), true );
        if( tile )
        {
          _d->patrolPoint->setPos( tile->pos() );
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
