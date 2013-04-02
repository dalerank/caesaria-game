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



#include "service_building.hpp"

#include <iostream>

#include "scenario.hpp"
#include "walker.hpp"
#include "walker_market_buyer.hpp"
#include "exception.hpp"
#include "gui_info_box.hpp"
#include "gettext.hpp"


ServiceBuilding::ServiceBuilding(const ServiceType &service)
{
   _service = service;
   setMaxWorkers(5);
   setWorkers(0);
   _serviceTimer = 0;
   _serviceDelay = 80;
   _serviceRange = 30;
}

ServiceType ServiceBuilding::getService() const
{
   return _service;
}

void ServiceBuilding::timeStep(const unsigned long time)
{
   Building::timeStep(time);

   if (_serviceTimer == 0)
   {
      deliverService();
      _serviceTimer = _serviceDelay;
   }
   else if (_serviceTimer > 0)
   {
      _serviceTimer -= 1;
   }

   _animation.nextFrame();
   Picture *pic = _animation.get_current_picture();
   if (pic != NULL)
   {
      int level = _fgPictures.size()-1;
      _fgPictures[level] = _animation.get_current_picture();
   }
}

void ServiceBuilding::destroy()
{
   for (std::list<Walker*>::iterator itWalker = _walkerList.begin(); itWalker != _walkerList.end(); ++itWalker)
   {
      Walker *walker = *itWalker;
      Scenario::instance().getCity().getWalkerList().remove(walker);
      delete walker;
   }
}

void ServiceBuilding::deliverService()
{
   // make a service walker and send him to his wandering
   ServiceWalker *walker = new ServiceWalker(_service);
   walker->setServiceBuilding(*this);
   walker->start();
   _walkerList.push_back(walker);
}

int ServiceBuilding::getServiceRange() const
{
   return _serviceRange;
}

void ServiceBuilding::serialize(OutputSerialStream &stream)
{
   WorkingBuilding::serialize(stream);
   stream.write_int(_serviceTimer, 2, 0, 1000);
   stream.write_int(_serviceDelay, 2, 0, 1000);
   stream.write_int(_serviceRange, 2, 0, 65535);
}

void ServiceBuilding::unserialize(InputSerialStream &stream)
{
   WorkingBuilding::unserialize(stream);
   _serviceTimer = stream.read_int(2, 0, 1000);
   _serviceDelay = stream.read_int(2, 0, 1000);
   _serviceRange = stream.read_int(2, 0, 65535);
}


std::map<ServiceType, ServiceBuilding*> ServiceBuilding::_specimen;

std::map<ServiceType, ServiceBuilding*>& ServiceBuilding::getSpecimen()
{
   if (_specimen.empty())
   {
      _specimen[S_WELL] = new BuildingWell();
      _specimen[S_FOUNTAIN] = new BuildingFountain();
      _specimen[S_ENGINEER] = new BuildingEngineer();
      _specimen[S_PREFECT] = new BuildingPrefect();
      _specimen[S_THEATER] = new BuildingTheater();
   }

   return _specimen;
}

GuiInfoBox* ServiceBuilding::makeInfoBox()
{
   GuiInfoService* box = new GuiInfoService(*this);
   return box;
}


BuildingWell::BuildingWell() : ServiceBuilding(S_WELL)
{
   setType(B_WELL);
   _size = 1;
   setPicture(PicLoader::instance().get_picture("utilitya", 1));
}

BuildingWell* BuildingWell::clone() const
{
   return new BuildingWell(*this);
}

void BuildingWell::deliverService()
{
   ServiceWalker walker(getService());
   walker.setServiceBuilding(*this);
   std::set<Building*> reachedBuildings = walker.getReachedBuildings(getTile().getI(), getTile().getJ());
   for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
   {
      Building &building = **itBuilding;
      building.applyService(walker);
   }
}


BuildingFountain::BuildingFountain() : ServiceBuilding(S_FOUNTAIN)
{
   setType(B_FOUNTAIN);
   _size = 1;
   setPicture(PicLoader::instance().get_picture("utilitya", 10));
}

BuildingFountain* BuildingFountain::clone() const
{
   return new BuildingFountain(*this);
}

