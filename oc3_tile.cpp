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

#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_building.hpp"

Tile::Tile( const TilePos& pos) : _terrain( 0, 0, 0, 0, 0, 0 )
{
  _pos = pos;
  _picture = NULL;
  _wasDrawn = false;
  _master_tile = NULL;
}


Tile::Tile(const Tile& clone)
{
  _pos = clone._pos;
  _picture = clone._picture;
  _master_tile = clone._master_tile;
  _terrain = clone._terrain;
  _wasDrawn = clone._wasDrawn;
}

int Tile::getI() const    {   return _pos.getI();   }

int Tile::getJ() const    {   return _pos.getJ();   }


void Tile::set_picture(Picture *picture)
{
  _picture = picture;
}

Picture& Tile::get_picture() const
{
  if( _picture==NULL ) 
  {
    THROW("error: picture is null");
  }

  return *_picture;
}

Tile* Tile::get_master_tile() const
{
  return _master_tile;
}

void Tile::set_master_tile(Tile* master)
{
  _master_tile = master;
}

const TerrainTile& Tile::get_terrain() const
{
  return _terrain;
}

TerrainTile& Tile::get_terrain()
{
  return _terrain;
}

bool Tile::is_flat() const
{
  return !(_terrain.isRock() || _terrain.isTree() || _terrain.isBuilding() || _terrain.isAqueduct());
}

TilePos Tile::getIJ() const
{
  return _pos;
}

bool Tile::is_master_tile() const
{
  return (_master_tile == this);
}
