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
#include "oc3_building.hpp"
#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_foreach.hpp"

static Tile invalidTile = Tile( TilePos( -1, -1 ) );
typedef std::vector< Tile > Row;

class TileGrid : public std::vector< Row >
{
};

class Tilemap::Impl : public TileGrid
{
public:
  Tile& at( const int i, const int j )
  {
    if( isInside( TilePos( i, j ) ) )
    {
      return TileGrid::at(i).at(j);
    }

    StringHelper::debug( 0xff, "Need inside point current=[%d, %d]", i, j );
    return invalidTile;
  }

  bool isInside( const TilePos& pos )
  {
    return( pos.getI() >= 0 && pos.getJ()>=0 && pos.getI() < size && pos.getJ() < size);
  }

  void resize( const int s )
  {
    size = s;

    // resize the tile array
    TileGrid::resize( size );
    for( int i = 0; i < size; ++i )
    {
      TileGrid::at( i ).reserve( size );

      for (int j = 0; j < size; ++j)
      {
        TileGrid::at( i ).push_back( Tile( TilePos( i, j ) ));
      }
    }
  }

  int size;
};

Tilemap::Tilemap() : _d( new Impl )
{
  _d->size = 0;
}

void Tilemap::resize( const int size )
{
  _d->resize( size );
}

bool Tilemap::isInside(const TilePos& pos ) const
{
  return _d->isInside( pos );
}

TilePos Tilemap::fit( const TilePos& pos ) const
{
  TilePos ret;
  ret.setI( math::clamp( pos.getI(), 0, _d->size ) );
  ret.setJ( math::clamp( pos.getJ(), 0, _d->size ) );
  return ret;
}

Tile& Tilemap::at(const int i, const int j)
{
  return _d->at( i, j );
}

const Tile& Tilemap::at(const int i, const int j) const
{
  return const_cast< Tilemap* >( this )->at( i, j );
}

Tile& Tilemap::at( const TilePos& ij )
{
  return _d->at( ij.getI(), ij.getJ() );
}

const Tile& Tilemap::at( const TilePos& ij ) const
{
  return const_cast<Tilemap*>( this )->at( ij.getI(), ij.getJ() );
}

int Tilemap::getSize() const
{
  return _d->size;
}

TilemapTiles Tilemap::getRectangle( const TilePos& start, const TilePos& stop, const bool corners /*= true*/ )
{
  TilemapTiles res;

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

TilemapTiles Tilemap::getRectangle( const TilePos& pos, const Size& size, const bool corners /*= true */ )
{
  return getRectangle( pos, pos + TilePos( size.getWidth()-1, size.getHeight()-1), corners );
}

// Get tiles inside of rectangle
TilemapTiles Tilemap::getArea(const TilePos& start, const TilePos& stop )
{
   TilemapTiles res;

   for (int i = start.getI(); i <= stop.getI(); ++i)
   {
      for (int j = start.getJ(); j <= stop.getJ(); ++j)
      {
         if( isInside( TilePos( i, j ) ))
         {
            res.push_back( &at( TilePos( i, j ) ) );
         }
      }
   }

   return res;
}

TilemapTiles Tilemap::getArea( const TilePos& start, const Size& size )
{
  return getArea( start, start + TilePos( size.getWidth()-1, size.getHeight()-1 ) );
}

void Tilemap::save( VariantMap& stream ) const
{
  // saves the graphics map
  VariantList bitsetInfo;
  VariantList desInfo;
  VariantList idInfo;

  TilemapArea tiles = const_cast< Tilemap* >( this )->getArea( TilePos( 0, 0 ), Size( _d->size ) );
  foreach( Tile* tile, tiles )
  {
    bitsetInfo.push_back( TileHelper::encode( *tile ) );
    desInfo.push_back( tile->getDesirability() );
    idInfo.push_back( tile->getOriginalImgId() );
  }

  stream[ "bitset" ]       = bitsetInfo;
  stream[ "desirability" ] = desInfo;
  stream[ "imgId" ]        = idInfo;
  stream[ "size" ]         = _d->size;
}

void Tilemap::load( const VariantMap& stream )
{
  VariantList bitsetInfo = stream.get( "bitset" ).toList();
  VariantList desInfo    = stream.get( "desirability" ).toList();
  VariantList idInfo     = stream.get( "imgId" ).toList();

  int size = stream.get( "size" ).toInt();

  resize( size );

  VariantList::iterator imgIdIt        = idInfo.begin();
  VariantList::iterator bitsetInfoIt   = bitsetInfo.begin();
  VariantList::iterator desirabilityIt = desInfo.begin();

  TilemapArea tiles = getArea( TilePos( 0, 0 ), Size( _d->size ) );
  for( TilemapArea::iterator it = tiles.begin(); it != tiles.end(); 
       it++, imgIdIt++, bitsetInfoIt++, desirabilityIt++ )
  {
    Tile* tile = *it;

    TileHelper::decode( *tile, (*bitsetInfoIt).toInt() );
    tile->appendDesirability( (*desirabilityIt).toInt() );

    int imgId = (*imgIdIt).toInt();
    if( imgId != 0 )
    {
      std::string picName = TileHelper::convId2PicName( imgId );
      Picture& pic = Picture::load( picName );

      tile->setOriginalImgId( imgId );

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
          sub_tile.setMasterTile(master);
          sub_tile.setPicture(&pic);
        }
      }
    }
  }
}

Tilemap::~Tilemap()
{

}
