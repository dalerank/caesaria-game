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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "timber_logger.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "core/gettext.hpp"
#include "events/clearland.hpp"
#include "objects_factory.hpp"
#include "gfx/tile_config.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::lumber_mill, TimberLogger)

class TimberLogger::Impl
{
public:
  math::Distance distance2forest = 0;
  math::Distance workRange = 4;
  unsigned int treesCuted = 0;
};

TimberLogger::TimberLogger()
  : Factory(good::none, good::timber, object::lumber_mill, Size(2) ), _d( new Impl )
{
  setPicture( info().randomPicture( size() ) );

  _animation().load( ResourceGroup::commerce, 73, 10);
  _fgPictures().resize(2);
}

math::Percent TimberLogger::productivity()
{
  math::Percent workersPercentage = Factory::productivity();
  math::Percent distancePercentage = math::percentage( _d->workRange - _d->distance2forest,
                                                       _d->workRange );
  math::clamp_to<unsigned int>( distancePercentage, 0, 100 );

  return math::min( workersPercentage, distancePercentage );
}

void TimberLogger::timeStep(const unsigned long time)
{
  if( game::Date::isWeekChanged() )
  {
    _checkDistance2forest();
  }

  Factory::timeStep( time );
}

std::string TimberLogger::errorDesc() const
{
  if( _d->distance2forest > _d->workRange )
    return "##lumbermill_forest_too_far##";

  return Factory::errorDesc();
}

bool TimberLogger::canBuild( const city::AreaInfo& areaInfo ) const
{
  bool is_constructible = WorkingBuilding::canBuild( areaInfo );

  const Tile& tile = _findNearestForest( areaInfo );
  bool near_forest = tile.getFlag( Tile::tlTree );  // tells if the factory is next to a forest

  const_cast< TimberLogger* >( this )->_setError( near_forest ? "" : _("##lumber_mill_need_forest_near##"));

  return (is_constructible && near_forest);
}

void TimberLogger::load(const VariantMap& stream)
{
  Factory::load( stream );

  VARIANT_LOAD_ANY_D( _d, distance2forest, stream )
  VARIANT_LOAD_ANY_D( _d, workRange, stream )
  VARIANT_LOAD_ANY_D( _d, treesCuted, stream )
}

void TimberLogger::save(VariantMap& stream) const
{
  Factory::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, distance2forest )
  VARIANT_SAVE_ANY_D( stream, _d, workRange )
  VARIANT_SAVE_ANY_D( stream, _d, treesCuted )
}

void TimberLogger::_productReady()
{
  Factory::_productReady();
  _d->treesCuted++;

  if( _d->treesCuted > productRate() )
  {
    const Tile& tile = _findNearestForest( city::AreaInfo( _city(), pos() ) );
    events::dispatch<events::ClearTile>( tile.epos() );
    _d->treesCuted=0;
  }
}

const gfx::Tile& TimberLogger::_findNearestForest( const city::AreaInfo& areaInfo ) const
{
  Tilemap& tilemap = areaInfo.city->tilemap();
  TilesArray trees;
  if( areaInfo.city->getOption( PlayerCity::cutForest2timber ) )
    trees = tilemap.rect( areaInfo.pos + TilePos( _d->workRange, _d->workRange ),
                         size() + Size( 2 + _d->workRange ), Tilemap::CheckCorners );
  else
    trees = tilemap.rect( areaInfo.pos + TilePos( -1, -1 ), size() + Size( 2 ), Tilemap::CheckCorners );

  trees = trees.select( Tile::tlTree );

  math::Distance minimum = 9999;
  Tile* result = 0;
  for( auto tile : trees )
  {
    math::Distance distance = areaInfo.pos.distanceSqFrom( tile->epos() );
    if( distance < minimum )
    {
      result = tile;
      minimum = distance;
    }
  }

  return result ? *result : gfx::tile::getInvalid();
}

void TimberLogger::_checkDistance2forest()
{
  const Tile& tile = _findNearestForest( city::AreaInfo( _city(), pos() ) );
  _d->distance2forest = tile.i() >= 0
                          ? tile.epos().distanceFrom( pos() )
                          : _d->workRange + 1;
}
