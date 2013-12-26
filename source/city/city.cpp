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

#include "city.hpp"

#include "gfx/tile.hpp"
#include "objects/metadata.hpp"
#include "pathway/path_finding.hpp"
#include "core/exception.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "pathway/astarpathfinding.hpp"
#include "core/safetycast.hpp"
#include "city/migration.hpp"
#include "cityservice_workershire.hpp"
#include "cityservice_timers.hpp"
#include "cityservice_prosperity.hpp"
#include "cityservice_shoreline.hpp"
#include "cityservice_info.hpp"
#include "cityservice_animals.hpp"
#include "gfx/tilemap.hpp"
#include "objects/road.hpp"
#include "core/time.hpp"
#include "core/variant.hpp"
#include "core/stringhelper.hpp"
#include "walker/walkers_factory.hpp"
#include "core/gettext.hpp"
#include "build_options.hpp"
#include "cityservice_culture.hpp"
#include "city/funds.hpp"
#include "world/city.hpp"
#include "world/empire.hpp"
#include "trade_options.hpp"
#include "good/goodstore_simple.hpp"
#include "world/trading.hpp"
#include "walker/merchant.hpp"
#include "game/gamedate.hpp"
#include "cityservice_religion.hpp"
#include "core/foreach.hpp"
#include "events/event.hpp"
#include "cityservice_festival.hpp"
#include "win_targets.hpp"
#include "cityservice_roads.hpp"
#include "cityservice_fishplace.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "cityservice_disorder.hpp"
#include "world/merchant.hpp"
#include "city/helper.hpp"
#include "city/statistic.hpp"
#include "objects/forum.hpp"
#include "objects/senate.hpp"
#include "objects/house.hpp"
#include <set>

using namespace constants;

typedef std::vector< CityServicePtr > CityServices;

class WGrid
{
public:
  void clear()
  {
    for( Grid::iterator it=_grid.begin(); it != _grid.end(); it++ )
    {
      it->second.clear();
    }

    _grid.clear();
  }

  unsigned int hash( const TilePos& pos )
  {
    return (pos.getI() << 16) + pos.getJ();
  }

  void append( WalkerPtr& a )
  {
    const TilePos& pos = a->getIJ();
    if( pos.getI() >= 0 && pos.getJ() >= 0 )
    {
      _grid[ hash( pos ) ].push_back( a );
    }
  }

  void remove( WalkerPtr& a )
  {
    TilePos pos = a->getIJ();
    if( pos.getI() >= 0 && pos.getJ() >= 0 )
    {
      WalkerList& d = _grid[ hash( pos ) ];
      for( WalkerList::iterator it=d.begin(); it != d.end(); it++ )
      {
        if( *it == a )
        {
          d.erase( it );
          return;
        }
      }
    }
  }

  const WalkerList& at( TilePos pos )
  {
    if( pos.getI() >= 0 && pos.getJ() >= 0 )
    {
      return _grid[ hash( pos ) ];
    }
    else
    {
      Logger::warning( "WalkersGrid incorrect" );
      static WalkerList invalidList;
      return invalidList;
    }
  }

private:
  typedef std::map< int, WalkerList > Grid;
  Grid _grid;
};

class PlayerCity::Impl
{
public:
  int lastMonthCount;
  int population;
  CityFunds funds;  // amount of money
  std::string name;
  world::EmpirePtr empire;
  PlayerPtr player;

  TileOverlayList overlayList;
  WalkerList walkerList;

  //walkers fast access map !!!
  WGrid walkersGrid;
  //*********************** !!!

  CityServices services;
  bool needRecomputeAllRoads;
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
  void collectTaxes( PlayerCityPtr city);
  void payWages( PlayerCityPtr city );
  void calculatePopulation( PlayerCityPtr city );
  void beforeOverlayDestroyed(PlayerCityPtr city, TileOverlayPtr overlay );

oc3_signals public:
  Signal1<int> onPopulationChangedSignal;
  Signal1<std::string> onWarningMessageSignal;
  Signal2<TilePos,std::string> onDisasterEventSignal;
};

