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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "wallguard.hpp"
#include "city/city.hpp"
#include "name_generator.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "objects/military.hpp"
#include "pathway/pathway_helper.hpp"
#include "gfx/tilemap.hpp"
#include "animals.hpp"
#include "enemysoldier.hpp"
#include "objects/tower.hpp"
#include "objects/fortification.hpp"
#include "walker/spear.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"

using namespace constants;
using namespace gfx;

class WallGuard::Impl
{
public:
  typedef enum { doNothing=0, go2position, attackEnemy,
                 patrol, go2tower } State;
  TowerPtr base;
  State action;
  TilePos patrolPosition;
  int wait;
  double strikeForce, resistance;

  EnemySoldierPtr findNearbyEnemy(EnemySoldierList enemies , TilePos pos);
};

WallGuard::WallGuard( PlayerCityPtr city, walker::Type type ) : Soldier( city, type ), _d( new Impl )
{
  setName( NameGenerator::rand( NameGenerator::male ) );

  _d->patrolPosition = TilePos( -1, -1 );
  _d->wait = 0;
}

WallGuardPtr WallGuard::create(PlayerCityPtr city, walker::Type type)
{
  WallGuardPtr ret( new WallGuard( city, type ) );
  ret->drop();

  return ret;
}

void WallGuard::die()
{
  Soldier::die();

  switch( type() )
  {
  case walker::romeGuard:
    Corpse::create( _city(), pos(), ResourceGroup::citizen3, 233, 240 );
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF("not work yet");
  }
}

void WallGuard::timeStep(const unsigned long time)
{
  if( _d->wait > 0 )
  {
    _d->wait--;
    return;
  }

  Soldier::timeStep( time );

  switch( _d->action )
  {
  case Impl::attackEnemy:
  {
    EnemySoldierList enemies = _findEnemiesInRange( 7 );

    if( !enemies.empty() )
    {
      EnemySoldierPtr p = _d->findNearbyEnemy( enemies, pos() );
      turn( p->pos() );

      if( _animationRef().index() == (int)(_animationRef().frameCount()-1) )
      {
        SpearPtr spear = Spear::create( _city() );
        spear->toThrow( pos(), p->pos() );
        _d->wait = 60;
        _updateAnimation( time+1 );
      }
    }
    else
    {
      bool haveEnemies = _tryAttack();
      if( !haveEnemies )
      {
        _back2tower();
      }
    }
  }
  break;

  case Impl::patrol:
    if( GameDate::isDayChanged() )
    {
      bool haveEnemies = _tryAttack();
      if( !haveEnemies )
      {
        _back2tower();
      }
    }
  break;

  default: break;
  } // end switch( _d->action )
}

void WallGuard::save(VariantMap& stream) const
{
  Soldier::save( stream );

  stream[ "crtAction" ] = (int)_d->action;
  stream[ "base" ] = _d->base->pos();
  stream[ "strikeForce" ] = _d->strikeForce;
  stream[ "resistance" ] = _d->resistance;
  stream[ "patrolPosition" ] = _d->patrolPosition;
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(WallGuard) ) );
}

void WallGuard::load(const VariantMap& stream)
{
  Soldier::load( stream );

  _d->action = (Impl::State)stream.get( "crtAction" ).toInt();
  _d->strikeForce = stream.get( "strikeForce" );
  _d->resistance = stream.get( "resistance" );
  _d->patrolPosition = stream.get( "patrolPosition" );

  TilePos basePosition = stream.get( "base" );
  TowerPtr tower = ptr_cast<Tower>( _city()->getOverlay( basePosition ) );

  if( tower.isValid() )
  {
    _d->base = tower;
    tower->addWalker( this );
  }
  else
  {
    die();
  }
}

EnemySoldierList WallGuard::_findEnemiesInRange( unsigned int range )
{
  Tilemap& tmap = _city()->tilemap();
  EnemySoldierList walkers;

  for( unsigned int i=0; i < range; i++ )
  {
    TilePos offset( i, i );
    TilesArray tiles = tmap.getRectangle( pos() - offset, pos() + offset );

    foreach( tile, tiles )
    {
      WalkerList tileWalkers = _city()->getWalkers( walker::any, (*tile)->pos() );

      foreach( w, tileWalkers )
      {
        EnemySoldierPtr e = ptr_cast<EnemySoldier>( *w );
        if( e.isValid() )
        {
          walkers.push_back( e );
        }
      }
    }
  }

  return walkers;
}

