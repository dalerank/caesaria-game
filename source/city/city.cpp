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
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "pathway/astarpathfinding.hpp"
#include "core/safetycast.hpp"
#include "city/migration.hpp"
#include "core/variant_map.hpp"
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
#include "core/utils.hpp"
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
#include "cityservice_health.hpp"
#include "cityservice_military.hpp"
#include "cityservice_peace.hpp"
#include "game/resourcegroup.hpp"
#include "world/romechastenerarmy.hpp"
#include "walker/chastener_elephant.hpp"
#include "sentiment.hpp"
#include "walker/chastener.hpp"
#include "world/barbarian.hpp"
#include "objects/fort.hpp"
#include "events/showinfobox.hpp"
#include "walker/helper.hpp"
#include "walkergrid.hpp"
#include "events/showinfobox.hpp"
#include "cityservice_fire.hpp"

#include <set>

using namespace constants;
using namespace gfx;

namespace {
CAESARIA_LITERALCONST(tilemap)
CAESARIA_LITERALCONST(walkerIdCount)
CAESARIA_LITERALCONST(adviserEnabled)
CAESARIA_LITERALCONST(fishPlaceEnabled)
}

class PlayerCity::Impl
{
public:
  typedef std::map<PlayerCity::OptionType, int> Options;
  int population;
  city::Funds funds;  // amount of money

  PlayerPtr player;

  TileOverlayList newOverlays;
  TileOverlayList overlays;

  WalkerList newWalkers;
  WalkerList walkers;

  Picture empMapPicture;

  //walkers fast access map !!!
  city::WalkerGrid walkersGrid;
  //*********************** !!!

  city::SrvcList services;
  BorderInfo borderInfo;
  Tilemap tilemap;
  TilePos cameraStart;

  city::development::Options buildOptions;
  city::trade::Options tradeOptions;
  city::VictoryConditions targets;
  Options options;
  ClimateType climate;   
  Walker::UniqueId walkerIdCount;
  unsigned int age;
  int sentiment;

public:
  // collect taxes from all houses
  void collectTaxes( PlayerCityPtr city);
  void payWages( PlayerCityPtr city );
  void monthStep( PlayerCityPtr city, const DateTime& time );
  void calculatePopulation( PlayerCityPtr city );
  void beforeOverlayDestroyed(PlayerCityPtr city, TileOverlayPtr overlay );
  void updateWalkers(unsigned int time);
  void updateOverlays( PlayerCityPtr city, unsigned int time);
  void updateServices( PlayerCityPtr city, unsigned int time );
  void resolveNewIssue( city::Funds::IssueType type );

signals public:
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
  _d->funds.setTaxRate( 7 );
  _d->age = 0;
  _d->walkerIdCount = 1;
  _d->climate = game::climate::central;
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
  addService( city::Sentiment::create( this ) );
  addService( city::Fire::create( this ) );

  setPicture( Picture::load( ResourceGroup::empirebits, 1 ) );
  _initAnimation();

  setOption( updateRoads, 0 );
  setOption( godEnabled, 1 );
  setOption( zoomEnabled, 1 );
  setOption( zoomInvert, 1 );
  setOption( warningsEnabled, 1 );
  setOption( fishPlaceEnabled, 1 );
}

void PlayerCity::_initAnimation()
{
  _animation().clear();

  _animation().load( ResourceGroup::empirebits, 2, 6 );
  _animation().setLoop( true );
  _animation().setOffset( Point( 18, -7 ));
  _animation().setDelay( 2 );
}

void PlayerCity::timeStep(unsigned int time)
{
  if( game::Date::isYearChanged() )
  {
    _d->age++;
    _d->targets.decreaseReignYear();
  }

  if( game::Date::isMonthChanged() )
  {
    _d->monthStep( this, game::Date::current() );
  }

  if( game::Date::isWeekChanged() )
  {
    _d->calculatePopulation( this );
  }

  //update walkers access map
  _d->walkersGrid.clear();
  foreach( it, _d->walkers )
  {
    _d->walkersGrid.append( *it );
  }

  _d->updateWalkers( time );
  _d->updateOverlays( this, time );
  _d->updateServices( this, time );

  if( getOption( updateRoads ) > 0 )
  {
    setOption( updateRoads, 0 );
    // for each overlay
    foreach( it, _d->overlays )
    {
      ConstructionPtr construction = ptr_cast<Construction>( *it );
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeAccessRoads();
      }
    }   
  }
}

