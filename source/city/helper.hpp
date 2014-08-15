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
  SmartList< T > find( constants::building::Group group )
  {
    SmartList< T > ret;
    gfx::TileOverlayList& buildings = _city->overlays();
    foreach( item, buildings )
    {
      SmartPtr< T > b = ptr_cast< T >(*item);
      if( b.isValid() && (b->group() == group || group == constants::building::anyGroup ) )
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
    walkers << _city->walkers( constants::walker::all, p );
    foreach( it, walkers )
    {
      if( *it == caller.object() )
      {
        needMeMove = (it != walkers.begin());
        walkers.erase( it );
        break;
      }
    }

    return !walkers.empty();
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
        eslist << _city->walkers( constants::walker::any, (*itile)->pos() );

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
    if( overlay.isValid() && (overlay->type() == type || type == constants::building::any) )
    {
      return ptr_cast< T >( overlay );
    }

    return SmartPtr<T>();
  }  

  template< class T >
  SmartList< T > find( constants::walker::Type type,
                       TilePos start, TilePos stop=Helper::invalidPos )
  {
    SmartList< T > ret;

    WalkerList walkers = _city->walkers( type, start, stop );
    foreach( w, walkers )
    {
      SmartPtr< T > ptr = ptr_cast<T>( *w );
      if( ptr.isValid() )
      {
        ret.push_back( ptr );
      }
    }

    return ret;
  }

  template< class T >
  SmartList< T > find( const gfx::TileOverlay::Type type, TilePos start, TilePos stop )
  {
    SmartList< T > ret;

    gfx::TilesArray area = getArea( start, stop );
    foreach( tile, area )
    {
      SmartPtr<T> obj = ptr_cast< T >( (*tile)->overlay() );
      if( obj.isValid() && (obj->type() == type || type == constants::building::any) )
      {
        ret.push_back( obj );
      }
    }    

    return ret;
  }

  template< class T >
  SmartList< T > find( constants::building::Group group, TilePos start, TilePos stop )
  {
    SmartList< T > ret;

    gfx::TilesArray area = getArea( start, stop );

    foreach( tile, area )
    {
      SmartPtr<T> obj = ptr_cast< T >((*tile)->overlay());
      if( obj.isValid() && (obj->getClass() == group || group == constants::building::anyGroup) )
      {
        ret.push_back( obj );
      }
    }

    return ret;
  }

  template< class T >
  SmartList< T > getProducers( const Good::Type goodtype );

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
      obj--;
      if( obj == objects.end() ) { return objects.back(); }
      else { return *obj; }
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
    if( b.isValid() && (b->type() == type || type == constants::building::any ) )
    {
      ret.push_back( b );
    }
  }

  return ret;
}

template< class T >
SmartList<T> Helper::getProducers( const Good::Type goodtype )
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
