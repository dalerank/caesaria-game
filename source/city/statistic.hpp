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

class Statistic
{
public:
  Statistic( PlayerCity& rcity );
  void update( const unsigned long time );

  struct _Walkers
  {
    const WalkerList& find( walker::Type type ) const;

    PlayerCity& _city;
    std::map<int, WalkerList> cached;
  } walkers;
};

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
HouseList getHouses( PlayerCityPtr r, std::set<int> levels=std::set<int>() );
FarmList getFarms(PlayerCityPtr r, std::set<object::Type> which=std::set<object::Type>() );
OverlayList getNeighbors( OverlayPtr overlay, PlayerCityPtr r );

template< class T >
SmartList< T > getObjects( PlayerCityPtr r, object::Type type )
{
  if( r.isNull() )
    return SmartList<T>();

  SmartList< T > ret;
  const OverlayList& buildings = r->overlays();
  for (auto building : buildings)
  {
    if( building.isValid() && (type == object::any || building->type() == type) )
      ret.addIfValid( building.as<T>() );
  }

  return ret;
}

template<class T, class B>
SmartList<T> getNeighbors(PlayerCityPtr r, SmartPtr<B> overlay)
{
  OverlayList ovs = getNeighbors( ptr_cast<Overlay>( overlay ), r );
  SmartList<T> ret;
  ret << ovs;
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
SmartList< T > getWalkers( PlayerCityPtr r, walker::Type type,
                           TilePos start=gfx::tilemap::invalidLocation(),
                           TilePos stop=gfx::tilemap::invalidLocation() )
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
    for (auto tile : area)
    {
      const WalkerList& wlkOnTile = r->walkers( tile->pos() );
      walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
    }
  }

  SmartList< T > result;
  for (auto walker : walkersInArea)
  {
    if(type == walker::any || walker->type() == type)
      result.addIfValid( walker.as<T>() );
  }

  return result;
}

template< class T >
SmartList< T > getWalkers( PlayerCityPtr r )
{
  const WalkerList& walkers = r->walkers();

  SmartList< T > result;
  for (auto walker : walkers)
  {
    result.addIfValid( walker.as<T>() );
  }

  return result;
}


template< class T >
SmartPtr<T> getWalker( PlayerCityPtr r, walker::Type type, Walker::UniqueId id )
{
  const WalkerList& all = r->walkers();

  for (auto walker : all)
  {
    if((type == walker::any || walker->type() == type) && walker->uniqueId() == id )
      return walker.as<T>();
  }

  return SmartPtr<T>();
}

template< class T >
SmartList< T > getObjects( PlayerCityPtr r, std::set<object::Type> which )
{
  SmartList< T > ret;
  const OverlayList& ovs = r->overlays();

  for (auto overlay : ovs)
  {
    if( which.count( overlay->type() ) > 0 )
    {
      ret << overlay;
    }
  }

  return ret;
}

template< class T >
SmartPtr< T > nextObject( PlayerCityPtr r, SmartPtr< T > current )
{
  if( r.isNull() || current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = getObjects<T>( r, current->type() );

  auto it = std::find(objects.begin(), objects.end(), current);
  if (it != objects.end()) {
    it++;
    if( it == objects.end() )
      it = objects.begin();
    return *it;
  }

  return SmartPtr<T>();
}

template<class T>
SmartPtr<T> prewObject( PlayerCityPtr r, SmartPtr<T> current)
{
  if(  r.isNull() || current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = getObjects<T>( r, current->type() );

  auto it = std::find(objects.begin(), objects.end(), current);
  if (it != objects.end()) {
    if (it == objects.begin()) // MSVC compiler doesn't support circular lists. Neither standard does.
    {
      it = objects.end();
    }
    it--;
    return *it;
  }

  return SmartPtr<T>();
}

template<class T>
SmartPtr<T> getService( PlayerCityPtr r)
{
  if( r.isNull() )
    return 0;

  SrvcPtr ret = r->findService( T::defaultName() );
  return ptr_cast<T>( ret );
}

template< class T >
SmartList< T > getObjects( PlayerCityPtr r, const object::Type type, TilePos start, TilePos stop )
{
  SmartList< T > ret;

  gfx::TilesArea area( r->tilemap(), start, stop );
  for (auto tile : area)
  {
    SmartPtr<T> obj = ptr_cast< T >( tile->overlay() );
    if( obj.isValid() && (obj->type() == type || type == object::any) )
    {
      ret.push_back( obj );
    }
  }

  return ret;
}

template< class T >
SmartList< T > getObjects( PlayerCityPtr r, object::Group group, const TilePos& start, const TilePos& stop )
{
  SmartList< T > ret;

  gfx::TilesArea area( r->tilemap(), start, stop );

  for (auto tile : area)
  {
    SmartPtr<T> obj = tile->overlay().as<T>();
    if( obj.isValid() && (obj->getClass() == group || group == object::group::any ) )
    {
      ret.push_back( obj );
    }
  }

  return ret;
}

template< class T >
SmartList<T> getObjects( PlayerCityPtr r, object::Group group )
{
  SmartList<T> ret;
  const OverlayList& buildings = r->overlays();
  for (auto building : buildings)
  {
    SmartPtr<T> b = building.as<T>();
    if( b.isValid() && (b->group() == group || group == object::group::any ) )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template< class T >
SmartList<T> getObjects( PlayerCityPtr r )
{
  SmartList<T> ret;
  const OverlayList& buildings = r->overlays();
  for (auto building : buildings)
  {
    ret.addIfValid( building.as<T>() );
  }

  return ret;
}

template< class T >
SmartList< T > getObjectsNotIs( PlayerCityPtr r, const std::set<object::Type>& which )
{
  SmartList< T > ret;
  const OverlayList& overlays = r->overlays();

  for (auto overlay : overlays)
  {
    if( which.count( overlay->type() ) == 0 )
    {
      ret.addIfValid( overlay.as<T>() );
    }
  }

  return ret;
}

template< class T >
SmartList< T > getObjectsNotIs( PlayerCityPtr r, const std::set<object::Group>& which )
{
  SmartList< T > ret;
  const OverlayList& overlays = r->overlays();

  for (auto overlay : overlays)
  {
    if( which.count( overlay->group() ) == 0 )
    {
      ret.addIfValid( overlay.as<T>() );
    }
  }

  return ret;
}

}//end namespace statistic

}//end namespace city

#endif //__CAESARIA_CITYSTATISTIC_H_INCLUDED__
