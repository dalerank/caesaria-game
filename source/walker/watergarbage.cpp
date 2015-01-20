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

#include "watergarbage.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "pathway/pathway_helper.hpp"
#include "game/resourcegroup.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::waterGarbage, WaterGarbage)

WaterGarbagePtr WaterGarbage::create(PlayerCityPtr city)
{
  WaterGarbagePtr ret( new WaterGarbage( city ) );
  ret->drop();

  return ret;
}

WaterGarbage::WaterGarbage(PlayerCityPtr city )
  : Walker( city )
{
  _setType( walker::waterGarbage );
  _animation.load( ResourceGroup::sprites, 93, 6 );
  _animation.setDelay( 2 );
  _animation.setOffset( Point( -15, 0 ) );
  setSpeedMultiplier( 0.5 + math::random( 10 ) / 10.f );

  setName( _("##water_garbage##") );

  setFlag( vividly, false );
}

void WaterGarbage::send2City(const TilePos &start )
{
  Pathway path = PathwayHelper::create( start, _city()->borderInfo().boatExit, PathwayHelper::deepWater );
  if( !path.isValid() )
  {
    path = PathwayHelper::create( start, _city()->borderInfo().boatExit, PathwayHelper::water );
  }

  if( path.isValid() )
  {
    setPos( start );
    setPathway( path );
    go();
    _city()->addWalker( this );
  }
}

void WaterGarbage::timeStep(const unsigned long time)
{
  Walker::timeStep( time );
  _animation.update( time );
}

const Picture& WaterGarbage::getMainPicture()
{
  return _animation.currentFrame();
}

void WaterGarbage::_reachedPathway() { deleteLater(); }
WaterGarbage::~WaterGarbage() {}

void WaterGarbage::save( VariantMap& stream ) const
{
  Walker::save( stream );
}

void WaterGarbage::load( const VariantMap& stream )
{
  Walker::load( stream );
}
