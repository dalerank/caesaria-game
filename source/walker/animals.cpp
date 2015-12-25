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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "animals.hpp"
#include "core/variant_map.hpp"
#include "city/statistic.hpp"
#include "objects/construction.hpp"
#include "pathway/pathway_helper.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "helper.hpp"
#include "ability.hpp"
#include "walkers_factory.hpp"
#include "config.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::sheep, Sheep)
REGISTER_CLASS_IN_WALKERFACTORY(walker::wolf, Wolf)
REGISTER_CLASS_IN_WALKERFACTORY(walker::zebra, Zebra)

namespace {
const int maxWayAlert=30;
const float illnesValue=0.2f;
const int every4frame=4;
const int defaultAttackValue=-5;
}

class Animal::Impl
{
public:
  TilePos destination;
};

Animal::Animal(PlayerCityPtr city , walker::Type type)
  : Walker( city, type ), _d( new Impl )
{
  setFlag( Walker::vividly, true );
  setName( _("##animal##") );
}

void Animal::send2City(const TilePos& start)
{
  attach();
}

Animal::~Animal() {}

void Animal::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, destination )
}

void Animal::load( const VariantMap& stream )
{
  Walker::load( stream );
  VARIANT_LOAD_ANY_D( _d, destination, stream )
}

std::string Animal::thoughts(Thought th) const
{
  if( th == thCurrent )
  {
    return _("##sheep_baa##");
  }

  return "";
}

void Animal::_findNewWay( const TilePos& start )
{
  Pathway pathway = PathwayHelper::randomWay( _city(), start, config::distance::animalRandrom );

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

Sheep::Sheep( PlayerCityPtr city )
  : Herbivorous( city, walker::sheep )
{
}

void Herbivorous::_reachedPathway()
{
  Walker::_reachedPathway();

  if( _map().at( pos() ).getFlag( Tile::tlMeadow ) )
  {
    updateHealth( +100 );
  }

  _findNewWay( pos() );
}

void Herbivorous::_brokePathway(TilePos p) {  _findNewWay( pos() );}

void Herbivorous::_noWay()
{
  _noWayCount++;
  if( _noWayCount > maxWayAlert )
  {
    die();
    return;
  }

  _findNewWay( pos() );
}

Herbivorous::Herbivorous(PlayerCityPtr city, walker::Type type)
 : Animal( city, type )
{
  _setType( type );
  setName( WalkerHelper::getPrettyTypename( type ) );

  addAbility( Illness::create( illnesValue, every4frame ) );
  _noWayCount = 0;
}

void Herbivorous::send2City(const TilePos &start )
{
  _findNewWay( start );

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

class Wolf::Impl
{
public:
  TilePos attackPos;
};

Wolf::Wolf( PlayerCityPtr city )
  : Animal( city, walker::wolf ), _d( new Impl )
{
  setSpeedMultiplier( 0.8 + math::random( 60 ) / 100.f);
  setName( _("##wolf##") );

  addAbility( Illness::create( illnesValue, every4frame ) );
}

void Wolf::_reachedPathway()
{
  Walker::_reachedPathway();

  _findNewWay( pos() );
}

void Wolf::_centerTile()
{
  Animal::_centerTile();

  WalkerList walkers = _city()->statistic().walkers
                                           .neighbors<Walker>( pos() )
                                           .exclude<Wolf>();

  if( !walkers.empty() )
  {
    WalkerPtr wlk = walkers.random();

    turn( wlk->pos() );
    _setAction( acFight );
    //setSpeedMultiplier( 0.0 );
    _d->attackPos = wlk->pos();
  }
}

void Wolf::_findNewWay( const TilePos& start )
{  
  WalkerList walkers = _city()->statistic().walkers
                                           .find<Walker>( walker::any, config::distance::animalRandrom, start )
                                           .exclude<Wolf>();

  Pathway pathway;
  if( !walkers.empty() )
  {
    WalkerPtr wlk = walkers.random();
    pathway = PathwayHelper::create( start, wlk->pos(), PathwayHelper::allTerrain );
  }

  if( !pathway.isValid() )
  {
    pathway = PathwayHelper::randomWay( _city(), start, config::distance::animalRandrom );
  }

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

void Wolf::_brokePathway(TilePos p){  _findNewWay( pos() );}

bool Wolf::die()
{
  return Animal::die();
}

int Wolf::agressive() const { return 2; }

void Wolf::timeStep(const unsigned long time)
{
  Animal::timeStep( time );

  switch( action() )
  {
  case acFight:
  {
    WalkerList walkers = _city()->walkers( _d->attackPos ).exclude<Wolf>();

    if( !walkers.empty() )
    {
      walkers.front()->updateHealth( defaultAttackValue );
      walkers.front()->acceptAction( acFight, pos() );
    }
    else
    {
      _findNewWay( pos() );
    }
  }
  break;

  default:
  break;
  }
}

void Wolf::send2City(const TilePos &start )
{
  _findNewWay( start );

  attach();
}

Fish::Fish(PlayerCityPtr city)
 : Walker( city )
{
  setFlag( Walker::vividly, true );
  _setType( walker::unknown );

  setName( _("##fish##") );
}

Fish::~Fish() {}

Zebra::Zebra(PlayerCityPtr city)
  : Herbivorous( city, walker::zebra )
{
}
