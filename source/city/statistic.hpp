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

    template< class T >
    SmartList< T > find() const
    {
      const WalkerList& walkers = _parent.rcity.walkers();

      SmartList< T > result;
      for( auto w : walkers )
        result.addIfValid( w.as<T>() );

      return result;
    }

    template< class T >
    SmartPtr<T> find( walker::Type type, Walker::UniqueId id ) const
    {
      const WalkerList& all = _parent.rcity.walkers();

      if( type != walker::any )
      {
        for( auto wlk : all )
        {
          if( wlk->type() == type && wlk->uniqueId() == id )
            return wlk.as<T>();
        }
      }
      else
      {
        for( auto wlk : all )
        {
          if( wlk->uniqueId() == id )
            return wlk.as<T>();
        }
      }

      return SmartPtr<T>();
    }

    template< class T >
    SmartList< T > find( walker::Type type,
                         TilePos start=gfx::tilemap::invalidLocation(),
                         TilePos stop=gfx::tilemap::invalidLocation() ) const
    {
      WalkerList walkersInArea;

      TilePos stopPos = stop;

      if( start == gfx::tilemap::invalidLocation() )
      {
        const WalkerList& all =_parent.rcity.walkers();
        walkersInArea.insert( walkersInArea.end(), all.begin(), all.end() );
      }
      else if( stopPos == gfx::tilemap::invalidLocation() )
      {
        const WalkerList& wlkOnTile = _parent.rcity.walkers( start );
        walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
      }
      else
      {
        gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );
        for( auto tile : area)
        {
          const WalkerList& wlkOnTile = _parent.rcity.walkers( tile->pos() );
          walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
        }
      }

      SmartList< T > result;
      for( auto w : walkersInArea )
      {
        if( w->type() == type || type == walker::any )
          result.addIfValid( w.as<T>() );
      }

      return result;
    }


    Statistic& _parent;
    std::map<int, WalkerList> cached;
  } walkers;

  struct _Objects
  {
    template< class T >
    SmartList< T > find( std::set<object::Type> which ) const
    {
      SmartList< T > ret;
      const OverlayList& ovs = _parent.rcity.overlays();

      for( auto ov : ovs )
      {
        if( which.count( ov->type() ) > 0 )
        {
          ret << ov;
        }
      }

      return ret;
    }

    template< class T >
    SmartList<T> find() const
    {
      SmartList<T> ret;
      const OverlayList& buildings = _parent.rcity.overlays();
      for( auto item : buildings )
        ret.addIfValid( item.as<T>() );

      return ret;
    }

    template< class T >
    SmartList< T > find( object::Group group, const TilePos& start, const TilePos& stop )
    {
      SmartList< T > ret;

      gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );

      for( auto tile : area )
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
    SmartList<T> find( object::Group group ) const
    {
      SmartList<T> ret;
      const OverlayList& buildings = _parent.rcity.overlays();
      for( auto item : buildings )
      {
        SmartPtr<T> b = item.as<T>();
        if( b.isValid() && (b->group() == group || group == object::group::any ) )
        {
          ret.push_back( b );
        }
      }

      return ret;
    }

    template< class T >
    SmartList< T > find( object::Type type ) const
    {
      SmartList< T > ret;
      const OverlayList& buildings = _parent.rcity.overlays();
      for( auto bld : buildings )
      {
        if( bld.isValid() && (bld->type() == type || type == object::any) )
          ret.addIfValid( bld.as<T>() );
      }

      return ret;
    }

    template< class T >
    SmartList< T > find( const object::Type type, TilePos start, TilePos stop ) const
    {
      SmartList< T > ret;

      gfx::TilesArea area( _parent.rcity.tilemap(), start, stop );
      for( auto tile : area )
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
    SmartPtr< T > next( SmartPtr< T > current ) const
    {
      if( current.isNull() )
        return SmartPtr<T>();

      SmartList< T > objects = find<T>( current->type() );
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
    SmartPtr<T> prew( SmartPtr<T> current) const
    {
      if( current.isNull() )
        return SmartPtr<T>();

      SmartList< T > objects = find<T>( current->type() );
      foreach( obj, objects )
      {
        if( current == *obj )
        {
          if (obj == objects.begin()) // MSVC compiler doesn't support circular lists. Neither standart does.
          {
            obj = objects.end();
          }
          obj--;
          return *obj;
        }
      }

      return SmartPtr<T>();
    }

    template<class T, class B>
    SmartList<T> neighbors( SmartPtr<B> overlay) const
    {
      OverlayList ovs = neighbors( ptr_cast<Overlay>( overlay ), true );
      SmartList<T> ret;
      ret << ovs;
      return ret;
    }

    template< class T >
    SmartList<T> producers(const good::Product goodtype) const
    {
      SmartList< T > ret;
      const OverlayList& overlays = _parent.rcity.overlays();
      for( auto ov : overlays )
      {
        SmartPtr< T > b = ov.as<T>();
        if( b.isValid() && b->produceGoodType() == goodtype )
        {
          ret.push_back( b );
        }
      }

      return ret;
    }

    template< class T >
    SmartList< T > findNotIn( const std::set<object::Group>& which ) const
    {
      SmartList< T > ret;
      const OverlayList& ovs = _parent.rcity.overlays();

      for( auto ov : ovs )
      {
        if( which.count( ov->group() ) == 0 )
        {
          ret.addIfValid( ov.as<T>() );
        }
      }

      return ret;
    }


    enum { maxLaborDistance=8 };

    OverlayList neighbors( OverlayPtr overlay, bool v ) const;
    FarmList farms(std::set<object::Type> which=std::set<object::Type>() ) const;
    HouseList houses( std::set<int> levels=std::set<int>() ) const;
    int laborAccess( WorkingBuildingPtr wb ) const;

    Statistic& _parent;
  } objects;

  struct _Tax
  {
    unsigned int possible() const;
    unsigned int payersPercent() const;
    Statistic& _parent;

    static const int minServiceValue=25;
  } tax;

  struct WorkersInfo
  {
    int current;
    int need;
  };

  struct _Workers
  {
    WorkersInfo details() const;
    unsigned int need() const;
    int wagesDiff() const;
    unsigned int monthlyWages() const;
    float monthlyOneWorkerWages() const;
    unsigned int available() const;
    unsigned int worklessPercent() const;
    unsigned int workless() const;

    Statistic& _parent;
  } workers;

  struct _Population
  {
    CitizenGroup details() const;
    unsigned int current() const;

    Statistic& _parent;
  } population;

  struct _Food
  {
    unsigned int inGranaries() const;
    unsigned int monthlyConsumption() const;
    unsigned int possibleProducing() const;

    Statistic& _parent;
  } food;

  struct _Services
  {
    template<class T>
    SmartPtr<T> find() const
    {
      SrvcPtr ret = _parent.rcity.findService( T::defaultName() );
      return ptr_cast<T>( ret );
    }

    Statistic& _parent;
  } services;

  struct _Festival
  {
    unsigned int calcCost( FestivalType type ) const;

    enum { greatFestivalCostLimiter=5,
           middleFestivalCostLimiter=10,
           smallFestivalMinCost=10,
           smallFestivalCostLimiter=20,
           middleFestivalMinCost=20,
           greatFestivalMinCost=40 };

    Statistic& _parent;
  } festival;

  struct _Crime
  {
    unsigned int level() const;

    Statistic& _parent;
  } crime;

  struct _Goods
  {
    good::ProductMap details(bool includeGranary) const;
    bool canImport( good::Product type ) const;
    bool canProduce( good::Product type ) const;

    Statistic& _parent;
  } goods;

  struct _Health
  {
    unsigned int value() const;

    Statistic& _parent;
  } health;

  struct _Military
  {
    int months2lastAttack() const;

    Statistic& _parent;
  } military;

  PlayerCity& rcity;
};

namespace statistic
{

unsigned int blackHouses( PlayerCityPtr city );
HouseList getEvolveHouseReadyBy(PlayerCityPtr, const object::TypeSet& checkTypes);
HouseList getEvolveHouseReadyBy(PlayerCityPtr, const object::Type checkTypes);
float getBalanceKoeff( PlayerCityPtr city );
int getEntertainmentCoverage(PlayerCityPtr city, Service::Type service );

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
SmartList< T > getObjectsNotIs( PlayerCityPtr r, const std::set<object::Type>& which )
{
  SmartList< T > ret;
  const OverlayList& ovs = r->overlays();

  foreach( it, ovs )
  {
    if( which.count( (*it)->type() ) == 0 )
    {
      ret.addIfValid( (*it).as<T>() );
    }
  }

  return ret;
}

}//end namespace statistic

}//end namespace city

#endif //__CAESARIA_CITYSTATISTIC_H_INCLUDED__
