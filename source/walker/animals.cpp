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

namespace {
CAESARIA_LITERALCONST(destination)
}

class Animal::Impl
{
public:
  TilePos destination;
};

Animal::Animal(PlayerCityPtr city )
  : Walker( city ), _d( new Impl )
{
  _setType( walker::unknown );

  setName( _("##animal##") );
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
  stream[ lc_destination ] = _d->destination;
}

void Animal::load( const VariantMap& stream )
{
  Walker::load( stream );
  _d->destination = stream.get( lc_destination ).toTilePos();
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
  setName( _("##sheep##") );

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

class Wolf::Impl
{
public:
  TilePos attackPos;
};

Wolf::Wolf( PlayerCityPtr city )
  : Animal( city ), _d( new Impl )
{
  _setType( walker::wolf );
  setSpeedMultiplier( 0.8 + math::random( 60 ) / 100.f);
  setName( _("##wolf##") );

  addAbility( Illness::create( 0.2, 4 ) );
}

WalkerPtr Wolf::create(PlayerCityPtr city)
{
  WalkerPtr ret( new Wolf( city ) );
  ret->drop();

  return ret;
}

void Wolf::_reachedPathway()
{
  Walker::_reachedPathway();

  _findNewWay( pos() );
}

void Wolf::_centerTile()
{
  Animal::_centerTile();

  TilePos offset(1,1);
  WalkerList walkers = _city()->walkers( walker::any, pos() - offset, pos() + offset );
  walkers = walkers.exclude<Wolf>();

  if( !walkers.empty() )
  {
    WalkerList::iterator it = walkers.begin();
    std::advance( it, math::random( walkers.size() - 1 ) );

    turn( (*it)->pos() );
    _setAction( acFight );
    //setSpeedMultiplier( 0.0 );
    _d->attackPos = (*it)->pos();
  }
}

void Wolf::_findNewWay( const TilePos& start )
{
  TilePos offset(10,10);
  WalkerList walkers = _city()->walkers( walker::any, start - offset, start + offset );
  walkers = walkers.exclude<Wolf>();

  Pathway pathway;
  if( !walkers.empty() )
  {
    WalkerList::iterator it = walkers.begin();
    std::advance( it, math::random( walkers.size() - 1 ) );

    pathway = PathwayHelper::create( start, (*it)->pos(), PathwayHelper::allTerrain );
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
    WalkerList walkers = _city()->walkers( walker::any, _d->attackPos );
    walkers = walkers.exclude<Wolf>();

    if( !walkers.empty() )
    {
      walkers.front()->updateHealth( -1 );
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

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

Fish::Fish(PlayerCityPtr city)
 : Walker( city )
{
  _setType( walker::unknown );

  setName( _("##fish##") );
}

 Fish::~Fish() {}
