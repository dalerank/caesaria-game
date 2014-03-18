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

#include "garden.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "core/stringhelper.hpp"

Garden::Garden() : Construction(constants::construction::garden, Size(1) )
{
  // always set picture to 110 (tree garden) here, for sake of building preview
  // actual garden picture will be set upon building being constructed
  //setPicture( Picture::load( ResourceGroup::entertaiment, 110 ) ); // 110 111 112 113
}

void Garden::initTerrain(Tile& terrain)
{
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlGarden, true);
  terrain.setFlag( Tile::tlMeadow, isMeadow);
}

bool Garden::isWalkable() const
{
  return true;
}

bool Garden::isFlat() const
{
  return true;
}

bool Garden::isNeedRoadAccess() const
{
  return false;
}

void Garden::build(PlayerCityPtr city, const TilePos& p )
{
  // this is the same arrangement of garden tiles as existed in C3
  int theGrid[2][2] = {{113, 110}, {112, 111}};

  Construction::build( city, p );
  setPicture( ResourceGroup::entertaiment, theGrid[p.i() % 2][p.j() % 2] );

  if( size().area() == 1 )
  {
    TilesArray tilesAround = city->getTilemap().getRectangle( pos() - TilePos( 1, 1),
                                                              pos() + TilePos( 1, 1 ) );
    foreach( tile, tilesAround )
    {
      GardenPtr garden = ptr_cast<Garden>( (*tile)->overlay() );
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
  if( size().area() > 1 )
  {
    Construction::build( _city(), pos() );
  }
}

Desirability Garden::getDesirability() const
{
  Desirability ret = Construction::getDesirability();
  ret.base *= size().area();
  ret.range *= size().width();
  ret.step *= size().width();

  return ret;
}

std::string Garden::getSound() const
{
  return StringHelper::format( 0xff, "garden_%05d.wav", size().area() );
}

void Garden::update()
{
  TilesArray nearTiles = _city()->getTilemap().getArea( pos(), Size(2) );

  bool canGrow2squareGarden = ( nearTiles.size() == 4 ); // be carefull on map edges
  foreach( tile, nearTiles )
  {
    GardenPtr garden = ptr_cast<Garden>( (*tile)->overlay() );
    canGrow2squareGarden &= (garden.isValid() && garden->size().area() <= 2 );
  }

  if( canGrow2squareGarden )
  {   
    foreach( tile, nearTiles )
    {
      TileOverlayPtr overlay = (*tile)->overlay();

      //not delete himself
      if( overlay != this && overlay.isValid() )
      {
        overlay->deleteLater();
      }
    }

    city::Helper helper( _city() );
    helper.updateDesirability( this, false );
    setSize( 2 );
    Construction::build( _city(), pos() );
    setPicture( ResourceGroup::entertaiment, 114 + rand() % 3 );
    helper.updateDesirability( this, true );
  }
}
