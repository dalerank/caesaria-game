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

#ifndef __CAESARIA_TILE_CONFIG_H_INCLUDED__
#define __CAESARIA_TILE_CONFIG_H_INCLUDED__

#include "tile.hpp"

namespace gfx
{

class Tilemap;

namespace tile
{    
  int encode( const Tile& tt );
  int turnCoastTile(int imgid , Direction newDirection);
  TilePos hash2pos( unsigned int hash );
  Tile::Type findType( const std::string& name );
  void decode( Tile& tile, const int bitset);
  const Tile& getInvalid();
  Tile& getInvalidSafe();
  void fixPlateauFlags( Tile& tile );
}//end namespace tile

}//end namespace gfx

#endif //__CAESARIA_TILE_CONFIG_H_INCLUDED__
