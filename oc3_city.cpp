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


#include "oc3_city.hpp"

#include "oc3_tile.hpp"
#include "oc3_building_data.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_safetycast.hpp"
#include "oc3_cityservice_emigrant.hpp"
#include "oc3_cityservice_workershire.hpp"
#include "oc3_cityservice_timers.hpp"
#include "oc3_cityservice_prosperity.hpp"
#include "oc3_cityservice_shoreline.hpp"
#include "oc3_cityservice_info.hpp"
#include "oc3_cityservice_animals.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_road.hpp"
#include "oc3_time.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_walkermanager.hpp"
#include "oc3_gettext.hpp"
#include "oc3_city_build_options.hpp"
#include "oc3_building_house.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_building_forum.hpp"
#include "oc3_building_senate.hpp"
#include "oc3_cityservice_culture.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_empire_city.hpp"
#include "oc3_empire.hpp"
#include "oc3_city_trade_options.hpp"
#include "oc3_goodstore_simple.hpp"
#include "oc3_empire_trading.hpp"
#include "oc3_walker_merchant.hpp"
#include "oc3_gamedate.hpp"
#include "oc3_cityservice_religion.hpp"
#include "oc3_foreach.hpp"
#include "oc3_game_event.hpp"
#include "oc3_cityservice_festival.hpp"
#include "oc3_win_targets.hpp"
#include "oc3_cityservice_roads.hpp"

#include <set>

typedef std::vector< CityServicePtr > CityServices;

class City::Impl
{
public:
  int lastMonthCount;
  int population;
  CityFunds funds;  // amount of money
  std::string name;
  EmpirePtr empire;
  PlayerPtr player;

  LandOverlayList overlayList;
  WalkerList walkerList;
  CityServices services;
  bool needRecomputeAllRoads;
  int lastMonthTax;
  int lastMonthTaxpayer;
  BorderInfo borderInfo;
  Tilemap tilemap;
  TilePos cameraStart;
  Point location;
  CityBuildOptions buildOptions;
  CityTradeOptions tradeOptions;
  CityWinTargets targets;

  ClimateType climate;   
  UniqueId walkerIdCount;

  // collect taxes from all houses
  void collectTaxes( CityPtr city);
  void payWages( CityPtr city );
  void calculatePopulation( CityPtr city );
  void beforeOverlayDestroyed(CityPtr city, LandOverlayPtr overlay );
  void returnFiredWorkers( WorkingBuildingPtr building );
  void fireWorkers(HousePtr house );

oc3_signals public:
  Signal1<int> onPopulationChangedSignal;
  Signal1<std::string> onWarningMessageSignal;
  Signal2<const TilePos&, const std::string&> onDisasterEventSignal;
};

City::City() : _d( new Impl )
{
  _d->borderInfo.roadEntry = TilePos( 0, 0 );
  _d->borderInfo.roadExit = TilePos( 0, 0 );
  _d->borderInfo.boatEntry = TilePos( 0, 0 );
  _d->borderInfo.boatExit = TilePos( 0, 0 );
  _d->funds.resolveIssue( FundIssue( CityFunds::donation, 1000 ) );
  _d->population = 0;
  _d->needRecomputeAllRoads = false;
  _d->funds.setTaxRate( 7 );
  _d->walkerIdCount = 0;
  _d->climate = C_CENTRAL;
  _d->lastMonthCount = GameDate::current().getMonth();

  addService( CityServiceEmigrant::create( this ) );
  addService( CityServiceWorkersHire::create( this ) );
  addService( CityServicePtr( &CityServiceTimers::getInstance() ) );
  addService( CityServiceProsperity::create( this ) );
  addService( CityServiceShoreline::create( this ) );
  addService( CityServiceInfo::create( this ) );
  addService( CityServiceCulture::create( this ) );
  addService( CityServiceAnimals::create( this ) );
  addService( CityServiceReligion::create( this ) );
  addService( CityServiceFestival::create( this ) );
  addService( CityServiceRoads::create( this ) );
}

