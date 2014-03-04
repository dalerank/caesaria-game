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
#include "core/variant.hpp"
#include "city/city.hpp"
#include "game/resourcegroup.hpp"
#include "game/gamedate.hpp"

class ServiceBuilding::Impl
{
public:
  int serviceDelay;
  Service::Type service;
  int serviceTimer;
  int serviceRange;
  DateTime dateLastSend;
};

ServiceBuilding::ServiceBuilding(const Service::Type service,
                                 const Type type, const Size& size)
                                 : WorkingBuilding( type, size ), _d( new Impl )
{
   _d->service = service;
   setMaxWorkers(5);
   setWorkers(0);
   setServiceDelay( 80 );
   _d->serviceTimer = 0;
   _d->serviceRange = 30;
}

void ServiceBuilding::setServiceDelay( const int delay ){  _d->serviceDelay = delay;}
DateTime ServiceBuilding::getLastSendService() const { return _d->dateLastSend; }
void ServiceBuilding::_setLastSendService(DateTime time) { _d->dateLastSend = time; }

int ServiceBuilding::getTime2NextService() const
{
  float koeff = ( numberWorkers() > 0 ) ? (float)maxWorkers() / (float)numberWorkers() : 1.f;
  return (int)(getServiceDelay() * koeff);
}

Service::Type ServiceBuilding::getService() const{   return _d->service;}

void ServiceBuilding::timeStep(const unsigned long time)
{
   WorkingBuilding::timeStep(time);   

   if (_d->serviceTimer == 0)
   {
      deliverService();
      _d->serviceTimer = getTime2NextService();
   }
   else if (_d->serviceTimer > 0)
   {
      _d->serviceTimer -= 1;
   }

   _animationRef().update( time );
   const Picture& pic = _animationRef().getFrame();
   if( pic.isValid() )
   {
      _fgPicturesRef().back() = _animationRef().getFrame();
   }
}

void ServiceBuilding::destroy()
{
   WorkingBuilding::destroy();
}

void ServiceBuilding::deliverService()
{
  // make a service walker and send him to his wandering
  ServiceWalkerPtr serviceman = ServiceWalker::create( _getCity(), getService() );
  serviceman->setMaxDistance( getWalkerDistance() );
  serviceman->send2City( BuildingPtr( this ) );

  if( !serviceman->isDeleted() )
  {
    addWalker( serviceman.object() );
    _setLastSendService( GameDate::current() );
  }
}

int ServiceBuilding::getServiceRange() const {   return _d->serviceRange;}

void ServiceBuilding::save( VariantMap& stream ) const 
{
  WorkingBuilding::save( stream );
  stream[ "timer" ] = _d->serviceTimer;
  stream[ "delay" ] = _d->serviceDelay;
  stream[ "range" ] = _d->serviceRange;
}

void ServiceBuilding::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );
  _d->serviceTimer = (int)stream.get( "timer", 0 );
  _d->serviceDelay = (int)stream.get( "delay", 80 );
  _d->serviceRange = (int)stream.get( "range", 30 );
}

int ServiceBuilding::getServiceDelay() const{  return _d->serviceDelay;}
ServiceBuilding::~ServiceBuilding() {}
unsigned int ServiceBuilding::getWalkerDistance() const{  return 5; }

std::string ServiceBuilding::getWorkersState() const
{
  std::string srvcType = MetaDataHolder::getTypename( type() );
  std::string state = "unknown";

  if( getWalkers().size() > 0 )
  {
    state = "on_patrol";
  }
  else if( numberWorkers() > 0 && _d->serviceTimer < _d->serviceDelay / 4 )
  {
    state = "ready_for_work";
  }
  std::string currentState = StringHelper::format( 0xff, "##%s_%s##", srvcType.c_str(), state.c_str() );
  return currentState;
}
