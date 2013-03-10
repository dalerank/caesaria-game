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

#include <building.hpp>


class Walker;

class ServiceBuilding : public WorkingBuilding
{
public:
   ServiceBuilding(const ServiceType &service);

   ServiceType getService() const;
   void timeStep(const unsigned long time);
   virtual void destroy();  // handles the walkers

   int getServiceRange() const;  // max distance from building to road for road to propose the service

   // called when a service man should service the neighborhood
   virtual void deliverService();

   static std::map<ServiceType, ServiceBuilding*>& getSpecimen();

   virtual GuiInfoBox* makeInfoBox();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   ServiceType _service;
   int _serviceTimer;
   int _serviceDelay;
   int _serviceRange;
   std::list<Walker*> _walkerList;
   static std::map<ServiceType, ServiceBuilding*> _specimen;
};


class BuildingWell : public ServiceBuilding
{
public:
   BuildingWell();
   BuildingWell* clone() const;

   virtual void deliverService();
};


class BuildingFountain : public ServiceBuilding
{
public:
   BuildingFountain();
   BuildingFountain* clone() const;

   virtual void deliverService();
};


class BuildingEngineer : public ServiceBuilding
{
public:
   BuildingEngineer();
   BuildingEngineer* clone() const;
};


class BuildingPrefect : public ServiceBuilding
{
public:
   BuildingPrefect();
   BuildingPrefect* clone() const;
};

class EntertainmentBuilding : public ServiceBuilding
{
public:
   EntertainmentBuilding(const ServiceType &service);
   virtual void deliverService();
};

class BuildingTheater : public EntertainmentBuilding
{
public:
   BuildingTheater();
   BuildingTheater* clone() const;
};

class BuildingAmphiTheater : public EntertainmentBuilding
{
public:
   BuildingAmphiTheater();
   BuildingAmphiTheater* clone() const;
};

class BuildingCollosseum : public EntertainmentBuilding
{
public:
   BuildingCollosseum();
   BuildingCollosseum* clone() const;
};

class TempleCeres : public ServiceBuilding
{
public:
   TempleCeres();
   TempleCeres* clone() const;
};

class TempleNeptune : public ServiceBuilding
{
public:
   TempleNeptune();
   TempleNeptune* clone() const;
};

class TempleMars : public ServiceBuilding
{
public:
   TempleMars();
   TempleMars* clone() const;
};

class TempleVenus : public ServiceBuilding
{
public:
   TempleVenus();
   TempleVenus* clone() const;
};

class TempleMercure : public ServiceBuilding
{
public:
   TempleMercure();
   TempleMercure* clone() const;
};

class School : public ServiceBuilding
{
public:
   School();
   School* clone() const;
};

class Library : public ServiceBuilding
{
public:
   Library();
   Library* clone() const;
};

class College : public ServiceBuilding
{
public:
   College();
   College* clone() const;
};

class Baths : public ServiceBuilding
{
public:
   Baths();
   Baths* clone() const;
};

class Barber : public ServiceBuilding
{
public:
   Barber();
   Barber* clone() const;
};

class Doctor : public ServiceBuilding
{
public:
   Doctor();
   Doctor* clone() const;
};

class Hospital : public ServiceBuilding
{
public:
   Hospital();
   Hospital* clone() const;
};

class Forum : public ServiceBuilding
{
public:
   Forum();
   Forum* clone() const;
};

class Senate : public ServiceBuilding
{
public:
   Senate();
   Senate* clone() const;
};


class MarketBuyer;
class Market: public ServiceBuilding
{
public:
   Market();
   Market* clone() const;

   void timeStep(const unsigned long time);
   virtual GuiInfoBox* makeInfoBox();

   SimpleGoodStore& getGoodStore();
   std::list<GoodType> getMostNeededGoods();  // used by the market buyer

   // returns the quantity of needed goods
   int getGoodDemand(const GoodType &goodType);  // used by the market buyer

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   SimpleGoodStore _goodStore;
   MarketBuyer *_marketBuyer;
   int _buyerDelay;
};


#endif
