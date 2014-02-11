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
#include "empiremap.hpp"

namespace world
{

class Empire::Impl
{
public:
  CityList cities;
  Trading trading;
  EmpireMap emap;
  bool available;

  std::string playerCityName;
  int workerSalary;
};

Empire::Empire() : _d( new Impl )
{
  _d->trading.init( this );
  _d->workerSalary = 30;
  _d->available = true;
}

CityList Empire::getCities() const
{
  CityList ret;
  foreach( city, _d->cities )
  {
    if( (*city)->isAvailable() )
      ret.push_back( *city );
  }

  return ret;
}

Empire::~Empire()
{

}

void Empire::initialize(vfs::Path filename , vfs::Path filemap)
{
  VariantMap emap = SaveAdapter::load( filemap.toString() );
  _d->emap.initialize( emap );

  _d->cities.clear();
  VariantMap cities = SaveAdapter::load( filename.toString() );

  if( cities.empty() )
  {
    Logger::warning( "Can't load cities model from %s", filename.toString().c_str() );
    return;
  }

  foreach( item, cities )
  {
    CityPtr city = ComputerCity::create( this, item->first );
    addCity( city );
    city->load( item->second.toMap() );
    _d->emap.setCity( city->getLocation() );
  }
}

CityPtr Empire::addCity( CityPtr city )
{
  CityPtr ret = getCity( city->getName() );

  if( ret.isValid() )
  {
    Logger::warning( "City %s already exist", city->getName().c_str() );
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

CityPtr Empire::getCity( const std::string& name ) const
{
  foreach( city, _d->cities )
  {
    if( (*city)->getName() == name )
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
    if( (*city)->getName() == _d->playerCityName )
      continue;

    VariantMap vm_city;
    (*city)->save( vm_city );
    vm_cities[ (*city)->getName() ] = vm_city;
  }

  stream[ "cities" ] = vm_cities;
  stream[ "trade" ] = _d->trading.save();
  stream[ "enabled" ] = _d->available;
}

void Empire::load( const VariantMap& stream )
{
  VariantMap cities = stream.get( "cities" ).toMap();

  foreach( item, cities )
  {
    CityPtr city = getCity( item->first );
    if( city != 0 )
    {
      city->load( item->second.toMap() );
    }
  }

  _d->trading.load( stream.get( "trade").toMap() );
  _d->available = (bool)stream.get( "enabled", true );
}

void Empire::setCitiesAvailable(bool value)
{
  foreach( city, _d->cities ) { (*city)->setAvailable( value ); }
}

unsigned int Empire::getWorkerSalary() const {  return _d->workerSalary; }
bool Empire::isAvailable() const{  return _d->available; }

void Empire::createTradeRoute(std::string start, std::string stop )
{
  CityPtr startCity = getCity( start );
  CityPtr stopCity = getCity( stop );

  if( startCity != 0 && stopCity != 0 )
  {
    TraderoutePtr route = _d->trading.createRoute( start, stop );
    EmpireMap::TerrainType startType = (EmpireMap::TerrainType)startCity->getTradeType();
    EmpireMap::TerrainType stopType = (EmpireMap::TerrainType)stopCity->getTradeType();
    bool land = (startType & EmpireMap::land) && (stopType & EmpireMap::land);
    bool sea = (startType & EmpireMap::sea) && (stopType & EmpireMap::sea);

    PointsArray lpnts, spnts;
    if( land )
    {
      lpnts = _d->emap.getRoute( startCity->getLocation(), stopCity->getLocation(), EmpireMap::land );
    }

    if( sea )
    {
      spnts = _d->emap.getRoute( startCity->getLocation(), stopCity->getLocation(), EmpireMap::sea );
    }

    if( !lpnts.empty() || !spnts.empty() )
    {
      if( lpnts.empty() )
      {
        route->setPoints( spnts, true );
        return;
      }

      if( spnts.empty() )
      {
        route->setPoints( lpnts, false );
        return;
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
  }
}

TraderoutePtr Empire::getTradeRoute( unsigned int index )
{
  return _d->trading.getRoute( index );
}

TraderoutePtr Empire::getTradeRoute( const std::string& start, const std::string& stop )
{
  return _d->trading.getRoute( start, stop ); 
}

void Empire::timeStep( unsigned int time )
{
  _d->trading.update( time );

  foreach( city, _d->cities )
  {
    (*city)->timeStep( time );
  }
}

CityPtr Empire::initPlayerCity( CityPtr city )
{
  CityPtr ret = getCity( city->getName() );

  if( ret.isNull() )
  {
    Logger::warning("Can't init player city, that empire city with name %s no exist", city->getName().c_str() );
    _CAESARIA_DEBUG_BREAK_IF( "City already exist" );
    return ret;
  }

  city->setLocation( ret->getLocation() );
  _d->cities.remove( ret );
  _d->cities.push_back( city );
  _d->playerCityName = city->getName();

  return ret;
}

TraderouteList Empire::getTradeRoutes( const std::string& startCity )
{
  return _d->trading.getRoutes( startCity );
}

unsigned int EmpireHelper::getTradeRouteOpenCost( EmpirePtr empire, const std::string& start, const std::string& stop )
{
  CityPtr startCity = empire->getCity( start );
  CityPtr stopCity = empire->getCity( stop );

  if( startCity != 0 && stopCity != 0 )
  {
    int distance2City = (int)startCity->getLocation().getDistanceFrom( stopCity->getLocation() ); 
    distance2City = (distance2City / 100 + 1 ) * 200;

    return distance2City;
  }

  return 0;
}


TraderouteList Empire::getTradeRoutes()
{
  return _d->trading.getRoutes();
}

}//end namespace world
