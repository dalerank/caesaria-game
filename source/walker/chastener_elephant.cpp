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

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::romeChastenerElephant, ChastenerElephant)

struct ElephantWayCondition
{
  void tryMove( const Tile* tile, bool& ret )
  {
    BuildingPtr f = tile->overlay<Building>();
    ret = ( tile->isWalkable( true ) || f.isValid() );
  }

  TilePossibleCondition mayMove() { return makeDelegate( this, &ElephantWayCondition::tryMove ); }
};

ChastenerElephant::ChastenerElephant( PlayerCityPtr city )
    : EnemySoldier( city, walker::romeChastenerElephant )
{
  _excludeAttack().clear();
  _excludeAttack() << object::group::disaster
                   << object::group::road
                   << object::group::bridge;
}

Pathway ChastenerElephant::_findPathway2NearestConstruction( unsigned int range )
{
  Pathway ret;

  ElephantWayCondition condition;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    ConstructionList constructions = _findContructionsInRange( tmpRange );

    for( auto construction : constructions )
    {
      ret = PathwayHelper::create( pos(), construction->pos(), condition.mayMove() );
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
  ConstructionList constructions = _map().getNeighbors( pos() )
                                         .overlays<Construction>();
  for( auto ov : constructions )
  {
    if( ov.isValid() && !_excludeAttack().count( ov->group() ) )
    {
      ov->collapse();
    }
  }

  return EnemySoldier::_tryAttack();
}

int ChastenerElephant::agressive() const { return 4; }

bool ChastenerElephant::die()
{
  _city()->empire()->emperor().remSoldiers( _city()->name(), 1 );

  return EnemySoldier::die();
}
