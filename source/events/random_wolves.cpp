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

#include "random_wolves.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "city/cityservice_animals.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "walker/animals.hpp"
#include "gfx/tilemap.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

namespace {
CAESARIA_LITERALCONST(count)
CAESARIA_LITERALCONST(maxWolves)
}

class RandomWolves::Impl
{
public:
  bool isDeleted;
  unsigned int count;
  int maxWolves;
};

GameEventPtr RandomWolves::create()
{
  GameEventPtr ret( new RandomWolves() );
  ret->drop();

  return ret;
}

GameEventPtr RandomWolves::create(unsigned int wolvesNumber)
{
  RandomWolves* r = new RandomWolves();
  r->_d->count = wolvesNumber;

  GameEventPtr ret( r );
  ret->drop();

  return ret;
}

void RandomWolves::_exec( Game& game, unsigned int time)
{
  Logger::warning( "Execute random wolves event" );
  if( _d->count > 0 )
  {
    Tilemap& tmap = game.city()->tilemap();
    TilesArray border = tmap.getRectangle( TilePos( 0, 0 ), Size( tmap.size() ) );
    border = border.walkableTiles( true );

    TilesArray::iterator it = border.begin();
    std::advance( it, math::random( border.size()-1 ) );

    for( unsigned int k=0; k < _d->count; k++ )
    {
      WolfPtr wolf = ptr_cast<Wolf>( Wolf::create( game.city() ) );
      if( wolf.isValid() )
      {
        wolf->send2City( (*it)->pos() );
        wolf->wait( k * 20 );
      }
    }

    _d->count = 0;
  }

  if( _d->maxWolves >= 0 )
  {
    SmartPtr<city::Animals> srvc = ptr_cast<city::Animals>( game.city()->findService( city::Animals::defaultName() ) );
    if( srvc.isValid() )
    {
      srvc->setWolvesNumber( _d->maxWolves );
    }
  }
}

bool RandomWolves::_mayExec(Game&, unsigned int) const { return true; }
bool RandomWolves::isDeleted() const {  return _d->isDeleted; }

void RandomWolves::load(const VariantMap& stream)
{
  _d->count = stream.get( lc_count, 0 ).toUInt();
  _d->maxWolves = stream.get( lc_maxWolves, -1 );
}

VariantMap RandomWolves::save() const
{
  VariantMap ret;

  ret[ lc_count ] = (int)_d->count;
  ret[ lc_maxWolves ] = _d->maxWolves;
  return ret;
}

RandomWolves::RandomWolves() : _d( new Impl )
{
  _d->isDeleted = true;
}

}//end namespace events
