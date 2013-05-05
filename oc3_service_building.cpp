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



#include "oc3_service_building.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "oc3_scenario.hpp"
#include "oc3_servicewalker.hpp"
#include "oc3_walker_market_buyer.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_gettext.hpp"
#include "oc3_resourcegroup.hpp"

class ServiceBuilding::Impl
{
public:
  int serviceDelay;
};

ServiceBuilding::ServiceBuilding(const ServiceType service,
                                 const BuildingType type, const Size& size)
                                 : WorkingBuilding( type, size ), _d( new Impl )
{
   _service = service;
   setMaxWorkers(5);
   setWorkers(0);
   setServiceDelay( 80 );
   _serviceTimer = 0;
   _serviceRange = 30;
}

void ServiceBuilding::setServiceDelay( const int delay )
{
  _d->serviceDelay = delay;
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
      _serviceTimer = getServiceDelay();
   }
   else if (_serviceTimer > 0)
   {
      _serviceTimer -= 1;
   }

   _animation.update( time );
   Picture *pic = _animation.getCurrentPicture();
   if (pic != NULL)
   {
      int level = _fgPictures.size()-1;
      _fgPictures[level] = _animation.getCurrentPicture();
   }
}

void ServiceBuilding::destroy()
{
   for( Walkers::iterator itWalker = _walkerList.begin(); 
        itWalker != _walkerList.end(); ++itWalker)
   {
      (*itWalker)->deleteLater();
   }

   WorkingBuilding::destroy();
}

void ServiceBuilding::deliverService()
{
   // make a service walker and send him to his wandering
  ServiceWalkerPtr serviceman = ServiceWalker::create( BuildingPtr( this ),_service);
  serviceman->send2City();
  _addWalker( serviceman.as<Walker>() );
}

int ServiceBuilding::getServiceRange() const
{
   return _serviceRange;
}

void ServiceBuilding::serialize(OutputSerialStream &stream)
{
   WorkingBuilding::serialize(stream);
   stream.write_int(_serviceTimer, 2, 0, 1000);
   stream.write_int(_d->serviceDelay, 2, 0, 1000);
   stream.write_int(_serviceRange, 2, 0, 65535);
}

void ServiceBuilding::unserialize(InputSerialStream &stream)
{
   WorkingBuilding::unserialize(stream);
   _serviceTimer = stream.read_int(2, 0, 1000);
   _d->serviceDelay = stream.read_int(2, 0, 1000);
   _serviceRange = stream.read_int(2, 0, 65535);
}

GuiInfoBox* ServiceBuilding::makeInfoBox( Widget* parent )
{
   GuiInfoService* box = new GuiInfoService( parent, *this);
   return box;
}

int ServiceBuilding::getServiceDelay() const
{
  return _d->serviceDelay;
}

void ServiceBuilding::_addWalker( WalkerPtr walker )
{
  Scenario::instance().getCity().addWalker( walker );
  _walkerList.push_back( walker );
}

void ServiceBuilding::removeWalker( WalkerPtr walker )
{
  _walkerList.remove( walker );
}

const Walkers& ServiceBuilding::_getWalkerList() const
{
  return _walkerList;
}

ServiceBuilding::~ServiceBuilding()
{

}

BuildingWell::BuildingWell() : ServiceBuilding(S_WELL, B_WELL, Size(1) )
{
   _fireIncrement = 0;
   _damageIncrement = 0;
   setPicture( Picture::load("utilitya", 1) );
}

void BuildingWell::deliverService()
{
  ServiceWalkerPtr walker = ServiceWalker::create( BuildingPtr( this ), getService());
  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( getTile().getIJ() );
  for( ServiceWalker::ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); 
       itBuilding != reachedBuildings.end(); ++itBuilding)
  {
     (*itBuilding)->applyService( walker );
  }
}