void City::timeStep( unsigned int time )
{
  if( _d->lastMonthCount != GameDate::current().getMonth() )
  {
    _d->lastMonthCount = GameDate::current().getMonth();
    monthStep( GameDate::current() );
  }

  WalkerList::iterator walkerIt = _d->walkerList.begin();
  while (walkerIt != _d->walkerList.end())
  {
    try
    {
      WalkerPtr walker = *walkerIt;
      walker->timeStep( time );

      if( walker->isDeleted() )
      {
        // remove the walker from the walkers list  
        walkerIt = _d->walkerList.erase(walkerIt);       
      }
      else
      {
         ++walkerIt;
      }
    }
    catch(...)
    {
    }
  }

  LandOverlayList::iterator overlayIt = _d->overlayList.begin();
  while( overlayIt != _d->overlayList.end() )
  {
    try
    {   
      (*overlayIt)->timeStep( time );

      if( (*overlayIt)->isDeleted() )
      {
        _d->beforeOverlayDestroyed( this, *overlayIt );
        // remove the overlay from the overlay list
        (*overlayIt)->destroy();
        overlayIt = _d->overlayList.erase(overlayIt);
      }
      else
      {
         ++overlayIt;
      }
    }
    catch(...)
    {
      //int i=0;
    }
  }

  CityServices::iterator serviceIt=_d->services.begin();
  while( serviceIt != _d->services.end() )
  {
    (*serviceIt)->update( time );

    if( (*serviceIt)->isDeleted() )
    {
      (*serviceIt)->destroy();

      serviceIt = _d->services.erase(serviceIt);
    }
    else
      serviceIt++;
  }

  if( _d->needRecomputeAllRoads )
  {
    _d->needRecomputeAllRoads = false;
    foreach( LandOverlayPtr overlay, _d->overlayList )
    {
      // for each overlay
      ConstructionPtr construction = overlay.as<Construction>();
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeAccessRoads();
        // for some constructions we need to update picture
        if( construction->getType() == B_ROAD ) 
        {
          RoadPtr road = construction.as<Road>();
          road->updatePicture();
        }
      }
    }   
  }
}

void City::monthStep( const DateTime& time )
{
  _d->collectTaxes( this );
  _d->calculatePopulation( this );
  _d->payWages( this );

  _d->funds.resolveIssue( FundIssue( CityFunds::playerSalary, -_d->player->getSalary() ) );
  _d->player->appendMoney( _d->player->getSalary() );

  _d->funds.updateHistory( GameDate::current() );
}

WalkerList City::getWalkerList( const WalkerType type )
{
  if( type == WT_ALL )
  {
    return _d->walkerList;
  }

  WalkerList res;
  foreach( WalkerPtr walker, _d->walkerList )
  {
    if( walker->getType() == type  )
    {
      res.push_back( walker );
    }
  }

  return res;
}

LandOverlayList& City::getOverlayList()
{
  return _d->overlayList;
}

void City::setBorderInfo(const BorderInfo& info)
{
  int size = getTilemap().getSize();
  TilePos start( 0, 0 );
  TilePos stop( size-1, size-1 );
  _d->borderInfo.roadEntry = info.roadEntry.fit( start, stop );
  _d->borderInfo.roadExit = info.roadExit.fit( start, stop );
  _d->borderInfo.boatEntry = info.boatEntry.fit( start, stop );
  _d->borderInfo.boatExit = info.boatExit.fit( start, stop );
}

const BorderInfo&City::getBorderInfo() const
{
  return _d->borderInfo;
}

Tilemap& City::getTilemap()
{
   return _d->tilemap;
}

ClimateType City::getClimate() const     { return _d->climate;    }

void City::setClimate(const ClimateType climate) { _d->climate = climate; }

CityFunds& City::getFunds() const                  {  return _d->funds;   }

int City::getPopulation() const
{
   /* here we need to calculate population ??? */
   
   return _d->population;
}


