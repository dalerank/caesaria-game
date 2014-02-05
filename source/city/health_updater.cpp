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

#include "health_updater.hpp"
#include "game/game.hpp"
#include "helper.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "core/logger.hpp"

using namespace constants;

class HealthUpdater::Impl
{
public:
  PlayerCityPtr city;
  DateTime endTime;
  VariantMap events;
  bool isDeleted;
};

CityServicePtr HealthUpdater::create( PlayerCityPtr city )
{
  HealthUpdater* e = new HealthUpdater();
  e->_d->city = city;

  CityServicePtr ret( e );
  ret->drop();

  return ret;
}

void HealthUpdater::update( const unsigned int time)
{
  if( time % GameDate::getTickInMonth() == 0 )
  {
    _d->isDeleted = (_d->endTime < GameDate::current());

    Logger::warning( "Execute health updater service" );
    CityHelper helper( _d->city );
    HouseList houses = helper.find<House>( building::house );
    foreach( it, houses )
    {
      (*it)->up;
    }

    events::Dispatcher::instance().load( _d->events );
  }
}

bool HealthUpdater::isDeleted() const {  return _d->isDeleted; }

void HealthUpdater::load(const VariantMap& stream)
{
  VariantList vl = stream.get( "population" ).toList();
  _d->minPopulation = vl.get( 0, 0 ).toInt();
  _d->maxPopulation = vl.get( 1, 999999 ).toInt();
  _d->events = stream.get( "exec" ).toMap();
}

VariantMap HealthUpdater::save() const
{
  return VariantMap();
}

HealthUpdater::HealthUpdater() : CityService("healthUpdater"), _d( new Impl )
{
  _d->isDeleted = false;
}
