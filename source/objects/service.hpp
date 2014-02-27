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

#ifndef _CAESARIA_SERVICE_BUILDING_H_INCLUDE_
#define _CAESARIA_SERVICE_BUILDING_H_INCLUDE_

#include "objects/working.hpp"
#include "predefinitions.hpp"

class Widget;

class ServiceBuilding : public WorkingBuilding
{
public:
  ServiceBuilding( const Service::Type service,
                   const TileOverlay::Type type, const Size& size );

  virtual ~ServiceBuilding();

  Service::Type getService() const;
  virtual void timeStep(const unsigned long time);
  virtual void destroy();  // handles the walkers

  int getServiceRange() const;  // max distance from building to road for road to propose the service
  virtual int getServiceDelay() const;
  void setServiceDelay( const int delay );

  virtual int getTime2NextService() const;
  virtual DateTime getLastSendService() const;

  // called when a service man should service the neighborhood
  virtual void deliverService();
  
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual unsigned int getWalkerDistance() const;

  std::string getWorkersState() const;
private:  
  void _setLastSendService( DateTime time );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_SERVICE_BUILDING_H_INCLUDE_