PlayerCity::PlayerCity() : _d( new Impl )
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

  addService( CityMigration::create( this ) );
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
  addService( CityServiceFishPlace::create( this ) );
  addService( CityServiceDisorder::create( this ) );
}

void PlayerCity::timeStep( unsigned int time )
{
  if( _d->lastMonthCount != GameDate::current().getMonth() )
  {
    _d->lastMonthCount = GameDate::current().getMonth();
    monthStep( GameDate::current() );
  }

  //update walkers access map
  _d->walkersGrid.clear();
  foreach( WalkerPtr walker, _d->walkerList )
  {
    _d->walkersGrid.append( walker );
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
        _d->walkersGrid.remove( *walkerIt );
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

  TileOverlayList::iterator overlayIt = _d->overlayList.begin();
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
    foreach( TileOverlayPtr overlay, _d->overlayList )
    {
      // for each overlay
      ConstructionPtr construction = overlay.as<Construction>();
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeAccessRoads();
        // for some constructions we need to update picture
        if( construction->getType() == construction::road )
        {
          RoadPtr road = construction.as<Road>();
          road->updatePicture();
        }
      }
    }   
  }
}

void PlayerCity::monthStep( const DateTime& time )
{
  _d->collectTaxes( this );
  _d->calculatePopulation( this );
  _d->payWages( this );

  _d->funds.resolveIssue( FundIssue( CityFunds::playerSalary, -_d->player->getSalary() ) );
  _d->player->appendMoney( _d->player->getSalary() );

  _d->funds.updateHistory( GameDate::current() );
}

WalkerList PlayerCity::getWalkers( walker::Type type )
{
  if( type == walker::all )
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

WalkerList PlayerCity::getWalkers(walker::Type type, TilePos startPos, TilePos stopPos)
{
  TilePos invalidPos( -1, -1 );

  if( startPos == invalidPos )
    return getWalkers( type );

  WalkerList ret;
  if( stopPos == invalidPos )
  {
    stopPos = startPos;
  }

  TilesArray area = _d->tilemap.getArea( startPos, stopPos );
  foreach( Tile* tile, area)
  {
    WalkerList current = _d->walkersGrid.at( tile->getIJ() );

    foreach( WalkerPtr w, current )
    {
      if( w->getType() == type || type == walker::any )
      {
        ret.push_back( w );
      }
    }
  }

  return ret;
}

void PlayerCity::setBorderInfo(const BorderInfo& info)
{
  int size = getTilemap().getSize();
  TilePos start( 0, 0 );
  TilePos stop( size-1, size-1 );
  _d->borderInfo.roadEntry = info.roadEntry.fit( start, stop );
  _d->borderInfo.roadExit = info.roadExit.fit( start, stop );
  _d->borderInfo.boatEntry = info.boatEntry.fit( start, stop );
  _d->borderInfo.boatExit = info.boatExit.fit( start, stop );
}

TileOverlayList&  PlayerCity::getOverlays()         { return _d->overlayList; }
const BorderInfo& PlayerCity::getBorderInfo() const { return _d->borderInfo; }
Tilemap&          PlayerCity::getTilemap()          { return _d->tilemap; }
ClimateType       PlayerCity::getClimate() const    { return _d->climate;    }
void              PlayerCity::setClimate(const ClimateType climate) { _d->climate = climate; }
CityFunds&        PlayerCity::getFunds() const      {  return _d->funds;   }
int               PlayerCity::getPopulation() const {   return _d->population; }

void PlayerCity::Impl::collectTaxes(PlayerCityPtr city )
{
  CityHelper hlp( city );
  int lastMonthTax = 0;
  
  ForumList forums = hlp.find< Forum >( building::forum );
  foreach( ForumPtr forum, forums ) { lastMonthTax += forum->collectTaxes(); }

  SenateList senates = hlp.find< Senate >( building::senate );
  foreach( SenatePtr senate, senates ) { lastMonthTax += senate->collectTaxes(); }

  funds.resolveIssue( FundIssue( CityFunds::taxIncome, lastMonthTax ) );
}

void PlayerCity::Impl::payWages(PlayerCityPtr city)
{
  int wages = CityStatistic::getMontlyWorkersWages( city );
  funds.resolveIssue( FundIssue( CityFunds::workersWages, -wages ) );
}

void PlayerCity::Impl::calculatePopulation( PlayerCityPtr city )
{
  long pop = 0; /* population can't be negative - should be unsigned long long*/
  
  CityHelper helper( city );

  HouseList houseList = helper.find<House>( building::house );
  foreach( HousePtr house, houseList)
  {
    pop += house->getHabitants().count();
  }
  
  population = pop;
  onPopulationChangedSignal.emit( pop );
}

void PlayerCity::Impl::beforeOverlayDestroyed(PlayerCityPtr city, TileOverlayPtr overlay)
{
  if( overlay.is<Construction>() )
  {
    CityHelper helper( city );
    helper.updateDesirability( overlay.as<Construction>(), false );
  }
}

void PlayerCity::save( VariantMap& stream) const
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
  stream[ "tradeOptions" ] = _d->tradeOptions.save();

  // walkers
  VariantMap vm_walkers;
  int walkedId = 0;
  foreach( WalkerPtr walker, _d->walkerList )
  {
    VariantMap vm_walker;
    walker->save( vm_walker );
    vm_walkers[ StringHelper::format( 0xff, "%d", walkedId ) ] = vm_walker;
    walkedId++;
  }
  stream[ "walkers" ] = vm_walkers;

  // overlays
  VariantMap vm_overlays;
  foreach( TileOverlayPtr overlay, _d->overlayList )
  {
    VariantMap vm_overlay;
    overlay->save( vm_overlay );
    vm_overlays[ StringHelper::format( 0xff, "%d,%d", overlay->getTile().getI(),
                                                      overlay->getTile().getJ() ) ] = vm_overlay;
  }
  stream[ "overlays" ] = vm_overlays;

  VariantMap vm_services;
  foreach( CityServicePtr service, _d->services )
  {   
    vm_services[ service->getName() ] = service->save();
  }

  stream[ "services" ] = vm_services;
}

