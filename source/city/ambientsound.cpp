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

#include "ambientsound.hpp"
#include "city/city.hpp"
#include "gfx/tilemap_camera.hpp"
#include "sound/engine.hpp"
#include "core/utils.hpp"
#include "gfx/tileoverlay.hpp"
#include "core/foreach.hpp"

#include <set>

using namespace gfx;

namespace city
{

struct SoundEmitter
{  
  Tile* tile;
  TileOverlayPtr overlay;
  TilePos& camerapos;

  SoundEmitter( Tile* t, TilePos& cam ) : camerapos( cam )
  {
    tile = t;
    overlay = t->overlay();
  }

  bool operator < ( const SoundEmitter& a ) const
  {
    return ( tile->pos().distanceFrom( camerapos )
             < a.tile->pos().distanceFrom( camerapos ));
  }

  std::string getSound() const
  {
    if( overlay.isValid() )
    {
      return overlay->sound();
    }
    else
    {
      if( tile->getFlag( Tile::tlWater ) )
      {
        return "river_00001.ogg";
      }
      else if( tile->getFlag( Tile::tlTree ) )
      {

      }
      else
      {
        return utils::format( 0xff, "emptyland_%05d.ogg", (tile->i() * tile->j()) % 3 + 1  );
      }
    }

    return "";
  }

  float getDistance( TilePos p ) const
  {
    return tile->pos().distanceFrom( p );
  }

  bool isValid() const
  {
    return (overlay.isNull() || (overlay.isValid() && !overlay->isDeleted()) );
  }
};

class AmbientSound::Impl
{
public:
  Camera* camera;
  TilePos cameraPos;

  typedef std::set< SoundEmitter > Emitters;

  Emitters emitters;
};

SrvcPtr AmbientSound::create( PlayerCityPtr city, gfx::Camera* camera )
{
  SrvcPtr p( new AmbientSound( city, camera ) );
  p->drop();

  return p;
}

AmbientSound::~AmbientSound() {}

AmbientSound::AmbientSound( PlayerCityPtr city, gfx::Camera* camera )
: Srvc( city, defaultName() ), _d( new Impl )
{
  _d->camera = camera;
}

void AmbientSound::timeStep( const unsigned int time )
{
  if( time % 20 != 1 )
    return;

  if( !_d->camera )
    return;

  Tile* tile = _d->camera->centerTile();

  if( !tile )
    return;

  _d->cameraPos = tile->pos();

  audio::Engine& ae = audio::Engine::instance();

  //add new emitters
  TilePos offset( 3, 3 );
  TilesArray tiles = _city()->tilemap().getArea( _d->cameraPos - offset, _d->cameraPos + offset );

  foreach( tile, tiles ) { _d->emitters.insert( SoundEmitter( *tile, _d->cameraPos ) ); }

  //remove so far emitters
  for( Impl::Emitters::iterator i=_d->emitters.begin(); i != _d->emitters.end(); )
  {
    TilePos distance = _d->cameraPos - (*i).tile->pos();
    if( abs( distance.i() ) > 3 || abs( distance.j() ) > 3
        || !(*i).isValid() )
    {
      //ae.stop( (*i).getSound() );
      _d->emitters.erase( i++ );
    }
    else
    {
      ++i;
    }
  }

  //create emitters map
  std::set< std::string > processedSounds;
  for( Impl::Emitters::reverse_iterator i=_d->emitters.rbegin();
       i != _d->emitters.rend(); ++i )
  {
    std::string sound = i->getSound();

    if( sound.empty() )
      continue;

    std::set< std::string >::const_iterator tIt = processedSounds.find( sound );

    if( tIt == processedSounds.end() )
    {
      processedSounds.insert( sound );

      ae.play( sound, 256 / (3 *(i->getDistance( _d->cameraPos )+1)), audio::ambientSound  );
    }
  }
}

void AmbientSound::destroy()
{
  _d->emitters.clear();
  _d->camera = 0;
}

std::string AmbientSound::defaultName() { return CAESARIA_STR_EXT(AmbientSound); }

}//end namespace city
