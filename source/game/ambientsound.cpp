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
#include "city.hpp"
#include "gfx/tilemap_camera.hpp"
#include "sound/engine.hpp"

struct SoundEmitter
{
  int distance;
  TileOverlayPtr overlay;

  SoundEmitter( TilePos p, TileOverlayPtr ov )
  {
    overlay = ov;
    distance = p.distanceFrom( ov->getTilePos() );
  }

  bool operator < ( const SoundEmitter& other ) const
  {
    if( distance == other.distance )
      return (overlay < other.overlay);

    return ( distance < other.distance );
  }
};

class AmbientSound::Impl
{
public:
  PlayerCityPtr city;
  TilemapCamera* camera;

  typedef std::set< TileOverlayPtr > Overlays;
  Overlays overlays;
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

  TilePos cameraPos = tile->getIJ();

  audio::Engine& ae = audio::Engine::instance();

  //add new emitters
  TilePos offset( 3, 3 );
  TilesArray tiles = _d->city->getTilemap().getArea( cameraPos - offset, cameraPos + offset );
  foreach( Tile* tile, tiles )
  {
    if( tile->getOverlay().isValid() )
    {
      _d->overlays.insert( tile->getOverlay() );
    }
  }

  //remove so far emitters
  for( Impl::Overlays::iterator i=_d->overlays.begin(); i != _d->overlays.end(); )
  {
    TilePos distance = cameraPos - (*i)->getTilePos();
    if( abs( distance.getI() ) > 3 || abs( distance.getJ() ) > 3
        || (*i)->isDeleted() ) { _d->overlays.erase( i++ ); }
    else ++i;
  }

  //create emitters map
  std::set< SoundEmitter > soundMap;
  foreach( TileOverlayPtr ov, _d->overlays )
  {
    soundMap.insert( SoundEmitter( cameraPos, ov ) );
  }

  std::set< std::string > processedSounds;
  for( std::set< SoundEmitter >::reverse_iterator i=soundMap.rbegin();
       i != soundMap.rend(); i++ )
  {
    std::string sound = i->overlay->getSound();

    if( sound.empty() )
      continue;

    std::set< std::string >::const_iterator tIt = processedSounds.find( sound );

    if( tIt == processedSounds.end() )
    {
      processedSounds.insert( sound );

      ae.play( sound, 256 / (3 *(i->distance+1))  );
    }
  }
}