void PlayerCity::load( const VariantMap& stream )
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
  _d->tradeOptions.load( stream.get( "tradeOptions" ).toMap() );

  VariantMap overlays = stream.get( "overlays" ).toMap();
  foreach( VariantMap::value_type& item, overlays )
  {
    VariantMap overlayParams = item.second.toMap();
    VariantList config = overlayParams.get( "config" ).toList();

    TileOverlay::Type overlayType = (TileOverlay::Type)config.get( 0 ).toInt();
    TilePos pos = config.get( 2, TilePos( -1, -1 ) ).toTilePos();

    TileOverlayPtr overlay = TileOverlayFactory::getInstance().create( overlayType );
    if( overlay.isValid() && pos.getI() >= 0 )
    {
      overlay->build( this, pos );
      overlay->load( overlayParams );
      _d->overlayList.push_back( overlay );
    }
    else
    {
      Logger::warning( "Can't load overlay %s", item.first.c_str() );
    }
  }

  VariantMap walkers = stream.get( "walkers" ).toMap();
  foreach( VariantMap::value_type& item, walkers )
  {
    VariantMap walkerInfo = item.second.toMap();
    int walkerType = (int)walkerInfo.get( "type", 0 );

    WalkerPtr walker = WalkerManager::getInstance().create( walker::Type( walkerType ), this );
    if( walker.isValid() )
    {
      walker->load( walkerInfo );
      _d->walkerList.push_back( walker );
    }
    else
    {
      Logger::warning( "Can't load walker " + item.first );
    }
  }

  VariantMap services = stream.get( "services" ).toMap();
  foreach( VariantMap::value_type& item, services )
  {
    VariantMap servicesSave = item.second.toMap();

    CityServicePtr srvc = findService( item.first);
    if( srvc.isValid()  )
    {
      srvc->load( servicesSave );
    }
    else
    {
      Logger::warning( "Can't find service " + item.first );
    }
  }
}

void PlayerCity::addOverlay( TileOverlayPtr overlay ) { _d->overlayList.push_back( overlay ); }

