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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "statistic.hpp"
#include "objects/construction.hpp"
#include "helper.hpp"
#include "trade_options.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "objects/granary.hpp"
#include "objects/house_spec.hpp"
#include "good/store.hpp"
#include "city/funds.hpp"
#include "objects/farm.hpp"
#include "world/empire.hpp"
#include "objects/warehouse.hpp"
#include "cityservice_disorder.hpp"
#include "cityservice_military.hpp"
#include "core/time.hpp"
#include "objects/farm.hpp"
#include "cityservice_health.hpp"
#include "world/traderoute.hpp"
#include "core/logger.hpp"
#include <map>

using namespace constants;

namespace city
{

namespace statistic
{

namespace {
static const float popBalanceKoeff=1000.f;
static const int   pop4blackHouseCalc=300;
static const int   minServiceValue4Tax=25;
static const int   greatFestivalCostLimiter=5;
static const int   middleFestivalCostLimiter=10;
static const int   smallFestivalCostLimiter=20;
static const int   greatFestivalMinCost=40;
static const int   middleFestivalMinCost=20;
static const int   smallFestivalMinCost=10;
static const int   minBlackHouseDesirability =-10;
static const float maxBalanceKoeff=2.f;
static const float normalBalanceKoeff=1.f;
static const float minBalanceKoeff=.5f;
static const int   maxLaborDistance=8;
}

void getWorkersNumber(PlayerCityPtr city, int& workersNumber, int& maxWorkers )
{
  WorkingBuildingList buildings;
  buildings << city->overlays();

  workersNumber = 0;
  maxWorkers = 0;
  foreach( bld, buildings )
  {
    workersNumber += (*bld)->numberWorkers();
    maxWorkers += (*bld)->maximumWorkers();
  }
}

float getBalanceKoeff(PlayerCityPtr city)
{ 
  if( city.isNull() )
  {
    Logger::warning( "Statistic::getBalanceKoeff cityptr is null");
    return normalBalanceKoeff;
  }

  float result = atan( city->population() / popBalanceKoeff );
  return math::clamp(result, minBalanceKoeff, maxBalanceKoeff);
}

int getEntertainmentCoverage(PlayerCityPtr city, Service::Type service)
{
  int need = 0, have = 0;
  HouseList houses = findh( city );
  foreach( it, houses )
  {
    HousePtr house = *it;
    if( house->isEntertainmentNeed( service ) )
    {
      int habitants = house->habitants().count();
      need += habitants;
      have += (house->hasServiceAccess( service) ? habitants : 0);
    }
  }

  return ( have == 0
            ? 0
            : math::percentage( need, have) );
}

bool canImport(PlayerCityPtr city, good::Product type)
{
  world::EmpirePtr empire = city->empire();
  world::TraderouteList routes = empire->tradeRoutes( city->name() );
  bool haveImportWay = false;
  foreach( it, routes )
  {
    world::CityPtr partner = (*it)->partner( city->name() );
    const good::Store& goods = partner->exportingGoods();
    if( goods.capacity( type ) > 0 )
    {
      haveImportWay = true;
      break;
    }
  }

  return haveImportWay;
}

bool canProduce(PlayerCityPtr city, good::Product type)
{
  Helper helper( city );

  FactoryList buildings = helper.findProducers<Factory>( type );
  return !buildings.empty();
}

CitizenGroup getPopulation(PlayerCityPtr city)
{
  HouseList houses = city::statistic::findh( city );

  CitizenGroup ret;
  foreach( it, houses ) { ret += (*it)->habitants(); }

  return ret;
}

unsigned int getWorkersNeed(PlayerCityPtr city)
{
  int have, need;
  getWorkersNumber( city, have, need );
  return need < have ? 0 : need - have;
}

unsigned int getAvailableWorkersNumber(PlayerCityPtr city)
{
  HouseList houses = findh( city );

  int workersNumber = 0;
  foreach( h, houses )
  {
    workersNumber += (*h)->habitants().mature_n();
  }

  return workersNumber;
}

unsigned int getMonthlyWorkersWages(PlayerCityPtr city)
{
  int workersNumber, maxWorkers;
  getWorkersNumber( city, workersNumber, maxWorkers );

  if( workersNumber == 0 )
    return 0;

  //wages all worker in year
  //workers take salary in sestertius 1/100 part of dinarius
  int wages = workersNumber * getMonthlyOneWorkerWages( city );

  wages = std::max<int>( wages, 1 );

  return wages;
}

float getMonthlyOneWorkerWages(PlayerCityPtr city)
{
  return city->funds().workerSalary() / (10.f * DateTime::monthsInYear);
}

unsigned int getWorklessNumber(PlayerCityPtr city)
{
  HouseList houses = findh( city );

  int worklessNumber = 0;
  foreach( h, houses ) { worklessNumber += (*h)->unemployed(); }

  return worklessNumber;
}

unsigned int getWorklessPercent(PlayerCityPtr city)
{
  return math::percentage( getWorklessNumber( city ), getAvailableWorkersNumber( city ) );
}

unsigned int getCrimeLevel( PlayerCityPtr city )
{
  DisorderPtr ds;
  ds << city->findService( Disorder::defaultName() );

  return ds.isValid() ? ds->value() : 0;
}

unsigned int blackHouses( PlayerCityPtr city )
{
  unsigned int ret = 0;
  HouseList houses = findh( city );
  if( city->population() > pop4blackHouseCalc )
  {
    foreach( h, houses )
      ret += ((*h)->tile().param( gfx::Tile::pDesirability ) > minBlackHouseDesirability ? 0 : 1);
  }

  return ret;
}

unsigned int getFoodStock(PlayerCityPtr city)
{
  int foodSum = 0;

  GranaryList granaries = findo<Granary>( city, object::granery );
  foreach( gr, granaries ) { foodSum += (*gr)->store().qty(); }

  return foodSum;
}

unsigned int getFoodMonthlyConsumption(PlayerCityPtr city)
{
  int foodComsumption = 0;
  HouseList houses = findh( city );

  foreach( h, houses ) { foodComsumption += (*h)->spec().computeMonthlyFoodConsumption( *h ); }

  return foodComsumption;
}

unsigned int getFoodProducing(PlayerCityPtr city)
{
  int foodProducing = 0;
  FarmList farms = findo<Farm>( city, object::group::food );

  foreach( f, farms ) { foodProducing += (*f)->produceQty(); }

  return foodProducing;
}

unsigned int getTaxValue(PlayerCityPtr city)
{
  HouseList houses = findh( city  );

  float taxValue = 0.f;
  float taxRate = city->funds().taxRate();
  foreach( house, houses )
  {
    int maxhb = (*house)->maxHabitants();
    if( maxhb == 0 )
      continue;

    int maturehb = (*house)->habitants().mature_n();
    int housetax = (*house)->spec().taxRate();
    taxValue += housetax * maturehb * taxRate / maxhb;
  }

  return taxValue;
}

unsigned int getTaxPayersPercent(PlayerCityPtr city)
{
  HouseList houses = findh( city );

  unsigned int registered = 0;
  unsigned int population = 0;
  foreach( house, houses )
  {
    unsigned int hbCount = (*house)->habitants().count();
    population += hbCount;
    if( (*house)->getServiceValue( Service::forum ) > minServiceValue4Tax )
    {
      registered += hbCount;
    }
  }

  return math::percentage( registered, population );
}

unsigned int getHealth(PlayerCityPtr city)
{
  HealthCarePtr hc;
  hc << city->findService( HealthCare::defaultName() );
  return hc.isValid() ? hc->value() : 0;
}

int months2lastAttack(PlayerCityPtr city)
{
  MilitaryPtr ml;
  ml << city->findService( Military::defaultName() );
  return ml.isValid() ? ml->monthFromLastAttack() : 0;
}

int getWagesDiff(PlayerCityPtr city)
{
  return city->funds().workerSalary() - city->empire()->workerSalary();
}

unsigned int getFestivalCost(PlayerCityPtr city, FestivalType type)
{
  switch( type )
  {
  case smallFest: return int( city->population() / smallFestivalCostLimiter ) + smallFestivalMinCost;
  case middleFest: return int( city->population() / middleFestivalCostLimiter ) + middleFestivalMinCost;
  case greatFest: return int( city->population() / greatFestivalCostLimiter ) + greatFestivalMinCost;
  }

  return 0;
}

HouseList getEvolveHouseReadyBy(PlayerCityPtr city, const object::TypeSet& checkTypes )
{
  HouseList ret;

  HouseList houses = findh( city );

  foreach( it, houses )
  {
    object::Type btype;
    (*it)->spec().next().checkHouse( *it, NULL, &btype );
    if( checkTypes.count( btype ) )
    {    
      ret.push_back( *it );      
    }
  }

  return ret;
}

GoodsMap getGoodsMap(PlayerCityPtr city, bool includeGranary)
{
  GoodsMap cityGoodsAvailable;

  WarehouseList warehouses;
  warehouses << city->overlays();

  foreach( wh, warehouses )
  {
    foreach( goodType, good::all() )
    {
      cityGoodsAvailable[ *goodType ] += (*wh)->store().qty( *goodType );
    }
  }

  if( includeGranary )
  {
    GranaryList granaries;
    granaries << city->overlays();

    foreach( gg, granaries )
    {
      for( good::Product goodType=good::wheat; goodType <= good::vegetable; ++goodType )
      {
        cityGoodsAvailable[ goodType ] += (*gg)->store().qty( goodType );
      }
    }
  }

  return cityGoodsAvailable;
}

int getLaborAccessValue(PlayerCityPtr city, WorkingBuildingPtr wb)
{
  TilePos offset( maxLaborDistance, maxLaborDistance );
  TilePos wbpos = wb->pos();
  HouseList houses = findo<House>( city, object::house, wbpos - offset, wbpos + offset );
  float averageDistance = 0;
  foreach( it, houses )
  {
    if( (*it)->spec().level() < HouseLevel::smallVilla )
    {
      averageDistance += wbpos.distanceFrom( (*it)->pos() );
    }
  }

  if( houses.size() > 0 )
    averageDistance /= houses.size();

  return math::clamp( math::percentage( averageDistance, maxLaborDistance ) * 2, 25, 100 );
}

HouseList findh(PlayerCityPtr city, std::set<int> levels )
{
  HouseList ret;
  HouseList houses = findo<House>( city, object::house );
  if( levels.empty() )
    return houses;

  foreach( it, houses )
  {
    if( levels.count( (*it)->spec().level() ) > 0 )
    {
      ret << *it;
    }
  }

  return ret;
}

gfx::TilesArray tiles( PlayerCityPtr r, const TilePos &start, const TilePos &stop)
{
  return r->tilemap().getArea( start, stop );
}

FarmList findfarms(PlayerCityPtr r, std::set<object::Type> which)
{
  FarmList ret;
  FarmList farms = findo<Farm>( r, object::group::food );

  foreach( it, farms )
  {
    if( which.count( (*it)->type() ) > 0 )
    {
      ret << *it;
    }
  }

  return ret;
}

int taxValue(unsigned int population, int koeff)
{
  return population / 1000 * koeff;
}

HouseList getEvolveHouseReadyBy(PlayerCityPtr r, const object::Type checkType )
{
  object::TypeSet checkTypes;
  checkTypes.insert( checkType );
  return getEvolveHouseReadyBy( r, checkTypes );
}

}//end namespace statistic

}//end namespace city
