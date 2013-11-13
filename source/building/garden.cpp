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

#include "garden.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "game/city.hpp"
#include "game/tilemap.hpp"
#include "constants.hpp"

Garden::Garden() : Construction(constants::construction::B_GARDEN, Size(1) )
{
  // always set picture to 110 (tree garden) here, for sake of building preview
  // actual garden picture will be set upon building being constructed
  //setPicture( Picture::load( ResourceGroup::entertaiment, 110 ) ); // 110 111 112 113
}

void Garden::initTerrain(Tile& terrain)
{
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlBuilding, true ); // are gardens buildings or not???? try to investigate from original game
  terrain.setFlag( Tile::tlGarden, true);
  terrain.setFlag( Tile::tlMeadow, isMeadow);
}

bool Garden::isWalkable() const
{
  return true;
}

bool Garden::isNeedRoadAccess() const
{
  return false;
}

void Garden::build(PlayerCityPtr city, const TilePos& pos )
{
  // this is the same arrangement of garden tiles as existed in C3
  int theGrid[2][2] = {{113, 110}, {112, 111}};

  Construction::build( city, pos );
  setPicture( ResourceGroup::entertaiment, theGrid[pos.getI() % 2][pos.getJ() % 2] );

  if( getSize().getArea() == 1 )
  {
    TilemapTiles tilesAround = city->getTilemap().getRectangle( getTilePos() - TilePos( 1, 1),
                                                                getTilePos() + TilePos( 1, 1 ) );
    foreach( Tile* tile, tilesAround )
    {
      GardenPtr garden = tile->getOverlay().as<Garden>();
      if( garden.isValid() )
      {
        garden->update();
      }
    }
  }
}

void Garden::load(const VariantMap& stream)
{
  Construction::load( stream );

  //rebuild that garden have size=1 on basic build
  //after loading size may change to 2
  if( getSize().getArea() > 1 )
  {
    Construction::build( _getCity(), getTilePos() );
  }
}

void Garden::update()
{
  TilemapArea nearTiles = _getCity()->getTilemap().getArea( getTilePos(), Size(2) );

  bool canGrow2squareGarden = ( nearTiles.size() == 4 ); // be carefull on map edges
  foreach( Tile* tile, nearTiles )
  {
    GardenPtr garden = tile->getOverlay().as<Garden>();
    canGrow2squareGarden &= (garden.isValid() && garden->getSize().getArea() <= 2 );
  }

  if( canGrow2squareGarden )
  {   
    foreach( Tile* tile, nearTiles )
    {
      TileOverlayPtr overlay = tile->getOverlay();

      //not delete himself
      if( overlay != this && overlay.isValid() )
      {
        overlay->deleteLater();
      }
    }

    setSize( 2 );
    Construction::build( _getCity(), getTilePos() );
    setPicture( ResourceGroup::entertaiment, 114 + rand() % 3 );
  }
}