void City::Impl::collectTaxes( CityPtr city )
{
  CityHelper hlp( city );
  lastMonthTax = 0;
  lastMonthTaxpayer = 0;
  
  ForumList forums = hlp.getBuildings< Forum >( B_FORUM );
  foreach( ForumPtr forum, forums )
  {
    lastMonthTaxpayer += forum->getPeoplesReached();
    lastMonthTax += forum->collectTaxes();
  }

  std::list<SenatePtr> senates = hlp.getBuildings< Senate >( B_SENATE );
  foreach( SenatePtr senate, senates )
  {
    lastMonthTaxpayer += senate->getPeoplesReached();
    lastMonthTax += senate->collectTaxes();
  }

  funds.resolveIssue( FundIssue( CityFunds::taxIncome, lastMonthTax ) );
}

void City::Impl::payWages(CityPtr city)
{
  int wages = CityStatistic::getMontlyWorkersWages( city );
  funds.resolveIssue( FundIssue( CityFunds::workersWages, -wages ) );
}

void City::Impl::calculatePopulation( CityPtr city )
{
  long pop = 0; /* population can't be negative - should be unsigned long long*/
  
  CityHelper helper( city );

  HouseList houseList = helper.getBuildings<House>( B_HOUSE );
  foreach( HousePtr house, houseList)
  {
    pop += house->getHabitants().count();
  }
  
  population = pop;
  onPopulationChangedSignal.emit( pop );
}

void City::Impl::beforeOverlayDestroyed( CityPtr city, LandOverlayPtr overlay)
{
  if( overlay.is<Construction>() )
  {
    CityHelper helper( city );
    helper.updateDesirability( overlay.as<Construction>(), false );
  }

  if( overlay.is<WorkingBuilding>() )
  {
    returnFiredWorkers( overlay.as<WorkingBuilding>() );
  }
  else if( overlay.is<House>() )
  {
    fireWorkers( overlay.as<House>() );
  }
}

void City::Impl::fireWorkers( HousePtr house )
{
  int leftWorkers = house->getWorkersCount();
  const int defaultFireWorkersDistance = 40;

  for( int curRange=1; curRange < defaultFireWorkersDistance; curRange++ )
  {
    TilemapArea perimetr = tilemap.getRectangle( house->getTilePos() - TilePos( curRange, curRange ),
                                                 house->getSize() + Size( 2 * curRange ) );
    foreach( Tile* tile, perimetr )
    {
      WorkingBuildingPtr wrkBuilding = tile->getOverlay().as<WorkingBuilding>();
      if( wrkBuilding.isValid() )
      {
        int bldWorkersCount = wrkBuilding->getWorkers();
        wrkBuilding->removeWorkers( leftWorkers );
        leftWorkers -= math::clamp( bldWorkersCount, 0, leftWorkers );
      }

      if( !leftWorkers )
        return;
    }
  }
}


void City::Impl::returnFiredWorkers(WorkingBuildingPtr building )
{
  int workersCount = building->getWorkers();
  const int defaultFireWorkersDistance = 40;
  for( int curRange=1; curRange < defaultFireWorkersDistance; curRange++ )
  {
    TilemapArea perimetr = tilemap.getRectangle( building->getTilePos() - TilePos( curRange, curRange ),
                                                 building->getSize() + Size( 2 * curRange ) );
    foreach( Tile* tile, perimetr )
    {
      HousePtr house = tile->getOverlay().as<House>();     
      if( house.isValid() )
      {
        int lastWorkersCount = house->getServiceValue( Service::workersRecruter );
        house->appendServiceValue( Service::workersRecruter, workersCount );
        int currentWorkers = house->getServiceValue( Service::workersRecruter );

        int mayAppend = math::clamp( workersCount, 0, currentWorkers - lastWorkersCount );
        workersCount -= mayAppend;
      }

      if( !workersCount )
        return;
    }
  }
}

