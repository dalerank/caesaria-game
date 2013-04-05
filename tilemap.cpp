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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com



#include "tilemap.hpp"

#include <iostream>

#include "pic_loader.hpp"
#include "building.hpp"
#include "exception.hpp"
#include <oc3_positioni.h>

TerrainTile::TerrainTile()
{
   reset();
}

void TerrainTile::reset()
{
   _isWater = false;
   _isRock = false;
   _isTree = false;
   _isBuilding = false;
   _isRoad = false;
   _overlay = NULL;
}

bool TerrainTile::isConstructible() const
{
   return !(_isWater || _isRock || _isTree || _isBuilding || _isRoad);
}

bool TerrainTile::isDestructible() const
{
   return (_isTree || _isBuilding || _isRoad);
}

bool TerrainTile::isWater() const
{
   return _isWater;
}

void TerrainTile::setWater(const bool isWater)
{
   _isWater = isWater;
}

bool TerrainTile::isRock() const
{
   return _isRock;
}

void TerrainTile::setRock(const bool isRock)
{
   _isRock = isRock;
}

bool TerrainTile::isTree() const
{
   return _isTree;
}

void TerrainTile::setTree(const bool isTree)
{
   _isTree = isTree;
}

bool TerrainTile::isRoad() const
{
   return _isRoad;
}

void TerrainTile::setRoad(const bool isRoad)
{
   _isRoad = isRoad;
}

bool TerrainTile::isBuilding() const
{
   return _isBuilding;
}

void TerrainTile::setBuilding(const bool isBuilding)
{
   _isBuilding = isBuilding;
}

void TerrainTile::setOverlay(LandOverlay *overlay)
{
   _overlay = overlay;
}

LandOverlay *TerrainTile::getOverlay() const
{
   return _overlay;
}

int TerrainTile::encode() const
{
   int res = 0;
   if (isTree())
   {
      res += 0x1;
      res += 0x10;
   }
   if (isRock())
   {
      res += 0x2;
   }
   if (isWater())
   {
      res += 0x4;
   }
   if (isBuilding())
   {
      res += 0x8;
   }
   if (isRoad())
   {
      res += 0x40;
   }

   return res;
}

void TerrainTile::decode(const int bitset)
{
   reset();

   if (bitset & 0x1)
   {
      setTree(true);
   }
   if (bitset & 0x2)
   {
      setRock(true);
   }
   if (bitset & 0x4)
   {
      setWater(true);
   }
   if (bitset & 0x8)
   {
      setBuilding(true);
   }
   if (bitset & 0x10)
   {
      setTree(true);
   }
   if (bitset & 0x20)
   {
      // setGarden(true);
   }
   if (bitset & 0x40)
   {
      setRoad(true);
   }
   if (bitset & 0x100)
   {
      //setAqueduct(true);
   }
   if (bitset & 0x200)
   {
      //setElevation(true);
   }
   if (bitset & 0x400)
   {
      //setAccessRamp(true);
   }
   if (bitset & 0x800)
   {
      //setMeadow(true);
   }
   if (bitset & 0x4000)
   {
      //setWall(true);
   }
}

void TerrainTile::serialize(OutputSerialStream &stream)
{
   int terrainBitset = encode();
   stream.write_int(terrainBitset, 2, 0, 65535);
   stream.write_objectID(_overlay);
}

void TerrainTile::unserialize(InputSerialStream &stream)
{
   int terrainBitset = stream.read_int(2, 0, 65535);
   decode(terrainBitset);
   stream.read_objectID((void**)&_overlay);
}

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

int Tile::getI() const
{
   return _i;
}

int Tile::getJ() const
{
   return _j;
}


void Tile::set_picture(Picture *picture)
{
   _picture = picture;
}

Picture& Tile::get_picture() const
{
   if (_picture==NULL) THROW("error: picture is null");
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
   return get_terrain().isRoad();
}

TilePos Tile::getIJ() const
{
    return TilePos( _i, _j );
}

Tilemap::Tilemap()
{
   _size = 0;
}

void Tilemap::init(const int size)
{
   _size = size;

   // resize the tile array
   _tile_array.resize(_size);
   for (int i = 0; i < _size; ++i)
   {
      _tile_array[i].reserve(_size);

      for (int j = 0; j < _size; ++j)
      {
         _tile_array[i].push_back(Tile(i, j));
      }
   }
}

