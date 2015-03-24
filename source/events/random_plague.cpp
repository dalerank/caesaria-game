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

#include "random_plague.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"
#include "core/priorities.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "city/statistic.hpp"
#include "factory.hpp"

using namespace constants;
using namespace city;

namespace events
{

REGISTER_EVENT_IN_FACTORY(RandomPlague, "random_plague")

class RandomPlague::Impl
{
public:
  Ranged::Range popRange;
  bool isDeleted;
  int strong;
};

GameEventPtr RandomPlague::create()
{
  GameEventPtr ret( new RandomPlague() );
  ret->drop();

  return ret;
}

void RandomPlague::_exec( Game& game, unsigned int time)
{
  int population = game.city()->population();
  if( _d->popRange.contain( population ) )
  {
    Logger::warning( "Execute random plague event" );
    _d->isDeleted = true;

    HouseList houses = statistic::findh( game.city() );

    unsigned int number4burn = math::clamp<unsigned int>( (houses.size() * _d->strong / 100), 1u, 100u );

    for( unsigned int k=0; k < number4burn; k++ )
    {
      HousePtr house = houses.random();
      house->setState( pr::health, 0 );
    }
  }
}

bool RandomPlague::_mayExec(Game&, unsigned int) const { return true; }
bool RandomPlague::isDeleted() const {  return _d->isDeleted; }

void RandomPlague::load(const VariantMap& stream)
{
  _d->popRange.load( stream.get( literals::population ).toList() );
  VARIANT_LOAD_ANYDEF_D( _d, strong, 10, stream )
}

VariantMap RandomPlague::save() const
{
  VariantMap ret;
  ret[ literals::population ] = _d->popRange.save();
  VARIANT_SAVE_ANY_D( ret, _d, strong )
  return ret;
}

RandomPlague::RandomPlague() : _d( new Impl )
{
  _d->isDeleted = false;
  _d->popRange.min = 0;
  _d->popRange.max = 999999;
  _d->strong = 10;
}

}//end namespace events