void City::save( VariantMap& stream) const
{
  VariantMap vm_tilemap;
  _d->tilemap.save( vm_tilemap );

  stream[ "tilemap" ] = vm_tilemap;
  stream[ "roadEntry" ] = _d->borderInfo.roadEntry;
  stream[ "roadExit" ]  = _d->borderInfo.roadExit;
  stream[ "cameraStart" ] = _d->cameraStart;
  stream[ "boatEntry" ] = _d->borderInfo.boatEntry;
  stream[ "boatExit" ] = _d->borderInfo.boatExit;
  stream[ "climate" ] = _d->climate;
  stream[ "funds" ] = _d->funds.save();
  stream[ "population" ] = _d->population;
  stream[ "name" ] = Variant( _d->name );

  // walkers
  VariantMap vm_walkers;
  int walkedId = 0;
  foreach( WalkerPtr walker, _d->walkerList )
  {
    // std::cout << "WRITE WALKER @" << stream.tell() << std::endl;
    VariantMap vm_walker;
    walker->save( vm_walker );
    vm_walkers[ StringHelper::format( 0xff, "%d", walkedId ) ] = vm_walker;
    walkedId++;
  }
  stream[ "walkers" ] = vm_walkers;

  // overlays
  VariantMap vm_overlays;
  foreach( LandOverlayPtr overlay, _d->overlayList )
  {
    VariantMap vm_overlay;
    overlay->save( vm_overlay );
    vm_overlays[ StringHelper::format( 0xff, "%d,%d", overlay->getTile().getI(),
                                                      overlay->getTile().getJ() ) ] = vm_overlay;
  }

  stream[ "overlays" ] = vm_overlays;
}

void City::load( const VariantMap& stream )
{
  _d->tilemap.load( stream.get( "tilemap" ).toMap() );

  _d->borderInfo.roadEntry = TilePos( stream.get( "roadEntry" ).toTilePos() );
  _d->borderInfo.roadExit = TilePos( stream.get( "roadExit" ).toTilePos() );
  _d->borderInfo.boatEntry = TilePos( stream.get( "boatEntry" ).toTilePos() );
  _d->borderInfo.boatExit = TilePos( stream.get( "boatExit" ).toTilePos() );

  _d->climate = (ClimateType)stream.get( "climate" ).toInt(); 
  _d->funds.load( stream.get( "funds" ).toMap() );
  _d->population = (int)stream.get( "population", 0 );
  _d->cameraStart = TilePos( stream.get( "cameraStart" ).toTilePos() );
  _d->name = stream.get( "name" ).toString();
  _d->lastMonthCount = GameDate::current().getMonth();

  VariantMap overlays = stream.get( "overlays" ).toMap();
  foreach( VariantMap::value_type& item, overlays )
  {
    VariantMap overlay = item.second.toMap();
    TilePos buildPos( overlay.get( "pos" ).toTilePos() );
    int buildingType = (int)overlay.get( "buildingType", 0 );

    ConstructionPtr construction = ConstructionManager::getInstance().create( BuildingType( buildingType ) );
    if( construction.isValid() )
    {
      construction->build( this, buildPos );
      construction->load( overlay );
      _d->overlayList.push_back( construction.as<LandOverlay>() );
    }
  }

  VariantMap walkers = stream.get( "walkers" ).toMap();
  foreach( VariantMap::value_type& item, walkers )
  {
    VariantMap walkerInfo = item.second.toMap();
    int walkerType = (int)walkerInfo.get( "type", 0 );

    WalkerPtr walker = WalkerManager::getInstance().create( WalkerType( walkerType ), this );
    if( walker.isValid() )
    {
      walker->load( walkerInfo );
      _d->walkerList.push_back( walker );
    }
  }
}

void City::addOverlay( LandOverlayPtr overlay )
{
  _d->overlayList.push_back( overlay );
}

City::~City()
{
}

Signal1<int>& City::onPopulationChanged()
{
  return _d->onPopulationChangedSignal;
}

Signal1<int>& City::onFundsChanged()
{
  return _d->funds.onChange();
}

void City::addWalker( WalkerPtr walker )
{
  walker->setUniqueId( ++_d->walkerIdCount );
  _d->walkerList.push_back( walker );
}

void City::removeWalker( WalkerPtr walker )
{
  _d->walkerList.remove( walker );
}

void City::setCameraPos(const TilePos pos) { _d->cameraStart = pos; }
TilePos City::getCameraPos() const {return _d->cameraStart; }

