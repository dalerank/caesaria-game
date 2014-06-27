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

#include "cityservice_animals.hpp"
#include "city.hpp"
#include "gfx/tile.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "walker/animals.hpp"
#include "walker/constants.hpp"

using namespace constants;
using namespace gfx;

namespace city
{

namespace {
static const unsigned int defaultMaxSheeps = 10;
}

class Animals::Impl
{
public:
  unsigned int maxSheeps;
  unsigned int maxWolves;
};

SrvcPtr Animals::create(PlayerCityPtr city)
{
  Animals* ret = new Animals( city );

  return ret;
}

std::string Animals::defaultName() { return "animals"; }

void Animals::update(const unsigned int time)
{
  if( !GameDate::isMonthChanged() )
    return;

  Tilemap& tmap = _city.tilemap();
  TilesArray border = tmap.getRectangle( TilePos( 0, 0 ), Size( tmap.size() ) );
  border = border.walkableTiles( true );

  if( _d->maxSheeps > 0 )
  {
    WalkerList sheeps = _city.walkers( walker::sheep );
    if( sheeps.size() < _d->maxSheeps )
    {
      WalkerPtr sheep = Sheep::create( &_city );
      if( sheep.isValid() )
      {
        TilesArray::iterator it = border.begin();
        std::advance( it, math::random( border.size() ) );
        ptr_cast<Sheep>(sheep)->send2City( (*it)->pos() );
      }
    }
  }

  if( _d->maxWolves > 0 )
  {
    WalkerList wolves = _city.walkers( walker::wolf );
    if( wolves.size() < _d->maxWolves )
    {
      WalkerPtr wolf = Wolf::create( &_city );
      if( wolf.isValid() )
      {
        TilesArray::iterator it = border.begin();
        std::advance( it, math::random( border.size() ) );
        ptr_cast<Wolf>(wolf)->send2City( (*it)->pos() );
      }
    }
  }
}

void Animals::setWolvesNumber(unsigned int number) {  _d->maxWolves = number; }

Animals::Animals( PlayerCityPtr city )
  : Srvc( *city.object(), Animals::defaultName() ), _d( new Impl )
{
  _d->maxSheeps = defaultMaxSheeps;
  _d->maxWolves = 0;
}

}//end namespace city
