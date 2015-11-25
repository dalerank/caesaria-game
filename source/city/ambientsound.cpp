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
#include "gfx/tile_config.hpp"
#include "core/utils.hpp"
#include "objects/overlay.hpp"
#include "config.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "cityservice_factory.hpp"
#include "game/settings.hpp"

#include <set>

using namespace gfx;
using namespace config;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(AmbientSound,ambient_sound)

struct AmbientEmitter
{
  static const int msIntervalBetweenSwitch = 1000;
  struct Info
  {
    StringArray sounds;
    unsigned int lastAccessTime;
    int index;

    int nextIndex() const
    {
      return (index+1)%sounds.size();
    }

    Info() : lastAccessTime(0), index(0) {}
  };

  std::map< Tile::Type, Info > info;

  static AmbientEmitter& instance()
  {
    static AmbientEmitter inst;
    return inst;
  }

  void initialize( const vfs::Path& filename )
  {
    VariantMap types = config::load( filename );
    for( const auto& rtype : types )
    {
      Tile::Type type = gfx::tile::findType( rtype.first );
      if( type != Tile::tlUnknown )
      {
        info[ type ].sounds = rtype.second.toStringArray();
      }
    }
  }

  static std::string sound( Tile::Type tileType, unsigned int time )
  {
    AmbientEmitter& ambient = instance();
    auto it = ambient.info.find( tileType );
    if( it != ambient.info.end() )
    {
      Info& ref = it->second;
      if( time - ref.lastAccessTime > AmbientEmitter::msIntervalBetweenSwitch )
      {
        if( ref.sounds.size() > 0 )
        {
          ref.index = ref.nextIndex();
          ref.lastAccessTime = time;
          return ref.sounds[ ref.index ];
        }
      }
    }

    return "";
  }
};

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
    return ( tile->pos().distanceSqFrom( cameraPos )
             < a.tile->pos().distanceSqFrom( cameraPos ));
  }

  std::string sound( unsigned int time ) const
  {
    time = DateTime::elapsedTime();
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
        return AmbientEmitter::sound( Tile::tlGrass, time );
      }
    }

    return "";
  }

  float distance( const TilePos& p ) const
  {
    return tile->pos().distanceFrom( p );
  }

  float distancef( const Point& p ) const
  {
    return tile->mappos().distanceTo( p );
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

AmbientSound::~AmbientSound() {}

AmbientSound::AmbientSound(PlayerCityPtr city)
: Srvc( city, defaultName() ), _d( new Impl )
{
  _d->camera = 0;
  _d->emmitersArea.reserve( ambientsnd::maxDistance * ambientsnd::maxDistance + 1 );
  AmbientEmitter::instance().initialize( SETTINGS_RC_PATH(ambientsounds) );
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
  for( auto i=_d->emitters.rbegin(); i != _d->emitters.rend(); ++i )
  {
    resourceName = i->sound( time );

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

void AmbientSound::setCamera(Camera *camera)
{
  _d->camera = camera;
}

std::string AmbientSound::defaultName() { return CAESARIA_STR_EXT(AmbientSound); }

}//end namespace city