void PlayerCity::Impl::monthStep( PlayerCityPtr city, const DateTime& time )
{
  collectTaxes( city );
  payWages( city );

  if( funds.treasury() > 0 )
  {
    int playerSalary = player->salary();
    funds.resolveIssue( FundIssue( city::Funds::playerSalary, -playerSalary ) );
    player->appendMoney( playerSalary );
  }

  funds.updateHistory( game::Date::current() );
}

WalkerList PlayerCity::walkers( walker::Type rtype )
{
  if( rtype == walker::all )
  {
    return _d->walkers;
  }

  WalkerList res;
  foreach( w, _d->walkers )
  {
    if( (*w)->type() == rtype  )
    {
      res.push_back( *w );
    }
  }

  return res;
}

const WalkerList& PlayerCity::walkers(const TilePos& pos) { return _d->walkersGrid.at( pos ); }
const WalkerList& PlayerCity::walkers() const { return _d->walkers; }

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

TileOverlayList&  PlayerCity::overlays()         { return _d->overlays; }
const BorderInfo& PlayerCity::borderInfo() const { return _d->borderInfo; }

Picture PlayerCity::picture() const { return _d->empMapPicture; }
bool PlayerCity::isPaysTaxes() const { return _d->funds.getIssueValue( city::Funds::empireTax, city::Funds::lastYear ) > 0; }
bool PlayerCity::haveOverduePayment() const { return _d->funds.getIssueValue( city::Funds::overduePayment, city::Funds::thisYear ) > 0; }
Tilemap&          PlayerCity::tilemap()          { return _d->tilemap; }
ClimateType       PlayerCity::climate() const    { return _d->climate;    }
void              PlayerCity::setClimate(const ClimateType climate) { _d->climate = climate; }
city::Funds& PlayerCity::funds()  {  return _d->funds;   }
unsigned int PlayerCity::population() const { return _d->population; }

int PlayerCity::strength() const
{
  city::Helper helper( const_cast<PlayerCity*>( this ) );
  FortList forts = helper.find<Fort>( objects::any );

  int ret = 0;
  foreach( i, forts )
  {
    SoldierList soldiers = (*i)->soldiers();
    ret += soldiers.size();
  }

  return ret;
}

DateTime PlayerCity::lastAttack() const
{
  city::MilitaryPtr mil;
  mil << findService( city::Military::defaultName() );

  return mil.isValid() ? mil->lastAttack() : DateTime( -350, 0, 0 );
}

world::Nation PlayerCity::nation() const { return world::rome; }

void PlayerCity::Impl::collectTaxes(PlayerCityPtr city )
{
  city::Helper hlp( city );
  float lastMonthTax = 0;
  
  ForumList forums = hlp.find< Forum >( objects::forum );
  foreach( forum, forums ) { lastMonthTax += (*forum)->collectTaxes(); }

  SenateList senates = hlp.find< Senate >( objects::senate );
  foreach( senate, senates ) { lastMonthTax += (*senate)->collectTaxes(); }

  funds.resolveIssue( FundIssue( city::Funds::taxIncome, lastMonthTax ) );
}

void PlayerCity::Impl::payWages(PlayerCityPtr city)
{
  int wages = city::statistic::getMonthlyWorkersWages( city );

  if( funds.haveMoneyForAction( wages ) )
  {
    //funds.resolveIssue( FundIssue( city::Funds::workersWages, -wages ) );
    HouseList houses;
    houses << city->overlays();

    float salary = city::statistic::getMonthlyOneWorkerWages( city );
    float wages = 0;
    foreach( it, houses )
    {
      int workers = (*it)->workersCount();
      float house_wages = salary * workers;
      (*it)->appendMoney( house_wages );
      wages += house_wages;
    }
    funds.resolveIssue( FundIssue( city::Funds::workersWages, ceil( -wages ) ) );
  }
  else
  {

  }
}

