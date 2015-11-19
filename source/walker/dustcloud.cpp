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
// Copyright 2012- Dalerank, dalerankn8@gmail.com

#include "dustcloud.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway_helper.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/tilemap_config.hpp"
#include "walker/helper.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "walkers_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::dustCloud, DustCloud)

class DustCloud::Impl
{
public:
  gfx::Animation animation;
  struct {
    Point destination;
    Point source;
    PointF current;
    PointF speed;
    Point delta() const { return destination - source; }
  } worldway;

  struct {
    TilePos destination;
    TilePos source;
    Point destination2world() const
    {
      int yMultiplier = tilemap::cellSize().height();
      Point xOffset( 0, yMultiplier );
      return Point( destination.i(), destination.j() ) * yMultiplier + xOffset;
    }

    float distance() const { return source.distanceFrom( destination ); }

    Point source2world() const
    {
      int yMultiplier = tilemap::cellSize().height();
      Point xOffset( 0, yMultiplier );
      return Point( source.i(), source.j() ) * yMultiplier + xOffset;
    }

    TilePos nextStep() const { return source.nextStep( destination ); }

    bool mayMove() const { return destination != source; }
  } mapway;

};

void DustCloud::create(PlayerCityPtr city, const TilePos& start, unsigned int range)
{
  for( int direction=0; direction < 8; direction++ )
  {
    auto dustcloud = Walker::create<DustCloud>( city );

    TilePos offset;
    switch( direction )
    {
    case direction::north: offset = TilePos( 0, 1 ); break;
    case direction::northEast: offset = TilePos( 1, 1 ); break;
    case direction::east: offset = TilePos( 1, 0 ); break;
    case direction::southEast: offset = TilePos( 1, -1 ); break;
    case direction::south: offset = TilePos( 0, -1 ); break;
    case direction::southWest: offset = TilePos( -1, -1 ); break;
    case direction::west: offset = TilePos( -1, 0 ); break;
    case direction::northWest: offset = TilePos( -1, 1 ); break;
    }

    dustcloud->send2City( start, start + offset * range);
  }
}

DustCloud::DustCloud(PlayerCityPtr city )
  : Walker( city, walker::dustCloud ), _d( new Impl )
{
  _d->animation.load( ResourceGroup::sprites, 1, 8 );
  _d->animation.setDelay( Animation::hugeSlow );
  //_d->animation.setOffset( Point( 5, 7 ) );

  setName( _("##dust##") );

  setFlag( vividly, false );
}

void DustCloud::send2City(const TilePos &start, const TilePos& stop )
{
  _d->mapway.source = start;
  _d->mapway.destination = stop;

  if( !_d->mapway.mayMove() )
  {
    Logger::warning( "WARNING!!! DustCloud: start equale destination" );
    _d->mapway.destination = _d->mapway.source + TilePos( 1, 1 );
  }

  _d->worldway.destination = _d->mapway.destination2world();
  _d->worldway.source = _d->mapway.source2world();

  float delim = 6.f + math::random( 8 );
  _d->worldway.speed =  _d->worldway.delta().toPointF() / (_d->mapway.distance() * delim);
  _d->worldway.current = _d->worldway.source.toPointF();

  _setWpos( _d->worldway.source );

  attach();

  _pathway().init( _map().at( _d->mapway.source ) );
  _pathway().setNextTile( _map().at( _d->mapway.nextStep() ) );
}

void DustCloud::timeStep(const unsigned long time)
{
  switch( action() )
  {
  case Walker::acMove:
  {
    PointF saveCurrent = _d->worldway.current;
    _d->worldway.current += _d->worldway.speed;

    int yMultiplier = tilemap::cellSize().height();
    Point xOffset( 0, yMultiplier );
    TilePos rpos = TilePos( (_d->worldway.current.x() - xOffset.x()) / yMultiplier,
                            (_d->worldway.current.y() - xOffset.y()) / yMultiplier );

    _setLocation( rpos );

    _setWpos( _d->worldway.current.toPoint() );
    _d->animation.update( time );

    if( saveCurrent.getDistanceFrom( _d->worldway.destination.toPointF() ) <
        _d->worldway.current.getDistanceFrom( _d->worldway.destination.toPointF() ) )
    {
      _d->worldway.current = _d->worldway.destination.toPointF();
      _reachedPathway();
    }
  }
  break;

  default:
  break;
  }
}

const Picture& DustCloud::getMainPicture() {  return _d->animation.currentFrame(); }

void DustCloud::_reachedPathway() { deleteLater(); }
DustCloud::~DustCloud() {}

void DustCloud::save( VariantMap& stream ) const
{
  Walker::save( stream );
}

void DustCloud::load( const VariantMap& stream )
{
  Walker::load( stream );
}

void DustCloud::initialize(const VariantMap &options)
{
  VariantMap anim = options.get( "animation" ).toMap();
  if( !anim.empty() )
  {
    _d->animation.clear();
    _d->animation.load( anim.get( "rc" ).toString(),
                        anim.get( "start" ),
                        anim.get( "frames" ) );
    _d->animation.setDelay( anim.get( "delay" ) );
  }
}
