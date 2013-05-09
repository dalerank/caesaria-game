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

#include "oc3_building.hpp"
#include "oc3_predefinitions.hpp"

class Widget;

class ServiceBuilding : public WorkingBuilding
{
public:
   ServiceBuilding( const ServiceType service,
                    const BuildingType type, const Size& size );

   virtual ~ServiceBuilding();

   ServiceType getService() const;
   void timeStep(const unsigned long time);
   virtual void destroy();  // handles the walkers

   int getServiceRange() const;  // max distance from building to road for road to propose the service
   virtual int getServiceDelay() const;
   void setServiceDelay( const int delay );

   // called when a service man should service the neighborhood
   virtual void deliverService();

   virtual GuiInfoBox* makeInfoBox( Widget* parent );

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

   //void removeWalker( WalkerPtr walker );

protected:
  void _addWalker( WalkerPtr walker );
  const Walkers& _getWalkerList() const;

private:
   ServiceType _service;
   int _serviceTimer;
   
   int _serviceRange;
   Walkers _walkerList;
   static std::map<ServiceType, ServiceBuilding*> _specimen;

   class Impl;
   ScopedPtr< Impl > _d;
};

class BuildingWell : public ServiceBuilding
{
public:
  BuildingWell();

  char getDesirabilityInfluence() const; 
  void deliverService();
};

class BuildingFountain : public ServiceBuilding
{
public:
  BuildingFountain();

  virtual void deliverService();
};

class EntertainmentBuilding : public ServiceBuilding
{
public:
  EntertainmentBuilding( const ServiceType service, const BuildingType type,
                         const Size& size);
  virtual void deliverService();
};

class BuildingTheater : public EntertainmentBuilding
{
public:
   BuildingTheater();
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

class TempleCeres : public ServiceBuilding
{
public:
   TempleCeres();
};

class TempleNeptune : public ServiceBuilding
{
public:
   TempleNeptune();
};

class TempleMars : public ServiceBuilding
{
public:
   TempleMars();
};

class TempleVenus : public ServiceBuilding
{
public:
   TempleVenus();
};

class TempleMercure : public ServiceBuilding
{
public:
   TempleMercure();
};

class BigTempleCeres : public ServiceBuilding
{
public:
   BigTempleCeres();
};

class BigTempleNeptune : public ServiceBuilding
{
public:
   BigTempleNeptune();
};

class BigTempleMars : public ServiceBuilding
{
public:
   BigTempleMars();
};

class BigTempleVenus : public ServiceBuilding
{
public:
   BigTempleVenus();
};

class BigTempleMercure : public ServiceBuilding
{
public:
   BigTempleMercure();
};

class TempleOracle : public ServiceBuilding
{
public:
   TempleOracle();
};

class School : public ServiceBuilding
{
public:
   School();
};

class Library : public ServiceBuilding
{
public:
   Library();
};

class College : public ServiceBuilding
{
public:
   College();
};

class Baths : public ServiceBuilding
{
public:
   Baths();
};

class Barber : public ServiceBuilding
{
public:
   Barber();
};

class Doctor : public ServiceBuilding
{
public:
   Doctor();
};

class Hospital : public ServiceBuilding
{
public:
   Hospital();
};

class Forum : public ServiceBuilding
{
public:
   Forum();
};

#endif