void BuildingFountain::deliverService()
{
   ServiceWalker walker(getService());
   walker.setServiceBuilding(*this);
   std::set<Building*> reachedBuildings = walker.getReachedBuildings(getTile().getI(), getTile().getJ());
   for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
   {
      Building &building = **itBuilding;
      building.applyService(walker);
   }
}

BuildingEngineer::BuildingEngineer() : ServiceBuilding(S_ENGINEER)
{
   setType(B_ENGINEER);
   _size = 1;
   setPicture(PicLoader::instance().get_picture("transport", 56));
}

BuildingEngineer* BuildingEngineer::clone() const
{
   return new BuildingEngineer(*this);
}


BuildingPrefect::BuildingPrefect() : ServiceBuilding(S_PREFECT)
{
   setType(B_PREFECT);
   _size = 1;
   setPicture(PicLoader::instance().get_picture("security", 1));
}

BuildingPrefect* BuildingPrefect::clone() const
{
   return new BuildingPrefect(*this);
}


EntertainmentBuilding::EntertainmentBuilding(const ServiceType &service) : ServiceBuilding(service)
{
   switch (service)
   {
   case S_THEATER:
      _traineeMap[WTT_ACTOR] = 0;
      break;
   case S_AMPHITHEATER:
      _traineeMap[WTT_ACTOR] = 0;
      _traineeMap[WTT_GLADIATOR] = 0;
      break;
   case S_COLLOSSEUM:
      _traineeMap[WTT_GLADIATOR] = 0;
      _traineeMap[WTT_TAMER] = 0;
      break;
   default:
      break;
   }
}

void EntertainmentBuilding::deliverService()
{
   // we need all trainees types for the show
   int minLevel = 100;
   for (std::map<WalkerTraineeType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
   {
      minLevel = std::min(minLevel, itLevel->second);
   }

   if (minLevel > 10)
   {
      // all trainees are there for the show!
      for (std::map<WalkerTraineeType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
      {
         itLevel->second = itLevel->second - 10;
      }
      ServiceBuilding::deliverService();
   }
}


BuildingTheater::BuildingTheater() : EntertainmentBuilding(S_THEATER)
{
   setType(B_THEATER);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("entertainment", 13));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "entertainment", 14, 21);
   animLoader.change_offset(_animation, 60, 36);
   _fgPictures.resize(2);
   _fgPictures[0] = &PicLoader::instance().get_picture("entertainment", 35);
}

BuildingTheater* BuildingTheater::clone() const
{
   return new BuildingTheater(*this);
}


BuildingAmphiTheater::BuildingAmphiTheater() : EntertainmentBuilding(S_AMPHITHEATER)
{
   setType(B_AMPHITHEATER);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("entertainment", 1));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "entertainment", 2, 10);
   animLoader.change_offset(_animation, 100, 49);
   _fgPictures.resize(2);
   _fgPictures[0] = &PicLoader::instance().get_picture("entertainment", 12);
}

BuildingAmphiTheater* BuildingAmphiTheater::clone() const
{
   return new BuildingAmphiTheater(*this);
}


BuildingCollosseum::BuildingCollosseum() : EntertainmentBuilding(S_COLLOSSEUM)
{
   setType(B_COLLOSSEUM);
   _size = 5;
   setPicture(PicLoader::instance().get_picture("entertainment", 36));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "entertainment", 37, 13);
   animLoader.change_offset(_animation, 122, 81);
   _fgPictures.resize(2);
   _fgPictures[0] = &PicLoader::instance().get_picture("entertainment", 50);
}

BuildingCollosseum* BuildingCollosseum::clone() const
{
   return new BuildingCollosseum(*this);
}

TempleCeres::TempleCeres() : ServiceBuilding(S_TEMPLE_CERES)
{
   setType(B_TEMPLE_CERES);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 45));
}

TempleCeres* TempleCeres::clone() const
{
   return new TempleCeres(*this);
}

BigTempleCeres::BigTempleCeres() : ServiceBuilding(S_TEMPLE_CERES)
{
   setType(B_BIG_TEMPLE_CERES);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("security", 46));
}