void PlayerCity::Impl::calculatePopulation( PlayerCityPtr city )
{
  unsigned int pop = 0;

  city::Helper helper( city );

  HouseList houseList = helper.find<House>( objects::house );

  foreach( house, houseList) { pop += (*house)->habitants().count(); }
  
  population = pop;
  emit onPopulationChangedSignal( pop );
}

void PlayerCity::Impl::beforeOverlayDestroyed(PlayerCityPtr city, TileOverlayPtr overlay)
{
  city::Helper helper( city );
  helper.updateDesirability( overlay, city::Helper::offDesirability );
}

void PlayerCity::Impl::updateWalkers( unsigned int time )
{
  WalkerList::iterator walkerIt = walkers.begin();
  while( walkerIt != walkers.end() )
  {
    WalkerPtr walker = *walkerIt;
    walker->timeStep( time );
    if( walker->isDeleted() )
    {
      // remove the walker from the walkers list
      walkersGrid.remove( *walkerIt );
      walkerIt = walkers.erase(walkerIt);
    }
    else { ++walkerIt; }
  }
  walkers << newWalkers;
  newWalkers.clear();
}

void PlayerCity::Impl::updateOverlays( PlayerCityPtr city, unsigned int time )
{
  TileOverlayList::iterator overlayIt = overlays.begin();
  while( overlayIt != overlays.end() )
  {
    (*overlayIt)->timeStep( time );

    if( (*overlayIt)->isDeleted() )
    {
      beforeOverlayDestroyed( city, *overlayIt );
      // remove the overlay from the overlay list
      (*overlayIt)->destroy();
      overlayIt = overlays.erase(overlayIt);
    }
    else
    {
      ++overlayIt;
    }
  }

  overlays << newOverlays;
  newOverlays.clear();
}

void PlayerCity::Impl::updateServices( PlayerCityPtr city, unsigned int time)
{
  city::SrvcList::iterator serviceIt = services.begin();
  city::Timers::instance().update( time );
  while( serviceIt != services.end() )
  {
    (*serviceIt)->timeStep( time );

    if( (*serviceIt)->isDeleted() )
    {
      (*serviceIt)->destroy();
      serviceIt = services.erase(serviceIt);
    }
    else { ++serviceIt; }
    }
}

void PlayerCity::Impl::resolveNewIssue(city::Funds::IssueType type)
{
  switch( type )
  {
  case city::Funds::overdueEmpireTax:
    {
      int lastYearBrokenTribute = funds.getIssueValue( city::Funds::overdueEmpireTax, city::Funds::lastYear );
      std::string text = lastYearBrokenTribute > 0
                                ? "##for_second_year_broke_tribute##"
                                : "##current_year_notpay_tribute_warning##";
      events::GameEventPtr e = events::ShowInfobox::create( "##tribute_broken_title##",
                                                            text );
      e->dispatch();
    }
  break;

  default:
  break;

  }
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
  stream[ lc_fishPlaceEnabled ] = getOption( PlayerCity::fishPlaceEnabled );
  stream[ "godEnabled" ] = getOption( PlayerCity::godEnabled );
  stream[ "zoomEnabled"] = getOption( PlayerCity::zoomEnabled );
  stream[ "zoomInvert"] = getOption( PlayerCity::zoomInvert );
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
  foreach( w, _d->walkers )
  {
    VariantMap vm_walker;
    walker::Type wtype = walker::unknown;
    try
    {
      wtype = (*w)->type();
      (*w)->save( vm_walker );
      vm_walkers[ utils::format( 0xff, "%d", walkedId ) ] = vm_walker;
    }
    catch(...)
    {
      Logger::warning( "ERROR: Cant save walker type " + WalkerHelper::getTypename( wtype ) );
    }

    walkedId++;
  }
  stream[ "walkers" ] = vm_walkers;

  Logger::warning( "City: save overlays information" );
  VariantMap vm_overlays;
  foreach( overlay, _d->overlays )
  {
    VariantMap vm_overlay;
    TileOverlay::Type otype = objects::unknown;

    try
    {
      otype = (*overlay)->type();
      (*overlay)->save( vm_overlay );
      vm_overlays[ utils::format( 0xff, "%d,%d", (*overlay)->pos().i(),
                                                 (*overlay)->pos().j() ) ] = vm_overlay;
    }
    catch(...)
    {
      Logger::warning( "ERROR: Cant save overlay type " + MetaDataHolder::findTypename( otype ) );
    }
  }
  stream[ "overlays" ] = vm_overlays;

  Logger::warning( "City: save services information" );
  VariantMap vm_services;
  foreach( service, _d->services )
  {   
    vm_services[ (*service)->name() ] = (*service)->save();
  }

  stream[ "services" ] = vm_services;
  VARIANT_SAVE_ANY_D( stream, _d, age )

  Logger::warning( "City: finalize save map" );
}

