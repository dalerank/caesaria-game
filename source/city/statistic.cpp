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
#include "game/funds.hpp"
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
#include "city/states.hpp"
#include <map>

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

float getBalanceKoeff(PlayerCityPtr city)
{ 
  if( city.isNull() )
  {
    Logger::warning( "Statistic::getBalanceKoeff cityptr is null");
    return normalBalanceKoeff;
  }

  float result = atan( city->states().population / popBalanceKoeff );
  return math::clamp(result, minBalanceKoeff, maxBalanceKoeff);
}

int getEntertainmentCoverage(PlayerCityPtr city, Service::Type service)
{
  int need = 0, have = 0;
  HouseList houses = city->statistic().objects.houses();
  for( auto house : houses )
  {
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
  for( auto route : routes )
  {
    world::CityPtr partner = route->partner( city->name() );
    const good::Store& goods = partner->sells();
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

unsigned int getCrimeLevel( PlayerCityPtr city )
{
  DisorderPtr ds = getService<Disorder>( city );
  return ds.isValid() ? ds->value() : 0;
}

unsigned int blackHouses( PlayerCityPtr city )
{
  unsigned int ret = 0;
  HouseList houses = city->statistic().objects.houses();
  if( city->states().population > pop4blackHouseCalc )
  {
    for( auto house : houses )
      ret += ( house->tile().param( gfx::Tile::pDesirability ) > minBlackHouseDesirability ? 0 : 1);
  }

  return ret;
}

unsigned int getFoodStock(PlayerCityPtr city)
{
  int foodSum = 0;

  GranaryList granaries = city->statistic().objects.find<Granary>( object::granery );
  for( auto gr : granaries ) { foodSum += gr->store().qty(); }

  return foodSum;
}

unsigned int getFoodMonthlyConsumption(PlayerCityPtr city)
{
  int foodComsumption = 0;
  HouseList houses = city->statistic().objects.houses();

  for( auto house : houses )
    foodComsumption += house->spec().computeMonthlyFoodConsumption( house );

  return foodComsumption;
}

unsigned int getFoodProducing(PlayerCityPtr city)
{
  int foodProducing = 0;
  FarmList farms = city->statistic().objects.find<Farm>( object::group::food );

  for( auto farm : farms )
    foodProducing += farm->produceQty();

  return foodProducing;
}

unsigned int getTaxPayersPercent(PlayerCityPtr city)
{
  HouseList houses = city->statistic().objects.houses();

  unsigned int registered = 0;
  unsigned int population = 0;
  for( auto house : houses )
  {
    unsigned int hbCount = house->habitants().count();
    population += hbCount;
    if( house->getServiceValue( Service::forum ) > minServiceValue4Tax )
    {
      registered += hbCount;
    }
  }

  return math::percentage( registered, population );
}

unsigned int getHealth(PlayerCityPtr city)
{
  HealthCarePtr hc = getService<HealthCare>( city );
  return hc.isValid() ? hc->value() : 0;
}

int months2lastAttack(PlayerCityPtr city)
{
  MilitaryPtr ml = getService<Military>( city );
  return ml.isValid() ? ml->monthFromLastAttack() : 0;
}

unsigned int getFestivalCost(PlayerCityPtr city, FestivalType type)
{
  unsigned int pop = city->states().population;
  switch( type )
  {
  case smallFest: return int( pop / smallFestivalCostLimiter ) + smallFestivalMinCost;
  case middleFest: return int( pop / middleFestivalCostLimiter ) + middleFestivalMinCost;
  case greatFest: return int( pop / greatFestivalCostLimiter ) + greatFestivalMinCost;
  }

  return 0;
}

HouseList getEvolveHouseReadyBy(PlayerCityPtr city, const object::TypeSet& checkTypes )
{
  HouseList ret;

  HouseList houses = city->statistic().objects.houses();

  for( auto it : houses )
  {
    object::Type btype;
    it->spec().next().checkHouse( it, NULL, &btype );
    if( checkTypes.count( btype ) )
    {    
      ret.push_back( it );
    }
  }

  return ret;
}

good::ProductMap getProductMap(PlayerCityPtr city, bool includeGranary)
{
  good::ProductMap cityGoodsAvailable;

  WarehouseList warehouses = city->statistic().objects.find<Warehouse>( object::any );

  for( auto wh : warehouses )
  {
    good::ProductMap whStore = wh->store().details();
    cityGoodsAvailable += whStore;
  }

  if( includeGranary )
  {
    GranaryList granaries = city->statistic().objects.find<Granary>( object::any );

    for( auto gg : granaries )
    {
      good::ProductMap grStore = gg->store().details();
      cityGoodsAvailable += grStore;
    }
  }

  return cityGoodsAvailable;
}

int getLaborAccessValue(PlayerCityPtr city, WorkingBuildingPtr wb)
{
  TilePos offset( maxLaborDistance, maxLaborDistance );
  TilePos wbpos = wb->pos();
  HouseList houses = getObjects<House>( city, object::house, wbpos - offset, wbpos + offset );
  float averageDistance = 0;
  for( auto it : houses )
  {
    if( it->spec().level() < HouseLevel::smallVilla )
    {
      averageDistance += wbpos.distanceFrom( it->pos() );
    }
  }

  if( houses.size() > 0 )
    averageDistance /= houses.size();

  return math::clamp( math::percentage( averageDistance, maxLaborDistance ) * 2, 25, 100 );
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

Statistic::Statistic(PlayerCity& c)
  : walkers{ *this },
    objects{ *this },
    tax{ *this },
    workers{ *this },
    population{ *this },
    rcity( c )
{

}

void Statistic::update(const unsigned long time)
{
   walkers.cached.clear();
}

const WalkerList& Statistic::_Walkers::find(walker::Type type) const
{
  if( type == walker::all )
  {
    return _parent.rcity.walkers();
  }

  if( cached.count( type ) > 0 )
    return cached.at( type );

  WalkerList& wl = const_cast<_Walkers*>( this )->cached[ type ];
  for( auto w : _parent.rcity.walkers() )
  {
    if( w->type() == type )
      wl.push_back(w);
  }

  return wl;
}

unsigned int Statistic::_Tax::value() const
{
  HouseList houses = _parent.objects.houses();

  float taxValue = 0.f;
  float taxRate = _parent.rcity.treasury().taxRate();

  for( auto house : houses )
  {
    int maxhb = house->capacity();
    if( maxhb == 0 )
      continue;

    int maturehb = house->habitants().mature_n();
    int housetax = house->spec().taxRate();
    taxValue += housetax * maturehb * taxRate / maxhb;
  }

  return taxValue;
}

HouseList Statistic::_Objects::houses(std::set<int> levels) const
{
  const HouseList& houses = find<House>( object::house );
  if( levels.empty() )
    return houses;

  HouseList ret;
  for( auto it : houses )
  {
    if( levels.count( it->spec().level() ) > 0 )
    {
      ret << it;
    }
  }

  return ret;
}

Statistic::WorkersInfo Statistic::_Workers::details() const
{
  WorkersInfo ret;

  WorkingBuildingList buildings = _parent.objects.find<WorkingBuilding>( object::any );

  ret.current = 0;
  ret.need = 0;
  for( auto bld : buildings )
  {
    ret.current += bld->numberWorkers();
    ret.need += bld->maximumWorkers();
  }

  return ret;
}

unsigned int Statistic::_Workers::need() const
{
  WorkersInfo wInfo = details();
  return wInfo.need < wInfo.current ? 0 : wInfo.need - wInfo.current;
}

int Statistic::_Workers::wagesDiff() const
{
  return _parent.rcity.treasury().workerSalary() - _parent.rcity.empire()->workerSalary();
}

CitizenGroup Statistic::_Population::details() const
{
  HouseList houses = _parent.objects.houses();

  CitizenGroup ret;
  for( auto house : houses ) { ret += house->habitants(); }

  return ret;
}

FarmList Statistic::_Objects::farms( std::set<object::Type> which) const
{
  FarmList ret;
  FarmList farms = find<Farm>( object::group::food );

  for( auto it : farms )
  {
    if( which.count( it->type() ) > 0 )
    {
      ret << it;
    }
  }

  return ret;
}

unsigned int Statistic::_Population::current() const
{
  unsigned int pop = 0;
  HouseList houseList = _parent.objects.houses();

  for (auto house : houseList)
  {
    pop += house->habitants().count();
  }

  return pop;
}

unsigned int Statistic::_Workers::monthlyWages() const
{
  Statistic::WorkersInfo wInfo = details();

  if( wInfo.current == 0 )
    return 0;

  //wages all worker in year
  //workers take salary in sestertius 1/100 part of dinarius
  int wages = wInfo.current * monthlyOneWorkerWages();

  return std::max<int>( wages, 1 );
}

float Statistic::_Workers::monthlyOneWorkerWages() const
{
  return _parent.rcity.treasury().workerSalary() / (10.f * DateTime::monthsInYear);
}

unsigned int Statistic::_Workers::available() const
{
  HouseList houses = _parent.objects.houses();

  int workersNumber = 0;
  for( auto house : houses )
    workersNumber += house->habitants().mature_n();

  return workersNumber;
}

OverlayList Statistic::_Objects::neighbors(OverlayPtr overlay, bool v) const
{
  if( overlay.isNull() )
    return OverlayList();

  Size size = overlay->size();
  TilePos start = overlay->pos();
  TilePos stop = start + TilePos( 2, 2 ) + TilePos( size.width(), size.height() );
  OverlayList ret;
  gfx::TilesArray tiles = _parent.rcity.tilemap().getRectangle( start, stop );
  std::set<OverlayPtr> checked;
  for( auto tile : tiles )
  {
    OverlayPtr ov = tile->overlay();
    if( ov.isValid() && checked.count( ov ) == 0 )
    {
      checked.insert( ov );
      ret.push_back( ov );
    }
  }

  return ret;
}

unsigned int Statistic::_Workers::worklessPercent() const
{
  return math::percentage( workless(), available() );
}

unsigned int Statistic::_Workers::workless() const
{
  HouseList houses = _parent.objects.houses();

  int worklessNumber = 0;
  for( auto house : houses ) { worklessNumber += house->unemployed(); }

  return worklessNumber;
}

}//end namespace city
