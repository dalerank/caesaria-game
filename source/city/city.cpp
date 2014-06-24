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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "city.hpp"
#include "objects/construction.hpp"
#include "gfx/tile.hpp"
#include "objects/metadata.hpp"
#include "pathway/path_finding.hpp"
#include "core/exception.hpp"
#include "events/winmission.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "pathway/astarpathfinding.hpp"
#include "core/safetycast.hpp"
#include "city/migration.hpp"
#include "cityservice_workershire.hpp"
#include "cityservice_timers.hpp"
#include "cityservice_prosperity.hpp"
#include "cityservice_religion.hpp"
#include "cityservice_festival.hpp"
#include "cityservice_roads.hpp"
#include "cityservice_fishplace.hpp"
#include "cityservice_shoreline.hpp"
#include "cityservice_info.hpp"
#include "requestdispatcher.hpp"
#include "cityservice_disorder.hpp"
#include "cityservice_animals.hpp"
#include "cityservice_culture.hpp"
#include "gfx/tilemap.hpp"
#include "objects/road.hpp"
#include "core/time.hpp"
#include "core/variant.hpp"
#include "core/stringhelper.hpp"
#include "walker/walkers_factory.hpp"
#include "core/gettext.hpp"
#include "build_options.hpp"
#include "city/funds.hpp"
#include "world/city.hpp"
#include "world/empire.hpp"
#include "trade_options.hpp"
#include "good/goodstore_simple.hpp"
#include "world/trading.hpp"
#include "walker/merchant.hpp"
#include "game/gamedate.hpp"
#include "core/foreach.hpp"
#include "events/event.hpp"
#include "victoryconditions.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "world/merchant.hpp"
#include "city/helper.hpp"
#include "city/statistic.hpp"
#include "objects/forum.hpp"
#include "objects/senate.hpp"
#include "objects/house.hpp"
#include "world/empiremap.hpp"
#include "walker/seamerchant.hpp"
#include "cityservice_factory.hpp"
#include "sound/player.hpp"
#include "world/emperor.hpp"
#include "events/empiretax.hpp"
#include "cityservice_health.hpp"
#include <set>
#include "cityservice_military.hpp"
#include "cityservice_peace.hpp"
#include "game/resourcegroup.hpp"
#include "world/romechastenerarmy.hpp"
#include "walker/chastener.hpp"

using namespace constants;
using namespace gfx;

namespace {
CAESARIA_LITERALCONST(tilemap)
CAESARIA_LITERALCONST(walkerIdCount)
CAESARIA_LITERALCONST(adviserEnabled)
}

typedef std::vector< city::SrvcPtr > CityServices;

class WGrid
{
public:
  void clear()
  {
    foreach( it, _grid )
    {
      it->second.clear();
    }

    _grid.clear();
  }  

  void append( WalkerPtr& a )
  {
    const TilePos& pos = a->pos();
    if( pos.i() >= 0 && pos.j() >= 0 )
    {
      _grid[ TileHelper::hash( pos ) ].push_back( a );
    }
  }