BuildingFountain::BuildingFountain() : ServiceBuilding(S_FOUNTAIN, B_FOUNTAIN, Size(1))
{  
   int id;
   
   std::srand((unsigned int)std::time(NULL));
   
   id = std::rand() % 4;
   
   std::cout << id << std::endl;
   
   setPicture( Picture::load( ResourceGroup::utilitya, 26));
   _animation.load( ResourceGroup::utilitya, 27, 7);
   //animLoader.fill_animation_reverse(_animation, "utilitya", 25, 7);
   _animation.setOffset( Point( 14, 26 ) );
  _fgPictures.resize(1);
   
  //2 10 18 26
  // utilitya 10      - empty 
  // utilitya 11 - 17 - working fontain
   
  // the first fountain's (10) ofsets ~ 11, 23 
  /*AnimLoader animLoader(PicLoader::instance());
  animLoader.fill_animation(_animation, "utilitya", 11, 7); 
  animLoader.change_offset(_animation, 11, 23);
  _fgPictures.resize(1);*/
  
  // the second (2)    ~ 8, 42
  // the third (18)    ~ 8, 24
  // the 4rd   (26)    ~14, 26
   
   
}

void BuildingFountain::deliverService()
{
  ServiceWalkerPtr walker = ServiceWalker::create( BuildingPtr( this ), getService());
  ServiceWalker::ReachedBuildings reachedBuildings = walker->getReachedBuildings( getTile().getIJ() );
  for( ServiceWalker::ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
  {
    (*itBuilding)->applyService( walker );
  }
}

EntertainmentBuilding::EntertainmentBuilding(const ServiceType service, 
                                             const BuildingType type,
                                             const Size& size ) 
  : ServiceBuilding(service, type, size)
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


BuildingTheater::BuildingTheater() : EntertainmentBuilding(S_THEATER, B_THEATER, Size(2))
{
  setPicture( Picture::load( "entertainment", 13));

   _animation.load("entertainment", 14, 21);
   _animation.setOffset( Point( 60, 36 ) );
  
   _fgPictures.resize(2);
   _fgPictures[0] = &Picture::load("entertainment", 35);
}

BuildingAmphiTheater::BuildingAmphiTheater() : EntertainmentBuilding(S_AMPHITHEATER, B_AMPHITHEATER, Size(3))
{
  setPicture( Picture::load("entertainment", 1));

   _animation.load("entertainment", 2, 10);
   _animation.setOffset( Point( 100, 49 ) );
   _fgPictures.resize(2);
   _fgPictures[0] = &Picture::load("entertainment", 12);
}

BuildingCollosseum::BuildingCollosseum() : EntertainmentBuilding(S_COLLOSSEUM, B_COLLOSSEUM, Size(5) )
{
  setPicture( Picture::load("entertainment", 36));

   _animation.load("entertainment", 37, 13);
   _animation.setOffset( Point( 122, 81 ) );
   _fgPictures.resize(2);
   _fgPictures[0] = &Picture::load("entertainment", 50);
}

//------------

BuildingHippodrome::BuildingHippodrome() : EntertainmentBuilding(S_HIPPODROME, B_HIPPODROME, Size(5) )
{
  setPicture( Picture::load("circus", 5));
    getPicture().set_offset(0,106);
    Picture* logo = &Picture::load("circus", 3);
    Picture* logo1 = &Picture::load("circus", 1);
    logo -> set_offset(150,181);
    logo1 -> set_offset(300,310);
    _fgPictures.resize(5);
    _fgPictures.at(0) = logo;
    _fgPictures.at(1) = logo1;
}

//-----------

TempleCeres::TempleCeres() : ServiceBuilding(S_TEMPLE_CERES, B_TEMPLE_CERES, Size(2))
{
  setPicture( Picture::load( ResourceGroup::security, 45));
}

BigTempleCeres::BigTempleCeres() : ServiceBuilding(S_TEMPLE_CERES, B_BIG_TEMPLE_CERES, Size(3))
{
  setPicture( Picture::load( ResourceGroup::security, 46));
}

TempleNeptune::TempleNeptune() : ServiceBuilding(S_TEMPLE_NEPTUNE, B_TEMPLE_NEPTUNE, Size(2) )
{
  setPicture( Picture::load( ResourceGroup::security, 47));
}

BigTempleNeptune::BigTempleNeptune() : ServiceBuilding(S_TEMPLE_NEPTUNE, B_BIG_TEMPLE_NEPTUNE, Size(3))
{
  setPicture(Picture::load( ResourceGroup::security, 48));
}

TempleMars::TempleMars() : ServiceBuilding(S_TEMPLE_MARS, B_TEMPLE_MARS, Size(2))
{
  setPicture( Picture::load( ResourceGroup::security, 51));
}

BigTempleMars::BigTempleMars() : ServiceBuilding(S_TEMPLE_MARS, B_BIG_TEMPLE_MARS, Size(3))
{
  setPicture( Picture::load( ResourceGroup::security, 52));
}

TempleVenus::TempleVenus() : ServiceBuilding(S_TEMPLE_VENUS, B_TEMPLE_VENUS, Size(2))
{
  setPicture(Picture::load(ResourceGroup::security, 53));
}

BigTempleVenus::BigTempleVenus() : ServiceBuilding(S_TEMPLE_VENUS, B_BIG_TEMPLE_VENUS, Size(3))
{
  setPicture(Picture::load( ResourceGroup::security, 54));
}

TempleMercure::TempleMercure() : ServiceBuilding(S_TEMPLE_MERCURE, B_TEMPLE_MERCURE, Size(2))
{
  setPicture( Picture::load( ResourceGroup::security, 49));
}

BigTempleMercure::BigTempleMercure() : ServiceBuilding(S_TEMPLE_MERCURE, B_BIG_TEMPLE_MERCURE, Size(3))
{
  setPicture(Picture::load( ResourceGroup::security, 50));
}

TempleOracle::TempleOracle() : ServiceBuilding(S_TEMPLE_ORACLE, B_TEMPLE_ORACLE, Size(2) )
{
  setPicture( Picture::load( ResourceGroup::security, 55));
   
  _animation.load( ResourceGroup::security, 56, 6);
  _animation.setOffset( Point( 9, 30 ) );
  _fgPictures.resize(1);   
}

School::School() : ServiceBuilding(S_SCHOOL, B_SCHOOL, Size(2))
{
  setPicture( Picture::load( ResourceGroup::commerce, 83));
}

Library::Library() : ServiceBuilding(S_LIBRARY, B_LIBRARY, Size(2))
{
  setPicture( Picture::load( ResourceGroup::commerce, 84));
}

College::College() : ServiceBuilding(S_COLLEGE, B_COLLEGE, Size(3))
{
  setPicture( Picture::load( ResourceGroup::commerce, 85));
}

Baths::Baths() : ServiceBuilding(S_BATHS, B_BATHS, Size(2) )
{
  setPicture( Picture::load( ResourceGroup::security, 21));

   _animation.load( ResourceGroup::security, 22, 10);
   _animation.setOffset( Point( 23, 25 ) );
   _fgPictures.resize(2);
}

Barber::Barber() : ServiceBuilding(S_BARBER, B_BARBER, Size(1))
{
  setPicture( Picture::load( ResourceGroup::security, 19));
}

Doctor::Doctor() : ServiceBuilding(S_DOCTOR, B_DOCTOR, Size(1))
{
  setPicture( Picture::load( ResourceGroup::security, 20));
}

Hospital::Hospital() : ServiceBuilding(S_HOSPITAL, B_HOSPITAL, Size(3 ) )
{
  setPicture( Picture::load( ResourceGroup::security, 44));
}

Forum::Forum() : ServiceBuilding(S_FORUM, B_FORUM, Size(2))
{
  setPicture( Picture::load( "govt", 10));
}

Market::Market() : ServiceBuilding(S_MARKET, B_MARKET, Size(2) )
{
   setMaxWorkers(5);
   setWorkers(0);

   _marketBuyer = NULL;
   _buyerDelay = 10;
   // _name = _("Marche");
   setPicture(PicLoader::instance().get_picture("commerce", 1));
   _fgPictures.resize(1);  // animation

   _goodStore.setMaxQty(5000);
   _goodStore.setMaxQty(G_WHEAT, 400);
   _goodStore.setMaxQty(G_POTTERY, 300);
   _goodStore.setCurrentQty(G_WHEAT, 200);

   _animation.load( "commerce", 2, 10);
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


GuiInfoBox* Market::makeInfoBox( Widget* parent )
{
   GuiInfoMarket* box = new GuiInfoMarket( parent, *this);
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
