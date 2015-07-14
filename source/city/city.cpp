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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "city.hpp"
#include "objects/construction.hpp"
#include "gfx/tile.hpp"
#include "pathway/path_finding.hpp"
#include "core/position.hpp"
#include "objects/objects_factory.hpp"
#include "pathway/astarpathfinding.hpp"
#include "core/safetycast.hpp"
#include "core/variant_map.hpp"
#include "gfx/tilemap.hpp"
#include "objects/road.hpp"
#include "core/time.hpp"
#include "core/variant.hpp"
#include "core/utils.hpp"
#include "walker/walkers_factory.hpp"
#include "core/gettext.hpp"
#include "build_options.hpp"
#include "game/funds.hpp"
#include "world/city.hpp"
#include "world/empire.hpp"
#include "trade_options.hpp"
#include "good/storage.hpp"
#include "world/trading.hpp"
#include "walker/merchant_land.hpp"
#include "game/gamedate.hpp"
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
#include "walker/merchant_sea.hpp"
#include "cityservice_factory.hpp"
#include "world/emperor.hpp"
#include "game/resourcegroup.hpp"
#include "world/romechastenerarmy.hpp"
#include "walker/chastener_elephant.hpp"
#include "walker/chastener.hpp"
#include "world/barbarian.hpp"
#include "objects/fort.hpp"
#include "events/showinfobox.hpp"
#include "walker/helper.hpp"
#include "walkergrid.hpp"
#include "events/showinfobox.hpp"
#include "gfx/helper.hpp"
#include "game/difficulty.hpp"
#include "active_points.hpp"
#include "game/player.hpp"
#include "scribes.hpp"
#include "statistic.hpp"
#include "states.hpp"
#include "city/states.hpp"
#include "core/flowlist.hpp"
#include "economy.hpp"
#include "city_impl.hpp"
#include "sentiment.hpp"
#include "cityservice_timers.hpp"
#include "cityservice_military.hpp"
#include "core/requirements.hpp"
#include "cityservice_prosperity.hpp"
#include "cityservice_culture.hpp"
#include "cityservice_peace.hpp"
#include "ambientsound.hpp"

#include <set>

using namespace gfx;
using namespace events;
using namespace config;

namespace config {
CAESARIA_LITERALCONST(tilemap)
static const int minimumOldFormat = 58;
}

class PlayerCity::Impl
{
public:
  city::Economy economy;  // amount of money
  city::Overlays overlays;
  city::Services services;
  city::ActivePoints activePoints;
  city::Scribes scribes;
  city::development::Options buildOptions;
  city::trade::Options tradeOptions;
  city::VictoryConditions targets;
  city::States states;
  city::Walkers walkers;
  city::Options options;

  PlayerPtr player;

  Picture empMapPicture;

  BorderInfo borderInfo;
  Tilemap tilemap;
  TilePos cameraStart;

  int sentiment;

  struct
  {
    WalkerList walkers;

    void clear()
    {
      walkers.clear();
    }
  } cached;

public:
  // collect taxes from all houses
  void monthStep( PlayerCityPtr city, const DateTime& time );
  void calculatePopulation( PlayerCityPtr city );

signals public:
  Signal1<int> onPopulationChangedSignal;
  Signal1<std::string> onWarningMessageSignal;
  Signal2<TilePos,std::string> onDisasterEventSignal;
  Signal0<> onChangeBuildingOptionsSignal;
};

PlayerCity::PlayerCity(world::EmpirePtr empire)
  :  City( empire ), _d( new Impl )
{
  _d->borderInfo.roadEntry = TilePos( 0, 0 );
  _d->borderInfo.roadExit = TilePos( 0, 0 );
  _d->borderInfo.boatEntry = TilePos( 0, 0 );
  _d->borderInfo.boatExit = TilePos( 0, 0 );
  _d->economy.resolveIssue( econ::Issue( econ::Issue::donation, 1000 ) );
  _d->states.population = 0;
  _d->economy.setTaxRate( econ::Treasury::defaultTaxPrcnt );
  _d->states.age = 0;
  _d->walkers.idCount = 1;
  _d->sentiment = city::Sentiment::defaultValue;
  _d->empMapPicture.load( ResourceGroup::empirebits, 1 );

  _d->services.initialize( this, ":/services.model" );

  setPicture( Picture( ResourceGroup::empirebits, 1 ) );
  _initAnimation();

  setOption( updateRoads, 0 );
  setOption( godEnabled, 1 );
  setOption( zoomEnabled, 1 );
  setOption( zoomInvert, 1 );
  setOption( warningsEnabled, 1 );
  setOption( fishPlaceEnabled, 1 );
  setOption( fireKoeff, 100 );
  setOption( collapseKoeff, 100 );
  setOption( barbarianAttack, 0 );
  setOption( legionAttack, 0 );
  setOption( climateType, game::climate::central );
  setOption( c3gameplay, 0 );
  setOption( highlightBuilding, 1 );
  setOption( destroyEpidemicHouses, 0 );
  setOption( difficulty, game::difficulty::usual );

  _d->states.nation = world::nation::rome;
}

