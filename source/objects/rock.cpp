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

#include "rock.hpp"
#include "gfx/tile.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilearea.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "gfx/imgid.hpp"
#include "core/foreach.hpp"
#include "coast.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::rock, Rock)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::plateau, Plateau)

namespace {
  static Renderer::PassQueue riftPassQueue=Renderer::PassQueue(1,Renderer::ground);
}

Rock::Rock() : Overlay( object::rock, Size::square(1) )
{
  // always set picture to 110 (tree garden) here, for sake of building preview
  // actual garden picture will be set upon building being constructed
  setPicture( info().randomPicture( Size::square(1) ) ); // 110 111 112 113
}

void Rock::initTerrain(Tile& tile)
{
  tile.terrain().clear();
  tile.terrain().rock = true;
}

bool Rock::isWalkable() const {  return false; }
bool Rock::isDestructible() const { return false; }
bool Rock::isFlat() const{ return _flat;}

bool Rock::build( const city::AreaInfo& areainfo )
{
  // this is the same arrangement of garden tiles as existed in C3
  Overlay::build( areainfo );

  bool isUpdated = update();

  if( !isUpdated )
  {
    tile().setPicture( picture() );
    tile().setImgId( imgid::fromResource( picture().name() ) );
    tile().setAnimation( Animation() );
  }

  return true;
}

void Rock::destroy()
{
  for( auto tile : area() )
    tile->terrain().rock = false;
}

void Rock::setPicture(Picture picture)
{
  Overlay::setPicture( picture );
  _flat = picture.width() / (float)picture.height() >= 1.7f;
}

bool Rock::update()
{
  TilesArea nearTiles9( _map(), pos(), Size::square(3) );

  bool canGrow2hugeRock = ( nearTiles9.size() == 9 );
  for( auto tile : nearTiles9 )
  {
    auto rock = tile->overlay<Rock>();
    canGrow2hugeRock &= (rock.isValid() && rock->size().area() <= 2 );
  }

  if( canGrow2hugeRock )
  {
    for( auto tile : nearTiles9 )
    {
      OverlayPtr overlay = tile->overlay();

      //not delete himself
      if( overlay != this && overlay.isValid() )
      {
        overlay->deleteLater();
      }
    }

    setSize( Size::square( 3 ) );
    city::AreaInfo areainfo( _city(), pos() );
    Overlay::build( areainfo );
    setPicture( info().randomPicture( size() ) );
    return true;
  }


  TilesArea nearTiles( _map(), pos(), Size::square(2) );
  bool canGrow2squareRock = ( nearTiles.size() == 4 ); // be carefull on map edges
  for( auto tile : nearTiles )
  {
    auto rock = tile->overlay<Rock>();
    canGrow2squareRock &= (rock.isValid() && rock->size().area() <= 2 );
  }

  if( canGrow2squareRock )
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

    setSize( Size::square( 2 ) );
    city::AreaInfo areainfo( _city(), pos() );
    Overlay::build( areainfo );
    setPicture( info().randomPicture( size() ) );
    return true;
  }

  return false;
}

SmartList<Rock> Rock::neighbors() const
{
  TilePos w2p( size().width(), size().height() );
  return _map().rect( pos() - TilePos(1,1), pos() + w2p, true )
               .overlays()
               .select<Rock>();
}

Plateau::Plateau()
{
  setType( object::plateau );
  setPicture( info().randomPicture( Size::square(1) ) );
}

bool Plateau::update() { return true; }
