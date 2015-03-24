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

using namespace constants;
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
  int population = game.city()->population();
  if( _d->popRange.contain( population ) )
  {
    Logger::warning( "Execute random collapse event" );
    _d->isDeleted = true;

    Priorities<object::Group> exclude;
    exclude << object::group::water
            << object::group::road
            << object::group::disaster;

    ConstructionList ctrs = statistic::findo<Construction>( game.city(), object::any );

    if( _d->priority != object::group::unknown )
    {
      for( ConstructionList::iterator it=ctrs.begin(); it != ctrs.end(); )
      {
        if( (*it)->group() != _d->priority ) { it = ctrs.erase( it ); }
        else { ++it; }
      }
    }
    else
    {
      for( ConstructionList::iterator it=ctrs.begin(); it != ctrs.end(); )
      {
        if( exclude.count( (*it)->group() ) ) { it = ctrs.erase( it ); }
        else { ++it; }
      }
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
  _d->popRange.load( stream.get( literals::population ).toList() );
  VARIANT_LOAD_ANY_D( _d, strong, stream );
  VARIANT_LOAD_ENUM_D( _d, priority, stream );
}

VariantMap RandomDamage::save() const
{
  VariantMap ret;
  ret[ literals::population ] = _d->popRange.save();
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
