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

#ifndef __CAESARIA_WALKER_GRID_H_INCLUDED__
#define __CAESARIA_WALKER_GRID_H_INCLUDED__

#include "walker/predefinitions.hpp"
#include "gfx/tile.hpp"
#include <vector>

namespace city
{

class WalkerGrid
{

public:

  WalkerGrid()
  {
	  _gsize = 0;
  }

  void clear();

  void append( WalkerPtr a );
  void resize(Size size );
  const Size& size() const;
  void remove( WalkerPtr a );

  const WalkerList& at(const TilePos &pos );

private:
  unsigned int _offset(const TilePos &pos);

  typedef std::vector< WalkerList > Grid;

  Size _size;
  unsigned int _gsize;
  Grid _grid;
};

}

#endif//__CAESARIA_WALKER_GRID_H_INCLUDED__