  void remove( WalkerPtr& a )
  {
    TilePos pos = a->pos();
    if( pos.i() >= 0 && pos.j() >= 0 )
    {
      WalkerList& d = _grid[ TileHelper::hash( pos ) ];
      foreach( it, d )
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
    if( pos.i() >= 0 && pos.j() >= 0 )
    {
      return _grid[ TileHelper::hash( pos ) ];
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
  typedef std::map<PlayerCity::OptionType, int> Options;
  int population;
  city::Funds funds;  // amount of money

  PlayerPtr player;

  TileOverlayList overlayList;
  WalkerList walkerList;
  Picture empMapPicture;

  //walkers fast access map !!!
  WGrid walkersGrid;
  //*********************** !!!

  CityServices services;
  bool needRecomputeAllRoads;
  BorderInfo borderInfo;
  Tilemap tilemap;
  TilePos cameraStart;

  city::BuildOptions buildOptions;
  city::TradeOptions tradeOptions;
  city::VictoryConditions targets;
  Options options;
  ClimateType climate;   
  UniqueId walkerIdCount;
  int sentiment;

  // collect taxes from all houses
  void collectTaxes( PlayerCityPtr city);
  void payWages( PlayerCityPtr city );
  void monthStep( PlayerCityPtr city, const DateTime& time );
  void calculatePopulation( PlayerCityPtr city );
  void beforeOverlayDestroyed(PlayerCityPtr city, TileOverlayPtr overlay );

oc3_signals public:
  Signal1<int> onPopulationChangedSignal;
  Signal1<std::string> onWarningMessageSignal;
  Signal2<TilePos,std::string> onDisasterEventSignal;
  Signal0<> onChangeBuildingOptionsSignal;
};

PlayerCity::PlayerCity(world::EmpirePtr empire)
  : City( empire ), _d( new Impl )
{
  _d->borderInfo.roadEntry = TilePos( 0, 0 );
  _d->borderInfo.roadExit = TilePos( 0, 0 );
  _d->borderInfo.boatEntry = TilePos( 0, 0 );
  _d->borderInfo.boatExit = TilePos( 0, 0 );
  _d->funds.resolveIssue( FundIssue( city::Funds::donation, 1000 ) );
  _d->population = 0;
  _d->needRecomputeAllRoads = false;
  _d->funds.setTaxRate( 7 );
  _d->walkerIdCount = 0;
  _d->climate = C_CENTRAL;
  _d->sentiment = 60;
  _d->empMapPicture = Picture::load( ResourceGroup::empirebits, 1 );

  addService( city::Migration::create( this ) );
  addService( city::WorkersHire::create( this ) );
  addService( city::ProsperityRating::create( this ) );
  addService( city::Shoreline::create( this ) );
  addService( city::Info::create( this ) );
  addService( city::CultureRating::create( this ) );
  addService( city::Animals::create( this ) );
  addService( city::Religion::create( this ) );
  addService( city::Festival::create( this ) );
  addService( city::Roads::create( this ) );
  addService( city::Fishery::create( this ) );
  addService( city::Disorder::create( this ) );
  addService( city::request::Dispatcher::create( this ) );
  addService( city::Military::create( this ) );
  addService( audio::Player::create( this ) );
  addService( city::HealthCare::create( this ));
  addService( city::Peace::create( this ) );
}

void PlayerCity::timeStep(unsigned int time)
{
  if( GameDate::isMonthChanged() )
  {
    _d->monthStep( this, GameDate::current() );

    if( GameDate::isYearChanged() )
    {
      events::GameEventPtr e = events::EmpireTax::create( name() );
      e->dispatch();
    }
  }

  if( GameDate::isWeekChanged() )
  {
    _d->calculatePopulation( this );
  }

  //update walkers access map
  _d->walkersGrid.clear();
  foreach( it, _d->walkerList )
  {
    _d->walkersGrid.append( *it );
  }

  WalkerList::iterator walkerIt = _d->walkerList.begin();
  while( walkerIt != _d->walkerList.end() )
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
  city::Timers::getInstance().update( time );
  while( serviceIt != _d->services.end() )
  {
    (*serviceIt)->update( time );

    if( (*serviceIt)->isDeleted() )
    {
      (*serviceIt)->destroy();
      serviceIt = _d->services.erase(serviceIt);
    }
    else
      ++serviceIt;
  }

  if( _d->needRecomputeAllRoads )
  {
    _d->needRecomputeAllRoads = false;
    foreach( it, _d->overlayList )
    {
      TileOverlayPtr overlay = *it;
      // for each overlay
      ConstructionPtr construction = ptr_cast<Construction>( overlay );
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeAccessRoads();
        // for some constructions we need to update picture
        if( construction->type() == construction::road )
        {
          RoadPtr road = ptr_cast<Road>( construction );
          road->updatePicture();
        }
      }
    }   
  }
}

void PlayerCity::Impl::monthStep( PlayerCityPtr city, const DateTime& time )
{
  collectTaxes( city );
  payWages( city );

  int playerSalary = player->salary();
  funds.resolveIssue( FundIssue( city::Funds::playerSalary, -playerSalary ) );
  player->appendMoney( playerSalary );

  funds.updateHistory( GameDate::current() );
}

WalkerList PlayerCity::getWalkers( walker::Type rtype )
{
  if( rtype == walker::all )
  {
    return _d->walkerList;
  }

  WalkerList res;
  foreach( w, _d->walkerList )
  {
    if( (*w)->type() == rtype  )
    {
      res.push_back( *w );
    }
  }

  return res;
}

WalkerList PlayerCity::getWalkers(walker::Type rtype, TilePos startPos, TilePos stopPos)
{
  TilePos invalidPos( -1, -1 );

  if( startPos == invalidPos )
    return getWalkers( rtype );

  WalkerList ret;
  if( stopPos == invalidPos )
  {
    stopPos = startPos;
  }

  TilesArray area = _d->tilemap.getArea( startPos, stopPos );
  foreach( tile, area)
  {
    WalkerList current = _d->walkersGrid.at( (*tile)->pos() );

    foreach( w, current )
    {
      if( (*w)->type() == rtype || rtype == walker::any )
      {
        ret.push_back( *w );
      }
    }
  }

  return ret;
}

void PlayerCity::setBorderInfo(const BorderInfo& info)
{
  int size = tilemap().size();
  TilePos start( 0, 0 );
  TilePos stop( size-1, size-1 );
  _d->borderInfo.roadEntry = info.roadEntry.fit( start, stop );
  _d->borderInfo.roadExit = info.roadExit.fit( start, stop );
  _d->borderInfo.boatEntry = info.boatEntry.fit( start, stop );
  _d->borderInfo.boatExit = info.boatExit.fit( start, stop );
}

TileOverlayList&  PlayerCity::overlays()         { return _d->overlayList; }
const BorderInfo& PlayerCity::borderInfo() const { return _d->borderInfo; }

Picture PlayerCity::picture() const { return _d->empMapPicture; }
Tilemap&          PlayerCity::tilemap()          { return _d->tilemap; }
ClimateType       PlayerCity::climate() const    { return _d->climate;    }
void              PlayerCity::setClimate(const ClimateType climate) { _d->climate = climate; }
city::Funds&      PlayerCity::funds() const      {  return _d->funds;   }
int               PlayerCity::population() const {   return _d->population; }

void PlayerCity::Impl::collectTaxes(PlayerCityPtr city )
{
  city::Helper hlp( city );
  int lastMonthTax = 0;
  
  ForumList forums = hlp.find< Forum >( building::forum );
  foreach( forum, forums ) { lastMonthTax += (*forum)->collectTaxes(); }

  SenateList senates = hlp.find< Senate >( building::senate );
  foreach( senate, senates ) { lastMonthTax += (*senate)->collectTaxes(); }

  funds.resolveIssue( FundIssue( city::Funds::taxIncome, lastMonthTax ) );
}

void PlayerCity::Impl::payWages(PlayerCityPtr city)
{
  int wages = city::Statistic::getMontlyWorkersWages( city );
  funds.resolveIssue( FundIssue( city::Funds::workersWages, -wages ) );
}

void PlayerCity::Impl::calculatePopulation( PlayerCityPtr city )
{
  long pop = 0; /* population can't be negative - should be unsigned long long*/
  
  city::Helper helper( city );

  HouseList houseList = helper.find<House>( building::house );

  foreach( house, houseList) { pop += (*house)->habitants().count(); }
  
  population = pop;
  oc3_emit onPopulationChangedSignal( pop );
}

void PlayerCity::Impl::beforeOverlayDestroyed(PlayerCityPtr city, TileOverlayPtr overlay)
{
  city::Helper helper( city );
  helper.updateDesirability( overlay, city::Helper::offDesirability );
}

void PlayerCity::save( VariantMap& stream) const
{
  Logger::warning( "City: create save map" );
  City::save( stream );

  Logger::warning( "City: save tilemap information");
  VariantMap vm_tilemap;
  _d->tilemap.save( vm_tilemap );

  stream[ lc_tilemap    ] = vm_tilemap;
  stream[ lc_walkerIdCount   ] = (unsigned int)_d->walkerIdCount;

  Logger::warning( "City: save main paramters ");
  stream[ "roadEntry"  ] = _d->borderInfo.roadEntry;
  stream[ "roadExit"   ] = _d->borderInfo.roadExit;
  stream[ "cameraStart"] = _d->cameraStart;
  stream[ "boatEntry"  ] = _d->borderInfo.boatEntry;
  stream[ "boatExit"   ] = _d->borderInfo.boatExit;
  stream[ "climate"    ] = _d->climate;
  stream[ lc_adviserEnabled ] = getOption( PlayerCity::adviserEnabled );
  stream[ "population" ] = _d->population;

  Logger::warning( "City: save finance information" );
  stream[ "funds" ] = _d->funds.save();

  Logger::warning( "City: save trade/build/win options" );
  stream[ "tradeOptions" ] = _d->tradeOptions.save();
  stream[ "buildOptions" ] = _d->buildOptions.save();
  stream[ "winTargets"   ] = _d->targets.save();

  Logger::warning( "City: save walkers information" );
  VariantMap vm_walkers;
  int walkedId = 0;
  foreach( w, _d->walkerList )
  {
    VariantMap vm_walker;
    (*w)->save( vm_walker );
    vm_walkers[ StringHelper::format( 0xff, "%d", walkedId ) ] = vm_walker;
    walkedId++;
  }
  stream[ "walkers" ] = vm_walkers;

  Logger::warning( "City: save overlays information" );
  VariantMap vm_overlays;
  foreach( overlay, _d->overlayList )
  {
    VariantMap vm_overlay;
    (*overlay)->save( vm_overlay );
    vm_overlays[ StringHelper::format( 0xff, "%d,%d", (*overlay)->pos().i(),
                                                      (*overlay)->pos().j() ) ] = vm_overlay;
  }
  stream[ "overlays" ] = vm_overlays;

  Logger::warning( "City: save services information" );
  VariantMap vm_services;
  foreach( service, _d->services )
  {   
    vm_services[ (*service)->getName() ] = (*service)->save();
  }

  stream[ "services" ] = vm_services;

  Logger::warning( "City: finalize save map" );
}

void PlayerCity::load( const VariantMap& stream )
{
  Logger::warning( "City: start parse savemap" );
  City::load( stream );
  _d->tilemap.load( stream.get( lc_tilemap ).toMap() );
  _d->walkerIdCount = (UniqueId)stream.get( lc_walkerIdCount ).toUInt();

  Logger::warning( "City: parse main params" );
  _d->borderInfo.roadEntry = TilePos( stream.get( "roadEntry" ).toTilePos() );
  _d->borderInfo.roadExit = TilePos( stream.get( "roadExit" ).toTilePos() );
  _d->borderInfo.boatEntry = TilePos( stream.get( "boatEntry" ).toTilePos() );
  _d->borderInfo.boatExit = TilePos( stream.get( "boatExit" ).toTilePos() );  
  _d->climate = (ClimateType)stream.get( "climate" ).toInt(); 
  _d->population = (int)stream.get( "population", 0 );
  _d->cameraStart = TilePos( stream.get( "cameraStart" ).toTilePos() );

  setOption( adviserEnabled, stream.get( lc_adviserEnabled, 1 ) );

  Logger::warning( "City: parse funds" );
  _d->funds.load( stream.get( "funds" ).toMap() );

  Logger::warning( "City: parse trade/build/win params" );
  _d->tradeOptions.load( stream.get( "tradeOptions" ).toMap() );
  _d->buildOptions.load( stream.get( "buildOptions" ).toMap() );
  _d->targets.load( stream.get( "winTargets").toMap() );

  Logger::warning( "City: load overlays" );
  VariantMap overlays = stream.get( "overlays" ).toMap();
  foreach( item, overlays )
  {
    VariantMap overlayParams = item->second.toMap();
    VariantList config = overlayParams.get( "config" ).toList();

    TileOverlay::Type overlayType = (TileOverlay::Type)config.get( 0 ).toInt();
    TilePos pos = config.get( 2, TilePos( -1, -1 ) ).toTilePos();

    TileOverlayPtr overlay = TileOverlayFactory::instance().create( overlayType );
    if( overlay.isValid() && pos.i() >= 0 )
    {
      overlay->build( this, pos );
      overlay->load( overlayParams );
      _d->overlayList.push_back( overlay );
    }
    else
    {
      Logger::warning( "City: can't load overlay " + item->first );
    }
  }

  Logger::warning( "City: parse walkers info" );
  VariantMap walkers = stream.get( "walkers" ).toMap();
  foreach( item, walkers )
  {
    VariantMap walkerInfo = item->second.toMap();
    int walkerType = (int)walkerInfo.get( "type", 0 );

    WalkerPtr walker = WalkerManager::instance().create( walker::Type( walkerType ), this );
    if( walker.isValid() )
    {
      walker->load( walkerInfo );
      _d->walkerList.push_back( walker );
    }
    else
    {
      Logger::warning( "City: can't load walker " + item->first );
    }
  }

  Logger::warning( "City: load service info" );
  VariantMap services = stream.get( "services" ).toMap();
  foreach( item, services )
  {
    VariantMap servicesSave = item->second.toMap();

    city::SrvcPtr srvc = findService( item->first );
    if( srvc.isNull() )
    {
      Logger::warning( "City: " + item->first + " is not basic service, try load by name" );

      srvc = city::ServiceFactory::create( item->first, this );
      if( srvc.isValid() )
      {
        Logger::warning( "City: creating service " + item->first + " directly");
        addService( srvc );
      }
    }

    if( srvc.isValid() )
    {
      srvc->load( servicesSave );
    }
    else
    {
      Logger::warning( "Can't find service " + item->first );
    }
  }
}

void PlayerCity::addOverlay( TileOverlayPtr overlay ) { _d->overlayList.push_back( overlay ); }

PlayerCity::~PlayerCity() {}

void PlayerCity::addWalker( WalkerPtr walker )
{
  walker->setUniqueId( ++_d->walkerIdCount );
  _d->walkerList.push_back( walker );
}

city::SrvcPtr PlayerCity::findService( const std::string& name ) const
{
  foreach( service, _d->services )
  {
    if( name == (*service)->getName() )
      return *service;
  }

  return city::SrvcPtr();
}

void PlayerCity::setBuildOptions(const city::BuildOptions& options)
{
  _d->buildOptions = options;
  oc3_emit _d->onChangeBuildingOptionsSignal();
}

Signal1<std::string>& PlayerCity::onWarningMessage() { return _d->onWarningMessageSignal; }
Signal2<TilePos,std::string>& PlayerCity::onDisasterEvent() { return _d->onDisasterEventSignal; }
Signal0<>&PlayerCity::onChangeBuildingOptions(){ return _d->onChangeBuildingOptionsSignal; }
const city::BuildOptions& PlayerCity::buildOptions() const { return _d->buildOptions; }
const city::VictoryConditions& PlayerCity::victoryConditions() const {   return _d->targets; }
void PlayerCity::setVictoryConditions(const city::VictoryConditions& targets) { _d->targets = targets; }
TileOverlayPtr PlayerCity::getOverlay( const TilePos& pos ) const { return _d->tilemap.at( pos ).overlay(); }
PlayerPtr PlayerCity::player() const { return _d->player; }

city::TradeOptions& PlayerCity::tradeOptions() { return _d->tradeOptions; }
void PlayerCity::delayTrade(unsigned int month){  }

const GoodStore& PlayerCity::importingGoods() const {   return _d->tradeOptions.importingGoods(); }
const GoodStore& PlayerCity::exportingGoods() const {   return _d->tradeOptions.exportingGoods(); }
unsigned int PlayerCity::tradeType() const { return world::EmpireMap::sea | world::EmpireMap::land; }

void PlayerCity::setOption(PlayerCity::OptionType opt, int value) { _d->options[ opt ] = value; }
void PlayerCity::updateRoads() {   _d->needRecomputeAllRoads = true; }
Signal1<int>& PlayerCity::onPopulationChanged() {  return _d->onPopulationChangedSignal; }
Signal1<int>& PlayerCity::onFundsChanged() {  return _d->funds.onChange(); }
void PlayerCity::setCameraPos(const TilePos pos) { _d->cameraStart = pos; }
TilePos PlayerCity::cameraPos() const {return _d->cameraStart; }
void PlayerCity::addService( city::SrvcPtr service ) {  _d->services.push_back( service ); }

int PlayerCity::prosperity() const
{
  SmartPtr<city::ProsperityRating> csPrsp = ptr_cast<city::ProsperityRating>( findService( city::ProsperityRating::getDefaultName() ) );
  return csPrsp.isValid() ? csPrsp->getValue() : 0;
}

int PlayerCity::getOption(PlayerCity::OptionType opt) const
{
  Impl::Options::const_iterator it = _d->options.find( opt );
  return (it != _d->options.end() ? it->second : 0 );
}

void PlayerCity::clean()
{
  _d->services.clear();
}

PlayerCityPtr PlayerCity::create( world::EmpirePtr empire, PlayerPtr player )
{
  PlayerCityPtr ret( new PlayerCity( empire ) );
  ret->_d->player = player;
  ret->drop();

  return ret;
}

int PlayerCity::culture() const
{
  SmartPtr<city::CultureRating> csClt = ptr_cast<city::CultureRating>( findService( city::CultureRating::getDefaultName() ) );
  return csClt.isValid() ? csClt->value() : 0;
}

int PlayerCity::peace() const
{
  SmartPtr<city::Peace> peace = ptr_cast<city::Peace>( findService( city::Peace::getDefaultName() ) );
  return peace.isValid() ? peace->value() : 0;
}

int PlayerCity::sentiment() const
{
  return _d->sentiment;
}

int PlayerCity::favour() const { return empire()->emperor().relation( name() ); }

void PlayerCity::addObject( world::ObjectPtr object )
{
  if( is_kind_of<world::Merchant>( object ) )
  {
    world::MerchantPtr merchant = ptr_cast<world::Merchant>( object );
    if( merchant->isSeaRoute() )
    {
      SeaMerchantPtr cityMerchant = ptr_cast<SeaMerchant>( SeaMerchant::create( this, merchant ) );
      cityMerchant->send2city();
    }
    else
    {
      MerchantPtr cityMerchant = ptr_cast<Merchant>( Merchant::create( this, merchant ) );
      cityMerchant->send2city();
    }
  }
  else if( is_kind_of<world::RomeChastenerArmy>( object ) )
  {
    world::RomeChastenerArmyPtr army = ptr_cast<world::RomeChastenerArmy>( object );
    for( unsigned int k=0; k < army->soldiersNumber(); k++ )
    {
      ChastenerPtr soldier = Chastener::create( this, walker::romeChasternerSoldier );
      soldier->send2City( borderInfo().roadEntry );
    }
  }
}

void PlayerCity::empirePricesChanged(Good::Type gtype, int bCost, int sCost)
{
  _d->tradeOptions.setBuyPrice( gtype, bCost );
  _d->tradeOptions.setSellPrice( gtype, sCost );
}
