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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "service.hpp"

#include <cstdlib>
#include <ctime>

#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "core/gettext.hpp"
#include "core/variant_map.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"
#include "core/utils.hpp"
#include "config.hpp"

using namespace gfx;
using namespace constants;
using namespace config;

class ServiceBuilding::Impl
{
public:
  int serviceDelay;
  Service::Type service;
  int serviceRange;
  DateTime lastSend;
};

ServiceBuilding::ServiceBuilding(const Service::Type service,
                                 const object::Type type, const Size& size)
                                 : WorkingBuilding( type, size ), _d( new Impl )
{
   _d->service = service;
   setMaximumWorkers( servicebld::defaultWorkers );
   setWorkers(0);
   setServiceDelay( DateTime::daysInWeek );
   _d->serviceRange = servicebld::defaultRange;
}

void ServiceBuilding::setServiceDelay( const int delay ){  _d->serviceDelay = delay;}
DateTime ServiceBuilding::lastSendService() const { return _d->lastSend; }
void ServiceBuilding::_setLastSendService(DateTime time) { _d->lastSend = time; }

int ServiceBuilding::time2NextService() const
{
  float workersRate = (numberWorkers() > 0 ? productivity() : 100) / 100.f;
  float laborAccessKoeff = laborAccessPercent() / 100.f;
  float timeKoeff = math::clamp( 1 / (workersRate * laborAccessKoeff), 1.f, 4.f );

  return (int)(serviceDelay() * timeKoeff);
}

Service::Type ServiceBuilding::serviceType() const{   return _d->service;}

void ServiceBuilding::timeStep(const unsigned long time)
{
  WorkingBuilding::timeStep(time);

  if( game::Date::isDayChanged() )
  {
    int serviceDelay = time2NextService();
    if( _d->lastSend.daysTo( game::Date::current() ) > serviceDelay )
    {
       deliverService();
       _d->lastSend = game::Date::current();
    }
  }
 }

void ServiceBuilding::destroy()
{
  WorkingBuilding::destroy();
}

void ServiceBuilding::deliverService()
{
  if( !isActive() )
    return;

  // make a service walker and send him to his wandering
  ServiceWalkerPtr serviceman = ServiceWalker::create( _city(), serviceType() );
  serviceman->send2City( this, _getWalkerOrders() );

  if( !serviceman->isDeleted() )
  {
    addWalker( serviceman.object() );
    _setLastSendService( game::Date::current() );
  }
}

int ServiceBuilding::serviceRange() const { return _d->serviceRange;}

void ServiceBuilding::save( VariantMap& stream ) const 
{
  WorkingBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, lastSend )
  VARIANT_SAVE_ANY_D( stream, _d, serviceDelay )
  VARIANT_SAVE_ANY_D( stream, _d, serviceRange )
}

void ServiceBuilding::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );
  VARIANT_LOAD_TIME_D( _d, lastSend, stream )
  VARIANT_LOAD_ANYDEF_D( _d, serviceDelay, 80, stream )
  VARIANT_LOAD_ANYDEF_D( _d, serviceRange, servicebld::defaultRange, stream )
}

void ServiceBuilding::buildingsServed(const std::set<BuildingPtr>&, ServiceWalkerPtr) {}
int ServiceBuilding::serviceDelay() const{  return _d->serviceDelay;}
ServiceBuilding::~ServiceBuilding() {}
unsigned int ServiceBuilding::walkerDistance() const{  return _d->serviceRange; }

std::string ServiceBuilding::workersStateDesc() const
{
  std::string srvcType = object::toString( type() );
  std::string state = "unknown";

  if( walkers().size() > 0 )
  {
    state = "on_patrol";
  }
  else if( numberWorkers() > 0  )
  {
    state = _d->lastSend.daysTo( game::Date::current() ) < 2
              ? "ready_for_work"
              : "prepare_for_work";
  }
  std::string currentState = utils::format( 0xff, "##%s_%s##", srvcType.c_str(), state.c_str() );
  return currentState;
}

int ServiceBuilding::_getWalkerOrders() const
{
  return ServiceWalker::goLowerService|ServiceWalker::anywayWhenFailed;
}
