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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "house.hpp"

#include "gfx/tile.hpp"
#include "objects/house_level.hpp"
#include "core/stringhelper.hpp"
#include "core/exception.hpp"
#include "walker/workerhunter.hpp"
#include "walker/immigrant.hpp"
#include "objects/market.hpp"
#include "objects/objects_factory.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "gfx/tilemap.hpp"
#include "game/gamedate.hpp"
#include "good/goodstore_simple.hpp"
#include "city/helper.hpp"
#include "core/foreach.hpp"
#include "constants.hpp"
#include "events/event.hpp"
#include "events/fireworkers.hpp"
#include "objects/desirability.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "city/funds.hpp"

using namespace constants;

namespace {
  enum { maxNegativeStep=-3, maxPositiveStep=3 };
}

class House::Impl
{
public:
  typedef std::map< Service::Type, Service > Services;
  int picIdOffset;
  int houseId;  // pictureId
  int houseLevel;
  float healthLevel;
  HouseLevelSpec spec;  // characteristics of the current house level
  Desirability desirability;
  SimpleGoodStore goodStore;
  Services services;  // value=access to the service (0=no access, 100=good access)
  int maxHabitants;
  DateTime taxCheckInterval;
  DateTime lastTaxationDate;
  std::string evolveInfo;
  CitizenGroup habitants;
  int currentYear;
  int changeCondition;

  void updateHealthLevel();
  void initGoodStore( int size );
  void consumeServices();
  void consumeGoods(HousePtr house);
  void consumeFoods(HousePtr house);
};

House::House(const int houseId) : Building( building::house ), _d( new Impl )
{
  _d->houseId = houseId;
  _d->taxCheckInterval = DateTime( -400, 1, 1 );
  _d->picIdOffset = ( rand() % 10 > 6 ? 1 : 0 );
  _d->healthLevel = 100;
  HouseSpecHelper& helper = HouseSpecHelper::getInstance();
  _d->houseLevel = helper.getHouseLevel( houseId );
  _d->spec = helper.getHouseLevelSpec( _d->houseLevel );
  setName( _d->spec.getLevelName() );
  _d->desirability.base = -3;
  _d->desirability.range = 3;
  _d->desirability.step = 1;
  _d->changeCondition = 0;
  _d->currentYear = GameDate::current().year();
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

void House::_makeOldHabitants()
{
  CitizenGroup newHabitants = _d->habitants;
  newHabitants.makeOld();
  newHabitants[ CitizenGroup::newborn ] = 0; //birth+health function from mature habitants count
  newHabitants[ CitizenGroup::longliver ] = 0; //death-health function from oldest habitants count

  _updateHabitants( newHabitants );
}

void House::_updateHabitants( const CitizenGroup& group )
{
  int deltaWorkersNumber = group.count( CitizenGroup::mature ) - _d->habitants.count( CitizenGroup::mature );

  _d->habitants = group;

  _d->services[ Service::recruter ].setMax( _d->habitants.count( CitizenGroup::mature ) );

  int firedWorkersNumber = _d->services[ Service::recruter ] + deltaWorkersNumber;
  _d->services[ Service::recruter ] += deltaWorkersNumber;

  if( firedWorkersNumber < 0 )
  {
    events::GameEventPtr e = events::FireWorkers::create( getTilePos(), abs( firedWorkersNumber ) );
    e->dispatch();
  }
}

void House::_checkEvolve()
{
  bool validate = _d->spec.checkHouse( this, &_d->evolveInfo );
  if( !validate )
  {
    _d->changeCondition--;
    if( _d->changeCondition <= maxNegativeStep )
    {
      _d->changeCondition = 0;
      levelDown();
    }
  }
  else
  {
    _d->evolveInfo = "";
    bool mayUpgrade =  _d->spec.next().checkHouse( this, &_d->evolveInfo );
    if( mayUpgrade )
    {
      _d->changeCondition++;
      if( _d->changeCondition >= maxPositiveStep )
      {
        _d->changeCondition = 0;
        levelUp();
      }
    }
    else
    {
      _d->changeCondition = 0;
    }
  }

  if( _d->changeCondition < 0 )
  {
    std::string why;
    _d->spec.checkHouse( this, &why );
    if( why.empty() )
    {
      why = why.substr( why.size() - 2 );
      why += "_degrade##";
    }
    else
    {
      why = "##house_willbe_degrade##";
    }

    _d->evolveInfo = why;
  }
  else if( _d->changeCondition > 0 )
  {
    _d->evolveInfo = _("##house_evolves_at##");
  }
}

void House::timeStep(const unsigned long time)
{
  if( _d->habitants.empty()  )
    return;

  if( _d->currentYear != GameDate::current().year() )
  {
    _d->currentYear = GameDate::current().year();
    _makeOldHabitants();    
  }

  if( time % getSpec().getServiceConsumptionInterval() == 0 )
  {
    _d->consumeServices();
    _d->updateHealthLevel();            
  }

  if( time % getSpec().getFoodConsumptionInterval() == 0 )
  {
    _d->consumeFoods( this );
  }

  if( time % getSpec().getGoodConsumptionInterval() == 0 )
  {
    _d->consumeGoods( this );
  }

  if( _d->taxCheckInterval.month() != GameDate::current().month() )
  {
    _d->taxCheckInterval = GameDate::current();
    float cityTax = _getCity()->getFunds().getTaxRate() / 100.f;
    appendServiceValue( Service::forum, (cityTax * _d->spec.getTaxRate() * _d->habitants.count( CitizenGroup::mature ) / 12.f) );

    _checkEvolve();    

    appendServiceValue( Service::crime, _d->spec.getCrime() + 2 );
    cancelService( Service::recruter );    

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

      Immigrant::send2city( _getCity(), homeless, getTile() );
    }
  }

  Building::timeStep( time );
}

