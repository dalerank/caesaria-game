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

#include "oc3_building_house.hpp"

#include "oc3_tile.hpp"
#include "oc3_house_level.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_exception.hpp"
#include "oc3_walker_workerhunter.hpp"
#include "oc3_walker_immigrant.hpp"
#include "oc3_building_market.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_variant.hpp"
#include "oc3_empire.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_city.hpp"
#include "oc3_foreach.hpp"
#include "oc3_game_event_mgr.hpp"

class House::Impl
{
public:
  typedef std::map<Service::Type, int> ServiceAccessMap;
  int picIdOffset;
  int houseId;  // pictureId
  int houseLevel;
  float healthLevel;
  HouseLevelSpec levelSpec;  // characteristics of the current house level
  BuildingData::Desirability desirability;
  SimpleGoodStore goodStore;
  ServiceAccessMap serviceAccess;  // value=access to the service (0=no access, 100=good access)
  int currentHabitants;
  int maxHabitants;
  int freeWorkersCount;
  DateTime lastPayDate;
  std::string condition4Up;  

  bool mayPayTax()
  {
    return lastPayDate.getMonthToDate( GameDate::current() ) > 2;
  }

  int getAvailableTax()
  {
    return levelSpec.getTaxRate() * currentHabitants;
  }

  void updateHealthLevel()
  {
    float delim = 1 + (((serviceAccess[Service::well]>0 || serviceAccess[Service::fontain]>0) ? 1 : 0))
                + ((serviceAccess[Service::doctor]>0 || serviceAccess[Service::hospital]) ? 1 : 0)
                + (serviceAccess[Service::baths] ? 0.7 : 0)
                + (serviceAccess[Service::barber] ? 0.3 : 0);

    float decrease = 0.3f / delim;

    healthLevel = math::clamp<float>( healthLevel - decrease, 0, 100 );
  }

  void initGoodStore( int size )
  {
    int rsize = 25 * size * houseLevel;
    goodStore.setMaxQty( rsize * 10 );  // no limit
    goodStore.setMaxQty(Good::wheat, rsize );
    goodStore.setMaxQty(Good::fish, rsize );
    goodStore.setMaxQty(Good::meat, rsize );
    goodStore.setMaxQty(Good::fruit, rsize );
    goodStore.setMaxQty(Good::vegetable, rsize );
    goodStore.setMaxQty(Good::pottery, rsize );
    goodStore.setMaxQty(Good::furniture, rsize);
    goodStore.setMaxQty(Good::oil, rsize );
    goodStore.setMaxQty(Good::wine, rsize );
  }
};

House::House(const int houseId) : Building( B_HOUSE ), _d( new Impl )
{
  _d->houseId = houseId;
  _d->lastPayDate = DateTime( -400, 1, 1 );
  _d->picIdOffset = ( rand() % 10 > 6 ? 1 : 0 );
  _d->freeWorkersCount = 0;
  _d->healthLevel = 100;
  HouseSpecHelper& helper = HouseSpecHelper::getInstance();
  _d->houseLevel = helper.getHouseLevel( houseId );
  _d->levelSpec = helper.getHouseLevelSpec( _d->houseLevel );
  setName( _d->levelSpec.getLevelName() );
  _d->currentHabitants = 0;
  _d->desirability.base = -3;
  _d->desirability.range = 3;
  _d->desirability.step = 1;
  _fireLevel = 0;

  _d->initGoodStore( 1 );

  // init the service access
  for (int i = 0; i<Service::S_MAX; ++i)
  {
    // for every service type
    Service::Type service = Service::Type(i);
    _d->serviceAccess[service] = 0;
  }

  _update();
}

