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
#include "pathway/pathway_helper.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/helper.hpp"
#include "walker/helper.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::dustCloud, DustCloud)

class DustCloud::Impl
{
public:
  gfx::Animation animation;
  Point dstPos, srcPos;
  TilePos from, dst;
  PointF deltaMove;
  PointF currentPos;
};

WalkerPtr DustCloud::create(PlayerCityPtr city)
{
  WalkerPtr ret( new DustCloud( city ) );
  ret->initialize( WalkerHelper::getOptions( walker::dustCloud ) );
  ret->drop();

  return ret;
}

void DustCloud::create(PlayerCityPtr city, const TilePos& start, unsigned int range)
{
  for( int direction=0; direction < 8; direction++ )
  {
    DustCloud* dc = new DustCloud( city );
    dc->initialize( WalkerHelper::getOptions( walker::dustCloud ) );

    TilePos offset;
    switch( direction )
    {
    case north: offset = TilePos( 0, 1 ); break;
    case northEast: offset = TilePos( 1, 1 ); break;
    case east: offset = TilePos( 1, 0 ); break;
    case southEast: offset = TilePos( 1, -1 ); break;
    case south: offset = TilePos( 0, -1 ); break;
    case southWest: offset = TilePos( -1, -1 ); break;
    case west: offset = TilePos( -1, 0 ); break;
    case northWest: offset = TilePos( -1, 1 ); break;
    }

    dc->send2City( start, start + offset * range);
    dc->drop();
  }
}

DustCloud::DustCloud(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::dustCloud );
  _d->animation.load( ResourceGroup::sprites, 1, 8 );
  _d->animation.setDelay( 9 );
  //_d->animation.setOffset( Point( 5, 7 ) );

  setName( _("##dust##") );

  setFlag( vividly, false );
}

void DustCloud::send2City(const TilePos &start, const TilePos& stop )
{
  _d->from = start;
  _d->dst = stop;

  if( _d->from == _d->dst )
  {
    Logger::warning( "WARNING!!! DustCloud: start equale destination" );
    _d->dst = _d->from + TilePos( 1, 1 );
  }

  int yMultiplier = tilemap::cellSize().height();
  Point xOffset( 0, yMultiplier );
  _d->dstPos = Point( _d->dst.i(), _d->dst.j() ) * yMultiplier + xOffset;
  _d->srcPos = Point( _d->from.i(), _d->from.j() ) * yMultiplier + xOffset;

  float delim = 6.f + math::random( 8 );
  _d->deltaMove = ( _d->dstPos - _d->srcPos ).toPointF() / (_d->from.distanceFrom( _d->dst ) * delim);
  _d->currentPos = _d->srcPos.toPointF();

  _setWpos( _d->srcPos );

  attach();

  Tilemap& tmap = _city()->tilemap();
  _pathwayRef().init( tmap.at( _d->from ) );
  _pathwayRef().setNextTile( tmap.at( _d->from.nextStep( _d->dst ) ));
}

void DustCloud::timeStep(const unsigned long time)
{
  switch( action() )
  {
  case Walker::acMove:
  {
    PointF saveCurrent = _d->currentPos;
    _d->currentPos += _d->deltaMove;

    int yMultiplier = tilemap::cellSize().height();
    Point xOffset( 0, yMultiplier );
    TilePos rpos = TilePos( (_d->currentPos.x() - xOffset.x()) / yMultiplier,
                            (_d->currentPos.y() - xOffset.y()) / yMultiplier );

    Tile& t = _city()->tilemap().at( rpos );
    _setLocation( &t );

    _setWpos( _d->currentPos.toPoint() );
    _d->animation.update( time );

    if( saveCurrent.getDistanceFrom( _d->dstPos.toPointF() ) <
        _d->currentPos.getDistanceFrom( _d->dstPos.toPointF() ) )
    {
      _d->currentPos = _d->dstPos.toPointF();
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
