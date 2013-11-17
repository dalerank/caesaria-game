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

#include "house.hpp"

#include "gfx/tile.hpp"
#include "game/house_level.hpp"
#include "core/exception.hpp"
#include "walker/workerhunter.hpp"
#include "walker/immigrant.hpp"
#include "building/market.hpp"
#include "game/tileoverlay_factory.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "game/tilemap.hpp"
#include "game/gamedate.hpp"
#include "game/goodstore_simple.hpp"
#include "game/city.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "events/event.hpp"
#include "events/fireworkers.hpp"

using namespace constants;

class House::Impl
{
public:
  typedef std::map< Service::Type, Service > Services;
  int picIdOffset;
  int houseId;  // pictureId
  int houseLevel;
  float healthLevel;
  HouseLevelSpec spec;  // characteristics of the current house level
  MetaData::Desirability desirability;
  SimpleGoodStore goodStore;
  Services services;  // value=access to the service (0=no access, 100=good access)
  int maxHabitants;
  DateTime lastPayDate;
  std::string condition4Up;  
  CitizenGroup habitants;
  int currentYear;

  bool mayPayTax()
  {
    return lastPayDate.getMonthToDate( GameDate::current() ) > 0;
  }

  float getAvailableTax()
  {
    return spec.getTaxRate() * habitants.count( CitizenGroup::mature ) / 12.f;
  }

