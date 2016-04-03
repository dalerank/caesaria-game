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
#include <GameObjects>
#include <GameGfx>
#include <GamePathway>
#include <GameCore>
#include <GameWalkers>
#include <GameCity>
#include <GameApp>
#include <GameWorld>
#include <GameGood>
#include <GameEvents>
#include <GameLogger>

#include "walkergrid.hpp"
#include "active_points.hpp"
#include "economy.hpp"
#include "city_impl.hpp"
#include "ambientsound.hpp"

#include <set>

using namespace gfx;
using namespace events;
using namespace config;

static SimpleLogger LOG_CITY( "City" );
typedef std::map<PlayerCity::TileType, Tile*> TileTypeMap;

namespace config {
GAME_LITERALCONST(tilemap)
static const int minimumOldFormat = 58;
}

class PlayerCity::Impl
{
public:
  city::Economy funds;  // amount of money
  city::Overlays overlays;
  city::Services services;
  city::ActivePoints activePoints;
  city::Scribes scribes;
  city::development::Options buildOptions;
  city::trade::Options tradeOptions;
  city::VictoryConditions winTargets;
  city::States states;
  city::Walkers walkers;
  city::Options options;
  ScopedPtr<city::Statistic> statistic;

  PlayerPtr player;

  Picture empMapPicture;

  TileTypeMap border;
  Tilemap tilemap;
  TilePos cameraStart;

  int sentiment;

public:
  // collect taxes from all houses
  void monthStep( PlayerCityPtr city, const DateTime& time );
  void calculatePopulation();

 struct {
  Signal1<std::string> onWarningMessage;
  Signal2<TilePos,std::string> onDisasterEvent;
  Signal0<> onBuildingOptionsChanged;
 } signal;
};

PlayerCity::PlayerCity(world::EmpirePtr empire)
  :  City( empire ), _d( new Impl )
{
  LOG_CITY.warn( "Start initialize" );

  setBorderInfo( roadEntry, TilePos( 0, 0 ) );
  setBorderInfo( roadExit, TilePos( 0, 0 ) );
  setBorderInfo( boatEntry, TilePos( 0, 0 ) );
  setBorderInfo( boatExit, TilePos( 0, 0 ) );

  _d->funds.resolveIssue( econ::Issue( econ::Issue::donation, 1000 ) );
  _d->states.population = 0;
  _d->states.birth = game::Date::current();
  _d->funds.setTaxRate( econ::Treasury::defaultTaxPrcnt );
  _d->states.age = 0;
  _d->statistic.createInstance( *this );
  _d->walkers.idCount = 1;
  _d->sentiment = city::Sentiment::defaultValue;
  _d->empMapPicture.load( ResourceGroup::empirebits, 1 );

  _d->services.initialize( this, ":/services.model" );

  setOption( updateRoadsOnNextFrame, 0 );
  setOption( godEnabled, 1 );
  setOption( zoomEnabled, 1 );
  setOption( zoomInvert, OSystem::isMac() ? 1 : 0 );
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
  setOption( forestFire, 1 );
  setOption( showGodsUnhappyWarn, 1 );
  setOption( forestGrow, 1 );
  setOption( warfNeedTimber, 1 );
  setOption( riversideAsWell, 1 );
  setOption( soldiersHaveSalary, 1 );
  setOption( housePersonalTaxes, 1 );
  setOption( ironInRocks, 1 );
  setOption( farmUseMeadows, 1 );

  _setNation( world::nation::roman );
}

std::string PlayerCity::about(Object::AboutType type)
{
  std::string ret;
  switch(type)
  {
  case aboutEmpireMap: ret = "##empiremap_our_city##";     break;
  case aboutEmpireAdvInfo: ret = "##empiremap_our_city##"; break;
  case aboutEmtype: ret = "world_ourcity"; break;
  default:        ret = "##ourcity_unknown_about##";  break;
  }

  return ret;
}

void PlayerCity::timeStep(unsigned int time)
{
  if( game::Date::isYearChanged() )
  {
    _d->states.age++;
    _d->winTargets.decreaseReignYear();
  }

  if( game::Date::isMonthChanged() )
  {
    _d->monthStep( this, game::Date::current() );
  }

  if( game::Date::isWeekChanged() )
  {
    _d->calculatePopulation();
  }

  //update walkers access map
  _d->statistic->update( time );
  _d->walkers.update( this, time );
  _d->overlays.update( this, time );
  _d->services.update( this, time );
  city::Timers::instance().update( time );

  if( getOption( updateRoadsOnNextFrame ) > 0 )
  {
    setOption( updateRoadsOnNextFrame, 0 );
    _d->overlays.recalcRoadAccess();
  }
}

