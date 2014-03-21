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
    for( const_iterator it=begin(); it != end(); ++it )
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

  TilesArray& operator=(const TilesArray& a)
  {
    resize( a.size() );
    memcpy( &front(), &a.front(), (unsigned int)(sizeof(Tile*) * a.size()) );
    /*for( const_iterator i=a.begin(); i != a.end(); i++ )
    {
      push_back( *i );
    }*/

    return *this;
  }

  TilesArray walkableTiles( bool alllands=false ) const
  {
    TilesArray ret;
    for( const_iterator i=begin(); i != end(); ++i )
    {
      if( (*i)->isWalkable( alllands ) )
          ret.push_back( *i );
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