void House::timeStep(const unsigned long time)
{
  if( _d->currentHabitants > 0 )
  {
    if( time % 16 == 0 )
    {
       // consume services
       for( int i = 0; i < Service::S_MAX; ++i)
       {
          Service::Type service = (Service::Type) i;
          _d->serviceAccess[service] = std::max(_d->serviceAccess[service] - 1, 0);
       }

       cancelService( Service::S_WORKERS_HUNTER );
       _d->updateHealthLevel();
    }

    if( time % 64 == 0 )
    {
      // consume goods
      for( int i = 0; i < Good::goodCount; ++i)
      {
         Good::Type goodType = (Good::Type) i;
         int montlyGoodsQty = _d->levelSpec.computeMonthlyConsumption( *this, goodType, true );
         _d->goodStore.setCurrentQty( goodType, std::max( _d->goodStore.getCurrentQty(goodType) - montlyGoodsQty, 0) );
      }

      bool validate = _d->levelSpec.checkHouse( this );
      if( !validate )
      {
        levelDown();
      }
      else
      {
        _d->condition4Up = "";
        validate = _d->levelSpec.next().checkHouse( this, &_d->condition4Up );
        if( validate && _d->currentHabitants > 0 )
        {
           levelUp();
        }
      }

      int homeless = math::clamp( _d->currentHabitants - _d->maxHabitants, 0, 0xff );

      if( homeless > 0 )
      {
        _d->currentHabitants = math::clamp( _d->currentHabitants, 0, _d->maxHabitants );

        ImmigrantPtr im = Immigrant::create( _getCity() );
        im->setCapacity( homeless );
        im->send2City( getTile() );
      }
    }

    Building::timeStep( time );
  }
}

GoodStore& House::getGoodStore()
{
   return _d->goodStore;
}


const HouseLevelSpec& House::getLevelSpec() const
{
   return _d->levelSpec;
}

void House::_tryUpdate_1_to_11_lvl( int level4grow, int startSmallPic, int startBigPic, const char desirability )
{
  CityHelper helper( _getCity() );

  if( getSize() == 1 )
  {
    Tilemap& tmap = _getCity()->getTilemap();
    TilemapTiles area = tmap.getArea( getTile().getIJ(), Size(2) );
    bool mayGrow = true;

    foreach( Tile* tile, area )
    {
      if( tile == NULL )
      {
        mayGrow = false;   //some broken, can't grow
        break;
      }

      HousePtr house = tile->getOverlay().as<House>();
      if( house != NULL && house->getLevelSpec().getHouseLevel() == level4grow )
      {
        if( house->getSize().getWidth() > 1 )  //bigger house near, can't grow
        {
          mayGrow = false;
          break;
        }            
      }
      else
      {
        mayGrow = false; //no house near, can't grow
        break;
      }
    }

    if( mayGrow )
    {
      int sumHabitants = getNbHabitants();
      int sumFreeWorkers = _d->freeWorkersCount;
      TilemapTiles::iterator delIt=area.begin();
      HousePtr selfHouse = (*delIt)->getOverlay().as<House>();

      delIt++; //don't remove himself
      for( ; delIt != area.end(); delIt++ )
      {
        HousePtr house = (*delIt)->getOverlay().as<House>();
        if( house.isValid() )
        {
          house->deleteLater();
          house->_d->currentHabitants = 0;

          sumHabitants += house->getNbHabitants();
          sumFreeWorkers += house->_d->freeWorkersCount;

          selfHouse->getGoodStore().storeAll( house->getGoodStore() );
        }
      }

      _d->currentHabitants = sumHabitants;
      _d->freeWorkersCount = sumFreeWorkers;

      //reset desirability level with old house size
      helper.updateDesirability( this, false );

      _update();
      setSize( getSize() + Size(1) );      
      build( _getCity(), getTile().getIJ() );
      //set new desirability level
      helper.updateDesirability( this, true );
    }
  }

  //that this house will be upgrade, we need decrease current desirability level
  helper.updateDesirability( this, false );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;
  //now upgrade groud area to new desirability
  helper.updateDesirability( this, true );

  bool bigSize = getSize().getWidth() > 1;
  _d->houseId = bigSize ? startBigPic : startSmallPic; 
  _d->picIdOffset = bigSize ? 0 : ( (rand() % 10 > 6) ? 1 : 0 );
}


void House::levelUp()
{
  _d->houseLevel++;   
  _d->picIdOffset = 0;
     
  switch (_d->houseLevel)
  {
  case 1:
    _d->houseId = 1;
    _d->desirability.base = -3;
    _d->desirability.step = 1;
  break;

  case 2: _tryUpdate_1_to_11_lvl( 1, 1, 5, -3);
  break;
  
  case 3: _tryUpdate_1_to_11_lvl( 2, 3, 6, -3 ); 
  break;
  
  case 4: _tryUpdate_1_to_11_lvl( 3, 7, 11, -2 );
  break;
  
  case 5: _tryUpdate_1_to_11_lvl( 4, 9, 12, -2 );
  break;

  case 6: _tryUpdate_1_to_11_lvl( 5, 13, 17, -2 );
  break;

  case 7: _tryUpdate_1_to_11_lvl( 6, 15, 18, -2 );
  break;

  case 8: _tryUpdate_1_to_11_lvl( 7, 19, 23, -1 );
  break;

  case 9: _tryUpdate_1_to_11_lvl( 8, 21, 24, -1 );
  break;

  case 10: _tryUpdate_1_to_11_lvl( 9, 25, 29, 0 );
  break;

  case 11: _tryUpdate_1_to_11_lvl( 10, 27, 30, 0 );
  break;
  }

  _d->levelSpec = HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel);

  _update();
}