void City::addService( CityServicePtr service )
{
  _d->services.push_back( service );
}

CityServicePtr City::findService( const std::string& name ) const
{
  foreach( CityServicePtr service, _d->services )
  {
    if( name == service->getName() )
      return service;
  }

  return CityServicePtr();
}

Signal1<std::string>& City::onWarningMessage()
{
  return _d->onWarningMessageSignal;
}

Signal2<const TilePos&, const std::string& >& City::onDisasterEvent()
{
  return _d->onDisasterEventSignal;
}

const CityBuildOptions& City::getBuildOptions() const
{
  return _d->buildOptions;
}

void City::setBuildOptions(const CityBuildOptions& options)
{
  _d->buildOptions = options;
}

const CityWinTargets& City::getWinTargets() const
{
  return _d->targets;
}

void City::setWinTargets(const CityWinTargets& targets)
{
  _d->targets = targets;
}

int City::getProsperity() const
{
  CityServicePtr csPrsp = findService( CityServiceProsperity::getDefaultName() );
  return csPrsp.isValid() ? csPrsp.as<CityServiceProsperity>()->getValue() : 0;
}

CityPtr City::create( EmpirePtr empire, PlayerPtr player )
{
  CityPtr ret( new City );
  ret->_d->empire = empire;
  ret->_d->player = player;
  ret->drop();

  return ret;
}

LandOverlayPtr City::getOverlay( const TilePos& pos ) const
{
  return _d->tilemap.at( pos ).getOverlay();
}

int City::getLastMonthTax() const
{
  return _d->lastMonthTax;
}

int City::getLastMonthTaxpayer() const
{
  return _d->lastMonthTaxpayer;
}

PlayerPtr City::getPlayer() const
{
  return _d->player;
}

int City::getCulture() const
{
  CityServicePtr csPrsp = findService( CityServiceCulture::getDefaultName() );
  return csPrsp.isValid() ? csPrsp.as<CityServiceCulture>()->getValue() : 0;
}

std::string City::getName() const
{
  return _d->name;
}

void City::setName( const std::string& name )
{
  _d->name = name;
}

CityTradeOptions& City::getTradeOptions()
{
  return _d->tradeOptions;
}

void City::setLocation( const Point& location )
{
  _d->location = location;
}

Point City::getLocation() const
{
  return _d->location;
}

void City::resolveMerchantArrived( EmpireMerchantPtr merchant )
{
  WalkerPtr cityMerchant = Merchant::create( this, merchant );
  cityMerchant.as<Merchant>()->send2City();
}

const GoodStore& City::getSells() const
{
  return _d->tradeOptions.getSells();
}

const GoodStore& City::getBuys() const
{
  return _d->tradeOptions.getBuys();
}

EmpirePtr City::getEmpire() const
{
  return _d->empire;
}

void City::updateRoads()
{
  _d->needRecomputeAllRoads = true;
}

TilemapArea CityHelper::getArea(BuildingPtr building)
{
  return _city->getTilemap().getArea( building->getTilePos(), building->getSize() );
}


void CityHelper::updateDesirability( ConstructionPtr construction, bool onBuild )
{
  Tilemap& tilemap = _city->getTilemap();

  const BuildingData::Desirability dsrbl = construction->getDesirabilityInfo();
  int mul = ( onBuild ? 1 : -1);

  //change desirability in selfarea
  TilemapArea area = tilemap.getArea( construction->getTilePos(), construction->getSize() );
  foreach( Tile* tile, area )
  {
    tile->appendDesirability( mul * dsrbl.base );
  }

  //change deisirability around
  int current = mul * dsrbl.base;
  for( int curRange=1; curRange <= dsrbl.range; curRange++ )
  {
    TilemapArea perimetr = tilemap.getRectangle( construction->getTilePos() - TilePos( curRange, curRange ),
                                                 construction->getSize() + Size( 2 * curRange ) );
    foreach( Tile* tile, perimetr )
    {
      tile->appendDesirability( current );
    }

    current += mul * dsrbl.step;
  }
}