PlayerCity::~PlayerCity(){}

void PlayerCity::addWalker( WalkerPtr walker )
{
  walker->setUniqueId( ++_d->walkerIdCount );
  _d->walkerList.push_back( walker );
}

CityServicePtr PlayerCity::findService( const std::string& name ) const
{
  foreach( CityServicePtr service, _d->services )
  {
    if( name == service->getName() )
      return service;
  }

  return CityServicePtr();
}

Signal1<std::string>& PlayerCity::onWarningMessage() { return _d->onWarningMessageSignal; }
Signal2<TilePos,std::string>& PlayerCity::onDisasterEvent() { return _d->onDisasterEventSignal; }
const CityBuildOptions& PlayerCity::getBuildOptions() const { return _d->buildOptions; }
void PlayerCity::setBuildOptions(const CityBuildOptions& options) { _d->buildOptions = options; }
const CityWinTargets& PlayerCity::getWinTargets() const {   return _d->targets; }
void PlayerCity::setWinTargets(const CityWinTargets& targets) { _d->targets = targets; }
TileOverlayPtr PlayerCity::getOverlay( const TilePos& pos ) const { return _d->tilemap.at( pos ).getOverlay(); }
PlayerPtr PlayerCity::getPlayer() const { return _d->player; }
std::string PlayerCity::getName() const {  return _d->name; }
void PlayerCity::setName( const std::string& name ) {   _d->name = name;}
CityTradeOptions& PlayerCity::getTradeOptions() { return _d->tradeOptions; }
void PlayerCity::setLocation( const Point& location ) {   _d->location = location; }
Point PlayerCity::getLocation() const {   return _d->location; }
const GoodStore& PlayerCity::getSells() const {   return _d->tradeOptions.getSells(); }
const GoodStore& PlayerCity::getBuys() const {   return _d->tradeOptions.getBuys(); }
world::EmpirePtr PlayerCity::getEmpire() const {   return _d->empire; }
void PlayerCity::updateRoads() {    _d->needRecomputeAllRoads = true; }
Signal1<int>& PlayerCity::onPopulationChanged() {  return _d->onPopulationChangedSignal; }
Signal1<int>& PlayerCity::onFundsChanged() {  return _d->funds.onChange(); }
void PlayerCity::removeWalker( WalkerPtr walker ) { _d->walkerList.remove( walker ); }
void PlayerCity::setCameraPos(const TilePos pos) { _d->cameraStart = pos; }
TilePos PlayerCity::getCameraPos() const {return _d->cameraStart; }
void PlayerCity::addService( CityServicePtr service ) {  _d->services.push_back( service ); }

int PlayerCity::getProsperity() const
{
  CityServicePtr csPrsp = findService( CityServiceProsperity::getDefaultName() );
  return csPrsp.isValid() ? csPrsp.as<CityServiceProsperity>()->getValue() : 0;
}

PlayerCityPtr PlayerCity::create( world::EmpirePtr empire, PlayerPtr player )
{
  PlayerCityPtr ret( new PlayerCity );
  ret->_d->empire = empire;
  ret->_d->player = player;
  ret->drop();

  return ret;
}

int PlayerCity::getCulture() const
{
  CityServicePtr csPrsp = findService( CityServiceCulture::getDefaultName() );
  return csPrsp.isValid() ? csPrsp.as<CityServiceCulture>()->getValue() : 0;
}

int PlayerCity::getPeace() const
{
  //CityServicePtr csPrsp = findService( CityServicePeace::getDefaultName() );
  return 0;//csPrsp.isValid() ? csPrsp.as<CityServiceCulture>()->getValue() : 0;
}

int PlayerCity::getFavour() const
{
  //CityServicePtr csPrsp = findService( CityServiceFavour::getDefaultName() );
  return 0;//csPrsp.isValid() ? csPrsp.as<CityServiceCulture>()->getValue() : 0;
}

void PlayerCity::arrivedMerchant( world::MerchantPtr merchant )
{
  WalkerPtr cityMerchant = Merchant::create( this, merchant );
  cityMerchant.as<Merchant>()->send2City();
}
