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

Tile::Tile(const int i, const int j)
{
  _i = i;
  _j = j;
  _picture = NULL;
  _master_tile = NULL;
  _terrain.reset();
}


Tile::Tile(const Tile& clone)
{
  _i = clone._i;
  _j = clone._j;
  _picture = clone._picture;
  _master_tile = clone._master_tile;
  _terrain = clone._terrain;
}

int Tile::getI() const    {   return _i;   }

int Tile::getJ() const    {   return _j;   }


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
  return TilePos( _i, _j );
}

bool Tile::is_master_tile() const
{
  return (_master_tile == this);
}
