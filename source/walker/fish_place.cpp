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

#include "fish_place.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "gfx/tilemap.hpp"
#include "pathway/pathway_helper.hpp"
#include "pathway/pathway.hpp"
#include "walker/walker.hpp"
#include "constants.hpp"
#include "core/gettext.hpp"
#include "core/variant_map.hpp"
#include "walkers_factory.hpp"
#include "objects/construction.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::fishPlace, FishPlace)

class FishPlace::Impl
{
public:
  int fishCount;
  Point basicOffset;
  Animation animation;
};

FishPlace::FishPlace( PlayerCityPtr city )
  : Fish( city ), _d( new Impl )
{
  _setType( walker::fishPlace );
  setSpeedMultiplier( 0.1f );

  setName( _("##fish_place##") );

  _d->fishCount = math::random( 100 );

  if( _d->fishCount > 1 )
  {
    _d->animation.load( config::rc.land3a, 19, 24); //big fish place
    _d->basicOffset = Point( -41, 122 );
    _d->animation.setOffset( _d->basicOffset );
  }
  else
  {
    _d->animation.load( config::rc.land3a, 1, 18);
    _d->basicOffset =  Point( 0, 55 );
    _d->animation.setOffset( _d->basicOffset );
  } //small fish place
  _d->animation.setDelay( Animation::slow );

  setFlag( vividly, false );
}

FishPlace::~FishPlace(){}

void FishPlace::timeStep(const unsigned long time)
{
  Walker::timeStep( time );

  _d->animation.update( time );
}

void FishPlace::save(VariantMap& stream) const
{
  Walker::save( stream );
  stream[ "fishCount" ] = _d->fishCount;
}

void FishPlace::load(const VariantMap& stream)
{
  Walker::load( stream );
  _d->fishCount = stream.get( "fishCount" );
}

const Picture& FishPlace::getMainPicture()
{
  return _d->animation.currentFrame();
}

void FishPlace::send2city(TilePos rpos, TilePos dst )
{
  if( dst.i() < 0 || dst.j() < 0 )
  {
    dst = _city()->getBorderInfo( PlayerCity::boatExit ).epos();
  }

  _findway( rpos, dst );
}

void FishPlace::_reachedPathway()
{
  if( pos() == _city()->getBorderInfo( PlayerCity::boatExit).epos() )
  {
    deleteLater();
  }
  else
  {
    _findway( pos(), _city()->getBorderInfo( PlayerCity::boatExit ).epos() );
  }
}


void FishPlace::_findway( TilePos start, TilePos end)
{
  Pathway pathway = PathwayHelper::create( start, end, PathwayHelper::deepWaterFirst );
  if( !pathway.isValid() )
  {
    deleteLater();
  }
  else
  {
    attach();
    setPos( start );
    setPathway( pathway );
    go();
  }
}
