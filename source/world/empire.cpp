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

#include "empire.hpp"
#include "computer_city.hpp"
#include "core/variant.hpp"
#include "core/time.hpp"
#include "core/saveadapter.hpp"
#include "core/stringhelper.hpp"
#include "trading.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "traderoute.hpp"
#include "object.hpp"
#include "rome.hpp"
#include "empiremap.hpp"
#include "emperor.hpp"
#include "objects_factory.hpp"
#include "game/settings.hpp"
#include "game/gamedate.hpp"
#include "city/funds.hpp"

namespace world
{

class Empire::Impl
{
public:
  CityList cities;
  Trading trading;
  EmpireMap emap;
  ObjectList objects;
  int rateInterest;
  Emperor emperor;
  unsigned int treasury;
  bool available;

  std::string playerCityName;
  int workerSalary;
  unsigned int objUid;

public:
  void takeTaxes();
  void checkLoans();
};

Empire::Empire() : _d( new Impl )
{
  _d->trading.init( this );
  _d->workerSalary = 30;
  _d->available = true;
  _d->treasury = 0;
  _d->objUid = 0;
  _d->rateInterest = 10;
  _d->emperor.init( *this );
}

CityList Empire::cities() const
{
  CityList ret;
  foreach( city, _d->cities )
  {
    if( (*city)->isAvailable() )
      ret.push_back( *city );
  }

  return ret;
}

Empire::~Empire(){}

void Empire::initialize(vfs::Path filename, vfs::Path filemap)
{
  VariantMap emap = SaveAdapter::load( filemap.toString() );
  _d->emap.initialize( emap );

  _d->cities.clear();
  VariantMap cities = SaveAdapter::load( filename.toString() );

  if( cities.empty() )
  {
    Logger::warning( "Empire: can't load cities model from %s", filename.toString().c_str() );
    return;
  }

  foreach( item, cities )
  {
    CityPtr city = ComputerCity::create( this, item->first );
    addCity( city );
    city->load( item->second.toMap() );
    _d->emap.setCity( city->location() );
  }

  //initialize capital
  CityPtr stubRome = new Rome( this );
  stubRome->drop();

  CityPtr rome = findCity( Rome::defaultName );
  if( rome.isValid() )
  {
    stubRome->setLocation( rome->location() );
  }

  _d->cities.remove( rome );
  _d->cities.push_back( stubRome );
}

void Empire::addObject(ObjectPtr obj)
{
  if( obj->name().empty() )
  {
    obj->setName( obj->type() + StringHelper::i2str( _d->objUid++ ) );
    _d->objects.push_back( obj );
  }
}

CityPtr Empire::addCity( CityPtr city )
{
  CityPtr ret = findCity( city->name() );

  if( ret.isValid() )
  {
    Logger::warning( "Empire: city %s already exist", city->name().c_str() );
    _CAESARIA_DEBUG_BREAK_IF( "City already exist" );
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

CityPtr Empire::findCity( const std::string& name ) const
{
  foreach( city, _d->cities )
  {
    if( (*city)->name() == name )
    {
      return *city;
    }
  }

  return CityPtr();
}

void Empire::save( VariantMap& stream ) const
{
  VariantMap vm_cities;
  foreach( city, _d->cities )
  {
    //not need save city player
    if( (*city)->name() == _d->playerCityName )
      continue;

    VariantMap vm_city;
    (*city)->save( vm_city );
    vm_cities[ (*city)->name() ] = vm_city;
  }

  VariantMap vm_objects;
  foreach( obj, _d->objects)
  {
    VariantMap objSave;
    (*obj)->save( objSave );
    vm_objects[ (*obj)->name() ] = objSave;
  }

  stream[ "cities"  ] = vm_cities;
  stream[ "objects" ] = vm_objects;
  stream[ "trade"   ] = _d->trading.save();
  stream[ "enabled" ] = _d->available;
  stream[ "emperor" ] = _d->emperor.save();
  stream[ "objUid"  ] = _d->objUid;
  stream[ "rateInterest" ] = _d->rateInterest;
}

void Empire::load( const VariantMap& stream )
{
  VariantMap cities = stream.get( "cities" ).toMap();

  _d->objUid = stream.get( "objUid" );

  foreach( item, cities )
  {
    CityPtr city = findCity( item->first );
    if( city != 0 )
    {
      city->load( item->second.toMap() );
    }
  }

  _d->objects.clear();
  VariantMap objects = stream.get( "objects" ).toMap();  
  foreach( item, objects )
  {
    const VariantMap& vm = item->second.toMap();
    std::string objectType = vm.get( "type" ).toString();

    ObjectPtr obj = ObjectsFactory::instance().create( objectType, this );
    obj->load( vm );
    _d->objects << obj;
  }

  _d->trading.load( stream.get( "trade").toMap() );
  _d->available = (bool)stream.get( "enabled", true );
  _d->rateInterest = stream.get( "rateInterest", 10 );

  _d->emperor.load( stream.get( "emperor" ).toMap() );
}

void Empire::setCitiesAvailable(bool value)
{
  foreach( city, _d->cities ) { (*city)->setAvailable( value ); }
}

unsigned int Empire::workerSalary() const {  return _d->workerSalary; }
bool Empire::isAvailable() const{  return _d->available; }
void Empire::setAvailable(bool value) { _d->available = value; }

void Empire::setPrice(Good::Type gtype, int buy, int sell)
{
  _d->trading.setPrice( gtype, buy, sell );
  foreach( it, _d->cities)
  {
    (*it)->empirePricesChanged( gtype, buy, sell );
  }
}

void Empire::changePrice(Good::Type gtype, int buy, int sell)
{
  int b, s;
  _d->trading.getPrice( gtype, b, s );
  setPrice( gtype, b + buy, s + sell );
}

void Empire::getPrice(Good::Type gtype, int& buy, int& sell) const
{
  _d->trading.getPrice( gtype, buy, sell );
}

TraderoutePtr Empire::createTradeRoute(std::string start, std::string stop )
{
  CityPtr startCity = findCity( start );
  CityPtr stopCity = findCity( stop );

  if( startCity != 0 && stopCity != 0 )
  {
    TraderoutePtr route = _d->trading.createRoute( start, stop );
    EmpireMap::TerrainType startType = (EmpireMap::TerrainType)startCity->tradeType();
    EmpireMap::TerrainType stopType = (EmpireMap::TerrainType)stopCity->tradeType();
    bool land = (startType & EmpireMap::land) && (stopType & EmpireMap::land);
    bool sea = (startType & EmpireMap::sea) && (stopType & EmpireMap::sea);

    PointsArray lpnts, spnts;
    if( land )
    {
      lpnts = _d->emap.findRoute( startCity->location(), stopCity->location(), EmpireMap::land );
    }

    if( sea )
    {
      spnts = _d->emap.findRoute( startCity->location(), stopCity->location(), EmpireMap::sea );
    }

    if( !lpnts.empty() || !spnts.empty() )
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

  return TraderoutePtr();
}

TraderoutePtr Empire::findRoute( unsigned int index ) {  return _d->trading.findRoute( index ); }

TraderoutePtr Empire::findRoute( const std::string& start, const std::string& stop )
{
  return _d->trading.findRoute( start, stop ); 
}

void Empire::timeStep( unsigned int time )
{    
  _d->trading.timeStep( time );
  _d->emperor.timeStep( time );

  foreach( city, _d->cities )
  {
    (*city)->timeStep( time );
  }

  for( ObjectList::iterator it=_d->objects.begin(); it != _d->objects.end(); )
  {
    (*it)->timeStep( time );
    if( (*it)->isDeleted() ) { it =_d->objects.erase( it ); }
    else { ++it; }
  }

  if( GameDate::isMonthChanged() )
  {
    _d->checkLoans();
  }

  if( GameDate::isYearChanged() )
  {
    _d->takeTaxes();
  }
}

const EmpireMap &Empire::map() const { return _d->emap; }

Emperor& Empire::emperor() { return _d->emperor; }
CityPtr Empire::rome() const { return findCity( Rome::defaultName ); }

CityPtr Empire::initPlayerCity( CityPtr city )
{
  CityPtr ret = findCity( city->name() );

  if( ret.isNull() )
  {
    Logger::warning("Empire: can't init player city, city with name %s no exist", city->name().c_str() );
    //_CAESARIA_DEBUG_BREAK_IF( "Empire: can't init player city" );
    return CityPtr();
  }

  city->setLocation( ret->location() );
  _d->cities.remove( ret );
  _d->cities.push_back( city );
  _d->playerCityName = city->name();

  for( int k=Good::none; k < Good::goodCount; k++ )
  {
    int buy, sell;
    getPrice( Good::Type(k), buy, sell );
    city->empirePricesChanged( Good::Type(k), buy, sell );
  }

  return ret;
}

const ObjectList& Empire::objects() const{  return _d->objects; }

ObjectPtr Empire::findObject(const std::string& name) const
{
  foreach( i, _d->objects )
  {
    if( (*i)->name() == name )
    {
      return *i;
    }
  }

  CityPtr city = findCity( name );

  return ptr_cast<Object>( city );
}

TraderouteList Empire::tradeRoutes( const std::string& startCity ){  return _d->trading.routes( startCity );}

unsigned int EmpireHelper::getTradeRouteOpenCost( EmpirePtr empire, const std::string& start, const std::string& stop )
{
  CityPtr startCity = empire->findCity( start );
  CityPtr stopCity = empire->findCity( stop );

  if( startCity != 0 && stopCity != 0 )
  {
    int distance2City = (int)startCity->location().getDistanceFrom( stopCity->location() ); 
    distance2City = (distance2City / 100 + 1 ) * 200;

    return distance2City;
  }

  return 0;
}

GovernorRanks EmpireHelper::getRanks()
{
  std::map<unsigned int, GovernorRank > sortRanks;

  VariantMap vm = SaveAdapter::load( GameSettings::rcpath( GameSettings::ranksModel ) );
  foreach( i, vm )
  {
    VariantMap rankInfo = i->second.toMap();

    GovernorRank rank;
    rank.rankName = i->first;
    rank.prettyName = rankInfo.get( "name" ).toString();
    rank.salary = (int)rankInfo.get( "salary" );

    sortRanks[ rank.salary ] = rank;
  }

  GovernorRanks ranks;
  foreach( i, sortRanks )
    ranks.push_back( i->second );

  return ranks;
}

TraderouteList Empire::tradeRoutes(){  return _d->trading.routes();}

void Empire::Impl::checkLoans()
{
  foreach( it, cities)
  {
    CityPtr city = *it;

    int loanValue = city->funds().treasury();
    if( loanValue < 0 )
    {
      int loanPercent = std::max( 1, abs( loanValue / ( rateInterest * 12 ) ));

      if( loanPercent > 0 )
      {
        if( city->funds().haveMoneyForAction( loanPercent ) )
        {
          city->funds().resolveIssue( FundIssue( city::Funds::credit, -loanPercent ) );
          treasury += loanPercent;
        }
        else
        {
          city->funds().resolveIssue( FundIssue( city::Funds::overduePayment, loanPercent ) );
        }
      }
    }
  }
}

void Empire::Impl::takeTaxes()
{
  foreach( it, cities)
  {
    CityPtr city = *it;

    int profit = city->funds().getIssueValue( city::Funds::cityProfit, city::Funds::lastYear );

    int empireTax = 0;
    if( profit <= 0 )
    {
      empireTax = (city->population() / 1000) * 100;
    }
    else
    {
      int minimumExpireTax = (city->population() / 1000) * 100 + 50;
      empireTax = math::clamp( profit / 4, minimumExpireTax, 9999 );
    }

    if( city->funds().treasury() > empireTax )
    {
      city->funds().resolveIssue( FundIssue( city::Funds::empireTax, -empireTax ) );

      treasury += empireTax;
      emperor.cityTax( city->name(), empireTax );
    }
  }
}

}//end namespace world
