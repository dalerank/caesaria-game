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

#include "oc3_low_bridge.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"

#include <vector>

class LowBridgeSubTile
{
public:
  LowBridgeSubTile( const TilePos& pos, int index )
  {
    _pos = pos;
    _picture = Picture::load( ResourceGroup::transport, index );
    _picture.addOffset(30*(_pos.getI()+_pos.getJ()), 15*(_pos.getJ()-_pos.getI()));
  }

  TilePos _pos;
  Picture _picture;
};

typedef std::vector< LowBridgeSubTile > LowBridgeSubTiles;

class LowBridge::Impl
{
public:
  LowBridgeSubTiles subtiles;
};

bool LowBridge::canBuild( const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  Tile& tile = tilemap.at( pos );

  TilePos endPos;
  bool foundEndBridge=false;
  
  _d->subtiles.clear();
  const_cast< LowBridge* >( this )->_fgPictures.clear();
 
  //if( is_constructible )
  {
    if( tile.getTerrain().getOriginalImgId() == 384 || tile.getTerrain().getOriginalImgId() == 385 
        || tile.getTerrain().getOriginalImgId() == 386 || tile.getTerrain().getOriginalImgId() == 387 )
    {    
      is_constructible = true;
      PtrTilesArea tiles = tilemap.getFilledRectangle( pos - TilePos( 10, 0), pos );
      for( PtrTilesArea::reverse_iterator it=tiles.rbegin(); it != tiles.rend(); it++ )
      {
        if( (*it)->getTerrain().getOriginalImgId() == 376 || (*it)->getTerrain().getOriginalImgId() == 377 
            || (*it)->getTerrain().getOriginalImgId() == 378 || (*it)->getTerrain().getOriginalImgId() == 379 )
        {
          endPos = (*it)->getIJ();
          foundEndBridge = true;
        }
      }
    }
  }

  if( foundEndBridge )
  {
    PtrTilesArea tiles = tilemap.getFilledRectangle( endPos, pos );

    tiles.pop_back();
    tiles.pop_front();
    
    _d->subtiles.push_back( LowBridgeSubTile( tiles.front()->getIJ() - pos - TilePos( 1, 0 ), 67 ) );
    for( PtrTilesArea::iterator it=tiles.begin(); it != tiles.end(); it++ )
    {
     _d->subtiles.push_back( LowBridgeSubTile( (*it)->getIJ() - pos, 68 ) );
    }
    _d->subtiles.push_back( LowBridgeSubTile( tiles.back()->getIJ() - pos + TilePos( 1, 0 ), 69 ) );

    for( LowBridgeSubTiles::iterator it=_d->subtiles.begin(); it != _d->subtiles.end(); it++ )
    {
      const_cast< LowBridge* >( this )->_fgPictures.push_back( &(*it)._picture );
    }
  }
  
  return (is_constructible && foundEndBridge);
}

LowBridge::LowBridge() : Construction( B_LOW_BRIDGE, Size(1) ), _d( new Impl )
{
  setPicture( Picture::load( ResourceGroup::transport, 69 ) );
}

void LowBridge::setTerrain( TerrainTile& terrain )
{

}