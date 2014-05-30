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

#include "animals.hpp"
#include "core/variant.hpp"
#include "city/city.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "ability.hpp"

using namespace constants;
using namespace gfx;

class Animal::Impl
{
public:
  TilePos destination;
};

Animal::Animal(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::unknown );

  setName( _("##Animal##") );
}

void Animal::send2City(const TilePos &start )
{
  if( !isDeleted() )
  {
    _city()->addWalker( WalkerPtr( this ) );
  }
}

Animal::~Animal() {}

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

std::string Animal::getThinks() const{  return "##sheep_baa##";}

void Animal::_findNewWay( const TilePos& start )
{
  Pathway pathway = PathwayHelper::randomWay( _city(), start, 10 );

  if( pathway.isValid() )
  {
    setPos( start );
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

  Tilemap& tmap = _city()->tilemap();
  if( tmap.at( pos() ).getFlag( Tile::tlMeadow ) )
  {
    updateHealth( +100 );
  }

  _findNewWay( pos() );
}

void Sheep::_brokePathway(TilePos p){  _findNewWay( pos() );}

bool Sheep::die()
{
  bool created = Animal::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), "citizen04", 257, 264 );
    return true;
  }

  return created;
}

void Sheep::send2City(const TilePos &start )
{
  _findNewWay( start );

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}
