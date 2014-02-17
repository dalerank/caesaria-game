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

#include "random_fire.hpp"
#include "game/game.hpp"
#include "helper.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"
#include "core/logger.hpp"

using namespace constants;

class RandomFire::Impl
{
public:
  PlayerCityPtr city;
  int minPopulation, maxPopulation;
  VariantMap events;
  bool isDeleted;
};

CityServicePtr RandomFire::create( PlayerCityPtr city )
{
  RandomFire* e = new RandomFire();
  e->_d->city = city;

  CityServicePtr ret( e );
  ret->drop();

  return ret;
}

void RandomFire::update( const unsigned int time)
{
  if( time % GameDate::ticksInMonth() == 0 && !_d->isDeleted )
  {
    int population = _d->city->getPopulation();
    if( population > _d->minPopulation && population < _d->maxPopulation )
    {
      Logger::warning( "Execute random fire service" );
      _d->isDeleted = true;
      CityHelper helper( _d->city );
      HouseList houses = helper.find<House>( building::house );
      for( unsigned int k=0; k < houses.size() / 4; k++ )
      {
        HouseList::iterator it = houses.begin();
        std::advance( it, math::random( houses.size() ) );
        (*it)->burn();
      }

      events::Dispatcher::instance().load( _d->events );
    }
  }
}

std::string RandomFire::getDefaultName() { return "random_fire"; }
bool RandomFire::isDeleted() const {  return _d->isDeleted; }

void RandomFire::load(const VariantMap& stream)
{
  VariantList vl = stream.get( "population" ).toList();
  _d->minPopulation = vl.get( 0, 0 ).toInt();
  _d->maxPopulation = vl.get( 1, 999999 ).toInt();
  _d->events = stream.get( "exec" ).toMap();
}

VariantMap RandomFire::save() const
{
  VariantMap ret;
  VariantList vl_pop;
  vl_pop << _d->minPopulation << _d->maxPopulation;

  ret[ "population" ] = vl_pop;
  ret[ "exec" ] = _d->events;

  return ret;
}

RandomFire::RandomFire() : CityService( RandomFire::getDefaultName() ), _d( new Impl )
{
  _d->isDeleted = false;
}