void PlayerCity::Impl::monthStep( PlayerCityPtr city, const DateTime& time )
{
  funds.collectTaxes( city );
  funds.payWages( city );
  funds.payMayorSalary( city );
  funds.updateHistory( game::Date::current() );
}

void PlayerCity::Impl::calculatePopulation()
{
  states.population = statistic->population.current();
}

const WalkerList& PlayerCity::walkers(const TilePos& pos) { return _d->walkers.at( pos ); }
const WalkerList& PlayerCity::walkers() const { return _d->walkers; }

void PlayerCity::setBorderInfo(TileType type, const TilePos& pos)
{
  _d->border[ type ] = &_d->tilemap.at( pos );
}

const Tile& PlayerCity::getBorderInfo( TileType type ) const
{
  auto it = _d->border.find( type );
  Tile* tile = it != _d->border.end() ? it->second : nullptr;
  return tile ? *tile : tile::getInvalid();
}

const OverlayList& PlayerCity::overlays() const  { return _d->overlays; }
city::ActivePoints& PlayerCity::activePoints()   { return _d->activePoints; }
city::Scribes& PlayerCity::scribes()             { return _d->scribes; }
Picture PlayerCity::picture() const              { return _d->empMapPicture; }
bool PlayerCity::isPaysTaxes() const             { return _d->funds.getIssueValue( econ::Issue::empireTax, econ::Treasury::lastYear ) > 0; }
bool PlayerCity::haveOverduePayment() const      { return _d->funds.getIssueValue( econ::Issue::overduePayment, econ::Treasury::thisYear ) > 0; }
Tilemap& PlayerCity::tilemap()                   { return _d->tilemap; }
econ::Treasury& PlayerCity::treasury()           { return _d->funds; }

int PlayerCity::strength() const
{
  return statistic().objects
                    .find<Fort>()
                    .summ<int>(0, [](FortPtr f) { return f->soldiers_n(); } );
}

DateTime PlayerCity::lastAttack() const
{
  city::MilitaryPtr mil = statistic().services.find<city::Military>();
  return mil.isValid() ? mil->lastAttack() : DateTime( -350, 0, 0 );
}

void PlayerCity::save( VariantMap& stream) const
{
  LOG_CITY.info( "Create save map" );
  City::save( stream );

  LOG_CITY.info( "Save tilemap information" );
  VariantMap vm_tilemap;
  _d->tilemap.save( vm_tilemap );

  stream[ literals::tilemap    ] = vm_tilemap;
  VARIANT_SAVE_ENUM_D( stream, _d, walkers.idCount )

  LOG_CITY.info( "Save main paramters " );
  stream[ "roadEntry"  ] = getBorderInfo( PlayerCity::roadEntry ).epos();
  stream[ "roadExit"   ] = getBorderInfo( PlayerCity::roadExit ).epos();
  stream[ "boatEntry"  ] = getBorderInfo( PlayerCity::boatEntry ).epos();
  stream[ "boatExit"   ] = getBorderInfo( PlayerCity::boatExit ).epos();
  VARIANT_SAVE_CLASS_D( stream, _d, options )
  VARIANT_SAVE_ANY_D( stream, _d, cameraStart )
  VARIANT_SAVE_ANY_D( stream, _d, states.population )

  LOG_CITY.info( "Save finance information" );
  VARIANT_SAVE_CLASS_D( stream, _d, funds )
  VARIANT_SAVE_CLASS_D( stream, _d, scribes )

  LOG_CITY.info( "Save trade/build/win options" );
  VARIANT_SAVE_CLASS_D( stream, _d, tradeOptions )
  VARIANT_SAVE_CLASS_D( stream, _d, buildOptions )
  VARIANT_SAVE_CLASS_D( stream, _d, winTargets )

  LOG_CITY.info( "Save walkers information" );
  VARIANT_SAVE_CLASS_D( stream, _d, walkers )

  LOG_CITY.info( "Save overlays information" );
  VariantMap vm_overlays;
  for( auto overlay : _d->overlays )
  {
    VariantMap vm_overlay;
    object::Type otype = object::typeOrDefault( overlay );

    try
    {
      overlay->save( vm_overlay );
      auto pos = overlay->pos();
      vm_overlays[ fmt::format( "{},{}", pos.i(), pos.j() ) ] = vm_overlay;
    }
    catch(...)
    {
      LOG_CITY.error( "Can't save overlay type " + object::toString( otype ));
    }
  }
  stream[ "overlays" ] = vm_overlays;

  LOG_CITY.info( "Save services information" );
  VariantMap vm_services;
  for (auto service : _d->services)
  {
    vm_services[service->name() ] = service->save();
  }

  stream[ "saveFormat" ] = GAME_BUILD_NUMBER;
  stream[ "services" ] = vm_services;
  VARIANT_SAVE_ANY_D( stream, _d, states.age )
  VARIANT_SAVE_ANY_D( stream, _d, states.birth )
  VARIANT_SAVE_CLASS_D( stream, _d, activePoints )

  LOG_CITY.info( "Finalize save map" );
}

