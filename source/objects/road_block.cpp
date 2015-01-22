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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "road_block.hpp"
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

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::roadBlock, RoadBlock)

// I didn't decide what is the best approach: make Plaza as constructions or as upgrade to roads
RoadBlock::RoadBlock()
{
  // somewhere we need to delete original road and then we need to think
  // because as we remove original road we need to recompute adjacent tiles
  // or we will run into big troubles

  setType(objects::roadBlock);
  setPicture( ResourceGroup::roadBlock, 1 );
}

// Plazas can be built ONLY on top of existing roads
// Also in original game there was a bug:
// gamer could place any number of plazas on one road tile (!!!)
bool RoadBlock::canBuild(const CityAreaInfo& areaInfo) const
{
  //std::cout << "Plaza::canBuild" << std::endl;
  Tilemap& tilemap = areaInfo.city->tilemap();

  bool is_constructible = true;

  TilesArray area = tilemap.getArea( areaInfo.pos, size() ); // something very complex ???
  foreach( tile, area )
  {
    is_constructible &= is_kind_of<Road>( (*tile)->overlay() );
  }

  return is_constructible;
}

const Picture& RoadBlock::picture(const CityAreaInfo& areaInfo) const
{
  return picture();
}

void RoadBlock::appendPaved(int) {}

bool RoadBlock::build( const CityAreaInfo& info )
{
  RoadPtr road = ptr_cast<Road>( info.city->getOverlay( info.pos ) );
  if( road.isValid() )
  {
    road->setState( (Construction::Param)Road::lockTerrain, 1 );
  }

  Construction::build( info );
  //setPicture( MetaDataHolder::randomPicture( type(), size() ) );

  return true;
}

void RoadBlock::save(VariantMap& stream) const
{
  Road::save( stream );

  stream[ "picture" ] = Variant( picture().name() );
}

void RoadBlock::load(const VariantMap& stream)
{
  Road::load( stream );

  if( size().area() > 1 )
  {
    CityAreaInfo info = { _city(), pos(), TilesArray() };
    Construction::build( info );
  }

  setPicture( Picture::load( stream.get( "picture" ).toString() ) );
}

const Picture& RoadBlock::picture() const
{
  return tile().masterTile()
           ? Construction::picture()
           : Picture::load( ResourceGroup::roadBlock, 1);
}
