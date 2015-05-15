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

#include "education.hpp"
#include "game/resourcegroup.hpp"
#include "walker/serviceman.hpp"
#include "pathway/pathway.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "gfx/helper.hpp"
#include "game/gamedate.hpp"
#include "objects_factory.hpp"

REGISTER_CLASS_IN_OVERLAYFACTORY(object::school, School)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::library, Library)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::academy, Academy)

class School::Impl
{
public:
  typedef std::map<unsigned int, unsigned int> ServedBuildings;
  ServedBuildings srvBuidings;
  unsigned int currentPeopleServed;
  unsigned int maxMonthVisitors;
};

School::School() : ServiceBuilding(Service::school, object::school, Size(2)), _d( new Impl )
{
  _picture().load( ResourceGroup::commerce, 83 );
  _d->maxMonthVisitors = 75;
}

int School::currentVisitors() const { return _d->currentPeopleServed; }


void School::deliverService()
{
  if( numberWorkers() <= 0 )
    return;

  if( lastSendService().month() != game::Date::current().month() )
  {
    _d->currentPeopleServed = 0;
    _d->srvBuidings.clear();
  }

  if( walkers().size() < 3 && _d->currentPeopleServed < _d->maxMonthVisitors )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int School::walkerDistance() const {  return 26; }

void School::buildingsServed(const std::set<BuildingPtr>& buildings, ServiceWalkerPtr walker)
{
  if( walker->pathway().isReverse() )
    return;

  foreach( it, buildings )
  {
    HousePtr house = ptr_cast<House>( *it );
    if( house.isValid() )
    {
      unsigned int posHash = gfx::tile::hash(house->pos());
      _d->srvBuidings[ posHash ] = house->habitants().scholar_n();
    }
  }

  _d->currentPeopleServed = 0;
  foreach( it, _d->srvBuidings )
  {
    _d->currentPeopleServed += it->second;
  }

  if( _d->currentPeopleServed > _d->maxMonthVisitors )
    walker->return2Base();
}

int School::_getWalkerOrders() const
{
  return ServiceWalker::goLowerService|ServiceWalker::anywayWhenFailed|ServiceWalker::enterLastHouse;
}

Library::Library() : ServiceBuilding(Service::library, object::library, Size(2))
{
  _picture().load( ResourceGroup::commerce, 84 );
}

int Library::currentVisitors() const {  return 800; }

Academy::Academy() : ServiceBuilding(Service::academy, object::academy, Size(3))
{
  _picture().load( ResourceGroup::commerce, 85 );
}

int Academy::currentVisitors() const {  return 100; }

void Academy::deliverService()
{
  if( numberWorkers() > 0 && walkers().size() == 0 )
  {
    ServiceBuilding::deliverService();
  }
}

unsigned int Academy::walkerDistance() const{  return 26; }

std::string Academy::sound() const
{
  return (isActive() && numberWorkers() > 0
          ? ServiceBuilding::sound()
          : "");
}