void House::_tryDegrage_11_to_2_lvl( int smallPic, int bigPic, const char desirability )
{
  bool bigSize = getSize().getWidth() > 1;
  _d->houseId = bigSize ? bigPic : smallPic;
  _d->picIdOffset = bigSize ? 0 : ( rand() % 10 > 6 ? 1 : 0 );

  CityHelper helper( _getCity() );
  //clear current desirability influence
  helper.updateDesirability( this, false );

  _d->desirability.base = desirability;
  _d->desirability.step = desirability < 0 ? 1 : -1;
  //set new desirability level
  helper.updateDesirability( this, true );
}

void House::levelDown()
{
   _d->houseLevel--;
   _d->levelSpec = HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel);

   switch (_d->houseLevel)
   {
   case 1:
   {
     _d->houseId = 1;
     _d->picIdOffset = ( rand() % 10 > 6 ? 1 : 0 );

     Tilemap& tmap = _getCity()->getTilemap();

     if( getSize().getArea() > 1 )
     {
       TilemapTiles perimetr = tmap.getArea( getTilePos(), Size(2) );
       int peoplesPerHouse = getNbHabitants() / 4;
       _d->currentHabitants = peoplesPerHouse;
       foreach( Tile* tile, perimetr )
       {
         HousePtr house = ConstructionManager::getInstance().create( B_HOUSE ).as<House>();
         house->_d->currentHabitants = peoplesPerHouse;
         house->_d->houseId = smallHovel;
         house->_update();

         GameEventMgr::append( BuildEvent::create( tile->getIJ(), house.as<Construction>() ));
       }

       deleteLater();
     }
   }
   break;
   
   case 2: _tryDegrage_11_to_2_lvl( 1, 5, -3 );
   break;
   
   case 3: _tryDegrage_11_to_2_lvl( 3, 6, -3 );
   break;
   
   case 4: _tryDegrage_11_to_2_lvl( 7, 11, -2 );
   break;
   
   case 5: _tryDegrage_11_to_2_lvl( 9, 12, -2 );
   break;

   case 6: _tryDegrage_11_to_2_lvl( 13, 17, -2 );
   break;

   case 7: _tryDegrage_11_to_2_lvl( 15, 18, -2 );
   break;

   case 8: _tryDegrage_11_to_2_lvl( 19, 23, -1 );
   break;

   case 9: _tryDegrage_11_to_2_lvl( 21, 23, -1 );
   break;

   case 10: _tryDegrage_11_to_2_lvl( 25, 29, 0 );
   break;

   case 11: _tryDegrage_11_to_2_lvl( 27, 30, 0 );
   break;
   }

   _update();
}

void House::buyMarket( ServiceWalkerPtr walker )
{
  // std::cout << "House buyMarket" << std::endl;
  MarketPtr market = walker->getBase().as<Market>();
  GoodStore& marketStore = market->getGoodStore();

  GoodStore &houseStore = getGoodStore();
  for (int i = 0; i < Good::goodCount; ++i)
  {
    Good::Type goodType = (Good::Type) i;
    int houseQty = houseStore.getCurrentQty(goodType) / 10;
    int houseSafeQty = _d->levelSpec.computeMonthlyConsumption(*this, goodType, false )
                       + _d->levelSpec.next().computeMonthlyConsumption(*this, goodType, false );
    int marketQty = marketStore.getCurrentQty(goodType);
    if( houseQty < houseSafeQty && marketQty > 0  )
    {
       int qty = std::min( houseSafeQty - houseQty, marketQty);
       qty = math::clamp( qty, 0, houseStore.getFreeQty( goodType ) / 10 );

       if( qty > 0 )
       {
         GoodStock stock(goodType, qty);
         marketStore.retrieve(stock, qty);

         stock._currentQty = stock._maxQty = qty * 10;

         houseStore.store(stock, stock._currentQty );
       }
    }
  }
}