FortificationList WallGuard::_findNearestWalls( EnemySoldierPtr enemy )
{
  FortificationList ret;

  Tilemap& tmap = _city()->tilemap();
  for( int range=1; range < 8; range++ )
  {
    TilePos offset( range, range );
    TilePos ePos = enemy->pos();
    TilesArray tiles = tmap.getRectangle( ePos - offset, ePos + offset );

    foreach( tile, tiles )
    {
      FortificationPtr f = ptr_cast<Fortification>( (*tile)->overlay() );
      if( f.isValid() && f->mayPatrol() )
      {
        ret.push_back( f );
      }
    }
  }

  return ret;
}

bool WallGuard::_tryAttack()
{
  EnemySoldierList enemies = _findEnemiesInRange( 10 );

  if( !enemies.empty() )
  {
    //find nearest walkable wall
    EnemySoldierPtr soldierInAttackRange;
    double minDistance = 8;

    //enemy in attack range
    foreach( enemy, enemies )
    {
      double tmpDistance = (*enemy)->pos().distanceFrom( pos() );
      if( tmpDistance < minDistance )
      {
        minDistance = tmpDistance;
        soldierInAttackRange = *enemy;
      }
    }

    if( soldierInAttackRange.isValid() )
    {
      _d->action = Impl::attackEnemy;
      fight();
      _changeDirection();
    }
    else
    {
      PathwayPtr shortestWay;
      minDistance = 999;
      foreach( it, enemies )
      {
        EnemySoldierPtr enemy = *it;
        FortificationList nearestWall = _findNearestWalls( enemy );

        PathwayList wayList = _d->base->getWays( pos(), nearestWall );
        foreach( way, wayList )
        {
          double tmpDistance = (*way)->destination().pos().distanceFrom( enemy->pos() );
          if( tmpDistance < minDistance )
          {
            shortestWay = *way;
            minDistance = tmpDistance;
          }
        }
      }

      if( shortestWay.isValid() )
      {
        _updatePathway( *shortestWay.object() );
        _d->action = Impl::go2position;
        _setAction( acMove );
        go();
        return true;
      }
    }
  }

  return false;
}

void WallGuard::_back2tower()
{
  if( _d->base.isValid() )
  {
    _d->action = Impl::go2tower;
    TilesArray enter = _d->base->getEnterArea();

    if( !enter.empty() )
    {
      Pathway way = _d->base->getWay( pos(), enter.front()->pos() );
      setPathway( way );
      go();
    }

    if( !_pathwayRef().isValid() )
    {
      deleteLater();
    }
  }
  else
  {
    die();
  }
}

void WallGuard::_back2patrol()
{

}

void WallGuard::_reachedPathway()
{
  Soldier::_reachedPathway();

  switch( _d->action )
  {

  case Impl::go2position:
  {
    bool haveEnemies = _tryAttack();
    if( !haveEnemies )
    {
      _back2tower();
      _d->action = Impl::go2tower;
    }
  }
  break;

  case Impl::go2tower:
    deleteLater();
    _d->action = Impl::doNothing;
  break;

  default:
  break;
  }
}

void WallGuard::_brokePathway(TilePos p)
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
      _d->action = Impl::patrol;
      _setAction( acNone );
      setPathway( Pathway() );
    }
  }
}

void WallGuard::_centerTile()
{
  switch( _d->action )
  {
  case Impl::doNothing:
  break;

  case Impl::go2position:
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

void WallGuard::send2city( TowerPtr base, Pathway pathway )
{
  setPos( pathway.getStartPos() );
  _d->base = base;

  setPathway( pathway );
  go();

  _d->action = Impl::go2position;

  if( !isDeleted() )
  {
    _city()->addWalker( this );
  }
}

EnemySoldierPtr WallGuard::Impl::findNearbyEnemy(EnemySoldierList enemies, TilePos pos)
{
  EnemySoldierPtr ret;
  double minDistance = 999;
  foreach( it, enemies )
  {
    double tmpDistance = pos.distanceFrom( (*it)->pos() );
    if( tmpDistance > 2 && tmpDistance < minDistance )
    {
      minDistance = tmpDistance;
      ret = *it;
    }
  }

  return ret;
}
