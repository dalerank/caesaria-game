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

#include "chastener_elephant.hpp"
#include "city/city.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include "core/priorities.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::romeChastenerElephant, ChastenerElephant)

class ChastenerElephant::Impl
{
public:
  void mayMove( const Tile* tile, bool& ret )
  {
    BuildingPtr f = ptr_cast<Building>( tile->overlay() );
    ret = ( tile->isWalkable( true ) || f.isValid() );
  }
};

ChastenerElephant::ChastenerElephant( PlayerCityPtr city )
    : EnemySoldier( city, walker::romeChastenerElephant ), _d( new Impl )
{
  _excludeAttack().clear();
  _excludeAttack() << objects::disasterGroup
                   << objects::roadGroup
                   << objects::bridgeGroup;
}

Pathway ChastenerElephant::_findPathway2NearestConstruction( unsigned int range )
{
  Pathway ret;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    ConstructionList constructions = _findContructionsInRange( tmpRange );

    foreach( it, constructions )
    {
      ConstructionPtr c = ptr_cast<Construction>( *it );
      ret = PathwayHelper::create( pos(), c->pos(), makeDelegate( _d.data(), &Impl::mayMove ) );
      if( ret.isValid() )
      {
        return ret;
      }
    }
  }

  return Pathway();
}

bool ChastenerElephant::_tryAttack()
{
  Tilemap& tmap = _city()->tilemap();
  TilesArray tiles = tmap.getNeighbors( pos() );
  foreach( it, tiles )
  {
    ConstructionPtr ov = ptr_cast<Construction>( (*it)->overlay() );
    if( ov.isValid() && !_excludeAttack().count( ov->group() ) )
    {
      ov->collapse();
    }
  }

  return EnemySoldier::_tryAttack();
}

ChastenerElephantPtr ChastenerElephant::create( PlayerCityPtr city)
{
  ChastenerElephantPtr ret( new ChastenerElephant( city ) );
  ret->drop();

  return ret;
}

int ChastenerElephant::agressive() const { return -2; }

bool ChastenerElephant::die()
{
  _city()->empire()->emperor().remSoldiers( _city()->name(), 1 );

  return EnemySoldier::die();
}
