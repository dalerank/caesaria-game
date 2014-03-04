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

#include "prefect.hpp"
#include "core/position.hpp"
#include "objects/prefecture.hpp"
#include "objects/house.hpp"
#include "pathway/astarpathfinding.hpp"
#include "pathway/path_finding.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/helper.hpp"
#include "core/variant.hpp"
#include "name_generator.hpp"
#include "core/stringhelper.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"
#include "protestor.hpp"
#include "events/disaster.hpp"
#include "pathway/pathway_helper.hpp"

using namespace constants;

class Prefect::Impl
{
public:
  typedef enum { patrol=0,
                 findFire, go2fire, fightFire,
                 go2protestor, fightProtestor,
                 doNothing } PrefectAction;
    
  int water;
  TilePos endPatrolPoint;
  PrefectAction action;
};

Prefect::Prefect(PlayerCityPtr city )
: ServiceWalker( city, Service::prefect ), _d( new Impl )
{
  _setType( walker::prefect );
  _d->water = 0;
  _d->action = Impl::patrol;
  _setAnimation( gfx::prefect );

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void Prefect::_changeTile()
{
  Walker::_changeTile();
}

bool Prefect::_looks4Fire( ServiceWalker::ReachedBuildings& buildings, TilePos& p )
{
  buildings = getReachedBuildings( pos() );

  foreach( it, buildings )
  {
    if( (*it)->type() == building::burningRuins )
    {
      p = (*it)->pos();
      return true;
    }
  }

  return false;
}

bool Prefect::_looks4Protestor( TilePos& p )
{
  CityHelper helper( _getCity() );
  TilePos offset( 3, 3 );
  ProtestorList protestors = helper.find<Protestor>( walker::protestor, pos() - offset, pos() + offset );

  int minDistance=99;
  foreach( it, protestors )
  {
    ProtestorPtr protestor = *it;
    int distance = protestor->pos().distanceFrom( pos() );
    if( distance < minDistance )
    {
      minDistance =  distance;
      p = protestor->pos();
    }
  }

  return !protestors.empty();
}

bool Prefect::_checkPath2NearestFire( const ReachedBuildings& buildings )
{
  foreach( it, buildings )
  {
    BuildingPtr building = *it;
    if( building->type() != building::burningRuins )
      continue;

    if( building->pos().distanceFrom( pos() ) < 1.5f )
    {
      turn( building->pos() );
      _d->action = Impl::fightFire;
      _setAction( acFight );
      _setAnimation( gfx::prefectFightFire );
      setSpeed( 0.f );
      return true;
    }
  }

  foreach( it, buildings )
  {
    BuildingPtr building = *it;
    if( building->type() != building::burningRuins )
      continue;

    Pathway tmp = PathwayHelper::create( pos(), building->pos(), PathwayHelper::allTerrain );
    if( tmp.isValid() )
    {
      _d->action = Impl::go2fire;
      _updatePathway( tmp );
      _setAnimation( gfx::prefectDragWater );
      setSpeed( 1 );
      go();
      return true;
    }
  }

  return false;
}

void Prefect::_back2Prefecture()
{ 
  Pathway pathway = PathwayHelper::create( pos(), ptr_cast<Construction>( getBase() ),
                                           PathwayHelper::roadFirst );

  if( pathway.isValid() )
  {
    _d->action = Impl::patrol;
    _setAnimation( gfx::prefect );
    _updatePathway( pathway );
    _d->endPatrolPoint = pathway.destination().pos();
    setSpeed( 1 );
    go();
  }
  else
  {
    deleteLater();
  }
 }

void Prefect::_serveBuildings( ReachedBuildings& reachedBuildings )
{
  foreach( it, reachedBuildings )
  {
    BuildingPtr building = *it;
    building->applyService( ServiceWalkerPtr( this ) );

    HousePtr house = ptr_cast<House>( building );
    if( house.isValid() )
    {
      int healthLevel = house->getState( (Construction::Param)House::health );
      if( healthLevel < 1 )
      {
        house->deleteLater();
        events::GameEventPtr e = events::DisasterEvent::create( house->getTile(), events::DisasterEvent::plague );
        e->dispatch();
      }
    }
  }
}

void Prefect::_back2Patrol()
{
  Pathway pathway = PathwayHelper::create( pos(), _d->endPatrolPoint,
                                           PathwayHelper::allTerrain );

  if( pathway.isValid() )
  {
    _d->action = _d->water > 0 ? Impl::go2fire : Impl::patrol;
    _setAnimation( _d->water > 0 ? gfx::prefectDragWater : gfx::prefect );
    _updatePathway( pathway );
    setSpeed( 1 );
    go();
  }
  else
  {
    _back2Prefecture();
  }
}

bool Prefect::_findFire()
{
  TilePos firePos;
  ReachedBuildings reachedBuildings;
  bool haveBurningRuinsNear = _looks4Fire( reachedBuildings, firePos );
  if( haveBurningRuinsNear && _d->water > 0 )
  {
    return _checkPath2NearestFire( reachedBuildings );
  }

  return false;
}

void Prefect::_brokePathway(TilePos p)
{
  TileOverlayPtr overlay = _getCity()->getOverlay( p );
  if( overlay.isValid() && overlay->type() == building::burningRuins )
  {
    setSpeed( 0 );
    _setAction( acFight );
    _d->action = Impl::fightFire;
    _setAnimation( gfx::prefectFightFire );
    return;
  }
  else if( _d->water > 0 )
  {
    TilePos destination = _pathwayRef().destination().pos();

    Pathway pathway = PathwayHelper::create( pos(), destination, PathwayHelper::allTerrain );
    if( pathway.isValid() )
    {
      setSpeed( 1.f );
      _d->action = Impl::findFire;
      _setAnimation( gfx::prefectDragWater );

      setPathway( pathway );
      go();
      return;
    }
  }

  _back2Patrol();
}

void Prefect::_reachedPathway()
{
  switch( _d->action )
  {
  case Impl::patrol:
    if( getBase()->getEnterArea().contain( pos() )  )
    {
      deleteLater();
      _d->action = Impl::doNothing;
    }
    else
    {
      _back2Prefecture();
    }
  break;

  case Impl::go2fire:
    if( !_findFire() )
    {
      _back2Patrol();
    }
  break;

  case Impl::findFire:
    if( !_findFire() )
    {
      _back2Patrol();
    }
  break;

  default: break;
  }
}

void Prefect::_centerTile()
{
  switch( _d->action )
  {
  case Impl::doNothing:
  break; 

  case Impl::patrol:
  {
    TilePos protestorPos, firePos;
    ReachedBuildings reachedBuildings;
    bool haveProtestorNear = _looks4Protestor( protestorPos );
    bool haveBurningRuinsNear = _looks4Fire( reachedBuildings, firePos );

    if( haveProtestorNear )
    {      
      Pathway pathway = PathwayHelper::create( pos(), protestorPos, PathwayHelper::allTerrain );

      if( pathway.isValid() )
      {
        setSpeed( 1.5f );
        _updatePathway( pathway );
        go();

        _d->action = Impl::go2protestor;
      }
    }
    else if( haveBurningRuinsNear )
    {
      //tell our prefecture that need send prefect with water to fight with fire
      //on next deliverService

      //found fire, no water, go prefecture
      if( getBase().isValid() )
      {
        PrefecturePtr ptr = ptr_cast<Prefecture>( getBase() );
        if( ptr.isValid() )
          ptr->fireDetect( firePos );

        _back2Prefecture();
      }
    }
    else
    {
      _serveBuildings( reachedBuildings );
    }
  }
  break;

  case Impl::findFire:
  {
    _findFire();
  }
  break;

  case Impl::go2protestor:
  {
    TilePos protestorPos;
    bool haveProtestorNear = _looks4Protestor( protestorPos );
    if( haveProtestorNear )
    {
      if(  protestorPos.distanceFrom( pos() ) < 1.5f  )
      {
        _d->action = Impl::fightProtestor;
        setSpeed( 0.f );
        _setAction( acFight );
        _setAnimation( gfx::prefectFight );
        return;
      }
    }
    else
    {
      _back2Patrol();
    }
  }
  break;

  case Impl::go2fire:
  {
    BuildingPtr building = ptr_cast<Building>( _getNextTile().overlay() );
    if( building.isValid() && building->type() == building::burningRuins )
    {
      _d->action = Impl::fightFire;
      _d->endPatrolPoint = building->pos();
      _setAnimation( gfx::prefectFightFire );
      _setAction( acFight );
      setSpeed( 0.f );
      return;
    }
  }
  break;

  case Impl::fightFire:
  case Impl::fightProtestor:
  break;
  }
  Walker::_centerTile();
}

void Prefect::timeStep(const unsigned long time)
{
  ServiceWalker::timeStep( time );

  switch( _d->action )
  {
  case Impl::fightFire:
  {    
    BuildingPtr building = ptr_cast<Building>( _getNextTile().overlay() );
    bool inFire = (building.isValid() && building->type() == building::burningRuins );

    if( inFire )
    {
      ServiceWalkerPtr ptr( this );
      const float beforeFight = building->evaluateService( ptr );
      building->applyService( ptr );
      const float afterFight = building->evaluateService( ptr );
      _d->water -= math::clamp( (int)(beforeFight - afterFight), 0, 100 );

      if( afterFight == 0)
      {
        inFire = false;
      }
    }

    if( !inFire && _d->water > 0 )
    {
      if( !_findFire() )
      {
        _back2Patrol();
      }
    }
    else if( _d->water <= 0 )
    {
      _back2Prefecture();
    }
  }
  break;

  case Impl::fightProtestor:
  {
    CityHelper helper( _getCity() );
    ProtestorList protestors = helper.find<Protestor>( walker::protestor,
                                                       pos() - TilePos( 1, 1), pos() + TilePos( 1, 1) );

    if( !protestors.empty() )
    {
      ProtestorPtr p = protestors.front();

      turn( p->pos() );

      p->updateHealth( -3 );
      p->acceptAction( Walker::acFight, pos() );
    }
    else
    {
      _back2Patrol();
    }
  }
  break;

  default: break;
  } // end switch( _d->action )
}

Prefect::~Prefect()
{
}

float Prefect::getServiceValue() const
{
  return 5;
}

PrefectPtr Prefect::create(PlayerCityPtr city )
{
  PrefectPtr ret( new Prefect( city ) );
  ret->drop();

  return ret;
}

void Prefect::send2City(PrefecturePtr prefecture, int water/*=0 */ )
{
  _d->action = water > 0 ? Impl::findFire : Impl::patrol;
  _d->water = water;
  _setAnimation( water > 0 ? gfx::prefectDragWater : gfx::prefect );

  if( water > 0 )
  {
    setBase( prefecture.object() );

    _getCity()->addWalker( WalkerPtr( this ));
  }
  else
  {
    ServiceWalker::send2City( prefecture.object() );
  }

  if( _pathwayRef().isValid() )
  {
    _d->endPatrolPoint = _pathwayRef().destination().pos();
  }
}

void Prefect::die()
{
  ServiceWalker::die();

  Corpse::create( _getCity(), pos(), ResourceGroup::citizen2, 711, 718 );
}

std::string Prefect::getThinks() const
{
  switch( _d->action )
  {
  case Impl::go2fire: return "##prefect_goto_fire##";
  case Impl::fightFire: return "##prefect_fight_fire##";
  default: break;
  }

  return ServiceWalker::getThinks();
}

void Prefect::load( const VariantMap& stream )
{
   ServiceWalker::load( stream );
 
  _d->action = (Impl::PrefectAction)stream.get( "prefectAction" ).toInt();
  _d->water = (int)stream.get( "water" );
  _d->endPatrolPoint = stream.get( "endPoint" );

  _setAnimation( _d->water > 0 ? gfx::prefectDragWater : gfx::prefect );

  PrefecturePtr prefecture = ptr_cast<Prefecture>( getBase() );
  if( prefecture.isValid() )
  {
    prefecture->addWalker( WalkerPtr( this ) );
    _getCity()->addWalker( WalkerPtr( this ) );
  }
  
  if( prefecture.isNull() )
  {
    Logger::warning( "Not found prefecture on loading" );
    deleteLater();
  }
}

void Prefect::save( VariantMap& stream ) const
{
  ServiceWalker::save( stream );

  stream[ "type" ] = (int)walker::prefect;
  stream[ "water" ] = _d->water;
  stream[ "prefectAction" ] = (int)_d->action;
  stream[ "endPoint" ] = _d->endPatrolPoint;
  stream[ "__debug_typeName" ] = Variant( std::string( CAESARIA_STR_EXT(walker::prefect) ) );
}