void PlayerCity::load( const VariantMap& stream )
{  
  Logger::warning( "City: start parse savemap" );
  City::load( stream );
  _d->tilemap.load( stream.get( lc_tilemap ).toMap() );
  _d->walkersGrid.resize( Size( _d->tilemap.size() ) );
  _d->walkerIdCount = (Walker::UniqueId)stream.get( lc_walkerIdCount ).toUInt();
  setOption( PlayerCity::forceBuild, 1 );

  Logger::warning( "City: parse main params" );
  _d->borderInfo.roadEntry = TilePos( stream.get( "roadEntry" ).toTilePos() );
  _d->borderInfo.roadExit = TilePos( stream.get( "roadExit" ).toTilePos() );
  _d->borderInfo.boatEntry = TilePos( stream.get( "boatEntry" ).toTilePos() );
  _d->borderInfo.boatExit = TilePos( stream.get( "boatExit" ).toTilePos() );  
  _d->climate = (ClimateType)stream.get( "climate" ).toInt(); 
  _d->population = (int)stream.get( "population", 0 );
  _d->cameraStart = TilePos( stream.get( "cameraStart" ).toTilePos() );

  Logger::warning( "City: parse options" );
  setOption( adviserEnabled, stream.get( lc_adviserEnabled, 1 ) );
  setOption( fishPlaceEnabled, stream.get( lc_fishPlaceEnabled, 1 ) );
  setOption( godEnabled, stream.get( "godEnabled", 1 ) );
  setOption( zoomEnabled, stream.get( "zoomEnabled", 1 ) );
  setOption( zoomInvert, stream.get( "zoomInvert", 1 ) );

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
      CityAreaInfo info = { this, pos, TilesArray() };
      overlay->build( info );
      overlay->load( overlayParams );
      _d->overlays.push_back( overlay );
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
      _d->walkers.push_back( walker );
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

      srvc = city::ServiceFactory::create( this, item->first );
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

  setOption( PlayerCity::forceBuild, 0 );
  VARIANT_LOAD_ANY_D( _d, age, stream )

  _initAnimation();
}

void PlayerCity::addOverlay( TileOverlayPtr overlay ) { _d->newOverlays.push_back( overlay ); }

PlayerCity::~PlayerCity() {}

void PlayerCity::addWalker( WalkerPtr walker )
{
  walker->setUniqueId( ++_d->walkerIdCount );
  _d->newWalkers.push_back( walker );

  walker->setFlag( Walker::showDebugInfo, true );
}

city::SrvcPtr PlayerCity::findService( const std::string& name ) const
{
  foreach( service, _d->services )
  {
    if( name == (*service)->name() )
      return *service;
  }

  return city::SrvcPtr();
}

const city::SrvcList& PlayerCity::services() const { return _d->services; }

void PlayerCity::setBuildOptions(const city::development::Options& options)
{
  _d->buildOptions = options;
  emit _d->onChangeBuildingOptionsSignal();
}

unsigned int PlayerCity::age() const { return _d->age; }
Signal1<std::string>& PlayerCity::onWarningMessage() { return _d->onWarningMessageSignal; }
Signal2<TilePos,std::string>& PlayerCity::onDisasterEvent() { return _d->onDisasterEventSignal; }
Signal0<>&PlayerCity::onChangeBuildingOptions(){ return _d->onChangeBuildingOptionsSignal; }
const city::development::Options& PlayerCity::buildOptions() const { return _d->buildOptions; }
const city::VictoryConditions& PlayerCity::victoryConditions() const {   return _d->targets; }
void PlayerCity::setVictoryConditions(const city::VictoryConditions& targets) { _d->targets = targets; }
TileOverlayPtr PlayerCity::getOverlay( const TilePos& pos ) const { return _d->tilemap.at( pos ).overlay(); }
PlayerPtr PlayerCity::player() const { return _d->player; }

