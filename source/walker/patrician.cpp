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

#include "patrician.hpp"
#include "core/variant.hpp"
#include "game/city.hpp"
#include "game/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "game/tilemap.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "ability.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;

class Patrician::Impl
{
public:
  TilePos destination;
};

Patrician::Patrician( CityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::patrician );
  _setGraphic( rand() % 100 ? WG_RICH : WG_RICH2 );

  setName( _("##patrician##") );
}

void Patrician::send2City(const TilePos &start )
{
  if( !isDeleted() )
  {
    _getCity()->addWalker( WalkerPtr( this ) );
  }
}

Patrician::~Patrician()
{

}

void Patrician::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "destination" ] = _d->destination;
}

void Patrician::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->destination = stream.get( "destination" ).toTilePos();
}

void Patrician::_findNewWay( const TilePos& start )
{
  PathWay pathway = PathwayHelper::randomWay( _getCity(), start, 10 );

  if( pathway.isValid() )
  {
    setPathWay( pathway );
    setIJ( start );
    go();
  }
  else
  {
    die();
  }
}

void Patrician::die()
{
  Animal::die();

  if( _getGraphic() == WG_RICH )
  {
    Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen3, 809, 816 );
  }
  else
  {
    Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen3, 1017, 1024 );
  }
}

void Patrician::send2City(const TilePos &start )
{
  _findNewWay( start );

  if( !isDeleted() )
  {
    _getCity()->addWalker( this );
  }
}
