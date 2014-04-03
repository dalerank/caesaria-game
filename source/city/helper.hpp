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
#include "core/foreach.hpp"
#include "objects/service.hpp"

#include <set>

namespace city
{

class Helper
{
public:
  static const TilePos invalidPos;
  Helper( PlayerCityPtr city ) : _city( city ) {}

  template< class T >
  std::list< SmartPtr< T > > find( const gfx::TileOverlay::Type type );

  template< class T >
  std::list< SmartPtr< T > > find( constants::building::Group group )
  {
    std::list< SmartPtr< T > > ret;
    gfx::TileOverlayList& buildings = _city->getOverlays();
    foreach( item, buildings )
    {
      SmartPtr< T > b = ptr_cast< T >(*item);
      if( b.isValid() && (b->getClass() == group || group == constants::building::anyGroup ) )
      {
        ret.push_back( b );
      }
    }

    return ret;
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
  std::list< SmartPtr< T > > find( constants::walker::Type type,
                                   TilePos start, TilePos stop=Helper::invalidPos )
  {
    std::list< SmartPtr< T > > ret;

    WalkerList walkers = _city->getWalkers( type, start, stop );
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
  std::list< SmartPtr< T > > find( const gfx::TileOverlay::Type type, TilePos start, TilePos stop )
  {
    std::set< SmartPtr< T > > tmp;

    gfx::TilesArray area = getArea( start, stop );
    foreach( tile, area )
    {
      SmartPtr<T> obj = ptr_cast< T >( (*tile)->overlay() );
      if( obj.isValid() && (obj->type() == type || type == constants::building::any) )
      {
        tmp.insert( obj );
      }
    }    

    std::list< SmartPtr< T > > ret;
    foreach( obj, tmp )
    {
      ret.push_back( *obj );
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > find( constants::building::Group group, TilePos start, TilePos stop )
  {
    std::set< SmartPtr< T > > tmp;

    gfx::TilesArray area = getArea( start, stop );

    foreach( tile, area )
    {
      SmartPtr<T> obj = ptr_cast< T >((*tile)->overlay());
      if( obj.isValid() && (obj->getClass() == group || group == constants::building::anyGroup) )
      {
        tmp.insert( obj );
      }
    }

    std::list< SmartPtr< T > > ret;
    foreach( obj, tmp )
    {
      ret.push_back( obj );
    }

    return ret;
  }

  template< class T >
  std::list< SmartPtr< T > > getProducers( const Good::Type goodtype );

  template< class T >
  SmartPtr< T > next( const SmartPtr< T > current );

  template< class T >
  SmartPtr< T > prew( const SmartPtr< T > current );

  gfx::TilesArray getArea( gfx::TileOverlayPtr overlay );
  gfx::TilesArray getAroundTiles( gfx::TileOverlayPtr building );
  gfx::TilesArray getArea( TilePos start, TilePos stop );
  float getBalanceKoeff();

  void updateDesirability( ConstructionPtr construction, bool onBuild );

protected:
  PlayerCityPtr _city;
};

template<class T>
SmartPtr<T> Helper::prew(const SmartPtr<T> current)
{
  if( current.isNull() )
    return SmartPtr<T>();

  std::list< SmartPtr< T > > objects = find<T>( current->type() );
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
std::list< SmartPtr< T > > Helper::find( const gfx::TileOverlay::Type type )
{
  std::list< SmartPtr< T > > ret;
  gfx::TileOverlayList& buildings = _city->getOverlays();
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
std::list< SmartPtr< T > > Helper::getProducers( const Good::Type goodtype )
{
  std::list< SmartPtr< T > > ret;
  gfx::TileOverlayList& overlays = _city->getOverlays();
  foreach( item, overlays )
  {
    SmartPtr< T > b = ptr_cast<T>( *item );
    if( b.isValid() && b->getOutGoodType() == goodtype )
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

  std::list< SmartPtr< T > > objects = find<T>( current->type() );
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
