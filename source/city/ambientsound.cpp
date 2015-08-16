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
#include "gfx/tilearea.hpp"
#include "sound/engine.hpp"
#include "core/utils.hpp"
#include "objects/overlay.hpp"
#include "core/foreach.hpp"
#include "config.hpp"
#include "cityservice_factory.hpp"

#include <set>

using namespace gfx;
using namespace config;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(AmbientSound,ambient_sound)

struct SoundEmitter
{  
  Tile* tile;
  OverlayPtr overlay;
  TilePos& cameraPos;

  SoundEmitter( Tile* t, TilePos& cam ) : cameraPos( cam )
  {
    tile = t;
    overlay = t->overlay();
  }

  bool operator < ( const SoundEmitter& a ) const
  {
    return ( tile->pos().getDistanceFromSQ( cameraPos )
             < a.tile->pos().getDistanceFromSQ( cameraPos ));
  }

  std::string sound() const
  {
    if( overlay.isValid() )
    {
      return overlay->sound();
    }
    else
    {
      if( tile->getFlag( Tile::tlWater ) )
      {
        return "river_00001";
      }
      else if( tile->getFlag( Tile::tlTree ) )
      {

      }
      else
      {
        return utils::format( 0xff, "emptyland_%05d", (tile->i() * tile->j()) % 3 + 1  );
      }
    }

    return "";
  }

  float distance( TilePos p ) const
  {
    return tile->pos().distanceFrom( p );
  }

  bool isValid() const
  {
    return (overlay.isNull() || (overlay.isValid() && !overlay->isDeleted()) );
  }
};

typedef std::set< SoundEmitter > Emitters;
typedef std::set< int >  HashSet;

class AmbientSound::Impl
{
public:
  Camera* camera;
  TilePos cameraPos;
  Emitters emitters;

  TilesArea emmitersArea;
  HashSet processedSounds;
};

SrvcPtr AmbientSound::create(PlayerCityPtr city)
{
  SrvcPtr p( new AmbientSound( city ) );
  p->drop();

  return p;
}

AmbientSound::~AmbientSound() {}

AmbientSound::AmbientSound(PlayerCityPtr city)
: Srvc( city, defaultName() ), _d( new Impl )
{
  _d->camera = 0;
  _d->emmitersArea.reserve( ambientsnd::maxDistance * ambientsnd::maxDistance + 1 );
}

void AmbientSound::timeStep( const unsigned int time )
{
  if( time % ambientsnd::updateInterval != 1 )
    return;

  if( !_d->camera )
    return;

  Tile* tile = _d->camera->centerTile();

  if( !tile )
    return;

  _d->cameraPos = tile->pos();

  audio::Engine& ae = audio::Engine::instance();

  //add new emitters
  _d->emmitersArea.clear();
  _d->emmitersArea.add( _city()->tilemap(), _d->cameraPos, ambientsnd::maxDistance );

  for( auto tile : _d->emmitersArea )
    _d->emitters.insert( SoundEmitter( tile, _d->cameraPos ) );

  //remove so far emitters
  for( Emitters::iterator i=_d->emitters.begin(); i != _d->emitters.end(); )
  {
    TilePos distance = _d->cameraPos - (*i).tile->pos();
    if( abs( distance.i() ) > ambientsnd::maxDistance || abs( distance.j() ) > ambientsnd::maxDistance
        || !(*i).isValid() )
    {
      i = _d->emitters.erase( i );
    }
    else
    {
      ++i;
    }
  }

  //create emitters map
  _d->processedSounds.clear();

  std::string resourceName;
  resourceName.reserve(256);
  for( Emitters::reverse_iterator i=_d->emitters.rbegin(); i != _d->emitters.rend(); ++i )
  {
    resourceName = i->sound();

    if( resourceName.empty() )
      continue;

    unsigned int hash = Hash(resourceName);
    bool alsoResolved = _d->processedSounds.count( hash ) > 0;

    if( !alsoResolved )
    {
      _d->processedSounds.insert( hash );

      ae.play( resourceName, sound::maxLevel / (ambientsnd::maxDistance *(i->distance( _d->cameraPos )+1)), audio::ambient  );
    }
  }
}

void AmbientSound::destroy()
{
  _d->emitters.clear();
  _d->camera = 0;
}

void city::AmbientSound::setCamera(Camera *camera)
{
  _d->camera = camera;
}

std::string AmbientSound::defaultName() { return CAESARIA_STR_EXT(AmbientSound); }

}//end namespace city