bool Tilemap::is_inside(const int i, const int j) const
{
   return (i>=0 && j>=0 && i<_size && j<_size);
}

Tile& Tilemap::at(const int i, const int j)
{
   return _tile_array[i][j];
}

const Tile& Tilemap::at(const int i, const int j) const
{
   return _tile_array[i][j];
}

Tile& Tilemap::at( const TilePos& ij )
{
    return _tile_array[ ij.getI() ][ ij.getJ() ];
}

int Tilemap::getSize() const
{
   return _size;
}

std::list<Tile*> Tilemap::getRectangle(const int i1, const int j1, const int i2, const int j2, const bool corners)
{
   std::list<Tile*> res;

   int delta_corners = 0;
   if (! corners)
   {
      delta_corners = 1;
   }

   for (int i = i1+delta_corners; i <= i2-delta_corners; ++i)
   {
      if (is_inside(i, j1))
      {
         res.push_back(&at(i, j1));
      }

      if (is_inside(i, j2))
      {
         res.push_back(&at(i, j2));
      }
   }

   for (int j = j1+1; j <= j2-1; ++j)  // corners have been handled already
   {
      if (is_inside(i1, j))
      {
         res.push_back(&at(i1, j));
      }

      if (is_inside(i2, j))
      {
         res.push_back(&at(i2, j));
      }
   }

   return res;
}


std::list<Tile*> Tilemap::getFilledRectangle(const int i1, const int j1, const int i2, const int j2)
{
   std::list<Tile*> res;

   for (int i = i1; i <= i2; ++i)
   {
      for (int j = j1; j <= j2; ++j)
      {
         if (is_inside(i, j))
         {
            res.push_back(&at(i, j));
         }
      }
   }

   return res;
}


void Tilemap::serialize(OutputSerialStream &stream)
{
   stream.write_int(_size, 2, 0, 1000);

   // saves the graphics map
   PicLoader &picLoader = PicLoader::instance();
   std::string pic_name;
   short int imgId;   // 16bits
   for (int i=0; i<_size; ++i)
   {
      for (int j=0; j<_size; ++j)
      {
         Tile &tile = at(i, j);
         TerrainTile &terrain = tile.get_terrain();

         imgId = 0;

         if (&tile == tile.get_master_tile() || tile.get_master_tile() == NULL)
         {
            // this is a master tile
            if ( !terrain.isBuilding() )
            {
               // this is not a building
               pic_name = tile.get_picture().get_name();
               imgId = picLoader.get_pic_id_by_name(pic_name);
            }
         }

         stream.write_int(imgId, 2, 0, 65535);
         terrain.serialize(stream);
      }
   }
}

void Tilemap::unserialize(InputSerialStream &stream)
{
   _size = stream.read_int(2, 0, 1000);
   init(_size);

   // loads the graphics map
   PicLoader &picLoader = PicLoader::instance();
   short int imgId;   // 16bits
   for (int i=0; i<_size; ++i)
   {
      for (int j=0; j<_size; ++j)
      {
         Tile &tile = at(i, j);
         TerrainTile &terrain = tile.get_terrain();

         imgId = (short)stream.read_int(2, 0, 65535);

         // terrain
         terrain.unserialize(stream);
         // cannot access the overlay at this stage (it has not be unserialized yet)

         if (imgId != 0 && !terrain.isBuilding())
         {
            // master landscape tile!
            Picture& pic = picLoader.get_pic_by_id(imgId);

            int tile_size = (pic.get_width()+2)/60;  // size of the multi-tile. the multi-tile is a square.

            // master is the left-most subtile
            Tile* master;
            if (tile_size == 1)
            {
               master = NULL;
            }
            else
            {
               master = &at(i, j);
            }

            for (int di=0; di<tile_size; ++di)
            {
               // for each subrow of the multi-tile
               for (int dj=0; dj<tile_size; ++dj)
               {
                  // for each subcol of the multi-tile
                  Tile &sub_tile = at(i+di, j+dj);
                  sub_tile.set_master_tile(master);
                  sub_tile.set_picture(&pic);
               }
            }
         }

      }
   }
}