void PlayerCity::load( const VariantMap& stream )
{
  LOG_CITY.info( "Start parse savemap" );

  City::load( stream );
  _d->tilemap.load( stream.get( literals::tilemap ).toMap() );
  _d->walkers.grid.resize( Size::square( _d->tilemap.size() ) );
  VARIANT_LOAD_ENUM_D( _d, walkers.idCount, stream)

  LOG_CITY.info( "Parse main params" );
  setBorderInfo( roadEntry, stream.get( "roadEntry" ) );
  setBorderInfo( roadExit, stream.get( "roadExit" ) );
  setBorderInfo( boatEntry,stream.get( "boatEntry" ) );
  setBorderInfo( boatExit, stream.get( "boatExit" ) );
  VARIANT_LOAD_ANY_D( _d, states.population, stream )
  VARIANT_LOAD_ANY_D( _d, cameraStart, stream )
  VARIANT_LOAD_TIME_D( _d, states.birth, stream )

  LOG_CITY.info( "Parse options" );
  VARIANT_LOAD_CLASS_D_LIST( _d, options, stream )

  LOG_CITY.info( "Parse funds" );
  VARIANT_LOAD_CLASS_D( _d, funds, stream )
  VARIANT_LOAD_CLASS_D( _d, scribes, stream )

  LOG_CITY.info( "Parse trade/build/win params" );
  VARIANT_LOAD_CLASS_D( _d, tradeOptions, stream )
  VARIANT_LOAD_CLASS_D( _d, buildOptions, stream )
  _d->winTargets.load(stream.get( "winTargets").toMap());

  LOG_CITY.info( "Load overlays" );
  VariantMap vmOverlays = stream.get( "overlays" ).toMap();

  for( const auto& item : vmOverlays)
  {
    VariantMap overlayParams = item.second.toMap();
    VariantList config = overlayParams.get( "config" ).toList();

    object::Type overlayType = (object::Type)config.get(ovconfig::idxType).toInt();
    TilePos pos = config.get( ovconfig::idxLocation, TilePos::invalid() );

    auto overlay = Overlay::create( overlayType );
    if( overlay.isValid() && config::tilemap.isValidLocation( pos ) )
    {
      city::AreaInfo info(this, pos);
      info.onload = true;

      overlay->build(info);
      overlay->load(overlayParams);
      _d->overlays.push_back(overlay);
    }
    else
    {
      LOG_CITY.warn( "Can't load overlay " + item.first );
    }
  }

  for( auto overlay : _d->overlays )
  {
    overlay->afterLoad();
  }

  LOG_CITY.info( "Parse walkers info" );
  VariantMap walkers = stream.get( "walkers" ).toMap();
  for( const auto& item : walkers)
  {
    VariantMap walkerInfo = item.second.toMap();
    walker::Type walkerType = walkerInfo.get( "type", (int)walker::unknown ).toEnum<walker::Type>();

    WalkerPtr walker = Walker::create( walkerType, this );
    if( walker.isValid() )
    {
      walker->load( walkerInfo );
      _d->walkers.push_back( walker );
    }
    else
    {
      LOG_CITY.warn( "Can't load walker " + item.first );
    }
  }

  LOG_CITY.info( "Load service info" );
  VariantMap services = stream.get( "services" ).toMap();
  for( const auto& item : services)
  {
    VariantMap servicesSave = item.second.toMap();

    city::SrvcPtr srvc = findService( item.first );
    if( srvc.isNull() )
    {
      LOG_CITY.warn( "'" + item.first + "' is not basic service, trying to load by name" );

      srvc = city::ServiceFactory::create( this, item.first );
      if( srvc.isValid() )
      {
        LOG_CITY.warn( "Creating service '" + item.first + "' directly" );
        addService( srvc );
      }
    }

    if( srvc.isValid() )
    {
      srvc->load( servicesSave );
    }
    else
    {
      LOG_CITY.warn( "Can't find service '" + item.first + "'" );
    }
  }

  setOption( PlayerCity::constructorMode, 0 );
  VARIANT_LOAD_ANY_D( _d, states.age, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, activePoints, stream )
}

