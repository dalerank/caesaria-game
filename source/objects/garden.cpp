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

#include "garden.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::garden, Garden)

Garden::Garden() : Construction(constants::objects::garden, Size(1) )
{
  // always set picture to 110 (tree garden) here, for sake of building preview
  // actual garden picture will be set upon building being constructed
  //setPicture( Picture::load( ResourceGroup::entertaiment, 110 ) ); // 110 111 112 113
}

void Garden::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::tlGarden, true);
}

bool Garden::isWalkable() const {  return _flat; }
bool Garden::isFlat() const{ return _flat;}
bool Garden::isNeedRoadAccess() const{  return false;}

bool Garden::build( const CityAreaInfo& info )
{
  // this is the same arrangement of garden tiles as existed in C3
  Construction::build( info );
  setPicture( MetaDataHolder::randomPicture( type(), size() ) );

  if( size().area() == 1 )
  {
    TilesArray tilesAround = info.city->tilemap().getNeighbors(pos(), Tilemap::AllNeighbors);
    foreach( tile, tilesAround )
    {
      GardenPtr garden = ptr_cast<Garden>( (*tile)->overlay() );
      if( garden.isValid() )
      {
        garden->update();
      }
    }
  }

  return true;
}

void Garden::load(const VariantMap& stream)
{
  Construction::load( stream );

  //rebuild that garden have size=1 on basic build
  //after loading size may change to 2
  if( size().area() > 1 )
  {
    CityAreaInfo info = { _city(), pos(), TilesArray() };
    Construction::build( info );
  }

  setPicture( Picture::load( stream.get( "picture" ).toString() ) );
}

void Garden::save(VariantMap& stream) const
{
  Construction::save( stream );

  stream[ "picture" ] = Variant( picture().name() );
}

Desirability Garden::desirability() const
{
  Desirability ret = Construction::desirability();
  ret.base *= (size().area() * size().width());
  //ret.range *= size().width();
  ret.step = -ret.base / ret.range;

  return ret;
}

std::string Garden::sound() const
{
  return utils::format( 0xff, "garden_%05d", size().area() );
}

void Garden::destroy()
{
  city::Helper helper( _city() );
  TilesArray tiles = helper.getArea( this );
  foreach( it, tiles ) (*it)->setFlag( Tile::tlGarden, false );
}

void Garden::setPicture(Picture picture)
{
  Construction::setPicture( picture );
  _flat = picture.width()/ (float)picture.height() >= 1.7f;
}

void Garden::update()
{
  TilesArray nearTiles = _city()->tilemap().getArea( pos(), Size(2) );

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
    helper.updateDesirability( this, city::Helper::offDesirability );
    setSize( 2 );
    CityAreaInfo info = { _city(), pos(), TilesArray() };
    Construction::build( info );
    setPicture( MetaDataHolder::randomPicture( type(), size() ) );
    helper.updateDesirability( this, city::Helper::onDesirability );
  }
}
