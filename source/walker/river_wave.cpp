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

#include "river_wave.hpp"
#include "city/city.hpp"
#include "core/gettext.hpp"
#include "pathway/pathway_helper.hpp"
#include "objects/construction.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;
using namespace gfx;

RiverWavePtr RiverWave::create(PlayerCityPtr city)
{
  RiverWavePtr ret( new RiverWave( city ) );
  ret->drop();

  return ret;
}

RiverWave::RiverWave(PlayerCityPtr city )
  : Walker( city )
{
  _delay = math::random( 100 );
  _setType( walker::riverWave );
  _animation.load( ResourceGroup::sprites, 109, 5 );
  _animation.setDelay( 4 );
  _animation.setOffset( Point( 0, 0) );
  _animation.start( false );

  setName( _("##river_wave##") );

  setFlag( vividly, false );
}

void RiverWave::send2City(const TilePos &location )
{
  setPos( location );
  attach();
}

void RiverWave::timeStep(const unsigned long time)
{
  if( _delay > 0 )
  {
    _delay--;
    return;
  }

  _animation.update( time );
  if( _animation.isStopped() )
    deleteLater();
}

const Picture& RiverWave::getMainPicture()
{
  return _delay > 0 ? Picture::getInvalid() : _animation.currentFrame();
}

RiverWave::~RiverWave() {}

void RiverWave::save( VariantMap& stream ) const
{
  Walker::save( stream );
}

