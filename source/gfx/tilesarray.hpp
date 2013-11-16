// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _OPENCAESAR_TILESARRAY_INCLUDE_H_
#define _OPENCAESAR_TILESARRAY_INCLUDE_H_

#include "tile.hpp"

#include <list>

class TilesArray : public std::list<Tile*>
{
public:
  bool contain( TilePos tilePos ) const
  {
    for( const_iterator it=begin(); it != end(); it++ )
    {
      if( (*it)->getIJ() == tilePos )
        return true;
    }

    return false;
  }
};

#endif //_OPENCAESAR_TILESARRAY_INCLUDE_H_
