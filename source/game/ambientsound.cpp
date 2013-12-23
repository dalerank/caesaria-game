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

#include "ambientsound.hpp"
#include "city/city.hpp"
#include "gfx/tilemap_camera.hpp"
#include "sound/engine.hpp"
#include "core/stringhelper.hpp"

struct SoundEmitter
{
  Tile* tile;
  TileOverlayPtr overlay;
  TilePos& camerapos;

  SoundEmitter( Tile* t, TilePos& cam ) : camerapos( cam )
  {
    tile = t;
    overlay = t->getOverlay();
  }

  bool operator < ( const SoundEmitter& a ) const
  {
    return ( tile->getIJ().distanceFrom( camerapos )
             < a.tile->getIJ().distanceFrom( camerapos ));
  }

  std::string getSound() const
  {
    if( overlay.isValid() )
    {
      return overlay->getSound();
    }
    else
    {
      if( tile->getFlag( Tile::tlWater ) )
      {
        return "river_00001.wav";
      }
      else if( tile->getFlag( Tile::tlTree ) )
      {

      }
      else
      {
        return StringHelper::format( 0xff, "emptyland_%05d.wav", (tile->getI() * tile->getJ()) % 3 + 1  );
      }
    }

    return "";
  }

  float getDistance( TilePos p ) const
  {
    return tile->getIJ().distanceFrom( p );
  }

  bool isValid() const
  {
    return (overlay.isNull() || (overlay.isValid() && !overlay->isDeleted()) );
  }
};

class AmbientSound::Impl
{
public:
  PlayerCityPtr city;
  TilemapCamera* camera;
  TilePos cameraPos;

  typedef std::set< SoundEmitter > Emitters;
  Emitters emitters;
};

CityServicePtr AmbientSound::create(PlayerCityPtr city, TilemapCamera& camera )
{
  AmbientSound* ret = new AmbientSound( city );
  ret->_d->camera = &camera;

  CityServicePtr p( ret );
  p->drop();

  return p;
}

AmbientSound::AmbientSound(PlayerCityPtr city )
: CityService( "ambientsound" ), _d( new Impl )
{
  _d->city = city;
}

void AmbientSound::update( const unsigned int time )
{
  if( time % 5 != 1 )
    return;

  Tile* tile = _d->camera->getCenterTile();

  if( !tile )
    return;

  _d->cameraPos = tile->getIJ();

  audio::Engine& ae = audio::Engine::instance();

  //add new emitters
  TilePos offset( 3, 3 );
  TilesArray tiles = _d->city->getTilemap().getArea( _d->cameraPos - offset, _d->cameraPos + offset );
  foreach( Tile* tile, tiles )
  {
    _d->emitters.insert( SoundEmitter( tile, _d->cameraPos ) );
  }

  //remove so far emitters
  for( Impl::Emitters::iterator i=_d->emitters.begin(); i != _d->emitters.end(); )
  {
    TilePos distance = _d->cameraPos - (*i).tile->getIJ();
    if( abs( distance.getI() ) > 3 || abs( distance.getJ() ) > 3
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
       i != _d->emitters.rend(); i++ )
  {
    std::string sound = i->getSound();

    if( sound.empty() )
      continue;

    std::set< std::string >::const_iterator tIt = processedSounds.find( sound );

    if( tIt == processedSounds.end() )
    {
      processedSounds.insert( sound );

      ae.play( sound, 256 / (3 *(i->getDistance( _d->cameraPos )+1))  );
    }
  }
}
