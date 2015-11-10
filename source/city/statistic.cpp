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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "statistic.hpp"
#include "objects/construction.hpp"
#include "trade_options.hpp"
#include "walker/workerhunter.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "objects/granary.hpp"
#include "objects/house_spec.hpp"
#include "good/store.hpp"
#include "gfx/tilemap.hpp"
#include "goods_updater.hpp"
#include "game/funds.hpp"
#include "core/common.hpp"
#include "cityservice_workershire.hpp"
#include "objects/farm.hpp"
#include "objects/religion.hpp"
#include "world/empire.hpp"
#include "objects/warehouse.hpp"
#include "cityservice_disorder.hpp"
#include "cityservice_military.hpp"
#include "core/time.hpp"
#include "objects/farm.hpp"
#include "cityservice_health.hpp"
#include "world/traderoute.hpp"
#include "core/logger.hpp"
#include "world/trading.hpp"
#include "city/states.hpp"
#include <map>

namespace city
{

namespace {
static const float popBalanceKoeff=1000.f;
static const float maxBalanceKoeff=2.f;
static const float normalBalanceKoeff=1.f;
static const float minBalanceKoeff=.5f;
}

float Statistic::_Balance::koeff() const
{ 
  float result = atan( _parent.rcity.states().population / popBalanceKoeff );
  return math::clamp(result, minBalanceKoeff, maxBalanceKoeff);
}

int Statistic::_Entertainment::coverage(Service::Type service) const
{
  int need = 0, have = 0;
  auto houses = _parent.rcity.statistic().houses.all();
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

HouseList Statistic::_Houses::ready4evolve(const object::TypeSet& checkTypes ) const
{
  HouseList houses = find();

  for( auto it=houses.begin(); it != houses.end(); )
  {
    object::Type btype;
    (*it)->spec().next().checkHouse( *it, nullptr, &btype );
    if( checkTypes.count( btype ) ) it = houses.erase( it );
    else ++it;
  }

  return houses;
}

HouseList Statistic::_Houses::ready4evolve( const object::Type checkType ) const
{
  object::TypeSet checkTypes;
  checkTypes.insert( checkType );
  return ready4evolve( checkTypes );
}

HouseList Statistic::_Houses::habitable() const
{
  HouseList houses = find();

  for( auto it=houses.begin(); it != houses.end(); )
  {
    if( (*it)->habitants().count() > 0 ) ++it;
    else it = houses.erase( it );
  }

  return houses;
}

HouseList Statistic::_Houses::patricians( bool habitabl ) const
{
  HouseList houses = habitabl ? habitable() : all();

  for( auto it=houses.begin(); it != houses.end(); )
  {
    if( !(*it)->spec().isPatrician() ) it = houses.erase( it );
    else ++it;
  }

  return houses;
}

#if _MSC_VER >= 1300
#define INIT_SUBSTAT(a) a({*this})
#else
#define INIT_SUBSTAT(a) a{*this}
#endif
Statistic::Statistic(PlayerCity& c)
    : INIT_SUBSTAT(walkers),
      INIT_SUBSTAT(objects),
      INIT_SUBSTAT(tax),
      INIT_SUBSTAT(workers),
      INIT_SUBSTAT(population),
      INIT_SUBSTAT(food),
      INIT_SUBSTAT(services),
      INIT_SUBSTAT(festival),
      INIT_SUBSTAT(crime),
      INIT_SUBSTAT(goods),
      INIT_SUBSTAT(health),
      INIT_SUBSTAT(military),
      INIT_SUBSTAT(map),
      INIT_SUBSTAT(houses),
      INIT_SUBSTAT(religion),
      INIT_SUBSTAT(entertainment),
      INIT_SUBSTAT(balance),
      rcity( c )
{

}

void Statistic::update(const unsigned long time)
{
   walkers.cached.clear();
}

unsigned int Statistic::_Crime::level() const
{
  DisorderPtr ds = _parent.services.find<Disorder>();
  return ds.isValid() ? ds->value() : 0;
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

int Statistic::_Walkers::count(walker::Type type, TilePos start, TilePos stop) const
{
  int result = 0;
  TilePos stopPos = stop;

  if( start == gfx::tilemap::invalidLocation() )
  {
    const WalkerList& all =_parent.rcity.walkers();
    result = utils::countByType( all, type );
  }
  else if( stopPos == gfx::tilemap::invalidLocation() )
  {
    const WalkerList& wlkOnTile = _parent.rcity.walkers( start );
    result = utils::countByType( wlkOnTile, type );
  }
  else
  {
    gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );
    for( auto tile : area)
    {
      const WalkerList& wlkOnTile = _parent.rcity.walkers( tile->pos() );
      result += utils::countByType( wlkOnTile, type );
    }
  }

  return result;
}

unsigned int Statistic::_Tax::possible() const
{
  HouseList houses = _parent.houses.find();

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

gfx::TilesArray Statistic::_Map::perimetr(const TilePos& lu, const TilePos& rb) const
{
  return _parent.rcity.tilemap().rect( lu, rb );
}

void Statistic::_Map::updateTilePics() const
{
  const gfx::TilesArray& tiles = _parent.rcity.tilemap().allTiles();
  for( auto tile : tiles)
  {
    tile->setPicture( tile->picture().name() );
  }
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

size_t Statistic::_Workers::need() const
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
  HouseList houses = _parent.houses.find();

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
  HouseList houseList = _parent.houses.find();

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

size_t Statistic::_Workers::available() const
{
  HouseList houses = _parent.houses.find();

  int workersNumber = 0;
  for( auto house : houses )
    workersNumber += house->habitants().mature_n();

  return workersNumber;
}

size_t Statistic::_Objects::count(object::Type type) const
{
  size_t ret=0;
  const OverlayList& buildings = _parent.rcity.overlays();
  for( auto bld : buildings )
  {
    if( bld.isValid() && bld->type() == type )
      ret++;
  }

  return ret;
}

OverlayList Statistic::_Objects::neighbors(OverlayPtr overlay, bool v) const
{
  if( overlay.isNull() )
    return OverlayList();

  Size size = overlay->size();
  TilePos start = overlay->pos() - gfx::tilemap::unitLocation();
  TilePos stop = start + TilePos( size.width(), size.height() );
  OverlayList ret;
  gfx::TilesArray tiles = _parent.rcity.tilemap().rect( start, stop );
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
  HouseList houses = _parent.houses.find();

  int worklessNumber = 0;
  for( auto house : houses ) { worklessNumber += house->unemployed(); }

  return worklessNumber;
}

HirePriorities Statistic::_Workers::hirePriorities() const
{
  WorkersHirePtr wh = _parent.services.find<WorkersHire>();
  return wh.isValid() ? wh->priorities() : HirePriorities();
}

size_t Statistic::_Food::inGranaries() const
{
  int foodSum = 0;

  GranaryList granaries = _parent.objects.find<Granary>( object::granery );
  for( auto gr : granaries ) { foodSum += gr->store().qty(); }

  return foodSum;
}

size_t Statistic::_Food::monthlyConsumption() const
{
  int foodComsumption = 0;
  HouseList houses = _parent.houses.find();

  for( auto house : houses )
    foodComsumption += house->spec().computeMonthlyFoodConsumption( house );

  return foodComsumption;
}

unsigned int Statistic::_Tax::payersPercent() const
{
  HouseList houses = _parent.houses.find();

  unsigned int registered = 0;
  unsigned int population = 0;
  for( auto house : houses )
  {
    unsigned int hbCount = house->habitants().count();
    population += hbCount;
    if( house->getServiceValue( Service::forum ) > minServiceValue )
    {
      registered += hbCount;
    }
  }

  return math::percentage( registered, population );
}

size_t Statistic::_Food::possibleProducing() const
{
  int foodProducing = 0;
  FarmList farms = _parent.objects.farms();

  for( auto farm : farms )
    foodProducing += farm->produceQty();

  return foodProducing;
}

unsigned int Statistic::_Festival::calcCost(FestivalType type) const
{
  unsigned int pop = _parent.rcity.states().population;
  switch( type )
  {
  case smallFest: return int( pop / smallFestivalCostLimiter ) + smallFestivalMinCost;
  case middleFest: return int( pop / middleFestivalCostLimiter ) + middleFestivalMinCost;
  case greatFest: return int( pop / greatFestivalCostLimiter ) + greatFestivalMinCost;
  }

  return 0;
}

good::ProductMap Statistic::_Goods::inCity() const { return details( true ); }
good::ProductMap Statistic::_Goods::inWarehouses() const { return details( false ); }

good::ProductMap Statistic::_Goods::details(bool includeGranary) const
{
  good::ProductMap cityGoodsAvailable;

  auto warehouses = _parent.objects.find<Warehouse>();

  for( auto warehouse : warehouses )
  {
    good::ProductMap whStore = warehouse->store().details();
    cityGoodsAvailable += whStore;
  }

  if( includeGranary )
  {
    auto granaries = _parent.objects.find<Granary>( object::any );

    for( auto granary : granaries )
    {
      good::ProductMap grStore = granary->store().details();
      cityGoodsAvailable += grStore;
    }
  }

  return cityGoodsAvailable;
}

int Statistic::_Objects::laborAccess(WorkingBuildingPtr wb) const
{
  if( wb.isNull() )
    return 0;

  TilePos offset( maxLaborDistance, maxLaborDistance );
  TilePos wbpos = wb->pos();
  HouseList houses = find<House>( object::house, wbpos - offset, wbpos + offset );
  float averageDistance = 0;
  for( auto house : houses )
  {
    if( house->level() > HouseLevel::vacantLot
        && house->level() < HouseLevel::smallVilla )
    {
      averageDistance += wbpos.distanceFrom( house->pos() );
    }
  }

  if( houses.size() > 0 )
    averageDistance /= houses.size();

  return math::clamp( math::percentage( averageDistance, maxLaborDistance ) * 2, 25, 100 );
}

unsigned int Statistic::_Health::value() const
{
  HealthCarePtr h = _parent.services.find<HealthCare>();
  return h.isValid() ? h->value() : 100;
}

int Statistic::_Military::months2lastAttack() const
{
  MilitaryPtr ml = _parent.services.find<Military>();
  return ml.isValid() ? ml->monthFromLastAttack() : 0;
}

bool Statistic::_Goods::canImport(good::Product type) const
{
  world::EmpirePtr empire = _parent.rcity.empire();
  world::TraderouteList routes = empire->troutes().from( _parent.rcity.name() );
  bool haveImportWay = false;
  for( auto route : routes )
  {
    world::CityPtr partner = route->partner( _parent.rcity.name() );
    const good::Store& gs = partner->sells();
    if( gs.capacity( type ) > 0 )
    {
      haveImportWay = true;
      break;
    }
  }

  return haveImportWay;
}

bool Statistic::_Goods::canProduce(good::Product type) const
{
  FactoryList buildings = _parent.objects.producers<Factory>( type );
  return !buildings.empty();
}

bool Statistic::_Goods::isRomeSend(good::Product type) const
{
  auto updaters = _parent.rcity.services().select<GoodsUpdater>();
  for( auto updater : updaters )
  {
    if( updater->goodType() == type )
    {
      return true;
    }
  }

  return false;
}

unsigned int Statistic::_Houses::terribleNumber() const
{
  unsigned int ret = 0;
  HouseList hs = find();
  if( _parent.rcity.states().population > pop4shacksCalc )
  {
    for( auto house : hs )
      ret += ( house->tile().param( gfx::Tile::pDesirability ) > minShacksDesirability ? 0 : 1);
  }

  return ret;
}

HouseList Statistic::_Houses::all() const
{
  return _parent.objects.find<House>();
}

HouseList Statistic::_Houses::find(std::set<int> levels) const
{
  HouseList houses = _parent.objects.find<House>();
  if( levels.empty() )
    return houses;

  HouseList ret;
  for( auto it : houses )
  {
    if( levels.count( it->level() ) > HouseLevel::vacantLot )
    {
      ret << it;
    }
  }

  return ret;
}

TempleList Statistic::_Religion::temples() const
{
  return _parent.objects.find<Temple>( object::group::religion );
}

TempleOracleList Statistic::_Religion::oracles() const
{
  return _parent.objects.find<TempleOracle>( object::oracle );
}

}//end namespace city