void House::applyService( ServiceWalkerPtr walker )
{
  Building::applyService(walker);  // handles basic services, and remove service reservation

  Service::Type service = walker->getService();
  switch (service)
  {
  case Service::well:
  case Service::fontain:
  case Service::S_TEMPLE_NEPTUNE:
  case Service::S_TEMPLE_CERES:
  case Service::S_TEMPLE_VENUS:
  case Service::S_TEMPLE_MARS:
  case Service::S_TEMPLE_MERCURE:
  case Service::barber:
  case Service::baths:
  case Service::school:
  case Service::library:
  case Service::college:
  case Service::theater:
  case Service::amphitheater:
  case Service::colloseum:
  case Service::hippodrome:
    setServiceAccess(service, 100);
  break;

  case Service::hospital:
  case Service::doctor:
    _d->healthLevel += 10;
    setServiceAccess(service, 100);
  break;
  
  case Service::S_MARKET:
    buyMarket(walker);
  break;
 
  case Service::S_SENATE:
  case Service::S_FORUM:
    setServiceAccess(service, 100);
  break;

  case Service::S_TEMPLE_ORACLE:
  case Service::engineer:
  case Service::S_PREFECT:
  case Service::S_MAX:
  break;

  case Service::S_WORKERS_HUNTER:
  {
    if( !_d->freeWorkersCount )
      break;

    WorkersHunterPtr hunter = walker.as<WorkersHunter>();
    if( hunter.isValid() )
    {
      int hiredWorkers = math::clamp( _d->freeWorkersCount, 0, hunter->getWorkersNeeded() );
      _d->freeWorkersCount -= hiredWorkers;
      hunter->hireWorkers( hiredWorkers );
    }
  }
  break;

  default:
  break;
  }
}

float House::evaluateService(ServiceWalkerPtr walker)
{
  float res = 0.0;
  Service::Type service = walker->getService();
  if( _reservedServices.count(service) == 1 )
  {
     // service is already reserved
    return 0.0;
  }

  switch(service)
  {
  case Service::engineer: res = _damageLevel; break;
  case Service::S_PREFECT: res = _fireLevel; break;

  // this house pays taxes
  case Service::S_FORUM:
  case Service::S_SENATE:
    res = _d->mayPayTax() ? (float)_d->getAvailableTax() : 0.f;
  break;

  case Service::S_MARKET:
  {
    MarketPtr market = walker->getBase().as<Market>();
    GoodStore &marketStore = market->getGoodStore();
    GoodStore &houseStore = getGoodStore();
    for (int i = 0; i < Good::goodCount; ++i)
    {
      Good::Type goodType = (Good::Type) i;
      int houseQty = houseStore.getCurrentQty(goodType) / 10;
      int houseSafeQty = _d->levelSpec.computeMonthlyConsumption(*this, goodType, false)
                         + _d->levelSpec.next().computeMonthlyConsumption(*this, goodType, false );
      int marketQty = marketStore.getCurrentQty(goodType);
      if( houseQty < houseSafeQty && marketQty > 0)
      {
         res += std::min( houseSafeQty - houseQty, marketQty);
      }
    }
  }
  break;

  case Service::S_WORKERS_HUNTER:
  {
    res = (float)_d->freeWorkersCount;        
  }
  break;

  default:
  {
    return _d->levelSpec.evaluateServiceNeed( this, service);
  }
  break;
  }

  // std::cout << "House evaluateService " << service << "=" << res << std::endl;

  return res;
}

bool House::hasServiceAccess(const Service::Type service)
{
  return (_d->serviceAccess[service] > 0);
}

int House::getServiceValue( const Service::Type service)
{
  return _d->serviceAccess[service];
}

void House::setServiceAccess(const Service::Type service, const int access)
{
  _d->serviceAccess[service] = access;
}

int House::getNbHabitants()
{
  return _d->currentHabitants;
}

int House::getMaxHabitants()
{
  return _d->maxHabitants;
}

void House::_update()
{
  int picId = ( _d->houseId == smallHovel && _d->currentHabitants == 0 ) ? 45 : (_d->houseId + _d->picIdOffset);
  setPicture( ResourceGroup::housing, picId );
  setSize( Size( (getPicture().getWidth() + 2 ) / 60 ) );
  _d->maxHabitants = _d->levelSpec.getMaxHabitantsByTile() * getSize().getArea();
  _d->initGoodStore( getSize().getArea() );
}