void PlayerCity::addOverlay( OverlayPtr overlay ) { _d->overlays.postpone( overlay ); }

PlayerCity::~PlayerCity(){}

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
  emit _d->signal.onBuildingOptionsChanged();
}

bool PlayerCity::getBuildOption(const std::string& name, bool) const
{
  object::Type vtype = object::findType(name);
  return _d->buildOptions.isBuildingAvailable(vtype);
}

void PlayerCity::setBuildOption(const std::string& name, int value)
{
  object::Type vtype = object::findType(name);
  _d->buildOptions.setBuildingAvailable(vtype, value>0);
}

Signal1<std::string>& PlayerCity::onWarningMessage()        { return _d->signal.onWarningMessage; }
Signal2<TilePos,std::string>& PlayerCity::onDisasterEvent() { return _d->signal.onDisasterEvent; }
Signal0<>& PlayerCity::onChangeBuildingOptions()             { return _d->signal.onBuildingOptionsChanged; }
const city::development::Options& PlayerCity::buildOptions() const { return _d->buildOptions; }
const city::VictoryConditions& PlayerCity::victoryConditions() const {   return _d->winTargets; }
void PlayerCity::setVictoryConditions(const city::VictoryConditions& targets) { _d->winTargets = targets; }
OverlayPtr PlayerCity::getOverlay( const TilePos& pos ) const { return _d->tilemap.at( pos ).overlay(); }
gfx::Tile& PlayerCity::getTile(const TilePos& pos) const { return _d->tilemap.at(pos); }
PlayerPtr PlayerCity::mayor() const                         { return _d->player; }
city::trade::Options& PlayerCity::tradeOptions()            { return _d->tradeOptions; }
void PlayerCity::delayTrade(unsigned int month)             {  }
const good::Store& PlayerCity::sells() const                { return _d->tradeOptions.sells(); }
const good::Store& PlayerCity::buys() const                 { return _d->tradeOptions.buys(); }
ClimateType PlayerCity::climate() const                     { return _d->tilemap.climate(); }
unsigned int PlayerCity::tradeType() const                  { return world::EmpireMap::trSea | world::EmpireMap::trLand; }
void PlayerCity::setCameraPos(const TilePos pos)            { _d->cameraStart = pos; }
const TilePos& PlayerCity::cameraPos() const                       { return _d->cameraStart; }
void PlayerCity::addService( city::SrvcPtr service )        { _d->services.push_back( service ); }

const city::States &PlayerCity::states() const
{
  _d->states.money = _d->funds.money();
  _d->states.favor = empire()->emperor().relation(name()).value();
  return _d->states;
}

void PlayerCity::setOption(PlayerCity::OptionType opt, int value)
{
  _d->options[ opt ] = value;
  if( opt == c3gameplay )
  {
    if( value )
      events::dispatch<WarningMessage>( "WARNING! Enabled C3 gameplay only!", WarningMessage::negative );

    _d->options[ warfNeedTimber ] = !value;
    _d->options[ forestFire ] = !value;
    _d->options[ forestGrow ] = !value;
    _d->options[ destroyEpidemicHouses ] = !value;
    _d->options[ riversideAsWell ] = !value;
    _d->options[ soldiersHaveSalary ] = !value;
    _d->options[ housePersonalTaxes ] = !value;
    _d->options[ cutForest2timber ] = !value;
    _d->options[ ironInRocks      ] = !value;
  }
  else if( opt == svkBorderEnabled )
  {
    _d->tilemap.setSvkBorderEnabled(value>0);
  }
  else if( opt == climateType )
  {
    _d->tilemap.setClimate( (ClimateType)value );
  }
}

void PlayerCity::setOption(const std::string& name, int value)
{
  OptionType type = city::findOption(name);
  setOption(type, value);
}

int PlayerCity::prosperity() const
{
  return statistic().services.value<city::ProsperityRating>();
}

