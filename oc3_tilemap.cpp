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

#include "oc3_tile.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_building.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"

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
      _tile_array[i].push_back( Tile( TilePos( i, j ) ));
    }
  }
}

bool Tilemap::isInside(const TilePos& pos ) const
{
  return ( pos.getI() >= 0 && pos.getJ()>=0 && pos.getI() < _size && pos.getJ() < _size);
}

TilePos Tilemap::fit( const TilePos& pos ) const
{
  TilePos ret;
  ret.setI( math::clamp( pos.getI(), 0, _size ) );
  ret.setJ( math::clamp( pos.getJ(), 0, _size ) );
  return ret;
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

  for(int i = start.getI() + delta_corners; i <= stop.getI() - delta_corners; ++i)
  {
    if (isInside( TilePos( i, start.getJ() ) ))
    {
      res.push_back( &at(i, start.getJ() ));
    }

    if (isInside( TilePos( i, stop.getJ() ) ))
    {
      res.push_back( &at( i, stop.getJ() ));
    }
  }

  for (int j = start.getJ() + 1; j <= stop.getJ() - 1; ++j)  // corners have been handled already
  {
    if (isInside( TilePos( start.getI(), j ) ))
    {
      res.push_back(&at(start.getI(), j));
    }

    if (isInside( TilePos( stop.getI(), j ) ))
    {
      res.push_back(&at(stop.getI(), j));
    }
  }

  return res;
}

PtrTilesList Tilemap::getRectangle( const TilePos& pos, const Size& size, const bool corners /*= true */ )
{
  return getRectangle( pos, pos + TilePos( size.getWidth()-1, size.getHeight()-1), corners );
}

// Get tiles inside of rectangle
PtrTilesList Tilemap::getFilledRectangle(const TilePos& start, const TilePos& stop )
{
   PtrTilesList res;

   for (int i = start.getI(); i <= stop.getI(); ++i)
   {
      for (int j = start.getJ(); j <= stop.getJ(); ++j)
      {
         if( isInside( TilePos( i, j ) ))
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

void Tilemap::save( VariantMap& stream ) const
{
  // saves the graphics map
  VariantList bitsetInfo;
  VariantList desInfo;
  VariantList idInfo;

  PtrTilesArea tiles = const_cast< Tilemap* >( this )->getFilledRectangle( TilePos( 0, 0 ), Size( _size ) );
  for( PtrTilesArea::iterator it = tiles.begin(); it != tiles.end(); it++ )
  {
    bitsetInfo.push_back( (*it)->getTerrain().encode() );
    desInfo.push_back( (*it)->getTerrain().getDesirability() );
    idInfo.push_back( (*it)->getTerrain().getOriginalImgId() );
  }

  stream[ "bitset" ]       = bitsetInfo;
  stream[ "desirability" ] = desInfo;
  stream[ "imgId" ]        = idInfo;
  stream[ "size" ]         = _size;
}

void Tilemap::load( const VariantMap& stream )
{
  VariantList bitsetInfo = stream.get( "bitset" ).toList();
  VariantList desInfo    = stream.get( "desirability" ).toList();
  VariantList idInfo     = stream.get( "imgId" ).toList();

  int size = stream.get( "size" ).toInt();

  init( size );

  VariantList::iterator imgIdIt        = idInfo.begin();
  VariantList::iterator bitsetInfoIt   = bitsetInfo.begin();
  VariantList::iterator desirabilityIt = desInfo.begin();

  PtrTilesArea tiles = const_cast< Tilemap* >( this )->getFilledRectangle( TilePos( 0, 0 ), Size( _size ) );
  for( PtrTilesArea::iterator it = tiles.begin(); it != tiles.end(); 
       it++, imgIdIt++, bitsetInfoIt++, desirabilityIt++ )
  {
    Tile* tile = *it;

    tile->getTerrain().decode( (*bitsetInfoIt).toInt() );
    tile->getTerrain().appendDesirability( (*desirabilityIt).toInt() );

    int imgId = (*imgIdIt).toInt();
    if( imgId != 0 )
    {
      std::string picName = TerrainTileHelper::convId2PicName( imgId );
      Picture& pic = Picture::load( picName );

      tile->getTerrain().setOriginalImgId( imgId );

      int tile_size = (pic.getWidth()+2)/60;  // size of the multi-tile. the multi-tile is a square.

      // master is the left-most subtile
      Tile* master = (tile_size == 1) ? NULL : tile;
      
      for ( int di = 0; di<tile_size; ++di )
      {
        // for each subrow of the multi-tile
        for (int dj = 0; dj<tile_size; ++dj)
        {
          // for each subcol of the multi-tile
          Tile &sub_tile = at( tile->getIJ() + TilePos( di, dj ) );
          sub_tile.set_master_tile(master);
          sub_tile.set_picture(&pic);
        }
      }
    }
  }
}

Tilemap::~Tilemap()
{

}
