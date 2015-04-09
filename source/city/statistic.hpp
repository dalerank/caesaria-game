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

#ifndef __CAESARIA_CITYSTATISTIC_H_INCLUDED__
#define __CAESARIA_CITYSTATISTIC_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "core/variant.hpp"
#include "objects/overlay.hpp"
#include "good/productmap.hpp"
#include "predefinitions.hpp"
#include "festivaltype.hpp"
#include "objects/constants.hpp"
#include "game/citizen_group.hpp"
#include "game/service.hpp"
#include "gfx/helper.hpp"
#include "walker/walker.hpp"
#include "gfx/tilearea.hpp"
#include "city.hpp"

namespace city
{

namespace statistic
{

struct WorkersInfo
{
  int current;
  int need;
};

WorkersInfo getWorkersNumber( PlayerCityPtr city );
CitizenGroup getPopulation( PlayerCityPtr city );
unsigned int getWorkersNeed( PlayerCityPtr city );
unsigned int getAvailableWorkersNumber( PlayerCityPtr city );
unsigned int getMonthlyWorkersWages( PlayerCityPtr city );
float getMonthlyOneWorkerWages( PlayerCityPtr city );
unsigned int getWorklessNumber( PlayerCityPtr city );
unsigned int getWorklessPercent( PlayerCityPtr city );
unsigned int getFoodStock( PlayerCityPtr city );
unsigned int getFoodMonthlyConsumption( PlayerCityPtr city );
unsigned int getFoodProducing( PlayerCityPtr city );
unsigned int getTaxValue( PlayerCityPtr city );
unsigned int getTaxPayersPercent( PlayerCityPtr city );
unsigned int getHealth( PlayerCityPtr city );
unsigned int blackHouses( PlayerCityPtr city );
int months2lastAttack( PlayerCityPtr city );
int taxValue( unsigned int population, int koeff);
int getWagesDiff( PlayerCityPtr city );
unsigned int getFestivalCost( PlayerCityPtr city, FestivalType type );
HouseList getEvolveHouseReadyBy(PlayerCityPtr, const object::TypeSet& checkTypes);
HouseList getEvolveHouseReadyBy(PlayerCityPtr, const object::Type checkTypes);
unsigned int getCrimeLevel( PlayerCityPtr city );
good::ProductMap getProductMap(PlayerCityPtr city , bool includeGranary);
float getBalanceKoeff( PlayerCityPtr city );
int getLaborAccessValue( PlayerCityPtr city, WorkingBuildingPtr wb );
int getEntertainmentCoverage(PlayerCityPtr city, Service::Type service );
bool canImport( PlayerCityPtr city, good::Product type );
bool canProduce( PlayerCityPtr city, good::Product type );
template< class T > SmartList< T > findo( PlayerCityPtr r, object::Type type );
template< class T > SmartList< T > findo( PlayerCityPtr r, std::set<object::Type> which );
template< class T > SmartPtr< T > nexto( PlayerCityPtr r, SmartPtr< T > current );
template< class T > SmartPtr< T > prewo( PlayerCityPtr r, SmartPtr< T > current );
template< class T > SmartPtr< T > finds( PlayerCityPtr r );
template< class T > SmartList< T > findo( PlayerCityPtr r, object::Group group );
template<class T> bool isTileBusy( PlayerCityPtr r, TilePos p, WalkerPtr caller, bool& needMeMove );
template< class T > SmartList< T > findw( PlayerCityPtr r, walker::Type type,
                                          TilePos start, TilePos stop=TilePos(-1, -1) );
HouseList findh( PlayerCityPtr r, std::set<int> levels=std::set<int>() );
FarmList findfarms(PlayerCityPtr r, std::set<object::Type> which=std::set<object::Type>() );

template< class T >
SmartList< T > findo( PlayerCityPtr r, object::Type type )
{
  if( r.isNull() )
    return SmartList<T>();

  SmartList< T > ret;
  const OverlayList& buildings = r->overlays();
  foreach( item, buildings )
  {
    if( (*item).isValid() && ((*item)->type() == type || type == object::any ) )
    {
      SmartPtr< T > b = ptr_cast<T>( *item );
      if( b.isValid() )
        ret.push_back( b );
    }
  }

  return ret;
}

template<class T>
bool isTileBusy( PlayerCityPtr r, TilePos p, WalkerPtr caller, bool& needMeMove )
{
  needMeMove = false;
  SmartList<T> walkers;
  walkers << r->walkers( p );

  if( !walkers.empty() )
  {
    needMeMove = (caller.object() != walkers.front().object());
  }

  return walkers.empty() > 1;
}

template< class T >
SmartList< T > findw( PlayerCityPtr r, walker::Type type,
                      TilePos start, TilePos stop )
{
  WalkerList walkersInArea;

  TilePos stopPos = stop;

  if( start == gfx::tilemap::invalidLocation() )
  {
    const WalkerList& all = r->walkers();
    walkersInArea.insert( walkersInArea.end(), all.begin(), all.end() );
  }
  else if( stopPos == gfx::tilemap::invalidLocation() )
  {
    const WalkerList& wlkOnTile = r->walkers( start );
    walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
  }
  else
  {
    gfx::TilesArea area( r->tilemap(), start, stop );
    foreach( tile, area)
    {
      const WalkerList& wlkOnTile = r->walkers( (*tile)->pos() );
      walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
    }
  }

  SmartList< T > result;
  foreach( w, walkersInArea )
  {
    if( (*w)->type() == type || type == walker::any )
    {
      SmartPtr< T > ptr = ptr_cast<T>( *w );
      if( ptr.isValid() )
        result.push_back( ptr );
    }
  }

  return result;
}

template< class T >
SmartPtr<T> findw( PlayerCityPtr r, walker::Type type, Walker::UniqueId id )
{
  const WalkerList& all = r->walkers();

  if( type != walker::any )
  {
    foreach( it, all )
    {
      if((*it)->type() == type && (*it)->uniqueId() != id )
        return ptr_cast<T>( *it );
    }
  }
  else
  {
    foreach( it, all )
    {
      if( (*it)->uniqueId() != id )
        return ptr_cast<T>( *it );
    }
  }

  return SmartPtr<T>();
}

template< class T >
SmartList< T > findo( PlayerCityPtr r, std::set<object::Type> which )
{
  OverlayList ret;
  SmartList<T> ovs = r->overlays();

  foreach( it, ovs )
  {
    if( which.count( (*it)->type ) > 0 )
    {
      ret << *it;
    }
  }

  return ret;
}

template< class T >
SmartPtr< T > nexto( PlayerCityPtr r, SmartPtr< T > current )
{
  if( r.isNull() || current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = findo<T>( r, current->type() );
  foreach( obj, objects )
  {
    if( current == *obj )
    {
      obj++;
      if( obj == objects.end() ) { return *objects.begin(); }
      else { return *obj; }
    }
  }

  return SmartPtr<T>();
}

template<class T>
SmartPtr<T> prewo( PlayerCityPtr r, SmartPtr<T> current)
{
  if(  r.isNull() || current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = findo<T>( r, current->type() );
  foreach( obj, objects )
  {
    if( current == *obj )
    {
      if (obj == objects.begin()) // MSVC compiler doesn't support sircular lists. Neither standart does.
      {
        obj = objects.end();
      }
      obj--;
      return *obj;
    }
  }

  return SmartPtr<T>();
}

template<class T>
SmartPtr<T> finds( PlayerCityPtr r)
{
  if( r.isNull() )
    return 0;

  SrvcPtr ret = r->findService( T::defaultName() );
  return ptr_cast<T>( ret );
}

template< class T >
SmartList< T > findo( PlayerCityPtr r, const object::Type type, TilePos start, TilePos stop )
{
  SmartList< T > ret;

  gfx::TilesArea area( r->tilemap(), start, stop );
  foreach( tile, area )
  {
    SmartPtr<T> obj = ptr_cast< T >( (*tile)->overlay() );
    if( obj.isValid() && (obj->type() == type || type == object::any) )
    {
      ret.push_back( obj );
    }
  }

  return ret;
}

template< class T >
SmartList< T > find( PlayerCityPtr r, object::Group group, const TilePos& start, const TilePos& stop )
{
  SmartList< T > ret;

  gfx::TilesArea area( r->tilemap(), start, stop );

  foreach( tile, area )
  {
    SmartPtr<T> obj = ptr_cast< T >((*tile)->overlay());
    if( obj.isValid() && (obj->getClass() == group || group == object::group::any ) )
    {
      ret.push_back( obj );
    }
  }

  return ret;
}

template< class T >
SmartList< T > findo( PlayerCityPtr r, object::Group group )
{
  SmartList< T > ret;
  OverlayList& buildings = r->overlays();
  foreach( item, buildings )
  {
    SmartPtr< T > b = ptr_cast< T >(*item);
    if( b.isValid() && (b->group() == group || group == object::group::any ) )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template< class T >
SmartList< T > findoex( PlayerCityPtr r, std::set<object::Type> which )
{
  OverlayList ret;
  SmartList<T> ovs = r->overlays();

  foreach( it, ovs )
  {
    if( which.count( (*it)->type ) == 0 )
    {
      ret << *it;
    }
  }

  return ret;
}

}

}//end namespace city

#endif //__CAESARIA_CITYSTATISTIC_H_INCLUDED__
