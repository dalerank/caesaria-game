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

#include "oc3_tilemap.hpp"

#include <iostream>

#include "oc3_pic_loader.hpp"
#include "oc3_building.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"

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

bool Tilemap::is_inside(const TilePos& pos ) const
{
   return ( pos.getI() >= 0 && pos.getJ()>=0 && pos.getI()<_size && pos.getJ()<_size);
}

Tile& Tilemap::at(const int i, const int j)
{
   return _tile_array.at(i).at(j);
}

const Tile& Tilemap::at(const int i, const int j) const
{
   return _tile_array.at(i).at(j);
}

Tile& Tilemap::at( const TilePos& ij )
{
    return _tile_array.at(ij.getI()).at(ij.getJ());
    // bad! No check of arrays borders!!!
}

const Tile& Tilemap::at( const TilePos& ij ) const
{
    return _tile_array.at(ij.getI()).at(ij.getJ());
}

int Tilemap::getSize() const
{
   return _size;
}

PtrTilesList Tilemap::getRectangle( const TilePos& start, const TilePos& stop, const bool corners /*= true*/ )
{
    std::list<Tile*> res;

    int delta_corners = 0;
    if (! corners)
    {
        delta_corners = 1;
    }

    /*Rect maxRect( 0, 0, _size, _size );
    Rect rect( start.getI()+delta_corners, start.getJ()+delta_corners, 
               stop.getI()-delta_corners, stop.getJ()-delta_corners );

    rect.constrainTo( maxRect );
    for( int i=rect.getLeft(); i < rect.getRight(); i++ )
        for( int j=rect.getTop(); j < rect.getBottom(); j++ )
            ret.push_back( &at( TilePos( i, j ) ) );
    */
    for(int i = start.getI()+delta_corners; i <= stop.getI()-delta_corners; ++i)
    {
        if (is_inside( TilePos( i, start.getJ() ) ))
        {
            res.push_back( &at(i, start.getJ() ));
        }

        if (is_inside( TilePos( i, stop.getJ() ) ))
        {
            res.push_back( &at( i, stop.getJ() ));
        }
    }

    for (int j = start.getJ()+1; j <= stop.getJ()-1; ++j)  // corners have been handled already
    {
        if (is_inside( TilePos( start.getI(), j ) ))
        {
            res.push_back(&at(start.getI(), j));
        }

        if (is_inside( TilePos( stop.getI(), j ) ))
        {
            res.push_back(&at(stop.getI(), j));
        }
    }

    return res;
}

PtrTilesList Tilemap::getRectangle( const TilePos& pos, const Size& size, const bool corners /*= true */ )
{
    return getRectangle( pos, pos + TilePos( size.getWidth(), size.getHeight()), corners );
}

// Get tiles inside of rectangle
PtrTilesList Tilemap::getFilledRectangle(const TilePos& start, const TilePos& stop )
{
   PtrTilesList res;

   for (int i = start.getI(); i <= stop.getI(); ++i)
   {
      for (int j = start.getJ(); j <= stop.getJ(); ++j)
      {
         if( is_inside( TilePos( i, j ) ))
         {
            res.push_back(&at( TilePos( i, j ) ) );
         }
      }
   }

   return res;
}

PtrTilesList Tilemap::getFilledRectangle( const TilePos& start, const Size& size )
{
    return getFilledRectangle( start, start + TilePos( size.getWidth()-1, size.getHeight()-1 ) );
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
               imgId = TerrainTileHelper::convPicName2Id(pic_name);
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
           std::string picName = TerrainTileHelper::convId2PicName( imgId );
           Picture& pic = PicLoader::instance().get_picture( picName );

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

