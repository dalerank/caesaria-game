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
#include "city/statistic.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::garden, Garden)

Garden::Garden() : Construction( object::garden, Size(1) )
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
bool Garden::isNeedRoad() const{  return false;}

bool Garden::build( const city::AreaInfo& areainfo )
{
  // this is the same arrangement of garden tiles as existed in C3
  Construction::build( areainfo );
  setPicture( info().randomPicture( size() ) );

  if( size().area() == 1 )
  {
    auto gardens = areainfo.city->tilemap()
                              .getNeighbors(pos(), Tilemap::AllNeighbors)
                              .overlays()
                              .select<Garden>();
    for( auto garden : gardens )
      garden->update();
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
    city::AreaInfo info( _city(), pos() );
    Construction::build( info );
  }

  _picture().load( stream.get( "picture" ).toString() );
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
  TilesArray tiles = area();
  for( auto tile : tiles )
    tile->setFlag( Tile::tlGarden, false );
}

void Garden::setPicture(Picture picture)
{
  Construction::setPicture( picture );
  _flat = picture.width()/ (float)picture.height() >= 1.7f;
}

void Garden::update()
{
  TilesArea nearTiles( _map(), pos(), Size(2) );

  bool canGrow2squareGarden = ( nearTiles.size() == 4 ); // be carefull on map edges
  for( auto tile : nearTiles )
  {
    auto garden = tile->overlay<Garden>();
    canGrow2squareGarden &= (garden.isValid() && garden->size().area() <= 2 );
  }

  if( canGrow2squareGarden )
  {   
    for( auto tile : nearTiles )
    {
      OverlayPtr overlay = tile->overlay();

      //not delete himself
      if( overlay != this && overlay.isValid() )
      {
        overlay->deleteLater();
      }
    }

    Desirability::update( _city(), this, Desirability::off );

    setSize( Size( 2 ) );
    city::AreaInfo areainfo( _city(), pos() );
    Construction::build( areainfo );
    setPicture( info().randomPicture( size() ) );
    Desirability::update( _city(), this, Desirability::on );
  }
}
