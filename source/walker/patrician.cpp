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

#include "patrician.hpp"
#include "core/variant.hpp"
#include "city/city.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "ability.hpp"
#include "city/helper.hpp"
#include "core/variant_map.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"

using namespace constants;

class Patrician::Impl
{
public:
  TilePos destination;
};

Patrician::Patrician(PlayerCityPtr city )
  : Human( city ), _d( new Impl )
{
  _setType( walker::patrician );

  setName( _("##patrician##") );
}

PatricianPtr Patrician::create(PlayerCityPtr city)
{
  PatricianPtr ret( new Patrician( city ) );
  ret->drop();

  return ret;
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
  city::Helper helper( _city() );
  std::vector< objects::Type > bTypes;
  bTypes.push_back( objects::senate );

  ConstructionList buildings;

  foreach( it, bTypes )
  {
    buildings << helper.find<Construction>( *it );
  }

  Pathway pathway;

  for( size_t k=0; k < std::min<size_t>( 3, buildings.size() ); k++ )
  {
    ConstructionPtr constr = buildings.random();
    pathway = PathwayHelper::create( start, constr, PathwayHelper::roadOnly );

    if( pathway.isValid() )
    {
      break;
    }
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::randomWay( _city(), start, 10 );
  }

  if( pathway.isValid() )
  {
    setPos( start );
    setPathway( pathway );
    go();
  }
  else
  {
    Logger::warning( "Patrician: cant find way" );
    die();
  }
}

void Patrician::_reachedPathway()
{
  if( _pathwayRef().isReverse() )
  {
    deleteLater();
  }
  else
  {
    _pathwayRef().toggleDirection();
    go();
  }
}

bool Patrician::die()
{
  return Walker::die();

  /*if( _getAnimationType() == gfx::patricianMove )
  {
    Corpse::create( _getCity(), pos(), ResourceGroup::citizen3, 809, 816 );
  }
  else
  {
    Corpse::create( _getCity(), pos(), ResourceGroup::citizen3, 1017, 1024 );
  }*/
}

void Patrician::send2City(TilePos start )
{
  _findNewWay( start );

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}