void House::_tryEvolve_1_to_11_lvl( int level4grow, int startSmallPic, int startBigPic, const char desirability )
{
  CityHelper helper( _getCity() );

  if( getSize() == 1 )
  {
    Tilemap& tmap = _getCity()->getTilemap();
    TilesArray area = tmap.getArea( getTile().getIJ(), Size(2) );
    bool mayGrow = true;

    foreach( tile, area )
    {
      if( *tile == NULL )
      {
        mayGrow = false;   //some broken, can't grow
        break;
      }

      HousePtr house = (*tile)->getOverlay().as<House>();
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

      _d->initGoodStore( Size( getSize().getWidth() + 1 ).getArea() );

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

          house->_d->services[ Service::recruter ].setMax( 0 );

          selfHouse->getGoodStore().storeAll( house->getGoodStore() );
        }
      }

      _d->habitants = sumHabitants;
      setServiceValue( Service::recruter, sumFreeWorkers );

      //reset desirability level with old house size
      helper.updateDesirability( this, false );
      _d->houseId = startBigPic;
      _d->picIdOffset = 0;
      _update();

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

  case 2: _tryEvolve_1_to_11_lvl( 1, 1, 5, -3);
  break;
  
  case 3: _tryEvolve_1_to_11_lvl( 2, 3, 6, -3 );
  break;
  
  case 4: _tryEvolve_1_to_11_lvl( 3, 7, 11, -2 );
  break;
  
  case 5: _tryEvolve_1_to_11_lvl( 4, 9, 12, -2 );
  break;

  case 6: _tryEvolve_1_to_11_lvl( 5, 13, 17, -2 );
  break;

  case 7: _tryEvolve_1_to_11_lvl( 6, 15, 18, -2 );
  break;

  case 8: _tryEvolve_1_to_11_lvl( 7, 19, 23, -1 );
  break;

  case 9: _tryEvolve_1_to_11_lvl( 8, 21, 24, -1 );
  break;

  case 10: _tryEvolve_1_to_11_lvl( 9, 25, 29, 0 );
  break;

  case 11: _tryEvolve_1_to_11_lvl( 10, 27, 30, 0 );
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
      foreach( tile, perimetr )
      {
        HousePtr house = TileOverlayFactory::getInstance().create( building::house ).as<House>();
        house->_d->habitants = _d->habitants.retrieve( peoplesPerHouse );
        house->_d->houseId = smallHovel;
        house->_update();

        events::GameEventPtr event = events::BuildEvent::create( (*tile)->getIJ(), house.as<TileOverlay>() );
        event->dispatch();
      }

      _d->services[ Service::recruter ].setMax( 0 );
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
    int houseQty = houseStore.getQty(goodType);
    int houseSafeQty = _d->spec.computeMonthlyGoodConsumption( this, goodType, false )
                       + _d->spec.next().computeMonthlyGoodConsumption( this, goodType, false );
    houseSafeQty *= 6;

    int marketQty = marketStore.getQty(goodType);
    if( houseQty < houseSafeQty && marketQty > 0  )
    {
       int qty = std::min( houseSafeQty - houseQty, marketQty);
       qty = math::clamp( qty, 0, houseStore.getFreeQty( goodType ) );

       if( qty > 0 )
       {
         GoodStock stock(goodType, qty);
         marketStore.retrieve(stock, qty);

         stock.setCapacity( qty );
         stock.setQty( stock.capacity() );

         houseStore.store(stock, stock.qty() );
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
  case Service::religionNeptune:
  case Service::religionCeres:
  case Service::religionVenus:
  case Service::religionMars:
  case Service::religionMercury:
  case Service::barber:
  case Service::baths:
  case Service::school:
  case Service::library:
  case Service::academy:
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
    res = _d->services[ Service::forum ];
  break;

  case Service::market:
  {
    MarketPtr market = walker->getBase().as<Market>();
    GoodStore &marketStore = market->getGoodStore();
    GoodStore &houseStore = getGoodStore();
    for (int i = 0; i < Good::goodCount; ++i)
    {
      Good::Type goodType = (Good::Type) i;
      int houseQty = houseStore.getQty(goodType) / 10;
      int houseSafeQty = _d->spec.computeMonthlyGoodConsumption( this, goodType, false)
                         + _d->spec.next().computeMonthlyGoodConsumption( this, goodType, false );
      int marketQty = marketStore.getQty(goodType);
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

void House::_update()
{
  int picId = ( _d->houseId == smallHovel && _d->habitants.count() == 0 ) ? 45 : (_d->houseId + _d->picIdOffset);
  Picture pic = Picture::load( ResourceGroup::housing, picId );
  setPicture( pic );
  setSize( Size( (pic.getWidth() + 2 ) / 60 ) );
  _d->maxHabitants = _d->spec.getMaxHabitantsByTile() * getSize().getArea();
  _d->services[ Service::forum ].setMax( _d->spec.getTaxRate() * _d->maxHabitants );
  _d->initGoodStore( getSize().getArea() );
}

int House::getRoadAccessDistance() const
{
  return 2;
}

void House::addHabitants( CitizenGroup& habitants )
{
  int peoplesCount = math::clamp(  _d->maxHabitants - _d->habitants.count(), 0, _d->maxHabitants );
  CitizenGroup newState = _d->habitants;
  newState += habitants.retrieve( peoplesCount );

  _updateHabitants( newState );
  _update();
}

CitizenGroup House::remHabitants(int count)
{
  count = math::clamp<int>( count, 0, _d->habitants.count() );
  CitizenGroup hb = _d->habitants.retrieve( count );

  _updateHabitants( _d->habitants );

  return hb;
}

void House::destroy()
{
  _d->maxHabitants = 0;

  Immigrant::send2city( _getCity(), _d->habitants, getTile() );

  if( getWorkersCount() > 0 )
  {
    events::GameEventPtr e = events::FireWorkers::create( getTilePos(), getWorkersCount() );
    e->dispatch();
  }

  _d->habitants.clear();

  Building::destroy();
}

std::string House::getSound() const
{
  if( !_d->habitants.count() )
    return "";

  return StringHelper::format( 0xff, "house_%05d.wav", _d->houseLevel*10+1 );
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
  stream[ "changeCondition" ] = _d->changeCondition;

  VariantList vl_services;
  foreach( mapItem, _d->services )
  {
    vl_services.push_back( Variant( (int)mapItem->first) );
    vl_services.push_back( Variant( mapItem->second ) );
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
  _d->changeCondition = stream.get( "changeCondition", 0 );
  _d->goodStore.load( stream.get( "goodstore" ).toMap() );

  _d->initGoodStore( getSize().getArea() );

  _d->services[ Service::recruter ].setMax( _d->habitants.count( CitizenGroup::mature ) );
  VariantList vl_services = stream.get( "services" ).toList();
  for( VariantList::iterator it = vl_services.begin(); it != vl_services.end(); it++ )
  {
    Service::Type type = Service::Type( (int)(*it) );
    it++;
    _d->services[ type ] = (*it).toFloat(); //serviceValue
  }

  Building::build( _getCity(), getTilePos() );
  _update();
}

int House::getFoodLevel() const
{
  const Good::Type f[] = { Good::wheat, Good::fish, Good::meat, Good::fruit, Good::vegetable };
  std::set<Good::Type> foods( f, f+5 );

  int ret = 0;
  int foodLevel = getSpec().getMinFoodLevel();
  if( foodLevel == 0 )
    return 0;

  while( foodLevel > 0 )
  {
    Good::Type maxFtype = Good::none;
    int maxFoodQty = 0;
    foreach( ft, foods )
    {
      int tmpQty = _d->goodStore.getQty( *ft );
      if( tmpQty > maxFoodQty )
      {
        maxFoodQty = tmpQty;
        maxFtype = *ft;
      }
    }

    ret += maxFoodQty * 100 / _d->goodStore.capacity( maxFtype );
    foods.erase( maxFtype );
    foodLevel--;
  }

  ret /= getSpec().getMinFoodLevel();
  return ret;
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
  case Service::academy: return (lvl>1);
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
  float tax = getServiceValue( Service::forum );
  setServiceValue( Service::forum, 0 );
  _d->lastTaxationDate = GameDate::current();
  return tax;
}

int House::getHealthLevel() const{  return _d->healthLevel;}
DateTime House::getLastTaxation() const{  return _d->lastTaxationDate;}
std::string House::getEvolveInfo() const{  return _d->evolveInfo;}
Desirability House::getDesirability() const {  return _d->desirability; }
bool House::isWalkable() const{  return (_d->houseId == smallHovel && _d->habitants.count() == 0); }
bool House::isFlat() const {   return isWalkable(); }
const CitizenGroup& House::getHabitants() const  {  return _d->habitants; }
GoodStore& House::getGoodStore(){   return _d->goodStore;}
const HouseLevelSpec& House::getSpec() const{   return _d->spec; }
bool House::hasServiceAccess( Service::Type service) {  return (_d->services[service] > 0); }
float House::getServiceValue( Service::Type service){  return _d->services[service]; }
void House::setServiceValue( Service::Type service, float value) {  _d->services[service] = value; }
int House::getMaxHabitants() {  return _d->maxHabitants; }
void House::appendServiceValue( Service::Type srvc, float value){  setServiceValue( srvc, getServiceValue( srvc ) + value ); }


void House::Impl::updateHealthLevel()
{
  float delim = 1 + (((services[Service::well] > 0 || services[Service::fontain] > 0) ? 1 : 0))
      + ((services[Service::doctor] > 0 || services[Service::hospital] > 0) ? 1 : 0)
      + (services[Service::baths] > 0 ? 0.7 : 0)
      + (services[Service::barber] > 0 ? 0.3 : 0);

  float decrease = 0.3f / delim;

  healthLevel = math::clamp<float>( healthLevel - decrease, 0, 100 );
}

void House::Impl::initGoodStore(int size)
{
  int rsize = 25 * size * houseLevel;
  goodStore.setCapacity( rsize * 10 );  // no limit
  goodStore.setCapacity(Good::wheat, rsize );
  goodStore.setCapacity(Good::fish, rsize );
  goodStore.setCapacity(Good::meat, rsize );
  goodStore.setCapacity(Good::fruit, rsize );
  goodStore.setCapacity(Good::vegetable, rsize );
  goodStore.setCapacity(Good::pottery, rsize );
  goodStore.setCapacity(Good::furniture, rsize);
  goodStore.setCapacity(Good::oil, rsize );
  goodStore.setCapacity(Good::wine, rsize );
}

void House::Impl::consumeServices()
{
  int currentWorkersPower = services[ Service::recruter ];       //save available workers number
  float tax = services[ Service::forum ];

  foreach( s, services ) { s->second -= 1; } //consume services

  services[ Service::recruter ] = currentWorkersPower;     //restore available workers number
  services[ Service::forum ] = tax;
}

void House::Impl::consumeGoods( HousePtr house )
{
  for( int i = Good::olive; i < Good::goodCount; ++i)
  {
     Good::Type goodType = (Good::Type) i;
     int montlyGoodsQty = spec.computeMonthlyGoodConsumption( house, goodType, true );
     goodStore.setQty( goodType, std::max( goodStore.getQty(goodType) - montlyGoodsQty, 0) );
  }
}

void House::Impl::consumeFoods(HousePtr house)
{
  int consumeQty = spec.computeMonthlyFoodConsumption( house ) * spec.getGoodConsumptionInterval() / GameDate::getTickInMonth();

  int foodLevel = spec.getMinFoodLevel();
  int tmpConsumeQty = 0;

  // consume foods
  if( foodLevel > 2 )
  {
    tmpConsumeQty = consumeQty / 3;

    int fQty = goodStore.getQty( Good::fruit );
    int vQty = goodStore.getQty( Good::vegetable );

    Good::Type maxType = ( fQty > vQty ) ? Good::fruit : Good::vegetable;
    Good::Type minType = ( fQty > vQty ) ? Good::vegetable : Good::fruit;

    vQty = std::min( goodStore.getQty( minType ), tmpConsumeQty / 2 );
    goodStore.setQty( minType, std::max( goodStore.getQty( minType ) - vQty, 0) );

    fQty = ( tmpConsumeQty - vQty );
    goodStore.setQty( maxType, std::max( goodStore.getQty( maxType ) - fQty, 0) );
  }

  if( foodLevel > 1 )
  {
    tmpConsumeQty = (0 == tmpConsumeQty ? consumeQty / 2 : tmpConsumeQty);

    int mQty = std::min( goodStore.getQty( Good::meat ), tmpConsumeQty / 2 );
    int fQty = std::min( goodStore.getQty( Good::fish ), tmpConsumeQty / 2 );

    Good::Type maxType = ( fQty > mQty ) ? Good::fish : Good::meat;
    Good::Type minType = ( fQty > mQty ) ? Good::meat : Good::fish;

    mQty = std::min( goodStore.getQty( minType ), tmpConsumeQty / 2 );
    goodStore.setQty( minType, std::max( goodStore.getQty( minType ) - mQty, 0) );

    fQty = ( tmpConsumeQty - mQty );
    goodStore.setQty( maxType, std::max( goodStore.getQty( maxType ) - fQty, 0) );
  }

  if( foodLevel > 0 )
  {
    tmpConsumeQty = (0 == tmpConsumeQty ? consumeQty : tmpConsumeQty);

    int wQty = std::min( goodStore.getQty( Good::wheat ), tmpConsumeQty );
    goodStore.setQty( Good::wheat, std::max( goodStore.getQty(Good::wheat) - wQty, 0) );
  }
}
