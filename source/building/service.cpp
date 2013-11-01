// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
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
#include "game/city.hpp"
#include "game/resourcegroup.hpp"

class ServiceBuilding::Impl
{
public:
  int serviceDelay;
  Service::Type service;
  int serviceTimer;
  int serviceRange;
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

void ServiceBuilding::setServiceDelay( const int delay )
{
  _d->serviceDelay = delay;
}

Service::Type ServiceBuilding::getService() const
{
   return _d->service;
}

void ServiceBuilding::timeStep(const unsigned long time)
{
   WorkingBuilding::timeStep(time);   

   if (_d->serviceTimer == 0)
   {
      deliverService();
      _d->serviceTimer = getServiceDelay();
   }
   else if (_d->serviceTimer > 0)
   {
      _d->serviceTimer -= 1;
   }

   _getAnimation().update( time );
   const Picture& pic = _getAnimation().getCurrentPicture();
   if( pic.isValid() )
   {
      int level = getForegroundPictures().size()-1;
      _getForegroundPictures().at( level ) = _getAnimation().getCurrentPicture();
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
    addWalker( serviceman.as<Walker>() );
  }
}

int ServiceBuilding::getServiceRange() const
{
   return _d->serviceRange;
}

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

int ServiceBuilding::getServiceDelay() const
{
  return _d->serviceDelay;
}

ServiceBuilding::~ServiceBuilding()
{

}

unsigned int ServiceBuilding::getWalkerDistance() const
{
  return 5;
}
