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
#include "events/warningmessage.hpp"
#include "objects_factory.hpp"

using namespace gfx;
using namespace constants;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::roadBlock, RoadBlock)

RoadBlock::RoadBlock()
{
  setType(object::roadBlock);
  setPicture( ResourceGroup::roadBlock, 1 );
}

// Blocks can be built ONLY on top of existing roads
// Also in original game there was a bug:
// gamer could place any number of plazas on one road tile (!!!)
namespace pr
{
const Param errorBuild( utils::hash("rblock_error") );
}

bool RoadBlock::canBuild(const city::AreaInfo& areaInfo) const
{
  Tilemap& tilemap = areaInfo.city->tilemap();

  bool is_constructible = true;

  TilesArray area = tilemap.getArea( areaInfo.pos, size() ); // something very complex ???
  foreach( tile, area )
  {
    is_constructible &= is_kind_of<Road>( (*tile)->overlay() );
  }

  const_cast<RoadBlock*>( this )->setState( pr::errorBuild, !is_constructible );

  return is_constructible;
}

std::string RoadBlock::errorDesc() const
{
  if( state( pr::errorBuild ) )
    return "##roadblock_build_over_road##";

  return "";
}


const Picture& RoadBlock::picture(const city::AreaInfo& areaInfo) const
{
  return picture();
}

void RoadBlock::appendPaved(int) {}

bool RoadBlock::build( const city::AreaInfo& info )
{
  RoadPtr road = ptr_cast<Road>( info.city->getOverlay( info.pos ) );
  if( road.isValid() )
  {
    road->setState( pr::lockTerrain, 1 );
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
    city::AreaInfo info = { _city(), pos(), TilesArray() };
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
