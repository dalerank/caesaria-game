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
#include "objects/overlay.hpp"
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
  static const TilePos invalidPos;
  Helper( PlayerCityPtr city ) : _city( city ) {}

  template<class T>
  Pathway findFreeTile( TilePos target, TilePos currentPos, const int range )
  {
    for( int currentRange=1; currentRange <= range; currentRange++ )
    {
      TilePos offset( currentRange, currentRange );
      gfx::TilesArray tiles = _city->tilemap().getRectangle( currentPos - offset, currentPos + offset );
      tiles = tiles.walkables( true );

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
  SmartPtr< T > find( const object::Type type, const TilePos& pos )
  {   
    OverlayPtr overlay = _city->getOverlay( pos );
    if( overlay.isValid() && (overlay->type() == type || type == object::any) )
    {
      return overlay.as<T>();
    }

    return SmartPtr<T>();
  }    

  template< class T >
  SmartPtr< T > next( const SmartPtr< T > current );

  template< class T >
  SmartPtr< T > prew( const SmartPtr< T > current );

  gfx::TilesArray getAroundTiles(OverlayPtr building );
  HirePriorities getHirePriorities() const;
  void updateTilePics();

protected:
  PlayerCityPtr _city;
};

}//end namespace city

#endif //__CAESARIA_CITYHELPER_H_INCLUDED__
