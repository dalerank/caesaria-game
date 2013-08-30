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

#include "oc3_building_service.hpp"

#include <cstdlib>
#include <ctime>

#include "oc3_tile.hpp"
#include "oc3_scenario.hpp"
#include "oc3_walker_service.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_gettext.hpp"
#include "oc3_variant.hpp"
#include "oc3_city.hpp"
#include "oc3_resourcegroup.hpp"

class ServiceBuilding::Impl
{
public:
  int serviceDelay;
  ServiceType service;
  int serviceTimer;
  int serviceRange;
};

ServiceBuilding::ServiceBuilding(const ServiceType service,
                                 const BuildingType type, const Size& size)
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

ServiceType ServiceBuilding::getService() const
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
      int level = _fgPictures.size()-1;
      _fgPictures[level] = _getAnimation().getCurrentPicture();
   }
}

void ServiceBuilding::destroy()
{
   WorkingBuilding::destroy();
}

void ServiceBuilding::deliverService()
{
  // make a service walker and send him to his wandering
  ServiceWalkerPtr serviceman = ServiceWalker::create( Scenario::instance().getCity(), getService() );
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
  _d->serviceTimer = (int)stream.get( "timer" );
  _d->serviceDelay = (int)stream.get( "delay" );
  _d->serviceRange = (int)stream.get( "range" );
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

School::School() : ServiceBuilding(S_SCHOOL, B_SCHOOL, Size(2))
{
  setPicture( Picture::load( ResourceGroup::commerce, 83));
}

int School::getVisitorsNumber() const
{
  return 75;
}

Library::Library() : ServiceBuilding(S_LIBRARY, B_LIBRARY, Size(2))
{
  setPicture( Picture::load( ResourceGroup::commerce, 84));
}

int Library::getVisitorsNumber() const
{
  return 800;
}

College::College() : ServiceBuilding(S_COLLEGE, B_COLLEGE, Size(3))
{
  setPicture( Picture::load( ResourceGroup::commerce, 85));
}

int College::getVisitorsNumber() const
{
  return 100;
}
