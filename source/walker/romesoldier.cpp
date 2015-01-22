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

#include "romesoldier.hpp"
#include "city/helper.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "helper.hpp"
#include "enemysoldier.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include "animals.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_SOLDIER_IN_WALKERFACTORY( walker::legionary, walker::legionary, RomeSoldier, legionary )

namespace  {
static const int maxDistanceFromBase = 32;
enum {
  expedition=Soldier::userAction+1
 };
}

class RomeSoldier::Impl
{
public:
  TilePos basePos;
  TilePos patrolPosition;
  double strikeForce, resistance;
  std::string expedition;
};

RomeSoldier::RomeSoldier( PlayerCityPtr city, walker::Type type )
    : Soldier( city, type ), _d( new Impl )
{
  setName( NameGenerator::rand( NameGenerator::male ) );

  _d->patrolPosition = TilePos( -1, -1 );
}

RomeSoldierPtr RomeSoldier::create(PlayerCityPtr city, walker::Type type)
{
  RomeSoldierPtr ret( new RomeSoldier( city, type ) );
  ret->drop();

  return ret;
}

bool RomeSoldier::die()
{
  bool created = Soldier::die();

  if( !created )
  {
    WalkerPtr w = Corpse::create(_city(), this );
    Logger::warningIf( w.isNull(), "RomeSoldier: cannot create corpse for type " + WalkerHelper::getTypename( type() ) );
    return w.isValid();
  }

  return created;
}

void RomeSoldier::timeStep(const unsigned long time)
{
  Soldier::timeStep( time );

  if( game::Date::isMonthChanged() )
  {
    unsigned int dst2base = pos().distanceFrom( _d->basePos );
    if( dst2base > maxDistanceFromBase )
    {
      updateMorale( dst2base * -10 / maxDistanceFromBase );
      if( morale() == 0 )
      {
        _duckout();
      }
    }
  }

  switch( _subAction() )
  {
  case fightEnemy:
  {
    WalkerList enemies = _findEnemiesInRange( attackDistance() );

    if( !enemies.empty() )
    {
      WalkerPtr p = enemies.front();
      turn( p->pos() );
      p->updateHealth( -3 );
      p->acceptAction( Walker::acFight, pos() );
    }
    else
    {
      bool haveEnemy = _tryAttack();
      if( !haveEnemy )
        send2patrol();
    }
  }
  break;

  case patrol:
    if( game::Date::current().day() % 2 == 0 )
    {
      _tryAttack();
    }
  break;

  default: break;
  } // end switch( _d->action )
}

void RomeSoldier::return2fort() { _back2base(); }
void RomeSoldier::send2patrol() {  _back2base(); }

void RomeSoldier::save(VariantMap& stream) const
{
  Soldier::save( stream );

  VARIANT_SAVE_ANY_D( stream, _d, basePos );
  VARIANT_SAVE_ANY_D( stream, _d, strikeForce );
  VARIANT_SAVE_ANY_D( stream, _d, resistance );
  VARIANT_SAVE_ANY_D( stream, _d, patrolPosition );
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(RomeSoldier) ) );
}

FortPtr RomeSoldier::base() const { return ptr_cast<Fort>( _city()->getOverlay( _d->basePos ) ); }

void RomeSoldier::load(const VariantMap& stream)
{
  Soldier::load( stream );

  VARIANT_LOAD_ANY_D( _d, strikeForce, stream );
  VARIANT_LOAD_ANY_D( _d, resistance, stream );
  VARIANT_LOAD_ANY_D( _d, patrolPosition, stream );
  VARIANT_LOAD_ANY_D( _d, basePos, stream );

  FortPtr fort = ptr_cast<Fort>( _city()->getOverlay( _d->basePos ) );

  if( fort.isValid() )
  {
    fort->addWalker( this );
  }
  else
  {
    die();
  }
}

std::string RomeSoldier::thoughts(Thought th) const
{
  if( th == thCurrent )
  {
    city::Helper helper( _city() );

    TilePos offset( 10, 10 );
    EnemySoldierList enemies = helper.find<EnemySoldier>( walker::any, pos() - offset, pos() + offset );
    if( enemies.empty() )
    {
      return Soldier::thoughts( th );
    }
    else
    {
      RomeSoldierList ourSoldiers = helper.find<RomeSoldier>( walker::any, pos() - offset, pos() + offset );
      int enemyStrength = 0;
      int ourStrength = 0;

      foreach( it, enemies) { enemyStrength += (*it)->strike(); }
      foreach( it, ourSoldiers ) { ourStrength += (*it)->strike(); }

      if( ourStrength > enemyStrength )
      {
        int diff = enemyStrength > 0 ? ourStrength / enemyStrength : 99;
        switch( diff )
        {
        case 1: return "";

        case 4: return "##enemies_very_easy##";

        default: break;
        }
      }
      else
      {
        int diff = ourStrength > 0 ? enemyStrength / ourStrength : 99;
        switch( diff )
        {
        case 1:

        case 3: return "##enemies_hard_to_me##";
        case 4: return "##enemies_very_hard##";

        default: break;
        }
      }

      Logger::warning( "RomeSoldier: current thinks unknown state" );
      return "##enemies_unknown_state##";
    }
  }

  return "";
}