int House::getMaxDistance2Road() const
{
  return 2;
}

void House::addHabitants( const int newHabitCount )
{
  int peoplesCount = (std::min)( _d->currentHabitants + newHabitCount, _d->maxHabitants );
  _d->currentHabitants = peoplesCount;
  _d->freeWorkersCount += peoplesCount / 2;
  _update();
}

void House::destroy()
{
  int homeless = _d->currentHabitants;
  _d->currentHabitants = _d->maxHabitants;

  if( homeless > 0 )
  {
    ImmigrantPtr im = Immigrant::create( _getCity() );
    im->setCapacity( homeless );
    im->send2City( getTile() );
  }

  Building::destroy();
}

bool House::isWalkable() const
{
  return (_d->houseId == smallHovel && _d->currentHabitants == 0) ? true : false;
}

const BuildingData::Desirability& House::getDesirabilityInfo() const
{
  return _d->desirability;
}

void House::save( VariantMap& stream ) const
{
  Building::save( stream );

  stream[ "picIdOffset" ] = _d->picIdOffset;
  stream[ "houseId" ] = _d->houseId;
  stream[ "houseLevel" ] = _d->houseLevel;
  stream[ "desirability" ] = _d->desirability.base;
  stream[ "currentHubitants" ] = _d->currentHabitants;
  stream[ "maxHubitants" ] = _d->maxHabitants;
  stream[ "freeWorkersCount" ] = _d->freeWorkersCount;
  stream[ "goodstore" ] = _d->goodStore.save();
  stream[ "healthLevel" ] = _d->healthLevel;

  VariantList vl_services;
  foreach( Impl::ServiceAccessMap::value_type& mapItem, _d->serviceAccess )
  {
    vl_services.push_back( Variant( (int)mapItem.first) );
    vl_services.push_back( Variant( mapItem.second ) );
  }

  stream[ "services" ] = vl_services;
} 

void House::load( const VariantMap& stream )
{
  Building::load( stream );

  _d->picIdOffset = (int)stream.get( "picIdOffset", 0 );
  _d->houseId = (int)stream.get( "houseId", 0 );
  _d->houseLevel = (int)stream.get( "houseLevel", 0 );
  _d->healthLevel = (float)stream.get( "healthLevel", 0 );
  _d->levelSpec = HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel);

  _d->desirability.base = (int)stream.get( "desirability", 0 );
  _d->desirability.step = _d->desirability.base < 0 ? 1 : -1;

  _d->currentHabitants = (int)stream.get( "currentHubitants", 0 );
  _d->maxHabitants = (int)stream.get( "maxHubitants", 0 );
  _d->freeWorkersCount = (int)stream.get( "freeWorkersCount", 0 );

  _d->goodStore.load( stream.get( "goodstore" ).toMap() );

  _d->initGoodStore( getSize().getArea() );

  VariantList vl_services = stream.get( "services" ).toList();
  for( VariantList::iterator it = vl_services.begin(); it != vl_services.end(); it++ )
  {
    Service::Type type = Service::Type( (*it).toInt() );
    it++;
    int serviceValue = (*it).toInt();

    _d->serviceAccess[ type ] = serviceValue;
  }

  Building::build( _getCity(), getTilePos() );
  _update();
}

int House::getFoodLevel() const
{
  switch( _d->levelSpec.getHouseLevel() )
  {
  case smallHovel:
  case bigTent:
    return -1;
  
  case smallHut:
  case bigHut: 
  {
    int ret = _d->goodStore.getCurrentQty(Good::wheat);
    return ret;
  }
  
  default: 
    return -1;
  }
}

int House::getHealthLevel() const
{
  return _d->healthLevel;
}

int House::getScholars() const
{
  HouseLevelSpec level = getLevelSpec();
  if( level.getHouseLevel() < 3 )
  {
    return 0;
  }
  else if( level.isPatrician() )
  {
    return _d->currentHabitants / 4;
  }
  else
  {
    return _d->currentHabitants / 5;
  }
}

int House::collectTaxes()
{
  _d->lastPayDate = GameDate::current();
  return _d->getAvailableTax();
}

std::string House::getUpCondition() const
{
  return _d->condition4Up;
}