  void updateHealthLevel()
  {
    float delim = 1 + (((services[Service::well] > 0 || services[Service::fontain] > 0) ? 1 : 0))
                + ((services[Service::doctor] > 0 || services[Service::hospital] > 0) ? 1 : 0)
                + (services[Service::baths] > 0 ? 0.7 : 0)
                + (services[Service::barber] > 0 ? 0.3 : 0);

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

  void consumeServices()
  {
    int currentWorkersPower = services[ Service::recruter ];       //save available workers number
    for( Services::iterator it=services.begin(); it != services.end(); it++ )
    { it->second -= 1; } //consume services

    services[ Service::recruter ] = currentWorkersPower;     //restore available workers number
  }

  void makeOldHabitants()
  { 
    habitants.makeOld();
    habitants[ CitizenGroup::newborn ] = 0; //birth+helath function from mature habitants count
    habitants[ CitizenGroup::longliver ] = 0;
  }
};

House::House(const int houseId) : Building( building::house ), _d( new Impl )
{
  _d->houseId = houseId;
  _d->lastPayDate = DateTime( -400, 1, 1 );
  _d->picIdOffset = ( rand() % 10 > 6 ? 1 : 0 );
  _d->healthLevel = 100;
  HouseSpecHelper& helper = HouseSpecHelper::getInstance();
  _d->houseLevel = helper.getHouseLevel( houseId );
  _d->spec = helper.getHouseLevelSpec( _d->houseLevel );
  setName( _d->spec.getLevelName() );
  _d->desirability.base = -3;
  _d->desirability.range = 3;
  _d->desirability.step = 1;
  _d->currentYear = GameDate::current().getYear();
  updateState( Construction::fire, 0, false );

  _d->initGoodStore( 1 );

  // init the service access
  for( int i = 0; i<Service::srvCount; ++i )
  {
    // for every service type
    Service::Type service = Service::Type(i);
    _d->services[service] = Service();
  }
  _d->services[ Service::recruter ].setMax( 0 );
  _d->services[ Service::crime ] = 0;

  _update();
}

void House::timeStep(const unsigned long time)
{
  if( _d->habitants.empty()  )
    return;

  if( _d->currentYear != GameDate::current().getYear() )
  {
    _d->currentYear = GameDate::current().getYear();
    _d->makeOldHabitants();
  }

  if( time % 16 == 0 )
  {
    _d->consumeServices();
    _d->updateHealthLevel();    
    cancelService( Service::recruter );
  }

  if( time % 32 == 0 )
  {
    appendServiceValue( Service::crime, _d->spec.getCrime() + 2 );
  }

  if( time % 64 == 0 )
  {
    // consume goods
    for( int i = 0; i < Good::goodCount; ++i)
    {
       Good::Type goodType = (Good::Type) i;
       int montlyGoodsQty = _d->spec.computeMonthlyConsumption( *this, goodType, true );
       _d->goodStore.setCurrentQty( goodType, std::max( _d->goodStore.getCurrentQty(goodType) - montlyGoodsQty, 0) );
    }

    bool validate = _d->spec.checkHouse( this );
    if( !validate )
    {
      levelDown();
    }
    else
    {
      _d->condition4Up = "";
      if( _d->spec.next().checkHouse( this, &_d->condition4Up ) )
      {
         levelUp();
      }
    }

    int homelessCount = math::clamp( _d->habitants.count() - _d->maxHabitants, 0, 0xff );
    if( homelessCount > 0 )
    {
      CitizenGroup homeless = _d->habitants.retrieve( homelessCount );

      int workersFireCount = homeless.count( CitizenGroup::mature );
      if( workersFireCount > 0 )
      {
        events::GameEventPtr e = events::FireWorkers::create( getTilePos(), workersFireCount );
        e->dispatch();
      }

      Immigrant::send2City( _getCity(), homeless, getTile() );
    }
  }

  Building::timeStep( time );
}

GoodStore& House::getGoodStore()
{
   return _d->goodStore;
}


const HouseLevelSpec& House::getSpec() const
{
   return _d->spec;
}

void House::_tryUpdate_1_to_11_lvl( int level4grow, int startSmallPic, int startBigPic, const char desirability )
{
  CityHelper helper( _getCity() );

  if( getSize() == 1 )
  {
    Tilemap& tmap = _getCity()->getTilemap();
    TilesArray area = tmap.getArea( getTile().getIJ(), Size(2) );
    bool mayGrow = true;

    foreach( Tile* tile, area )
    {
      if( tile == NULL )
      {
        mayGrow = false;   //some broken, can't grow
        break;
      }

      HousePtr house = tile->getOverlay().as<House>();
      if( house != NULL && house->getSpec().getLevel() == level4grow )
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
      CitizenGroup sumHabitants = getHabitants();
      int sumFreeWorkers = getServiceValue( Service::recruter );
      TilesArray::iterator delIt=area.begin();
      HousePtr selfHouse = (*delIt)->getOverlay().as<House>();

      delIt++; //don't remove himself
      for( ; delIt != area.end(); delIt++ )
      {
        HousePtr house = (*delIt)->getOverlay().as<House>();
        if( house.isValid() )
        {
          house->deleteLater();
          house->_d->habitants.clear();

          sumHabitants += house->getHabitants();
          sumFreeWorkers += house->getServiceValue( Service::recruter );

          house->setServiceValue( Service::recruter, 0 );

          selfHouse->getGoodStore().storeAll( house->getGoodStore() );
        }
      }

      _d->habitants = sumHabitants;
      setServiceValue( Service::recruter, sumFreeWorkers );

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

  _d->spec = HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel);

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
  if( _d->houseLevel == smallHovel )
    return;

  _d->houseLevel--;
  _d->spec = HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel);

  switch (_d->houseLevel)
  {
  case 1:
  {
    _d->houseId = 1;
    _d->picIdOffset = ( rand() % 10 > 6 ? 1 : 0 );

    Tilemap& tmap = _getCity()->getTilemap();

    if( getSize().getArea() > 1 )
    {
      TilesArray perimetr = tmap.getArea( getTilePos(), Size(2) );
      int peoplesPerHouse = getHabitants().count() / 4;
      foreach( Tile* tile, perimetr )
      {
        HousePtr house = TileOverlayFactory::getInstance().create( building::house ).as<House>();
        house->_d->habitants = _d->habitants.retrieve( peoplesPerHouse );
        house->_d->houseId = smallHovel;
        house->_update();

        events::GameEventPtr event = events::BuildEvent::create( tile->getIJ(), house.as<TileOverlay>() );
        event->dispatch();
      }

      setServiceValue( Service::recruter, 0 );
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
    int houseSafeQty = _d->spec.computeMonthlyConsumption(*this, goodType, false )
                       + _d->spec.next().computeMonthlyConsumption(*this, goodType, false );
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

void House::appendServiceValue( Service::Type srvc, const int value)
{
  setServiceValue( srvc, getServiceValue( srvc ) + value );
}

void House::applyService( ServiceWalkerPtr walker )
{
  Building::applyService(walker);  // handles basic services, and remove service reservation

  Service::Type service = walker->getService();
  switch (service)
  {
  case Service::well:
  case Service::fontain:
  case Service::religionNeptune:
  case Service::religionCeres:
  case Service::religionVenus:
  case Service::religionMars:
  case Service::religionMercury:
  case Service::barber:
  case Service::baths:
  case Service::school:
  case Service::library:
  case Service::college:
  case Service::theater:
  case Service::amphitheater:
  case Service::colloseum:
  case Service::hippodrome:
    setServiceValue(service, 100);
  break;

  case Service::hospital:
  case Service::doctor:
    _d->healthLevel += 10;
    setServiceValue(service, 100);
  break;
  
  case Service::market:
    buyMarket(walker);
  break;
 
  case Service::senate:
  case Service::forum:
    setServiceValue(service, 100);
  break;

  case Service::prefect:
    appendServiceValue(Service::crime, -25);
  break;

  case Service::oracle:
  case Service::engineer:
  case Service::srvCount:
  break;

  case Service::recruter:
  {
    int svalue = getServiceValue( service );
    if( !svalue )
      break;

    RecruterPtr hunter = walker.as<Recruter>();
    if( hunter.isValid() )
    {
      int hiredWorkers = math::clamp( svalue, 0, hunter->getWorkersNeeded() );
      appendServiceValue( service, -hiredWorkers );
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
  case Service::engineer: res = getState( Construction::damage ); break;
  case Service::prefect: res = getState( Construction::fire ); break;

  // this house pays taxes
  case Service::forum:
  case Service::senate:
    res = _d->mayPayTax() ? _d->getAvailableTax() : 0.f;
  break;

  case Service::market:
  {
    MarketPtr market = walker->getBase().as<Market>();
    GoodStore &marketStore = market->getGoodStore();
    GoodStore &houseStore = getGoodStore();
    for (int i = 0; i < Good::goodCount; ++i)
    {
      Good::Type goodType = (Good::Type) i;
      int houseQty = houseStore.getCurrentQty(goodType) / 10;
      int houseSafeQty = _d->spec.computeMonthlyConsumption(*this, goodType, false)
                         + _d->spec.next().computeMonthlyConsumption(*this, goodType, false );
      int marketQty = marketStore.getCurrentQty(goodType);
      if( houseQty < houseSafeQty && marketQty > 0)
      {
         res += std::min( houseSafeQty - houseQty, marketQty);
      }
    }
  }
  break;

  case Service::recruter:
  {
    res = (float)getServiceValue( service );
  }
  break;   

  default:
  {
    return _d->spec.evaluateServiceNeed( this, service);
  }
  break;
  }

  // std::cout << "House evaluateService " << service << "=" << res << std::endl;

  return res;
}

bool House::hasServiceAccess( Service::Type service)
{
  return (_d->services[service] > 0);
}

int House::getServiceValue( Service::Type service)
{
  return _d->services[service];
}

void House::setServiceValue( Service::Type service, const int access)
{
  _d->services[service] = access;
}

TilesArray House::getEnterArea() const
{
  if( isWalkable() )
  {
    TilesArray ret;
    ret.push_back( &getTile() );
    return ret;
  }
  else
  {
    return Building::getEnterArea();
  }
}

int House::getMaxHabitants()
{
  return _d->maxHabitants;
}

void House::_update()
{
  int picId = ( _d->houseId == smallHovel && _d->habitants.count() == 0 ) ? 45 : (_d->houseId + _d->picIdOffset);
  Picture pic = Picture::load( ResourceGroup::housing, picId );
  setPicture( pic );
  setSize( Size( (pic.getWidth() + 2 ) / 60 ) );
  _d->maxHabitants = _d->spec.getMaxHabitantsByTile() * getSize().getArea();
  _d->initGoodStore( getSize().getArea() );
}

int House::getRoadAccessDistance() const
{
  return 2;
}

void House::addHabitants( CitizenGroup& habitants )
{
  int peoplesCount = math::clamp(  _d->maxHabitants - _d->habitants.count(), 0, _d->maxHabitants );
  CitizenGroup newHabitants = habitants.retrieve( peoplesCount );
  _d->habitants += newHabitants;
  _d->services[ Service::recruter ].setMax( _d->habitants.count( CitizenGroup::mature ) );
  _d->services[ Service::recruter ] += newHabitants.count( CitizenGroup::mature );
  _update();
}

const CitizenGroup& House::getHabitants() const
{
  return _d->habitants;
}

void House::destroy()
{
  _d->maxHabitants = 0;

  Immigrant::send2City( _getCity(), _d->habitants, getTile() );

  if( _d->services[ Service::recruter ] > 0 )
  {
    events::GameEventPtr e = events::FireWorkers::create( getTilePos(), getWorkersCount() );
    e->dispatch();
  }

  _d->habitants.clear();

  Building::destroy();
}

bool House::isWalkable() const
{
  return (_d->houseId == smallHovel && _d->habitants.count() == 0);
}

bool House::isFlat() const
{
  return isWalkable();
}

const MetaData::Desirability& House::getDesirabilityInfo() const
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
  stream[ "currentHubitants" ] = _d->habitants.save();
  stream[ "maxHubitants" ] = _d->maxHabitants;
  stream[ "goodstore" ] = _d->goodStore.save();
  stream[ "healthLevel" ] = _d->healthLevel;

  VariantList vl_services;
  foreach( Impl::Services::value_type& mapItem, _d->services )
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
  _d->spec = HouseSpecHelper::getInstance().getHouseLevelSpec(_d->houseLevel);

  _d->desirability.base = (int)stream.get( "desirability", 0 );
  _d->desirability.step = _d->desirability.base < 0 ? 1 : -1;

  _d->habitants.load( stream.get( "currentHubitants" ).toList() );
  _d->maxHabitants = (int)stream.get( "maxHubitants", 0 );

  _d->goodStore.load( stream.get( "goodstore" ).toMap() );

  _d->initGoodStore( getSize().getArea() );

  _d->services[ Service::recruter ].setMax( _d->habitants.count( CitizenGroup::mature ) );
  VariantList vl_services = stream.get( "services" ).toList();
  for( VariantList::iterator it = vl_services.begin(); it != vl_services.end(); it++ )
  {
    Service::Type type = Service::Type( (int)(*it) );
    it++;
    _d->services[ type ] = (*it).toInt(); //serviceValue
  }

  Building::build( _getCity(), getTilePos() );
  _update();
}

int House::getFoodLevel() const
{
  switch( _d->spec.getLevel() )
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

int House::getWorkersCount() const
{
  const Service& srvc = _d->services[ Service::recruter ];
  return srvc.getMax() - srvc.value();
}

bool House::isEducationNeed(Service::Type type) const
{
  int lvl = _d->spec.getMinEducationLevel();
  switch( type )
  {
  case Service::school: return (lvl>0);
  case Service::college: return (lvl>1);
  case Service::library: return (lvl>2);
  default: break;
  }

  return false;
}

bool House::isEntertainmentNeed(Service::Type type) const
{
  int lvl = _d->spec.getMinEntertainmentLevel();
  switch( type )
  {
  case Service::theater: return (lvl>=10);
  case Service::amphitheater: return (lvl>=30);
  case Service::colloseum: return (lvl>=60);
  case Service::hippodrome: return (lvl>=80);
  default: break;
  }

  return false;
}

float House::collectTaxes()
{
  if( _d->mayPayTax() )
  {
    _d->lastPayDate = GameDate::current();
    return _d->getAvailableTax();
  }

  return 0;
}

bool House::ready2Taxation() const
{
  return _d->mayPayTax();
}

std::string House::getUpCondition() const
{
  return _d->condition4Up;
}
