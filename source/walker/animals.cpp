// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "animals.hpp"
#include "core/variant.hpp"
#include "game/city.hpp"
#include "game/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "game/tilemap.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "ability.hpp"

using namespace constants;

class Animal::Impl
{
public:
  TilePos destination;
};

Animal::Animal(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::unknown );
  _setAnimation( gfx::unknown );

  setName( _("##Animal##") );
}

void Animal::send2City(const TilePos &start )
{
  if( !isDeleted() )
  {
    _getCity()->addWalker( WalkerPtr( this ) );
  }
}

Animal::~Animal()
{

}

void Animal::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destination" ] = _d->destination;
}

void Animal::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->destination = stream.get( "destination" ).toTilePos();
}

void Animal::_findNewWay( const TilePos& start )
{
  Pathway pathway = PathwayHelper::randomWay( _getCity(), start, 10 );

  if( pathway.isValid() )
  {
    setIJ( start );
    setPathway( pathway );
    go();
  }
  else
  {
    die();
  }
}

Sheep::Sheep( PlayerCityPtr city ) : Animal( city )
{
  _setAnimation( gfx::sheep );
  _setType( walker::sheep );
  setName( _("##Sheep##") );

  addAbility( Illness::create( 0.2, 4 ) );
}

WalkerPtr Sheep::create(PlayerCityPtr city)
{
  WalkerPtr ret( new Sheep( city ) );
  ret->drop();

  return ret;
}

void Sheep::_reachedPathway()
{
  Walker::_reachedPathway();

  Tilemap& tmap = _getCity()->getTilemap();
  if( tmap.at( getIJ() ).getFlag( Tile::tlMeadow ) )
  {
    updateHealth( +100 );
  }

  _findNewWay( getIJ() );
}

void Sheep::_brokePathway(TilePos pos)
{
  _findNewWay( getIJ() );
}

void Sheep::_changeTile()
{
  Walker::_changeTile();
  _animationRef().setDelay( 3 );
}

void Sheep::die()
{
  Animal::die();

  Corpse::create( _getCity(), getIJ(), "citizen04", 257, 264 );
}

void Sheep::send2City(const TilePos &start )
{
  _findNewWay( start );

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}
