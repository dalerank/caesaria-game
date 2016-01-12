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

#include "empire.hpp"
#include "computer_city.hpp"
#include "core/variant.hpp"
#include "core/time.hpp"
#include "core/saveadapter.hpp"
#include "core/utils.hpp"
#include "trading.hpp"
#include "objects/construction.hpp"
#include "core/logger.hpp"
#include "traderoute.hpp"
#include "object.hpp"
#include "rome.hpp"
#include "events/showinfobox.hpp"
#include "empiremap.hpp"
#include "emperor.hpp"
#include "game/player.hpp"
#include "objects_factory.hpp"
#include "game/settings.hpp"
#include "game/gamedate.hpp"
#include "game/funds.hpp"
#include "barbarian.hpp"
#include "city/statistic.hpp"
#include "city/states.hpp"
#include "config.hpp"
#include "core/flowlist.hpp"
#include "emperor_line.hpp"
#include "events/changeemperor.hpp"
#include "core/common.hpp"

using namespace config;

namespace world
{

static const int defaultInterestPercent=10;
static const int defaultBarbarianOnMap=1;
static const int minRomeSalary=10;
static const int maxRomeSalary=50;
static const int minimumCityTax=50;
static const int defaultCityTaxKoeff=100;
static const int cityTaxLimiter=4;

class Cities : public CityList
{
public:
  std::string playerCity;

  void setAvailable( bool value )
  {
    for( auto city : *this )
      city->setAvailable( value );
  }

  void update( unsigned int time )
  {
    for( auto city : *this )
      city->timeStep( time );
  }

  CityPtr find( const std::string& name ) const
  {
    return utils::findByName( *this, name );
  }

  VariantMap save() const
  {
    VariantMap ret;
    for( auto city : *this )
    {
      //not need save city player
      if( city->name() == playerCity )
        continue;

      VariantMap vm_city;
      std::string cityName;
      try
      {
        cityName = city->name();
        city->save( vm_city );
        ret[ cityName ] = vm_city;
      }
      catch(...)
      {
        Logger::warning( "!!! WARNING: Cant save information for city " + cityName );
      }
    }

    return ret;
  }

  void load( const VariantMap& stream )
  {
    std::string allCities = stream.get( "all" ).toString();
    if( !allCities.empty() )
    {
      setAvailable( allCities == "enabled" );
    }

    for( auto& item : stream )
    {
      CityPtr city = find( item.first );
      if( city.isValid() )
      {
        city->load( item.second.toMap() );
      }
      else
      {
        Logger::warning( "!!! WARNING: Cant find city {} on load", item.first );
      }
    }
  }
};

class Objects : public FlowList<Object>
{
public:
  unsigned int id;

  void update( unsigned int time )
  {
    for( auto obj : *this )
      obj->timeStep( time );

    utils::eraseIfDeleted( *this );
    merge();
  }

  VariantMap save() const
  {
    VariantMap ret;
    for( auto obj : *this)
    {
      VariantMap objSave;
      obj->save( objSave );
      ret[ obj->name() ] = objSave;
    }

    return ret;
  }

  void load( const VariantMap& stream, EmpirePtr empire )
  {
    for( const auto& item : stream )
    {
      const VariantMap& vm = item.second.toMap();
      std::string objectType = vm.get( "type" ).toString();

      ObjectPtr obj = ObjectsFactory::instance().create( objectType, empire );
      obj->load( vm );
      push_back( obj );
    }
  }
};

struct Economy
{
  int rateInterest;
  unsigned int treasury;
  int workerSalary;
};

class Empire::Impl
{
public:
  Cities cities;
  Trading trading;
  TradeRoutes troutes;
  EmpireMap emap;
  Objects objects;
  Economy economy;
  Emperor emperor;

