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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_CITYHELPER_H_INCLUDED__
#define __CAESARIA_CITYHELPER_H_INCLUDED__

#include "city.hpp"
#include "gfx/tileoverlay.hpp"
#include "gfx/tilesarray.hpp"
#include "good/good.hpp"
#include "gfx/tilesarray.hpp"
#include "gfx/tilemap.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/foreach.hpp"
#include "walker/walker.hpp"
#include "objects/service.hpp"
#include "city/industry.hpp"

#include <set>

namespace city
{

class Helper
{
public:
  enum { offDesirability=false,
         onDesirability=true };

  static const TilePos invalidPos;
  Helper( PlayerCityPtr city ) : _city( city ) {}

  template< class T >
  SmartList< T > find( const gfx::TileOverlay::Type type );

  template< class T >
  SmartList< T > find( constants::objects::Group group )
  {
    SmartList< T > ret;
    gfx::TileOverlayList& buildings = _city->overlays();
    foreach( item, buildings )
    {
      SmartPtr< T > b = ptr_cast< T >(*item);
      if( b.isValid() && (b->group() == group || group == constants::objects::anyGroup ) )
      {
        ret.push_back( b );
      }
    }

    return ret;
  }

  template<class T>
  bool isTileBusy( TilePos p, WalkerPtr caller, bool& needMeMove )
  {
    needMeMove = false;
    SmartList<T> walkers;
    walkers << _city->walkers( p );

    if( !walkers.empty() )
    {
      needMeMove = (caller.object() != walkers.front().object());
    }

    return walkers.empty() > 1;
  }

  template<class T>
  Pathway findFreeTile( TilePos target, TilePos currentPos, const int range )
  {
    for( int currentRange=1; currentRange <= range; currentRange++ )
    {
      TilePos offset( currentRange, currentRange );
      gfx::TilesArray tiles = _city->tilemap().getRectangle( currentPos - offset, currentPos + offset );
      tiles = tiles.walkableTiles( true );

      float crntDistance = target.distanceFrom( currentPos );
      foreach( itile, tiles )
      {
        SmartList<T> eslist;
        eslist << _city->walkers( (*itile)->pos() );

        if( !eslist.empty() )
          continue;

        if( target.distanceFrom( (*itile)->pos() ) > crntDistance )
          continue;

        Pathway pathway = PathwayHelper::create( currentPos, (*itile)->pos(), PathwayHelper::allTerrain );
        if( pathway.isValid() )
        {
          return pathway;
        }
      }
    }

    return Pathway();
  }

  template< class T >
  SmartPtr< T > find( const gfx::TileOverlay::Type type, const TilePos& pos )
  {   
    gfx::TileOverlayPtr overlay = _city->getOverlay( pos );
    if( overlay.isValid() && (overlay->type() == type || type == constants::objects::any) )
    {
      return ptr_cast< T >( overlay );
    }

    return SmartPtr<T>();
  }  

  template< class T >
  SmartList< T > find( constants::walker::Type type,
                       TilePos start, TilePos stop=Helper::invalidPos )
  {
    WalkerList walkersInArea;

    TilePos invalidPos( -1, -1 );
    TilePos stopPos = stop;

    if( start == invalidPos )
    {
      const WalkerList& all = _city->walkers();
      walkersInArea.insert( walkersInArea.end(), all.begin(), all.end() );
    }
    else if( stopPos == invalidPos )
    {
      const WalkerList& wlkOnTile = _city->walkers( start );
      walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
    }
    else
    {
      gfx::TilesArray area = _city->tilemap().getArea( start, stop );
      foreach( tile, area)
      {
        const WalkerList& wlkOnTile = _city->walkers( (*tile)->pos() );
        walkersInArea.insert( walkersInArea.end(), wlkOnTile.begin(), wlkOnTile.end() );
      }
    }

    SmartList< T > result;
    foreach( w, walkersInArea )
    {
      if( (*w)->type() == type || type == constants::walker::any )
      {
        SmartPtr< T > ptr = ptr_cast<T>( *w );
        if( ptr.isValid() )
          result.push_back( ptr );
      }
    }

    return result;
  }

  template< class T >
  SmartList< T > find( const constants::objects::Type type, TilePos start, TilePos stop=TilePos(-1,-1) )
  {
    SmartList< T > ret;

    gfx::TilesArray area = getArea( start, stop );
    foreach( tile, area )
    {
      SmartPtr<T> obj = ptr_cast< T >( (*tile)->overlay() );
      if( obj.isValid() && (obj->type() == type || type == constants::objects::any) )
      {
        ret.push_back( obj );
      }
    }    

    return ret;
  }

  template< class T >
  SmartList< T > find( constants::objects::Group group, TilePos start, TilePos stop )
  {
    SmartList< T > ret;

    gfx::TilesArray area = getArea( start, stop );

    foreach( tile, area )
    {
      SmartPtr<T> obj = ptr_cast< T >((*tile)->overlay());
      if( obj.isValid() && (obj->getClass() == group || group == constants::objects::anyGroup) )
      {
        ret.push_back( obj );
      }
    }

    return ret;
  }

  template< class T >
  SmartList< T > getProducers( const good::Product goodtype );

  template< class T >
  SmartPtr< T > next( const SmartPtr< T > current );

  template< class T >
  SmartPtr< T > prew( const SmartPtr< T > current );

  gfx::TilesArray getArea( gfx::TileOverlayPtr overlay );
  gfx::TilesArray getAroundTiles(gfx::TileOverlayPtr building );
  gfx::TilesArray getArea( TilePos start, TilePos stop );
  HirePriorities getHirePriorities() const;
  void updateTilePics();

  void updateDesirability(gfx::TileOverlayPtr overlay, bool onBuild );

protected:
  PlayerCityPtr _city;
};

template<class T>
SmartPtr<T> Helper::prew(const SmartPtr<T> current)
{
  if( current.isNull() )
    return SmartPtr<T>();

  SmartList< T > objects = find<T>( current->type() );
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

template< class T >
SmartList< T > Helper::find( const gfx::TileOverlay::Type type )
{
  SmartList< T > ret;
  gfx::TileOverlayList& buildings = _city->overlays();
  foreach( item, buildings )
  {
    SmartPtr< T > b = ptr_cast<T>( *item );
    if( b.isValid() && (b->type() == type || type == constants::objects::any ) )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template< class T >
SmartList<T> Helper::getProducers(const good::Product goodtype )
{
  SmartList< T > ret;
  gfx::TileOverlayList& overlays = _city->overlays();
  foreach( item, overlays )
  {
    SmartPtr< T > b = ptr_cast<T>( *item );
    if( b.isValid() && b->produceGoodType() == goodtype )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template< class T >
SmartPtr< T > Helper::next( const SmartPtr< T > current )
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

}//end namespace city

#endif //__CAESARIA_CITYHELPER_H_INCLUDED__
