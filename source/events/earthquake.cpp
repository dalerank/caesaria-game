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
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "gfx/tilemap.hpp"
#include "objects/rift.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

class EarthQuake::Impl
{
public:
  VariantMap events;
  unsigned int lastTimeUpdate;
  TilePos startPoint, endPoint, currentPoint;
};

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
  if( GameDate::isWeekChanged() && time != _d->lastTimeUpdate )
  {
    _d->lastTimeUpdate = time;
    Logger::warning( "Execute earthquake event" );

    Tilemap& tmap = game.city()->tilemap();
    Tile* currentTile = &tmap.at( _d->currentPoint );
    if( currentTile )
    {
      bool mayDestruct = currentTile->getFlag( Tile::isConstructible ) || currentTile->getFlag( Tile::isDestructible );
      if( mayDestruct )
      {
        ConstructionPtr c = ptr_cast<Construction>( currentTile->overlay() );
        if( c.isValid() )
        {
          c->collapse();
        }

        Rift* r = new Rift();
        r->build( game.city(), currentTile->pos() );

        RiftList rifts = r->neighbors();
        foreach( it, rifts )
        {
          (*it)->updatePicture();
        }

        //calculate next point
        TilePos offset( 1, 1 );
        TilesArray nextPoints = tmap.getRectangle( _d->currentPoint - offset, _d->currentPoint + offset, false );

        int lastDst = _d->currentPoint.distanceFrom( _d->endPoint);
        for( TilesArray::iterator it=nextPoints.begin(); it != nextPoints.end(); )
        {
          mayDestruct = (*it)->getFlag( Tile::isConstructible ) || (*it)->getFlag( Tile::isDestructible );
          int curDst = (*it)->pos().distanceFrom( _d->endPoint );
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
      }
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
  _d->events = stream.get( "exec" ).toMap();
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

  ret[ "exec" ] = _d->events;
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