  bool enabled;
  unsigned int maxBarbariansGroups;

public:
  void takeTaxes();
  void checkLoans();
  void checkBarbarians(EmpirePtr empire);
  void checkEmperorChanged();
};

Empire::Empire() : _d( new Impl )
{
  _d->trading.init( this );
  _d->troutes.init( this );
  _d->economy.workerSalary = econ::defaultSalary;
  _d->economy.rateInterest = defaultInterestPercent;
  _d->economy.treasury = 0;

  _d->enabled = true;
  _d->objects.id = 0;

  _d->maxBarbariansGroups = defaultBarbarianOnMap;
  _d->emperor.init( *this );
}

CityList Empire::cities() const
{
  CityList ret;
  for( auto city : _d->cities )
  {
    if( city->isAvailable() )
      ret.push_back( city );
  }

  return ret;
}

Empire::~Empire() {}

void Empire::_initializeObjects( vfs::Path filename )
{
  _d->objects.clear();

  VariantMap objects = config::load( filename.toString() );
  if( objects.empty() )
  {
    Logger::warning( "Empire: can't load objects model from " + filename.toString() );
    return;
  }

  _d->objects.load( objects, this );
}

void Empire::_initializeCities( vfs::Path filename )
{
  VariantMap citiesData = config::load( filename.toString() );

  _d->cities.clear();
  if( citiesData.empty() )
  {
    Logger::warning( "Empire: can't load cities model from " + filename.toString() );
    return;
  }

  for( const auto& item : citiesData )
  {
    CityPtr city = ComputerCity::create( this, item.first );
    addCity( city );
    city->load( item.second.toMap() );
    _d->emap.setCity( city->location() );
  }
}

void Empire::_initializeCapital()
{
  auto stubRome = Rome::create( this );

  CityPtr rome = findCity( Rome::defaultName );
  if( rome.isValid() )
  {
    stubRome->setLocation( rome->location() );
  }

  _d->cities.remove( rome );
  _d->cities.push_back( stubRome );
}

void Empire::initialize(vfs::Path citiesPath, vfs::Path objectsPath, vfs::Path filemap)
{
  VariantMap emap = config::load( filemap.toString() );
  _d->emap.load( emap );

  _initializeCities( citiesPath );
  _initializeObjects( objectsPath );

  _initializeCapital();
}

void Empire::addObject(ObjectPtr obj)
{
  if( obj->name().empty() )
  {          
    obj->setName( obj->type() + utils::i2str( _d->objects.id++ ) );
  }  

  for( auto object : _d->objects )
  {
    if( object == obj )
    {
      Logger::warning( "WARNING!!! Empire:addObject also have object with name " + obj->name() );
      return;
    }
  }

  _d->objects.postpone( obj );
}

CityPtr Empire::addCity( CityPtr city )
{
  CityPtr ret = findCity( city->name() );

  if( ret.isValid() )
  {
    Logger::warning( "Empire: city {0} already exist", city->name() );
    //_GAME_DEBUG_BREAK_IF( "City already exist" );
    return ret;
  }

  _d->cities.push_back( city );

  return ret;
}

EmpirePtr Empire::create()
{
  EmpirePtr ret( new Empire() );
  ret->drop();

  return ret;
}

CityPtr Empire::findCity( const std::string& name ) const { return _d->cities.find( name ); }

void Empire::save( VariantMap& stream ) const
{  
  VARIANT_SAVE_CLASS_D( stream, _d, cities               )
  VARIANT_SAVE_CLASS_D( stream, _d, objects              )
  VARIANT_SAVE_CLASS_D( stream, _d, trading              )
  VARIANT_SAVE_CLASS_D( stream, _d, emperor              )
  VARIANT_SAVE_ANY_D  ( stream, _d, enabled              )
  VARIANT_SAVE_ANY_D  ( stream, _d, objects.id           )
  VARIANT_SAVE_ANY_D  ( stream, _d, maxBarbariansGroups  )
  VARIANT_SAVE_ANY_D  ( stream, _d, economy.rateInterest )
  VARIANT_SAVE_ANY_D  ( stream, _d, economy.treasury     )
  VARIANT_SAVE_CLASS_D( stream, _d, troutes              )

}

void Empire::load( const VariantMap& stream )
{
  VARIANT_LOAD_ANYDEF_D( _d, objects.id,          _d->objects.id,           stream )
  VARIANT_LOAD_ANYDEF_D( _d, enabled,             _d->enabled,              stream )
  VARIANT_LOAD_ANYDEF_D( _d, maxBarbariansGroups, _d->maxBarbariansGroups,  stream )
  VARIANT_LOAD_ANYDEF_D( _d, economy.workerSalary,_d->economy.workerSalary, stream )
  VARIANT_LOAD_ANYDEF_D( _d, economy.rateInterest,_d->economy.rateInterest, stream )
  VARIANT_LOAD_ANYDEF_D( _d, economy.treasury,    _d->economy.treasury,     stream )
  VARIANT_LOAD_CLASS_D ( _d, trading,                                       stream )
  VARIANT_LOAD_CLASS_D ( _d, cities,                                        stream )
  VARIANT_LOAD_CLASS_D ( _d, emperor,                                       stream ) //patch from keeeeper
  VARIANT_LOAD_CLASS_D ( _d, troutes,                                       stream )
  _d->objects.load( stream.get( "objects" ).toMap(), this );
}

void Empire::setCitiesAvailable(bool value){ _d->cities.setAvailable( value ); }
unsigned int Empire::workerSalary() const {  return _d->economy.workerSalary; }
void Empire::setWorkerSalary(unsigned int value){ _d->economy.workerSalary = math::clamp<unsigned int>( value, minRomeSalary, maxRomeSalary ); }
bool Empire::isAvailable() const{  return _d->enabled; }
void Empire::setAvailable(bool value) { _d->enabled = value; }

void Empire::setPrice(good::Product gtype, const PriceInfo& prices )
{
  _d->trading.setPrice( gtype, prices.buy, prices.sell );
  for( auto city : _d->cities)
  {
    city->empirePricesChanged( gtype, prices );
  }
}

void Empire::changePrice(good::Product gtype, const PriceInfo& delta )
{
  world::PriceInfo prices = _d->trading.getPrice( gtype );
  prices.buy += delta.buy;
  prices.sell += delta.sell;
  setPrice( gtype, prices );
}

PriceInfo Empire::getPrice(good::Product gtype) const
{
  return _d->trading.getPrice( gtype );
}

void Empire::clear() {}

TraderoutePtr Empire::createTradeRoute(std::string start, std::string stop )
{
  CityPtr startCity = findCity( start );
  CityPtr stopCity = findCity( stop );

  bool startAndDstCorrect = startCity.isValid() && stopCity.isValid();
  if( startAndDstCorrect )
  {
    TraderoutePtr route = _d->troutes.create( start, stop );
    if( !route.isValid() )
    {
      Logger::warning( "WARNING!!! Trading::load cant create route from {0} to {1}",
                       start, stop );
      return route;
    }

    EmpireMap::TerrainType startType = (EmpireMap::TerrainType)startCity->tradeType();
    EmpireMap::TerrainType stopType = (EmpireMap::TerrainType)stopCity->tradeType();
    bool land = (startType & EmpireMap::trLand) && (stopType & EmpireMap::trLand);
    bool sea = (startType & EmpireMap::trSea) && (stopType & EmpireMap::trSea);

    PointsArray lpnts, spnts;
    if( land )
    {
      lpnts = _d->emap.findRoute( startCity->location(), stopCity->location(), EmpireMap::trLand );
    }

    if( sea )
    {
      spnts = _d->emap.findRoute( startCity->location(), stopCity->location(), EmpireMap::trSea );
    }

    bool haveLandOrSeaRoute = (!lpnts.empty() || !spnts.empty());
    if( haveLandOrSeaRoute )
    {
      if( lpnts.empty() )
      {
        route->setPoints( spnts, true );
        return route;
      }

      if( spnts.empty() )
      {
        route->setPoints( lpnts, false );
        return route;
      }

      if( spnts.size() > lpnts.size() )
      {
        route->setPoints( lpnts, false );
      }
      else
      {
        route->setPoints( spnts, true );
      }
    }

    return route;
  }
  else
  {
    Logger::warning( "!!! WARNING: Cant create road from {0} to {1}", start, stop );
  }

  return TraderoutePtr();
}

void Empire::timeStep( unsigned int time )
{    
  if( game::Date::isMonthChanged() )
  {
    _d->checkLoans();
    _d->checkBarbarians( this );
    _d->checkEmperorChanged();
  }

  if( game::Date::isYearChanged() )
  {
    _d->takeTaxes();
  }

  _d->trading.timeStep( time );
  _d->troutes.timeStep( time );
  _d->emperor.timeStep( time );
  _d->cities.update( time );
  _d->objects.update( time );
}

const EmpireMap& Empire::map() const { return _d->emap; }

Emperor& Empire::emperor() { return _d->emperor; }
CityPtr Empire::capital() const { return findCity( Rome::defaultName ); }
TradeRoutes& Empire::troutes() { return _d->troutes; }

CityPtr Empire::initPlayerCity( CityPtr city )
{
  CityPtr ret = findCity( city->name() );

  if( ret.isNull() )
  {
    Logger::warning( "Empire: can't init player city, city with name {} no exist", city->name() );
    return CityPtr();
  }

  city->setLocation( ret->location() );
  _d->cities.remove( ret );
  _d->cities.push_back( city );
  _d->cities.playerCity = city->name();

  for( auto& product : good::all() )
  {
    world::PriceInfo prices = getPrice( product );
    city->empirePricesChanged( product, prices );
  }

  return ret;
}

const ObjectList& Empire::objects() const{  return _d->objects; }

ObjectList Empire::findObjects( Point location, int deviance ) const
{
  ObjectList ret;
  int sqrDeviance = pow( deviance, 2 ); //not need calculate sqrt

  for( auto item : _d->objects )
  {
    if( item->isAvailable() && location.getDistanceFromSQ( item->location() ) < sqrDeviance )
    {        
      ret << item;
    }
  }

  for( auto city : _d->cities )
  {
    if( city->isAvailable() && location.getDistanceFromSQ( city->location() ) < sqrDeviance )
    {
      ret << city.as<Object>();
    }
  }

  return ret;
}


ObjectPtr Empire::findObject(const std::string& name) const
{
  for( auto obj : _d->objects )
  {
    if( obj->name() == name )
    {
      return obj;
    }
  }

  CityPtr city = findCity( name );

  return city.as<Object>();
}

float EmpireHelper::governorSalaryKoeff(CityPtr city)
{
  PlayerPtr pl = city->mayor();

  float result = 1.f;
  if( pl.isValid() )
  {
    world::GovernorRanks ranks = world::EmpireHelper::ranks();
    const world::GovernorRank& curRank = ranks[ math::clamp<int>( pl->rank(), 0, ranks.size() ) ];
    int normalSalary = curRank.salary;

    result = pl->salary() / (float)normalSalary;
  }

  return result;
}

GovernorRanks EmpireHelper::ranks()
{
  std::map<unsigned int, GovernorRank> sortRanks;

  VariantMap vm = config::load( SETTINGS_RC_PATH( ranksModel ) );
  for( auto& item : vm )
  {
    GovernorRank rank;
    rank.load( item.first, item.second.toMap() );
    sortRanks[ rank.level ] = rank;
  }

  GovernorRanks ret;
  for( auto& rank : sortRanks )
    ret.push_back( rank.second );

  return ret;
}

GovernorRank EmpireHelper::getRank(GovernorRank::Level level)
{
  GovernorRanks ranks = world::EmpireHelper::ranks();
  return ranks[ math::clamp<int>( level, 0, ranks.size() ) ];
}

void Empire::Impl::checkLoans()
{
  for( auto city : cities )
  {
    int loanValue = city->treasury().money();
    if( loanValue < 0 )
    {
      int loanPercent = std::max( 1, abs( loanValue / ( economy.rateInterest * DateTime::monthsInYear ) ));

      if( loanPercent > 0 )
      {
        if( city->treasury().haveMoneyForAction( loanPercent ) )
        {
          city->treasury().resolveIssue( econ::Issue( econ::Issue::credit, -loanPercent ) );
          economy.treasury += loanPercent;
        }
        else
        {
          city->treasury().resolveIssue( econ::Issue( econ::Issue::overduePayment, loanPercent ) );
        }
      }
    }
  }
}

void Empire::Impl::checkBarbarians( EmpirePtr empire )
{
  unsigned int barbarians_n = objects.count<Barbarian>();
  bool needYetOneBarbarianGroup = barbarians_n < maxBarbariansGroups;

  if( needYetOneBarbarianGroup )
  {
    BarbarianPtr brb = Barbarian::create( empire, Barbarian::startLocation );
    empire->addObject( brb.as<Object>() );
  }
}

void Empire::Impl::checkEmperorChanged()
{
  EmperorLine& emperors = EmperorLine::instance();
  std::string emperorName = emperors.getEmperor( game::Date::current() );
  bool emperorChanged = emperorName != emperor.name();

  if( emperorChanged )
  {
    VariantMap vm = emperors.getInfo( emperorName );

    events::GameEventPtr e = events::ChangeEmperor::create();
    e->load( vm );
    e->dispatch();
  }
}

void Empire::Impl::takeTaxes()
{
  for( auto city : cities )
  {
    int empireTax = 0;

    if( !city->isAvailable() )
      continue;

    if( city.is<Rome>() )
    {
      //no take taxes from capital
    }       
    else if( city.is<ComputerCity>() )
    {
      empireTax = econ::calcTaxValue( city->states().population, defaultCityTaxKoeff );
      economy.treasury += empireTax;
      emperor.citySentTax( city->name(), empireTax );
    }
    else
    {
      int profit = city->treasury().getIssueValue( econ::Issue::cityProfit, econ::Treasury::lastYear );
      if( profit <= 0 )
      {
        empireTax = econ::calcTaxValue( city->states().population, defaultCityTaxKoeff );
      }
      else
      {
        int minimumExpireTax =econ::calcTaxValue( city->states().population, defaultCityTaxKoeff ) + minimumCityTax;
        empireTax = math::clamp( profit / cityTaxLimiter, minimumExpireTax, 9999 );
        //emperor.citySentTax( city->name(), empireTax );
      }

      bool cityTooYoung4tax = city->states().age <= econ::cityAge4tax;
      if( cityTooYoung4tax )
        empireTax = 1;

      econ::Treasury& funds = city->treasury();

      if( funds.haveMoneyForAction( empireTax, econ::Treasury::debtDisabled ) )
      {
        funds.resolveIssue( econ::Issue( econ::Issue::empireTax, -empireTax ) );

        economy.treasury += empireTax;
        emperor.citySentTax( city->name(), empireTax );
      }
      else
      {
        city->treasury().resolveIssue( econ::Issue( econ::Issue::overdueEmpireTax, empireTax ) );
      }
    }
  }
}

}//end namespace world
