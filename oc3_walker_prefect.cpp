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

#include "oc3_walker_prefect.hpp"
#include "oc3_positioni.hpp"
#include "oc3_burningruins.hpp"
#include "oc3_buildingprefect.hpp"
#include "oc3_astarpathfinding.hpp"
#include "oc3_path_finding.hpp"

class WalkerPrefect::Impl
{
public:
  typedef enum { patrol=0, back2Prefecture, gotoFire, fightFire, doNothing } PrefectAction; 
    
  int water;
  PrefectAction action;
};

WalkerPrefect::WalkerPrefect( ServiceBuildingPtr building, int water )
: ServiceWalker( building.as<Building>(), S_PREFECT ), _d( new Impl )
{
  setMaxDistance( 10 );
  _d->water = water;
  _d->action = water > 0 ? Impl::gotoFire : Impl::patrol;
  _walkerGraphic = water > 0 ? WG_PREFECT_DRAG_WATER : WG_PREFECT;
}

void WalkerPrefect::onNewTile()
{
  Walker::onNewTile();
}

bool WalkerPrefect::_looks4Fire( ServiceWalker::ReachedBuildings& buildings, TilePos& pos )
{
  buildings = getReachedBuildings( getIJ() );

  for( ServiceWalker::ReachedBuildings::const_iterator itBuilding = buildings.begin(); 
    itBuilding != buildings.end(); ++itBuilding)
  {
    SmartPtr< BurningRuins > bruins = ( *itBuilding ).as<BurningRuins>();
    if( bruins.isValid() )
    {
      pos = bruins->getTile().getIJ();
      return true;
    }
  }

  return false;
}

void WalkerPrefect::_checkPath2NearestFire( const ReachedBuildings& buildings )
{
  PathWay bestPath;
  int minLength = 9999;
  for( ReachedBuildings::const_iterator itBuilding = buildings.begin(); 
       itBuilding != buildings.end(); ++itBuilding)
  {
    if( (*itBuilding)->getType() != B_BURNING_RUINS )
      continue;

    PathWay tmp;
    bool foundPath = Pathfinder::getInstance().getPath( getIJ(), (*itBuilding)->getTile().getIJ(), tmp, 
                                                        false, Size( 0 ) ); 
    if( foundPath && tmp.getLength() < minLength )
    {
      bestPath = tmp;
      minLength = tmp.getLength();
      
      if( tmp.getLength() == 1 )
        break;
    }
  }

  if( bestPath.getLength() > 0 )
  {
    _pathWay = bestPath;
    _pathWay.begin();
  }
}

void WalkerPrefect::onDestination()
{ 
}

void WalkerPrefect::_back2Prefecture()
{
  bool pathFound = Pathfinder::getInstance().getPath( getIJ(), getBase()->getTile().getIJ(),
    _pathWay, false, Size( 0 ) );

  if( !pathFound )
  {
    deleteLater();
    _d->action = Impl::doNothing;
  }
  else
  {
    _pathWay.begin();
  }

  _walkerGraphic = WG_PREFECT;
  _d->action = Impl::back2Prefecture;
}

void WalkerPrefect::onMidTile()
{
  ReachedBuildings reachedBuildings;
  TilePos firePos;
  bool haveBurningRuinsNear = _looks4Fire( reachedBuildings, firePos );  
  bool isDestination = _pathWay.isDestination();

  switch( _d->action )
  {
  case Impl::patrol:
    {
      if( haveBurningRuinsNear )
      {
        //tell our prefecture that need send prefect with water to fight with fire
        //on next deliverService

        //found fire, no water, go prefecture
        ((BuildingPrefect&)getBase()).fireDetect( firePos );         
        _back2Prefecture();

        Walker::onNewDirection();
      }
      else
      {
        for( ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); 
             itBuilding != reachedBuildings.end(); ++itBuilding)
        {
          (*itBuilding)->applyService( ServiceWalkerPtr( this ) );
        }
      }

      if( isDestination )
      {
        _back2Prefecture();
      }

      Walker::onMidTile();
    }
  break;

  case Impl::back2Prefecture:
    {
      if( haveBurningRuinsNear )
      {
        //tell our prefecture that need send prefect with water to fight with fire
        //on next deliverService
        ((BuildingPrefect&)getBase()).fireDetect( firePos );         
      }

      if( isDestination )
      {
        deleteLater();
        _d->action = Impl::doNothing;
      }

      Walker::onMidTile();
    }
  break;

  case Impl::gotoFire:
    {
      if( _pathWay.getDestination().getIJ().distanceFrom( getIJ() ) < 1.5f )
      {
        LandOverlayPtr overlay = _pathWay.getDestination().get_terrain().getOverlay();
        BurningRuinsPtr bruins = overlay.as<BurningRuins>();
        if( bruins.isValid() )
        {
          _d->action = Impl::fightFire;     
          _walkerGraphic = WG_PREFECT_FIGHTS_FIRE;
          Walker::onNewDirection();
          isDestination = false;
        }
      }
      
      if( isDestination )
      {
        if( !haveBurningRuinsNear || _d->water == 0 ) 
        {
          _back2Prefecture();
        }
        else
        {
          _walkerGraphic = WG_PREFECT_DRAG_WATER;
          _d->action = Impl::gotoFire;

          _checkPath2NearestFire( reachedBuildings );
          Walker::onNewDirection();
        }
      }
     
      Walker::onMidTile();
    }
  break;

  case Impl::fightFire:
  break;
  }
}

void WalkerPrefect::timeStep(const unsigned long time)
{
  ServiceWalker::timeStep( time );

  if( _d->action == Impl::fightFire )
  {    
    setSpeed( 0 );
    LandOverlayPtr overlay = _pathWay.getDestination().get_terrain().getOverlay();
    BurningRuinsPtr bruins = overlay.as<BurningRuins>(); 
    if( bruins.isValid() )
    {
      ServiceWalkerPtr ptr( this );
      const float beforeFight = bruins->evaluateService( ptr );
      bruins->applyService( ptr );
      const float afterFight = bruins->evaluateService( ptr );
      _d->water -= math::clamp( (int)(beforeFight - afterFight), 0, 100 );

      if( afterFight == 0)
          bruins = 0;
    }

    //we catch fire, check near tiles for other burning ruins
    if( bruins.isNull() || 0 == _d->water )
    {
      _walkerGraphic = WG_PREFECT_DRAG_WATER;
      _d->action = Impl::gotoFire;  
      setSpeed( 1 );
    }      

  }
}

WalkerPrefect::~WalkerPrefect()
{
}

float WalkerPrefect::getServiceValue() const
{
  return 5;
}

WalkerPrefectPtr WalkerPrefect::create( ServiceBuildingPtr building, int water )
{
  WalkerPrefectPtr ret( new WalkerPrefect( building, water ) );
  ret->drop();

  return ret;
}