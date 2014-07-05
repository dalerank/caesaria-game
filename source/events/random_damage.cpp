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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "random_damage.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"

using namespace constants;

namespace events
{

namespace {
CAESARIA_LITERALCONST(population)
CAESARIA_LITERALCONST(strong)
}

class RandomDamage::Impl
{
public:
  int minPopulation, maxPopulation;
  bool isDeleted;
  int strong;
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
    _d->isDeleted = true;
    ConstructionList ctrs;
    ctrs << game.city()->overlays();

    for( unsigned int k=0; k < (ctrs.size() * _d->strong / 100); k++ )
    {
      ConstructionList::iterator it = ctrs.begin();
      std::advance( it, math::random( ctrs.size()-1 ) );
      (*it)->collapse();
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
  _d->strong = stream.get( lc_strong, 10 );
}

VariantMap RandomDamage::save() const
{
  VariantMap ret;
  VariantList vl_pop;
  vl_pop << _d->minPopulation << _d->maxPopulation;

  ret[ lc_population ] = vl_pop;
  ret[ lc_strong ] = _d->strong;

  return ret;
}

RandomDamage::RandomDamage() : _d( new Impl )
{
  _d->isDeleted = false;
}

}//end namespace events
