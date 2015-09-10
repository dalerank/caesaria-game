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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "house_habitants.hpp"
#include "events/fireworkers.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "house_spec.hpp"
#include "house.hpp"

using namespace events;

int Habitants::homeless() const
{
  return math::clamp<int>( count() - capacity, 0, 0xff );
}

int Habitants::freeRoom() const { return math::max<int>( capacity - count(), 0 ); }

Habitants::Habitants()
{
  capacity = 0;
  workers.max = 0;
  workers.current = 0;
}

void Habitants::update( House& h, const CitizenGroup& group )
{
  int deltaWorkersNumber = (int)group.mature_n() - (int)mature_n();

  set( group );
  workers.max = mature_n();

  if( deltaWorkersNumber < 0 )
  {
    Logger::warning( "Habitants::update fired %d workers", deltaWorkersNumber );
    GameEventPtr e = FireWorkers::create( h.pos(), abs( deltaWorkersNumber ) );
    e->dispatch();
  }
  else
  {
    if( workers.current + deltaWorkersNumber > workers.max )
      Logger::warning( "WARNING!!! Habitants::update currentWorkers(%d) > maxWorkers(%d)", workers.current + deltaWorkersNumber, workers.max );

    workers.current = math::clamp<int>( workers.current + deltaWorkersNumber, 0, workers.max );
  }
}

void Habitants::updateCapacity(House& h)
{
  capacity = h.spec().tileCapacity() * h.size().area();
}

VariantMap Habitants::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY( ret, capacity )
  VARIANT_SAVE_ANY( ret, workers.current )
  VARIANT_SAVE_ANY( ret, workers.max )
  ret[ "groups" ] = CitizenGroup::save();

  return ret;
}

void Habitants::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANY( capacity, stream )
  VARIANT_LOAD_ANY( workers.current, stream )
  VARIANT_LOAD_ANY( workers.max, stream )

  CitizenGroup::load( stream.get( "groups").toList() );
}

void Habitants::makeGeneration(House& house)
{
  CitizenGroup nextGeneration = *this;
  nextGeneration.makeOld();

  unsigned int healthValue = house.state( pr::health );

  nextGeneration[ CitizenGroup::longliver ] = 0; //death-health function from oldest habitants count
  unsigned int peoples2remove = math::random( nextGeneration.aged_n() * ( 100 - healthValue ) / 100 );
  nextGeneration.retrieve( CitizenGroup::aged, peoples2remove+1 );

  unsigned int students_n = nextGeneration.count( 10, 19 );
  unsigned int youngs_n = nextGeneration.count( 20, 29);
  unsigned int matures_n = nextGeneration.count( 30, 39 );
  unsigned int olders_n = nextGeneration.count( 40, 49 );
  unsigned int newborns_n = students_n * math::random( 3 ) / 100 +    //at 3% of student add newborn
                            youngs_n   * math::random( 16) / 100 +    //at 16% of young people add newborn
                            matures_n  * math::random( 9 ) / 100 +    //at 9% of matures add newborn
                            olders_n   * math::random( 2 ) / 100;   //at 2% of aged peoples add newborn

  newborns_n = newborns_n * healthValue / 100 ;  //house health add compensation for newborn citizens

  unsigned int vacantRoom = house.capacity() - nextGeneration.count();
  newborns_n = math::clamp( newborns_n, 0u, vacantRoom );

  nextGeneration[ CitizenGroup::newborn ] = newborns_n; //birth+health function from mature habitants count
  update( house, nextGeneration );
}


void RecruterService::set(float i) { _habitants.workers.current = math::clamp<float>( i, 0, _habitants.workers.max ); }
float RecruterService::value() const { return _habitants.workers.current; }
int RecruterService::max() const { return _habitants.workers.max; }

void RecruterService::setMax(int)
{
  Logger::warning( "!!!Warning: RecruterService::setMax blocked. Max value drive by Habitants class." );
}