city::trade::Options& PlayerCity::tradeOptions() { return _d->tradeOptions; }
void PlayerCity::delayTrade(unsigned int month){  }

const good::Store& PlayerCity::importingGoods() const {   return _d->tradeOptions.importingGoods(); }
const good::Store& PlayerCity::exportingGoods() const {   return _d->tradeOptions.exportingGoods(); }
unsigned int PlayerCity::tradeType() const { return world::EmpireMap::sea | world::EmpireMap::land; }

Signal1<int>& PlayerCity::onPopulationChanged() {  return _d->onPopulationChangedSignal; }
Signal1<int>& PlayerCity::onFundsChanged() {  return _d->funds.onChange(); }
void PlayerCity::setCameraPos(const TilePos pos) { _d->cameraStart = pos; }
TilePos PlayerCity::cameraPos() const {return _d->cameraStart; }
void PlayerCity::addService( city::SrvcPtr service ) {  _d->services.push_back( service ); }
void PlayerCity::setOption(PlayerCity::OptionType opt, int value) { _d->options[ opt ] = value; }

int PlayerCity::prosperity() const
{
  city::ProsperityRatingPtr csPrsp;
  csPrsp << findService( city::ProsperityRating::defaultName() );
  return csPrsp.isValid() ? csPrsp->value() : 0;
}

int PlayerCity::getOption(PlayerCity::OptionType opt) const
{
  Impl::Options::const_iterator it = _d->options.find( opt );
  return (it != _d->options.end() ? it->second : 0 );
}

void PlayerCity::clean()
{
  foreach( it, _d->services )
  {
    (*it)->destroy();
  }

  _d->services.clear();

  _d->walkers.clear();
  _d->walkersGrid.clear();
  _d->overlays.clear();
  _d->tilemap.resize( 0 );
}

void PlayerCity::resize( unsigned int size)
{
  _d->tilemap.resize( size );
  _d->walkersGrid.resize( Size( size ) );
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
  city::CultureRatingPtr csClt;
  csClt << findService( city::CultureRating::defaultName() );
  return csClt.isValid() ? csClt->value() : 0;
}

int PlayerCity::peace() const
{
  city::PeacePtr p;
  p << findService( city::Peace::defaultName() );
  return p.isValid() ? p->value() : 0;
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
      ChastenerPtr soldier = Chastener::create( this, walker::romeChastenerSoldier );
      soldier->send2City( borderInfo().roadEntry );
      soldier->wait( game::Date::days2ticks( k ) / 2 );
      if( (k % 16) == 15 )
      {
        ChastenerElephantPtr elephant = ChastenerElephant::create( this );
        elephant->send2City( borderInfo().roadEntry );
        soldier->wait( game::Date::days2ticks( k ) );
      }
    }

    events::GameEventPtr e = events::ShowInfobox::create( "##romechastener_attack_title##", "##romechastener_attack_text##", true );
    e->dispatch();
  }
  else if( is_kind_of<world::Barbarian>( object ) )
  {
    world::BarbarianPtr brb = ptr_cast<world::Barbarian>( object );
    for( int k=0; k < brb->strength() / 2; k++ )
    {
      EnemySoldierPtr soldier = EnemySoldier::create( this, walker::etruscanSoldier );
      soldier->send2City( borderInfo().roadEntry );
      soldier->wait( game::Date::days2ticks( k ) / 2 );
    }

    events::GameEventPtr e = events::ShowInfobox::create( "##barbarian_attack_title##", "##barbarian_attack_text##", "/smk/spy_army.smk" );
    e->dispatch();
  }
  else if( is_kind_of<world::Messenger>( object ) )
  {
    world::MessengerPtr msm = ptr_cast<world::Messenger>( object );
    events::GameEventPtr e = events::ShowInfobox::create( msm->title(), msm->message() );
    e->dispatch();
  }
}

void PlayerCity::empirePricesChanged(good::Product gtype, int bCost, int sCost)
{
  _d->tradeOptions.setBuyPrice( gtype, bCost );
  _d->tradeOptions.setSellPrice( gtype, sCost );
}
