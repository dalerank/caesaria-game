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


#ifndef SERVICE_BUILDING_HPP
#define SERVICE_BUILDING_HPP

#include "oc3_working_building.hpp"
#include "oc3_predefinitions.hpp"

class Widget;

class ServiceBuilding : public WorkingBuilding
{
public:
  ServiceBuilding( const ServiceType service,
                   const BuildingType type, const Size& size );

  virtual ~ServiceBuilding();

  ServiceType getService() const;
  virtual void timeStep(const unsigned long time);
  virtual void destroy();  // handles the walkers

  int getServiceRange() const;  // max distance from building to road for road to propose the service
  virtual int getServiceDelay() const;
  void setServiceDelay( const int delay );

  // called when a service man should service the neighborhood
  virtual void deliverService();
  
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual unsigned int getWalkerDistance() const;

  //void removeWalker( WalkerPtr walker );
private:  
  static std::map<ServiceType, ServiceBuilding*> _specimen;

  class Impl;
  ScopedPtr< Impl > _d;
};

class EntertainmentBuilding : public ServiceBuilding
{
public:
  EntertainmentBuilding( const ServiceType service, const BuildingType type,
                         const Size& size);
  virtual void deliverService();
  virtual int getVisitorsNumber() const;
};

class Theater : public EntertainmentBuilding
{
public:
   Theater();

   virtual int getVisitorsNumber() const; 
};

class BuildingAmphiTheater : public EntertainmentBuilding
{
public:
   BuildingAmphiTheater();
};

class BuildingCollosseum : public EntertainmentBuilding
{
public:
   BuildingCollosseum();
};


class BuildingHippodrome : public EntertainmentBuilding
{
public:
   BuildingHippodrome();
};

class School : public ServiceBuilding
{
public:
   School();

   int getVisitorsNumber() const;
};

class Library : public ServiceBuilding
{
public:
   Library();

   int getVisitorsNumber() const;
};

class College : public ServiceBuilding
{
public:
   College();

   int getVisitorsNumber() const;
};

#endif
