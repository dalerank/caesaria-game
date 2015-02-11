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

#include "earthquake.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "core/variant_map.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "gfx/tilemap.hpp"
#include "events/disaster.hpp"
#include "factory.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

REGISTER_EVENT_IN_FACTORY(EarthQuake, "earthquake")

class EarthQuake::Impl
{
public:
  VariantMap events;
  unsigned int lastTimeUpdate;
  TilePos startPoint, endPoint, currentPoint;
};

GameEventPtr EarthQuake::create(TilePos start, TilePos stop)
{
  EarthQuake* eq = new EarthQuake();
  eq->_d->startPoint = start;
  eq->_d->endPoint = stop;
  eq->_d->currentPoint = start;

  GameEventPtr ret( eq );
  ret->drop();

  return ret;
}

GameEventPtr EarthQuake::create()
{
  GameEventPtr ret( new EarthQuake() );
  ret->drop();

  return ret;
}

bool EarthQuake::isDeleted() const
{
  return _d->currentPoint == _d->endPoint;
}

void EarthQuake::_exec( Game& game, unsigned int time)
{
  if( game::Date::isDayChanged() && time != _d->lastTimeUpdate )
  {
    _d->lastTimeUpdate = time;
    Logger::warning( "Execute earthquake event" );

    Tilemap& tmap = game.city()->tilemap();
    Tile* currentTile = &tmap.at( _d->currentPoint );
    if( currentTile )
    {
      bool mayDestruct = currentTile->getFlag( Tile::isConstructible );
      mayDestruct |= is_kind_of<Construction>( currentTile->overlay() );

      if( mayDestruct )
      {
        events::GameEventPtr e = events::Disaster::create( *currentTile, Disaster::rift );
        e->dispatch();
      }
    }

    //calculate next point
    TilesArray nextPoints = tmap.getNeighbors(_d->currentPoint, Tilemap::FourNeighbors);

    int lastDst = _d->currentPoint.getDistanceFromSQ(_d->endPoint);
    for( TilesArray::iterator it=nextPoints.begin(); it != nextPoints.end(); )
    {
      bool mayDestruct = (*it)->getFlag( Tile::isConstructible );
      mayDestruct |= is_kind_of<Construction>( (*it)->overlay() );
      int curDst = (*it)->pos().getDistanceFromSQ(_d->endPoint);
      if( !mayDestruct || (curDst > lastDst) ) { it = nextPoints.erase( it ); }
      else { ++it; }
    }

    if( nextPoints.empty() )
    {
      TilePos offset = _d->endPoint - _d->currentPoint;
      offset.setI( math::signnum( offset.i() ) );
      offset.setJ( math::signnum( offset.j() ) );
      _d->currentPoint += offset;
    }
    else
    {
      _d->currentPoint = nextPoints.random()->pos();
    }

    if( _d->currentPoint == _d->endPoint )
    {
      events::Dispatcher::instance().load( _d->events );
    }
  }
}

bool EarthQuake::_mayExec(Game&, unsigned int) const { return true; }

void EarthQuake::load(const VariantMap& stream)
{
  GameEvent::load( stream );
  _d->events = stream.get( "onFinished" ).toMap();
  _d->startPoint = stream.get( "start" ).toTilePos();
  _d->endPoint = stream.get( "end" ).toTilePos();
  _d->currentPoint = stream.get( "current", TilePos( -1, -1 ) ).toTilePos();

  if( _d->currentPoint == TilePos( -1, -1 ) )
  {
    _d->currentPoint = _d->startPoint;
  }
}

VariantMap EarthQuake::save() const
{
  VariantMap ret = GameEvent::save();

  ret[ "onFinished" ] = _d->events;
  ret[ "start" ] = _d->startPoint;
  ret[ "end" ] = _d->endPoint;
  ret[ "current" ] = _d->currentPoint;

  return ret;
}

EarthQuake::EarthQuake() : _d( new Impl )
{
  _d->currentPoint = TilePos( -1, -1 );
  _d->lastTimeUpdate = 0;
}

}//end namespace events
