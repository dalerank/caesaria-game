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

#include "random_damage.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "objects/road.hpp"
#include "objects/house.hpp"
#include "core/variant_map.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"
#include "core/priorities.hpp"
#include "city/statistic.hpp"
#include "factory.hpp"
#include "city/states.hpp"

using namespace city;

namespace events
{

REGISTER_EVENT_IN_FACTORY(RandomDamage, "random_collapse")

class RandomDamage::Impl
{
public:
  Ranged::Range popRange;
  bool isDeleted;
  int strong;
  object::Group priority;
};

GameEventPtr RandomDamage::create()
{
  GameEventPtr ret( new RandomDamage() );
  ret->drop();

  return ret;
}

void RandomDamage::_exec( Game& game, unsigned int time )
{
  int population = game.city()->states().population;
  if( _d->popRange.contain( population ) )
  {
    Logger::warning( "Execute random collapse event" );
    _d->isDeleted = true;

    ConstructionList ctrs;

    if( _d->priority != object::group::unknown )
    {
      ctrs = game.city()->statistic().objects.find<Construction>( _d->priority );
    }
    else
    {
      std::set<object::Group> exclude;
      exclude << object::group::water
              << object::group::road
              << object::group::disaster;

      ctrs = statistic::getObjectsNotIs<Construction>( game.city(), exclude );
    }

    unsigned int number4burn = math::clamp<unsigned int>( (ctrs.size() * _d->strong / 100), 1u, 100u );

    for( unsigned int k=0; k < number4burn; k++ )
    {
      ConstructionPtr building = ctrs.random();
      if( building.isValid() && !building->isDeleted() )
        building->collapse();
    }
  }
}

bool RandomDamage::_mayExec(Game&, unsigned int) const { return true; }
bool RandomDamage::isDeleted() const {  return _d->isDeleted; }

void RandomDamage::load(const VariantMap& stream)
{
  Ranged::load( stream );
  VARIANT_LOAD_ANY_D( _d, strong, stream );
  VARIANT_LOAD_ENUM_D( _d, priority, stream );
}

VariantMap RandomDamage::save() const
{
  VariantMap ret = Ranged::save();
  VARIANT_SAVE_ANY_D(ret, _d, strong );
  VARIANT_SAVE_ENUM_D(ret, _d, priority );

  return ret;
}

RandomDamage::RandomDamage() : _d( new Impl )
{
  _d->isDeleted = false;
  _d->popRange.min = 0;
  _d->popRange.max = 9999;
  _d->strong = 25;
  _d->priority = object::group::unknown;
}

}//end namespace events