void PlayerCity::_initAnimation()
{
  _animation().clear();
  _animation().load( "ourcity_anim" );
}

std::string PlayerCity::about(Object::AboutType type)
{
  std::string ret;
  switch(type)
  {
  case empireMap: ret = "##empiremap_our_city##";     break;
  case empireAdvInfo: ret = "##empiremap_our_city##"; break;
  default:        ret = "##ourcity_unknown_about##";  break;
  }

  return ret;
}

void PlayerCity::timeStep(unsigned int time)
{
  if( game::Date::isYearChanged() )
  {
    _d->states.age++;
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
  _d->cached.clear();

  _d->walkers.update( this, time );
  _d->overlays.update( this, time );
  _d->services.timeStep( this, time );
  city::Timers::instance().update( time );

  if( getOption( updateRoads ) > 0 )
  {
    setOption( updateRoads, 0 );
    // for each overlay
    for (auto it : _d->overlays)
    {
      ConstructionPtr construction = it.as<Construction>();
      if( construction != NULL )
      {
        // overlay matches the filter
        construction->computeRoadside();
      }
    }   
  }
}

void PlayerCity::Impl::monthStep( PlayerCityPtr city, const DateTime& time )
{
  economy.collectTaxes( city );
  economy.payWages( city );
  economy.payMayorSalary( city );
  economy.updateHistory( game::Date::current() );
}

const WalkerList& PlayerCity::walkers( walker::Type rtype )
{
  if( rtype == walker::all )
  {
    return _d->walkers;
  }

  _d->cached.walkers.clear();
  for( auto wlk : _d->walkers )
  {
    if (wlk->type() == rtype)
    {
      _d->cached.walkers.push_back(wlk);
    }
  }

  return _d->cached.walkers;
}

const WalkerList& PlayerCity::walkers(const TilePos& pos) { return _d->walkers.at( pos ); }
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

OverlayList&  PlayerCity::overlays()             { return _d->overlays; }
const OverlayList&PlayerCity::overlays() const   { return _d->overlays; }
city::ActivePoints& PlayerCity::activePoints()   { return _d->activePoints; }
city::Scribes &PlayerCity::scribes()             { return _d->scribes; }
const BorderInfo& PlayerCity::borderInfo() const { return _d->borderInfo; }
Picture PlayerCity::picture() const              { return _d->empMapPicture; }
bool PlayerCity::isPaysTaxes() const             { return _d->economy.getIssueValue( econ::Issue::empireTax, econ::Treasury::lastYear ) > 0; }
bool PlayerCity::haveOverduePayment() const      { return _d->economy.getIssueValue( econ::Issue::overduePayment, econ::Treasury::thisYear ) > 0; }
Tilemap& PlayerCity::tilemap()                   { return _d->tilemap; }
econ::Treasury& PlayerCity::treasury()           { return _d->economy;   }

int PlayerCity::strength() const
{
  FortList forts = city::statistic::getObjects<Fort>( const_cast<PlayerCity*>( this ) );

  int ret = 0;
  for (auto i : forts)
  {
    SoldierList soldiers = i->soldiers();
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

void PlayerCity::Impl::calculatePopulation( PlayerCityPtr city )
{
  unsigned int pop = 0;
  HouseList houseList = city::statistic::getHouses( city );

  for (auto house : houseList)
  {
    pop += house->habitants().count();
  }

  states.population = pop;
  emit onPopulationChangedSignal( pop );
}

void PlayerCity::save( VariantMap& stream) const
{
  Logger::warning( "City: create save map" );
  City::save( stream );

  Logger::warning( "City: save tilemap information");
  VariantMap vm_tilemap;
  _d->tilemap.save( vm_tilemap );

  stream[ literals::tilemap    ] = vm_tilemap;
  VARIANT_SAVE_ENUM_D( stream, _d, walkers.idCount )

  Logger::warning( "City: save main paramters ");
  stream[ "roadEntry"  ] = _d->borderInfo.roadEntry;
  stream[ "roadExit"   ] = _d->borderInfo.roadExit;
  stream[ "boatEntry"  ] = _d->borderInfo.boatEntry;
  stream[ "boatExit"   ] = _d->borderInfo.boatExit;
  VARIANT_SAVE_CLASS_D( stream, _d, options )
  VARIANT_SAVE_ANY_D( stream, _d, cameraStart )
  VARIANT_SAVE_ANY_D( stream, _d, states.population )

  Logger::warning( "City: save finance information" );
  stream[ "funds" ] = _d->economy.save();
  VARIANT_SAVE_CLASS_D( stream, _d, scribes )

  Logger::warning( "City: save trade/build/win options" );
  VARIANT_SAVE_CLASS_D( stream, _d, tradeOptions )
  VARIANT_SAVE_CLASS_D( stream, _d, buildOptions )
  stream[ "winTargets"   ] = _d->targets.save();

  Logger::warning( "City: save walkers information" );
  VariantMap vm_walkers;
  int walkedId = 0;
  for (auto w : _d->walkers)
  {
    VariantMap vm_walker;
    walker::Type wtype = walker::unknown;
    try
    {
      wtype = w->type();
      w->save( vm_walker );
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
  for (auto overlay : _d->overlays)
  {
    VariantMap vm_overlay;
    object::Type otype = object::unknown;

    try
    {
      otype = overlay->type();
      overlay->save( vm_overlay );
      auto pos = overlay->pos();
      vm_overlays[ utils::format( 0xff, "%d,%d", pos.i(), pos.j() ) ] = vm_overlay;
    }
    catch(...)
    {
      Logger::warning( "ERROR: Cant save overlay type " + object::toString( otype ) );
    }
  }
  stream[ "overlays" ] = vm_overlays;

  Logger::warning( "City: save services information" );
  VariantMap vm_services;
  for (auto service : _d->services)
  {
    vm_services[service->name() ] = service->save();
  }

  stream[ "saveFormat" ] = CAESARIA_BUILD_NUMBER;
  stream[ "services" ] = vm_services;
  VARIANT_SAVE_ANY_D( stream, _d, states.age )
  VARIANT_SAVE_CLASS_D( stream, _d, activePoints )

  Logger::warning( "City: finalize save map" );
}

void PlayerCity::load( const VariantMap& stream )
{  
  Logger::warning( "City: start parse savemap" );
  int saveFormat = stream.get( "saveFormat", minimumOldFormat );
  bool needLoadOld = saveFormat < CAESARIA_BUILD_NUMBER;

  if( needLoadOld )
  {
    Logger::warning( "!!! WARNING: Try load from format %d", saveFormat );
  }

  City::load( stream );
  _d->tilemap.load( stream.get( literals::tilemap ).toMap() );
  _d->walkers.grid.resize( Size( _d->tilemap.size() ) );
  VARIANT_LOAD_ENUM_D( _d, walkers.idCount, stream )

  Logger::warning( "City: parse main params" );
  _d->borderInfo.roadEntry = TilePos( stream.get( "roadEntry" ).toTilePos() );
  _d->borderInfo.roadExit = TilePos( stream.get( "roadExit" ).toTilePos() );
  _d->borderInfo.boatEntry = TilePos( stream.get( "boatEntry" ).toTilePos() );
  _d->borderInfo.boatExit = TilePos( stream.get( "boatExit" ).toTilePos() );  
  VARIANT_LOAD_ANY_D( _d, states.population, stream )
  VARIANT_LOAD_ANY_D( _d, cameraStart, stream )

  Logger::warning( "City: parse options" );
  VARIANT_LOAD_CLASS_D_LIST( _d, options, stream )
  setOption( PlayerCity::forceBuild, 1 );

  Logger::warning( "City: parse funds" );
  _d->economy.load( stream.get( "funds" ).toMap() );
  VARIANT_LOAD_CLASS_D( _d, scribes, stream )

  Logger::warning( "City: parse trade/build/win params" );
  VARIANT_LOAD_CLASS_D( _d, tradeOptions, stream )
  VARIANT_LOAD_CLASS_D( _d, buildOptions, stream )
  _d->targets.load( stream.get( "winTargets").toMap() );

  Logger::warning( "City: load overlays" );
  VariantMap overlays = stream.get( "overlays" ).toMap();

  for (auto item : overlays)
  {
    VariantMap overlayParams = item.second.toMap();
    VariantList config = overlayParams.get( "config" ).toList();

    object::Type overlayType = (object::Type)config.get( ovconfig::idxType ).toInt();
    TilePos pos = config.get( ovconfig::idxLocation, gfx::tilemap::invalidLocation() );

    OverlayPtr overlay = TileOverlayFactory::instance().create( overlayType );
    if( overlay.isValid() && gfx::tilemap::isValidLocation( pos ) )
    {
      city::AreaInfo info = { this, pos, TilesArray() };
      overlay->build( info );
      overlay->load( overlayParams );      
      //support old formats
      if( needLoadOld )
        overlay->debugLoadOld( saveFormat, overlayParams );

      _d->overlays.push_back( overlay );
    }
    else
    {
      Logger::warning( "City: can't load overlay " + item.first );
    }
  }

  Logger::warning( "City: parse walkers info" );
  VariantMap walkers = stream.get( "walkers" ).toMap();
  for (auto item : walkers)
  {
    VariantMap walkerInfo = item.second.toMap();
    int walkerType = (int)walkerInfo.get( "type", walker::unknown );

    WalkerPtr walker = WalkerManager::instance().create( walker::Type( walkerType ), this );
    if( walker.isValid() )
    {
      walker->load( walkerInfo );
      _d->walkers.push_back( walker );
    }
    else
    {
      Logger::warning( "City: can't load walker " + item.first );
    }
  }

  Logger::warning( "City: load service info" );
  VariantMap services = stream.get( "services" ).toMap();
  for (auto item : services)
  {
    VariantMap servicesSave = item.second.toMap();

    city::SrvcPtr srvc = findService( item.first );
    if( srvc.isNull() )
    {
      Logger::warning( "City: " + item.first + " is not basic service, try load by name" );

      srvc = city::ServiceFactory::create( this, item.first );
      if( srvc.isValid() )
      {
        Logger::warning( "City: creating service " + item.first + " directly");
        addService( srvc );
      }
    }

    if( srvc.isValid() )
    {
      srvc->load( servicesSave );
    }
    else
    {
      Logger::warning( "!!! WARNING: Can't find service " + item.first );
    }
  }

  setOption( PlayerCity::forceBuild, 0 );
  VARIANT_LOAD_ANY_D( _d, states.age, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, activePoints, stream )

  _initAnimation();
}

void PlayerCity::addOverlay( OverlayPtr overlay ) { _d->overlays.postpone( overlay ); }

PlayerCity::~PlayerCity() {}

void PlayerCity::addWalker( WalkerPtr walker )
{
  _d->walkers.postpone( walker );

  walker->setFlag( Walker::showDebugInfo, true );
}

city::SrvcPtr PlayerCity::findService( const std::string& name ) const
{
  for (auto service : _d->services)
  {
    if( name == service->name() )
      return service;
  }

  return city::SrvcPtr();
}

const city::SrvcList& PlayerCity::services() const { return _d->services; }

void PlayerCity::setBuildOptions(const city::development::Options& options)
{
  _d->buildOptions = options;
  emit _d->onChangeBuildingOptionsSignal();
}

const city::States &PlayerCity::states() const              { return _d->states; }
Signal1<std::string>& PlayerCity::onWarningMessage()        { return _d->onWarningMessageSignal; }
Signal2<TilePos,std::string>& PlayerCity::onDisasterEvent() { return _d->onDisasterEventSignal; }
Signal0<>&PlayerCity::onChangeBuildingOptions()             { return _d->onChangeBuildingOptionsSignal; }
const city::development::Options& PlayerCity::buildOptions() const { return _d->buildOptions; }
const city::VictoryConditions& PlayerCity::victoryConditions() const {   return _d->targets; }
void PlayerCity::setVictoryConditions(const city::VictoryConditions& targets) { _d->targets = targets; }
OverlayPtr PlayerCity::getOverlay( const TilePos& pos ) const { return _d->tilemap.at( pos ).overlay(); }
PlayerPtr PlayerCity::mayor() const                         { return _d->player; }
city::trade::Options& PlayerCity::tradeOptions()            { return _d->tradeOptions; }
void PlayerCity::delayTrade(unsigned int month)             {  }
const good::Store& PlayerCity::sells() const                { return _d->tradeOptions.sells(); }
const good::Store& PlayerCity::buys() const                 { return _d->tradeOptions.buys(); }
ClimateType PlayerCity::climate() const                     { return (ClimateType)getOption( PlayerCity::climateType ); }
unsigned int PlayerCity::tradeType() const                  { return world::EmpireMap::sea | world::EmpireMap::land; }
Signal1<int>& PlayerCity::onPopulationChanged()             { return _d->onPopulationChangedSignal; }
Signal1<int>& PlayerCity::onFundsChanged()                  { return _d->economy.onChange(); }
void PlayerCity::setCameraPos(const TilePos pos)            { _d->cameraStart = pos; }
TilePos PlayerCity::cameraPos() const                       { return _d->cameraStart; }
void PlayerCity::addService( city::SrvcPtr service )        { _d->services.push_back( service ); }
void PlayerCity::setOption(PlayerCity::OptionType opt, int value) { _d->options[ opt ] = value; }

int PlayerCity::prosperity() const
{
  city::ProsperityRatingPtr csPrsp;
  csPrsp << findService( city::ProsperityRating::defaultName() );
  return csPrsp.isValid() ? csPrsp->value() : 0;
}

int PlayerCity::getOption(PlayerCity::OptionType opt) const
{
  city::Options::const_iterator it = _d->options.find( opt );
  return (it != _d->options.end() ? it->second : 0 );
}

void PlayerCity::clean()
{
  for (auto it : _d->services)
  {
    it->destroy();
  }

  _d->services.clear();
  _d->walkers.clear();
  _d->overlays.clear();
  _d->tilemap.resize( 0 );
}

void PlayerCity::resize( unsigned int size)
{
  _d->tilemap.resize( size );
  _d->walkers.grid.resize( Size( size ) );
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

int PlayerCity::sentiment() const {  return _d->sentiment; }
int PlayerCity::favour() const { return empire()->emperor().relation( name() ); }

void PlayerCity::addObject( world::ObjectPtr object )
{
  if( object.is<world::Merchant>() )
  {
    world::MerchantPtr merchant = ptr_cast<world::Merchant>( object );
    if( merchant->isSeaRoute() )
    {
      SeaMerchantPtr cityMerchant = SeaMerchant::create( this, merchant );
      cityMerchant->send2city();
    }
    else
    {
      LandMerchantPtr cityMerchant = LandMerchant::create( this, merchant );
      cityMerchant->send2city();
    }
  }
  else if( object.is<world::RomeChastenerArmy>() )
  {
    world::RomeChastenerArmyPtr army = ptr_cast<world::RomeChastenerArmy>( object );
    if( !getOption( legionAttack ) )
    {
      army->killSoldiers( 100 );

      GameEventPtr e = ShowInfobox::create( _("##romechastener_attack_title##"), _("##romechastener_attack_disabled_by_player##"), true );
      e->dispatch();
      return;
    }

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

    GameEventPtr e = ShowInfobox::create( _("##romechastener_attack_title##"), _("##romechastener_attack_text##"), true );
    e->dispatch();
  }
  else if( object.is<world::Barbarian>() )
  {
    if( getOption( barbarianAttack ) > 0 )
    {
      world::BarbarianPtr brb = ptr_cast<world::Barbarian>( object );
      for( int k=0; k < brb->strength() / 2; k++ )
      {
        EnemySoldierPtr soldier = EnemySoldier::create( this, walker::etruscanSoldier );
        soldier->send2City( borderInfo().roadEntry );
        soldier->wait( game::Date::days2ticks( k ) / 2 );
      }

      GameEventPtr e = ShowInfobox::create( _("##barbarian_attack_title##"), _("##barbarian_attack_text##"), "spy_army" );
      e->dispatch();
    }
  }
  else if( object.is<world::Messenger>() )
  {
    world::MessengerPtr msm = ptr_cast<world::Messenger>( object );
    GameEventPtr e = ShowInfobox::create( msm->title(), msm->message() );
    e->dispatch();
  }
}

void PlayerCity::empirePricesChanged(good::Product gtype, const world::PriceInfo &prices)
{
  _d->tradeOptions.setBuyPrice( gtype, prices.buy );
  _d->tradeOptions.setSellPrice( gtype, prices.sell );
}
