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
#include "factory.hpp"

using namespace constants;

namespace events
{

namespace {
CAESARIA_LITERALCONST(population)
}

REGISTER_EVENT_IN_FACTORY(RandomDamage, "random_collapse")

class RandomDamage::Impl
{
public:
  int minPopulation, maxPopulation;
  bool isDeleted;
  int strong;
  int priority;
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
  if( population > _d->minPopulation && population < _d->maxPopulation )
  {
    Logger::warning( "Execute random collapse event" );
    _d->isDeleted = true;

    Priorities<int> exclude;
    exclude << objects::waterGroup
            << objects::roadGroup
            << objects::disasterGroup;

    ConstructionList ctrs;
    ctrs << game.city()->overlays();

    if( _d->priority != objects::unknown )
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
  VariantList vl = stream.get( lc_population ).toList();
  _d->minPopulation = vl.get( 0, 0 ).toInt();
  _d->maxPopulation = vl.get( 1, 999999 ).toInt();
  VARIANT_LOAD_ANY_D( _d, strong, stream );
  VARIANT_LOAD_ANY_D( _d, priority, stream );

}

VariantMap RandomDamage::save() const
{
  VariantMap ret;
  VariantList vl_pop;
  vl_pop << _d->minPopulation << _d->maxPopulation;

  ret[ lc_population ] = vl_pop;
  VARIANT_SAVE_ANY_D(ret, _d, strong );
  VARIANT_SAVE_ANY_D(ret, _d, priority );

  return ret;
}

RandomDamage::RandomDamage() : _d( new Impl )
{
  _d->isDeleted = false;
  _d->minPopulation = 0;
  _d->maxPopulation = 9999;
  _d->strong = 25;
  _d->priority = 0;
}

}//end namespace events