BigTempleCeres* BigTempleCeres::clone() const
{
   return new BigTempleCeres(*this);
}

TempleNeptune::TempleNeptune() : ServiceBuilding(S_TEMPLE_NEPTUNE)
{
   setType(B_TEMPLE_NEPTUNE);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 47));
}

TempleNeptune* TempleNeptune::clone() const
{
   return new TempleNeptune(*this);
}

BigTempleNeptune::BigTempleNeptune() : ServiceBuilding(S_TEMPLE_NEPTUNE)
{
   setType(B_BIG_TEMPLE_NEPTUNE);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("security", 48));
}

BigTempleNeptune* BigTempleNeptune::clone() const
{
   return new BigTempleNeptune(*this);
}

TempleMars::TempleMars() : ServiceBuilding(S_TEMPLE_MARS)
{
   setType(B_TEMPLE_MARS);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 51));
}

TempleMars* TempleMars::clone() const
{
   return new TempleMars(*this);
}

BigTempleMars::BigTempleMars() : ServiceBuilding(S_TEMPLE_MARS)
{
   setType(B_BIG_TEMPLE_MARS);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("security", 52));
}

BigTempleMars* BigTempleMars::clone() const
{
   return new BigTempleMars(*this);
}

TempleVenus::TempleVenus() : ServiceBuilding(S_TEMPLE_VENUS)
{
   setType(B_TEMPLE_VENUS);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 53));
}

TempleVenus* TempleVenus::clone() const
{
   return new TempleVenus(*this);
}

BigTempleVenus::BigTempleVenus() : ServiceBuilding(S_TEMPLE_VENUS)
{
   setType(B_BIG_TEMPLE_VENUS);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("security", 54));
}

BigTempleVenus* BigTempleVenus::clone() const
{
   return new BigTempleVenus(*this);
}

TempleMercure::TempleMercure() : ServiceBuilding(S_TEMPLE_MERCURE)
{
   setType(B_TEMPLE_MERCURE);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 49));
}

TempleMercure* TempleMercure::clone() const
{
   return new TempleMercure(*this);
}

BigTempleMercure::BigTempleMercure() : ServiceBuilding(S_TEMPLE_MERCURE)
{
   setType(B_BIG_TEMPLE_MERCURE);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("security", 50));
}

BigTempleMercure* BigTempleMercure::clone() const
{
   return new BigTempleMercure(*this);
}

TempleOracle::TempleOracle() : ServiceBuilding(S_TEMPLE_ORACLE)
{
   setType(B_TEMPLE_ORACLE);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 55));
}

TempleOracle* TempleOracle::clone() const
{
   return new TempleOracle(*this);
}


School::School() : ServiceBuilding(S_SCHOOL)
{
   setType(B_SCHOOL);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("commerce", 83));
}

School* School::clone() const
{
   return new School(*this);
}


Library::Library() : ServiceBuilding(S_LIBRARY)
{
   setType(B_LIBRARY);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("commerce", 84));
}

Library* Library::clone() const
{
   return new Library(*this);
}


College::College() : ServiceBuilding(S_COLLEGE)
{
   setType(B_COLLEGE);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("commerce", 85));
}

College* College::clone() const
{
   return new College(*this);
}


Baths::Baths() : ServiceBuilding(S_BATHS)
{
   setType(B_BATHS);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("security", 21));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "security", 22, 10);
   animLoader.change_offset(_animation, 23, 25);
   _fgPictures.resize(2);
}

Baths* Baths::clone() const
{
   return new Baths(*this);
}


Barber::Barber() : ServiceBuilding(S_BARBER)
{
   setType(B_BARBER);
   _size = 1;
   setPicture(PicLoader::instance().get_picture("security", 19));
}

Barber* Barber::clone() const
{
   return new Barber(*this);
}


Doctor::Doctor() : ServiceBuilding(S_DOCTOR)
{
   setType(B_DOCTOR);
   _size = 1;
   setPicture(PicLoader::instance().get_picture("security", 20));
}

Doctor* Doctor::clone() const
{
   return new Doctor(*this);
}


