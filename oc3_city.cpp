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
  Player* player;

  LandOverlayList overlayList;
  WalkerList walkerList;
  TilePos roadEntry; //coordinates can't be negative!
  CityServices services;
  bool needRecomputeAllRoads;
  int taxRate;
  int lastMonthTax;
  int lastMonthTaxpayer;
  TilePos roadExit;
  Tilemap tilemap;
  TilePos boatEntry;
  TilePos boatExit;
  TilePos cameraStart;
  Point location;
  CityBuildOptions buildOptions;
  CityTradeOptions tradeOptions;
  CityWinTargets targets;

  ClimateType climate;   
  UniqueId walkerIdCount;

  // collect taxes from all houses
  void collectTaxes( CityPtr city);
  void calculatePopulation( CityPtr city );

oc3_signals public:
  Signal1<int> onPopulationChangedSignal;
  Signal1<std::string> onWarningMessageSignal;
  Signal2<const TilePos&, const std::string&> onDisasterEventSignal;
};

City::City() : _d( new Impl )
{
  _d->roadEntry = TilePos( 0, 0 );
  _d->roadExit = TilePos( 0, 0 );
  _d->boatEntry = TilePos( 0, 0 );
  _d->boatExit = TilePos( 0, 0 );
  _d->funds.resolveIssue( FundIssue( CityFunds::donation, 1000 ) );
  _d->population = 0;
  _d->needRecomputeAllRoads = false;
  _d->taxRate = 7;
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
}

void City::timeStep( unsigned int time )
{
  if( _d->lastMonthCount != GameDate::current().getMonth() )
  {
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
      //int o=0;
      //volatile error here... WTF
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
         // remove the overlay from the overlay list
          (*overlayIt)->destroy();
          //delete (*overlayIt);

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

Tilemap& City::getTilemap()
{
   return _d->tilemap;
}

TilePos City::getBoatEntry() const { return _d->boatEntry; }
TilePos City::getBoatExit() const  { return _d->boatExit;  }

ClimateType City::getClimate() const     { return _d->climate;    }

void City::setClimate(const ClimateType climate) { _d->climate = climate; }

// paste here protection from bad values
void City::setRoadEntry( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->roadEntry = TilePos( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ),
                           math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setRoadExit( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->roadExit.setI( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ) );
  _d->roadExit.setJ( math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setBoatEntry(const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->boatEntry.setI( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ) );
  _d->boatEntry.setJ( math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

void City::setBoatExit( const TilePos& pos )
{
  int size = getTilemap().getSize();
  _d->boatExit.setI( math::clamp<unsigned int>( pos.getI(), 0, size - 1 ) );
  _d->boatExit.setJ( math::clamp<unsigned int>( pos.getJ(), 0, size - 1 ) );
}

int City::getTaxRate() const                 {  return _d->taxRate;    }
void City::setTaxRate(const int taxRate)     {  _d->taxRate = taxRate; }
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

void City::Impl::calculatePopulation( CityPtr city )
{
  long pop = 0; /* population can't be negative - should be unsigned long long*/
  
  CityHelper helper( city );

  HouseList houseList = helper.getBuildings<House>( B_HOUSE );
  foreach( HousePtr house, houseList)
  {
    pop += house->getNbHabitants();
  }
  
  population = pop;
  onPopulationChangedSignal.emit( pop );
}

void City::save( VariantMap& stream) const
{
  VariantMap vm_tilemap;
  _d->tilemap.save( vm_tilemap );

  stream[ "tilemap" ] = vm_tilemap;
  stream[ "roadEntry" ] = _d->roadEntry;
  stream[ "roadExit" ]  = _d->roadExit;
  stream[ "cameraStart" ] = _d->cameraStart;
  stream[ "boatEntry" ] = _d->boatEntry;
  stream[ "boatExit" ] = _d->boatExit;
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

  _d->roadEntry = TilePos( stream.get( "roadEntry" ).toTilePos() );
  _d->roadExit = TilePos( stream.get( "roadExit" ).toTilePos() );
  _d->boatEntry = TilePos( stream.get( "boatEntry" ).toTilePos() );
  _d->boatExit = TilePos( stream.get( "boatExit" ).toTilePos() );
  _d->climate = (ClimateType)stream.get( "climate" ).toInt(); 
  _d->funds.load( stream.get( "funds" ).toMap() );
  _d->population = stream.get( "population" ).toInt();
  _d->cameraStart = TilePos( stream.get( "cameraStart" ).toTilePos() );
  _d->name = stream.get( "name" ).toString();
  _d->lastMonthCount = GameDate::current().getMonth();

  VariantMap overlays = stream.get( "overlays" ).toMap();
  foreach( VariantMap::value_type& item, overlays )
  {
    VariantMap overlay = item.second.toMap();
    TilePos buildPos( overlay.get( "pos" ).toTilePos() );
    int buildingType = overlay.get( "buildingType" ).toInt();

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
    int walkerType = walkerInfo.get( "type" ).toInt();

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

TilePos City::getRoadEntry() const
{
  return _d->roadEntry;
}

TilePos City::getRoadExit() const
{
  return _d->roadExit;
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
  CityServicePtr csPrsp = findService( "prosperity" );
  return csPrsp.isValid() ? csPrsp.as<CityServiceProsperity>()->getValue() : 0;
}

CityPtr City::create( EmpirePtr empire, Player* player )
{
  CityPtr ret( new City );
  ret->_d->empire = empire;
  ret->_d->player = player;
  ret->drop();

  return ret;
}

LandOverlayPtr City::getOverlay( const TilePos& pos ) const
{
  return _d->tilemap.at( pos ).getTerrain().getOverlay();
}

int City::getLastMonthTax() const
{
  return _d->lastMonthTax;
}

int City::getLastMonthTaxpayer() const
{
  return _d->lastMonthTaxpayer;
}

Player*City::getPlayer() const
{
  return _d->player;
}

int City::getCulture() const
{
  CityServicePtr csPrsp = findService( "culture" );
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
  return _city->getTilemap().getFilledRectangle( building->getTilePos(), building->getSize() );
}
