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
#include "core/logger.hpp"
#include "core/common.hpp"
#include "config.hpp"

REGISTER_CLASS_IN_OVERLAYFACTORY(object::school, School)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::library, Library)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::academy, Academy)

class EducationBuilding::Impl
{
public:
  typedef std::map<unsigned int, unsigned int> ServedBuildings;
  ServedBuildings srvBuidings;
  unsigned int currentPeopleServed;
  unsigned int maxMonthVisitors;
};

School::School() : EducationBuilding(Service::school, object::school, Size(2))
{
  _picture().load( ResourceGroup::commerce, 83 );
  _d->maxMonthVisitors = config::educationbld::maxSchoolVisitors;
}

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

unsigned int School::walkerDistance() const { return 26; }

void School::buildingsServed(const std::set<BuildingPtr>& buildings, ServiceWalkerPtr walker)
{
  if( walker->pathway().isReverse() )
    return;

  std::set<HousePtr> houses = utils::select<House>( buildings );
  for( auto house : houses )
  {
    unsigned int posHash = gfx::tile::hash(house->pos());
    _d->srvBuidings[ posHash ] = house->habitants().scholar_n();
  }

  _d->currentPeopleServed = 0;
  for( auto bld : _d->srvBuidings )
  {
    _d->currentPeopleServed += bld.second;
  }

  if( _d->currentPeopleServed > _d->maxMonthVisitors )
    walker->return2Base();
}

int School::_getWalkerOrders() const
{
  return ServiceWalker::goServiceMaximum|ServiceWalker::anywayWhenFailed|ServiceWalker::enterLastHouse;
}

Library::Library() : EducationBuilding(Service::library, object::library, Size(2))
{
  _picture().load( ResourceGroup::commerce, 84 );
  _d->maxMonthVisitors = config::educationbld::maxLibraryVisitors;
  _d->currentPeopleServed = _d->maxMonthVisitors;
}

Academy::Academy() : EducationBuilding(Service::academy, object::academy, Size(3))
{
  _picture().load( ResourceGroup::commerce, 85 );
  _d->maxMonthVisitors = config::educationbld::maxAcademyVisitors;
  _d->currentPeopleServed = _d->maxMonthVisitors;
}

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

void EducationBuilding::initialize(const MetaData& mdata)
{
  ServiceBuilding::initialize( mdata );
  int maxServe = mdata.getOption( "maxServe" );
  if( maxServe > 0 )
    _d->maxMonthVisitors = maxServe;
}

EducationBuilding::EducationBuilding(const Service::Type service, const object::Type type, const Size& size)
  : ServiceBuilding( service, type, size ), _d( new Impl )
{

}

int EducationBuilding::currentVisitors() const { return _d->currentPeopleServed; }
int EducationBuilding::maxVisitors() const { return _d->maxMonthVisitors; }