int PlayerCity::getOption(PlayerCity::OptionType opt) const
{
  auto it = _d->options.find( opt );
  return (it != _d->options.end() ? it->second : 0 );
}

int PlayerCity::getOption(const std::string& optname,bool) const
{
  OptionType type = city::findOption(optname);
  return getOption(type);
}

Variant PlayerCity::getProperty(const std::string& name) const
{
  if (name == "roadExit") return getBorderInfo(roadExit).pos();
  if (name == "roadEntry") return getBorderInfo(roadEntry).pos();
  if (name == "boatEntry") return getBorderInfo(boatEntry).pos();

  return Variant();
}

void PlayerCity::clean()
{
  _d->services.destroyAll();
  _d->services.clear();
  _d->walkers.clear();
  city::Timers::instance().reset();
  _d->overlays.clear();
  _d->tilemap.resize( 0 );
}

void PlayerCity::resize( unsigned int size)
{
  _d->tilemap.resize( size );
  _d->walkers.grid.resize( Size::square( size ) );
}

const city::Statistic& PlayerCity::statistic() const { return *_d->statistic; }

PlayerCityPtr PlayerCity::create( world::EmpirePtr empire, PlayerPtr player )
{
  PlayerCityPtr ret( new PlayerCity( empire ) );
  ret->_d->player = player;
  ret->drop();

  return ret;
}

int PlayerCity::culture() const { return statistic().services.value<city::CultureRating>(); }
int PlayerCity::peace() const { return statistic().services.value<city::Peace>(); }

int PlayerCity::sentiment() const {  return _d->sentiment; }

void PlayerCity::addObject( world::ObjectPtr object )
{
  if( object.is<world::Merchant>() )
  {
    world::MerchantPtr merchant = ptr_cast<world::Merchant>( object );
    if( merchant->isSeaRoute() )
    {
      SeaMerchantPtr cityMerchant = Walker::create<SeaMerchant>( this, merchant );
      cityMerchant->send2city();
    }
    else
    {
      LandMerchantPtr cityMerchant = Walker::create<LandMerchant>( this, merchant );
      cityMerchant->send2city();
    }
  }
  else if( object.is<world::RomeChastenerArmy>() )
  {
    world::RomeChastenerArmyPtr army = ptr_cast<world::RomeChastenerArmy>( object );
    if( !getOption( legionAttack ) )
    {
      army->killSoldiers( 100 );

      events::dispatch<ShowInfobox>( _("##romechastener_attack_title##"), _("##romechastener_attack_disabled_by_player##"), true );
      return;
    }

    for( unsigned int k=0; k < army->soldiersNumber(); k++ )
    {
      ChastenerPtr soldier = Walker::create<Chastener>( this, walker::romeChastenerSoldier );
      soldier->send2City( getBorderInfo( roadEntry ).epos() );
      soldier->wait( game::Date::days2ticks( k ) / 2 );
      if( (k % 16) == 15 )
      {
        ChastenerElephantPtr elephant = Walker::create<ChastenerElephant>( this );
        elephant->send2City( getBorderInfo( roadEntry ).epos() );
        soldier->wait( game::Date::days2ticks( k ) );
      }
    }

    events::dispatch<ShowInfobox>( _("##romechastener_attack_title##"), _("##romechastener_attack_text##"), true );
  }
  else if( object.is<world::Barbarian>() )
  {
    if( getOption( barbarianAttack ) > 0 )
    {
      world::BarbarianPtr brb = ptr_cast<world::Barbarian>( object );
      for( int k=0; k < brb->strength() / 2; k++ )
      {
        EnemySoldierPtr soldier = Walker::create<EnemySoldier>( this, walker::etruscanSoldier );
        soldier->send2City( getBorderInfo( roadEntry ).epos() );
        soldier->wait( game::Date::days2ticks( k ) / 2 );
      }

      std::string title = _("##barbarian_attack_title##");
      std::string text = _("##barbarian_attack_text##");
      std::string video = "spy_army";
      events::dispatch<ShowInfobox>(title, text, true, video);
    }
  }
  else if( object.is<world::Messenger>() )
  {
    world::MessengerPtr msm = ptr_cast<world::Messenger>( object );
    events::dispatch<ShowInfobox>( msm->title(), msm->message() );
  }
}

void PlayerCity::empirePricesChanged(good::Product gtype, const world::PriceInfo &prices)
{
  _d->tradeOptions.setBuyPrice( gtype, prices.buy );
  _d->tradeOptions.setSellPrice( gtype, prices.sell );
}
