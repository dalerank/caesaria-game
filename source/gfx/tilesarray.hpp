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

#ifndef _CAESARIA_TILESARRAY_INCLUDE_H_
#define _CAESARIA_TILESARRAY_INCLUDE_H_

#include "tile.hpp"

#include <vector>
#include <cstring>

namespace gfx
{

class TilesArray : public std::vector<Tile*>
{
public:
  bool contain( TilePos tilePos ) const
  {
    foreach( it, *this )
    {
      if( (*it)->pos() == tilePos )
        return true;
    }

    return false;
  }

  TilesArray() {}

  TilesArray( const TilesArray& a )
  {
    *this = a;
  }

  TilePos leftUpCorner() const
  {
    if( empty() )
      return TilePos( -1, -1 );

    TilePos ret( 9999, 0 );
    foreach( it, *this )
    {
      const TilePos& cpos = (*it)->pos();
      if( cpos.i() < ret.i() ) { ret.setI( cpos.i() ); }
      if( cpos.j() > ret.j() ) { ret.setJ( cpos.j() ); }
    }

    return ret;
  }

  TilePos rightDownCorner() const
  {
    if( empty() )
      return TilePos( -1, -1 );

    TilePos ret( 0, 9999 );
    foreach( it, *this )
    {
      const TilePos& cpos = (*it)->pos();
      if( cpos.j() < ret.j() ) { ret.setJ( cpos.j() ); }
      if( cpos.i() > ret.i() ) { ret.setI( cpos.i() ); }
    }

    return ret;
  }

  TilesArray& operator=(const TilesArray& a)
  {
    resize( a.size() );
    memcpy( &front(), &a.front(), (unsigned int)(sizeof(Tile*) * a.size()) );   

    return *this;
  }

  TilesArray& append( const TilesArray& a )
  {
    insert( end(), a.begin(), a.end() );

    return *this;
  }

  TilesArray walkableTiles( bool alllands=false ) const
  {
    TilesArray ret;
    foreach( i, *this)
    {
      if( (*i)->isWalkable( alllands ) )
          ret.push_back( *i );
    }

    return ret;
  }

  TileOverlayList overlays() const
  {
    TileOverlayList ret;
    foreach( i, *this)
    {
      if( (*i)->overlay().isValid() )
        ret << (*i)->overlay();
    }

    return ret;
  }

  Tile* random() const
  {
    return size() > 0 ? (*this)[ math::random( size() ) ] : 0;
  }
};

}//end namespace

#endif //_CAESARIA_TILESARRAY_INCLUDE_H_