Hospital::Hospital() : ServiceBuilding(S_HOSPITAL)
{
   setType(B_HOSPITAL);
   _size = 3;
   setPicture(PicLoader::instance().get_picture("security", 44));
}

Hospital* Hospital::clone() const
{
   return new Hospital(*this);
}


Forum::Forum() : ServiceBuilding(S_FORUM)
{
   setType(B_FORUM);
   _size = 2;
   setPicture(PicLoader::instance().get_picture("govt", 10));
}

Forum* Forum::clone() const
{
   return new Forum(*this);
}

// govt 4  - senate
// govt 9  - advanced senate
// govt 5 ~ 8 - senate flags

Senate::Senate() : ServiceBuilding(S_SENATE)
{
   setType(B_SENATE);
   _size = 5;
   setPicture(PicLoader::instance().get_picture("govt", 4));
   std::cout << "Senate::Senate()" << std::endl;
}

Senate* Senate::clone() const
{
   return new Senate(*this);
}

Market::Market() : ServiceBuilding(S_MARKET)
{
   setType(B_MARKET);
   setMaxWorkers(5);
   setWorkers(0);

   _marketBuyer = NULL;
   _buyerDelay = 10;
   _size = 2;
   // _name = _("Marche");
   setPicture(PicLoader::instance().get_picture("commerce", 1));
   _fgPictures.resize(1);  // animation

   _goodStore.setMaxQty(5000);
   _goodStore.setMaxQty(G_WHEAT, 400);
   _goodStore.setMaxQty(G_POTTERY, 300);
   _goodStore.setCurrentQty(G_WHEAT, 200);

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, "commerce", 2, 10);
}

Market* Market::clone() const
{
   Market* res = new Market(*this);
   return res;
}


void Market::timeStep(const unsigned long time)
{
   ServiceBuilding::timeStep(time);

   if (_marketBuyer == NULL)
   {
      _buyerDelay -= 1;

      if (_buyerDelay == 0)
      {
         // the marketBuyer is ready to buy something!
         _marketBuyer = new MarketBuyer();
         _marketBuyer->setMarket(*this);
         _marketBuyer->start();
         _buyerDelay = 50;
      }
   }
   else
   {
      // there is a market buyer
      if (_marketBuyer->isDeleted())
      {
         delete _marketBuyer;
         _marketBuyer = NULL;
      }
   }

}


GuiInfoBox* Market::makeInfoBox()
{
   GuiInfoMarket* box = new GuiInfoMarket(*this);
   return box;
}


SimpleGoodStore& Market::getGoodStore()
{
   return _goodStore;
}


std::list<GoodType> Market::getMostNeededGoods()
{
   std::list<GoodType> res;

   std::multimap<float, GoodType> mapGoods;  // ordered by demand

   for (int n = 0; n<G_MAX; ++n)
   {
      // for all types of good
      GoodType goodType = (GoodType) n;
      GoodStock &stock = _goodStore.getStock(goodType);
      int demand = stock._maxQty - stock._currentQty;
      if (demand > 99)
      {
         mapGoods.insert(std::make_pair(float(stock._currentQty)/float(stock._maxQty), goodType));
      }
   }

   for (std::multimap<float, GoodType>::iterator itMap = mapGoods.begin(); itMap != mapGoods.end(); ++itMap)
   {
      GoodType goodType = itMap->second;
      res.push_back(goodType);
   }

   return res;
}


int Market::getGoodDemand(const GoodType &goodType)
{
   int res = 0;
   GoodStock &stock = _goodStore.getStock(goodType);
   res = stock._maxQty - stock._currentQty;
   res = (res/100)*100;  // round at the lowest century
   return res;
}

void Market::serialize(OutputSerialStream &stream)
{
   ServiceBuilding::serialize(stream);
   _goodStore.serialize(stream);
   stream.write_objectID(_marketBuyer);
   stream.write_int(_buyerDelay, 2, 0, 65535);
}

void Market::unserialize(InputSerialStream &stream)
{
   ServiceBuilding::unserialize(stream);
   _goodStore.unserialize(stream);
   stream.read_objectID((void**)&_marketBuyer);
   _buyerDelay = stream.read_int(2, 0, 1000);
}
