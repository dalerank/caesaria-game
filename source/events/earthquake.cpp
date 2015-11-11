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
#include "objects/construction.hpp"
#include "events/disaster.hpp"
#include "gfx/helper.hpp"
#include "factory.hpp"

using namespace gfx;

namespace events
{

REGISTER_EVENT_IN_FACTORY(EarthQuake, "earthquake")

class EarthQuake::Impl
{
public:
  VariantMap onFinished;
  unsigned int lastTimeUpdate;
  TilePos start, end, current;
};

GameEventPtr EarthQuake::create(TilePos start, TilePos stop)
{
  EarthQuake* eq = new EarthQuake();
  eq->_d->start = start;
  eq->_d->end = stop;
  eq->_d->current = start;

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
  return _d->current == _d->end;
}

void EarthQuake::_exec( Game& game, unsigned int time)
{
  if( game::Date::isDayChanged() && time != _d->lastTimeUpdate )
  {
    _d->lastTimeUpdate = time;
    Logger::warning( "Execute earthquake event" );

    Tilemap& tmap = game.city()->tilemap();
    Tile* currentTile = &tmap.at( _d->current );
    if( currentTile )
    {
      bool mayDestruct = currentTile->getFlag( Tile::isConstructible );
      mayDestruct |= currentTile->overlay().is<Construction>();

      if( mayDestruct )
      {
        events::dispatch<Disaster>( *currentTile, Disaster::rift );
      }
    }

    //calculate next point
    TilesArray nextPoints = tmap.getNeighbors(_d->current, Tilemap::FourNeighbors);

    int lastDst = _d->current.getDistanceFromSQ(_d->end);
    for( TilesArray::iterator it=nextPoints.begin(); it != nextPoints.end(); )
    {
      bool mayDestruct = (*it)->getFlag( Tile::isConstructible );
      mayDestruct |= (*it)->overlay().is<Construction>();
      int curDst = (*it)->pos().getDistanceFromSQ(_d->end);

      if( !mayDestruct || (curDst > lastDst) ) { it = nextPoints.erase( it ); }
      else { ++it; }
    }

    if( nextPoints.empty() )
    {
      TilePos offset = _d->end - _d->current;
      offset.setI( math::signnum( offset.i() ) );
      offset.setJ( math::signnum( offset.j() ) );
      _d->current += offset;
    }
    else
    {
      _d->current = nextPoints.random()->pos();
    }

    if( _d->current == _d->end )
    {
      events::Dispatcher::instance().load( _d->onFinished );
    }
  }
}

bool EarthQuake::_mayExec(Game&, unsigned int) const { return true; }

void EarthQuake::load(const VariantMap& stream)
{
  GameEvent::load( stream );
  VARIANT_LOAD_VMAP_D( _d, onFinished, stream )
  VARIANT_LOAD_ANY_D( _d, start, stream )
  VARIANT_LOAD_ANY_D( _d, end, stream )
  VARIANT_LOAD_ANYDEF_D( _d, current, TilePos( -1, -1), stream )

  if( _d->current == gfx::tilemap::invalidLocation() )
  {
    _d->current = _d->start;
  }
}

VariantMap EarthQuake::save() const
{
  VariantMap ret = GameEvent::save();

  VARIANT_SAVE_ANY_D( ret, _d, onFinished )
  VARIANT_SAVE_ANY_D( ret, _d, start )
  VARIANT_SAVE_ANY_D( ret, _d, end )
  VARIANT_SAVE_ANY_D( ret, _d, current )

  return ret;
}

EarthQuake::EarthQuake() : _d( new Impl )
{
  _d->current = gfx::tilemap::invalidLocation();
  _d->lastTimeUpdate = 0;
}

}//end namespace events