TilePos RomeSoldier::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->basePos;
  case plDestination: return _d->patrolPosition;
  default: break;
  }

  return Soldier::places( type );
}

RomeSoldier::~RomeSoldier(){}

WalkerList RomeSoldier::_findEnemiesInRange( unsigned int range )
{
  Tilemap& tmap = _city()->tilemap();
  WalkerList walkers;

  TilePos offset( range, range );
  TilesArray tiles = tmap.getArea( pos() - offset, pos() + offset );

  FortPtr fort = base();
  bool attackAnimals = fort.isValid() ? fort->isAttackAnimals() : false;

  foreach( tile, tiles )
  {
    WalkerList tileWalkers = _city()->walkers( (*tile)->pos() );

    foreach( w, tileWalkers )
    {
      if( (*w)->agressive() > 0 )  { walkers << *w; }
      else if( attackAnimals && is_kind_of<Animal>( *w ) ) { walkers << *w; }
    }
  }

  return walkers;
}

ConstructionList RomeSoldier::_findContructionsInRange(unsigned int) { return ConstructionList(); }


bool RomeSoldier::_tryAttack()
{
  ConstructionList constructions = _findContructionsInRange( attackDistance() );
  TilePos targetPos;
  if( !constructions.empty() )
  {
    _setSubAction( Soldier::destroyBuilding );
    targetPos = constructions.front()->pos();
    fight();
  }
  else
  {
    WalkerList enemies = _findEnemiesInRange( attackDistance() );
    if( !enemies.empty() )
    {
      _setSubAction( Soldier::fightEnemy );
      targetPos = enemies.front()->pos();
      fight();
    }
  }

  if( action() == acFight )
  {
    city::Helper helper( _city() );
    bool needMove = false;
    helper.isTileBusy<Soldier>( pos(), this, needMove );
    if( needMove )
    {
      _move2freePos( targetPos );
    }
  }

  return action() == acFight;
}

Pathway RomeSoldier::_findPathway2NearestEnemy( unsigned int range )
{
  Pathway ret;

  for( unsigned int tmpRange=1; tmpRange <= range; tmpRange++ )
  {
    WalkerList walkers = _findEnemiesInRange( tmpRange );

    foreach( w, walkers)
    {
      ret = PathwayHelper::create( pos(), (*w)->pos(), PathwayHelper::allTerrain );
      if( ret.isValid() )
      {
        return ret;
      }
    }
  }

  return Pathway();
}

void RomeSoldier::_back2base()
{
  FortPtr b = base();
  if( b.isValid() )
  {
    Pathway way = PathwayHelper::create( pos(), b->freeSlot(), PathwayHelper::allTerrain );

    if( way.isValid() )
    {
      setPathway( way );
      _setSubAction( go2position );
      go();
      return;
    }
  }
  else
  {
    die();
  }
}

void RomeSoldier::_duckout()
{
  Pathway way = PathwayHelper::way2border( _city(), pos() );
  if( !way.isValid() )
  {
    way = PathwayHelper::randomWay( _city(), pos(), maxDistanceFromBase );
  }

  if( way.isValid() )
  {
    _setSubAction( duckout );
    setPathway( way );
    go();
  }
  else
  {
    die();
  }
}

void RomeSoldier::_reachedPathway()
{
  Soldier::_reachedPathway();

  switch( (int)_subAction() )
  {

  case expedition:
    deleteLater();
  break;

  case go2position:
  {
    city::Helper helper( _city() );
    WalkerList walkersOnTile = helper.find<Walker>( type(), pos() );
    walkersOnTile.remove( this );

    if( walkersOnTile.size() > 0 ) //only me in this tile
    {
      _back2base();
    }
    else
    {
      _setSubAction( patrol );
    }
  }
  break;

  case duckout:
  {
    deleteLater();
  }
  break;

  default:
  break;
  }
}

void RomeSoldier::_brokePathway(TilePos p)
{
  Soldier::_brokePathway( p );

  if( _d->patrolPosition.i() >= 0 )
  {
    Pathway way = PathwayHelper::create( pos(), _d->patrolPosition,
                                         PathwayHelper::allTerrain );

    if( way.isValid() )
    {
      setPathway( way );
      go();
    }
    else
    {
      _setSubAction( patrol );
      _setAction( acNone );
      setPathway( Pathway() );
    }
  }
}

void RomeSoldier::_centerTile()
{
  switch( _subAction() )
  {
  case doNothing:
  break;

  case go2position:
  {
    if( _tryAttack() )
      return;
  }
  break;

  default:
  break;
  }

  Walker::_centerTile();
}

void RomeSoldier::send2city(FortPtr base, TilePos pos )
{
  setPos( pos );
  _d->basePos = base->pos();
  _back2base();

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

void RomeSoldier::send2expedition(const std::string& name)
{
  _d->expedition = name;
  TilePos cityEnter = _city()->borderInfo().roadEntry;

  Pathway way = PathwayHelper::create( pos(), cityEnter, PathwayHelper::allTerrain );
  if( way.isValid() )
  {
    setPathway( way );
    _setSubAction( (SldrAction)expedition );
    go();
  }
}
