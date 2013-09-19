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

#include "oc3_building_ruins.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"
#include "oc3_walker_service.hpp"
#include "oc3_tile.hpp"
#include "oc3_city.hpp"
#include "oc3_game_event_mgr.hpp"

BurningRuins::BurningRuins() : ServiceBuilding(Service::S_BURNING_RUINS, B_BURNING_RUINS, Size(1) )
{
    _fireLevel = 99;

    setPicture( Picture::load( ResourceGroup::land2a, 187) );
    _getAnimation().load( ResourceGroup::land2a, 188, 8 );
    _getAnimation().setOffset( Point( 14, 26 ) );
    _fgPictures.resize(1);           
}

void BurningRuins::timeStep(const unsigned long time)
{
    ServiceBuilding::timeStep(time);

    if (time % 16 == 0 )
    {
      if( _fireLevel > 0 )
      {
        _fireLevel -= 1;
        if( _fireLevel == 50 )
        {
          setPicture(Picture::load( ResourceGroup::land2a, 214));
          _getAnimation().clear();
          _getAnimation().load( ResourceGroup::land2a, 215, 8);
          _getAnimation().setOffset( Point( 14, 26 ) );
        }
        else if( _fireLevel == 25 )
        {
          setPicture( Picture::load( ResourceGroup::land2a, 223));
          _getAnimation().clear();
          _getAnimation().load(ResourceGroup::land2a, 224, 8);
          _getAnimation().setOffset( Point( 14, 18 ) );
        }
      }
      else
      {
        deleteLater();
        _getAnimation().clear();
        _fgPictures.clear();
      }           
    }
}

void BurningRuins::destroy()
{
  ServiceBuilding::destroy();

  GameEventMgr::append( BuildEvent::create( getTilePos(), B_BURNED_RUINS ) );
}

void BurningRuins::deliverService()
{
  /*ServiceWalker walker(getService());
  walker.setServiceBuilding(*this);
  std::set<Building*> reachedBuildings = walker.getReachedBuildings(getTile().getI(), getTile().getJ());
  for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
  {
    Building &building = **itBuilding;
    building.applyService(walker);
  }*/
}

void BurningRuins::burn()
{

}

void BurningRuins::build( CityPtr city, const TilePos& pos )
{
  ServiceBuilding::build( city, pos );
  //while burning can't remove it
  getTile().setFlag( Tile::tlTree, false );
  getTile().setFlag( Tile::tlBuilding, false );
  getTile().setFlag( Tile::tlRoad, false );
  getTile().setFlag( Tile::tlRock, true );
}   

bool BurningRuins::isWalkable() const
{
  return (_fireLevel == 0);
}

float BurningRuins::evaluateService( ServiceWalkerPtr walker )
{
  if ( Service::S_PREFECT == walker->getService() )
  {
    return _fireLevel;
  }

  return 0;
}

void BurningRuins::applyService(ServiceWalkerPtr walker)
{
  if ( Service::S_PREFECT == walker->getService() )
  {
    _fireLevel = math::clamp<float>( _fireLevel - walker->getServiceValue(), 0.f, 100.f );
  }
}

bool BurningRuins::isNeedRoadAccess() const
{
  return false;
}

void BurnedRuins::timeStep( const unsigned long time )
{

}

BurnedRuins::BurnedRuins() : Building( B_BURNED_RUINS, Size(1) )
{
  setPicture( Picture::load( ResourceGroup::land2a, 111 + rand() % 8 ));
}

void BurnedRuins::build( CityPtr city, const TilePos& pos )
{
  Building::build( city, pos);

  getTile().setFlag( Tile::tlBuilding, true );
  getTile().setFlag( Tile::tlRock, false );
}

bool BurnedRuins::isWalkable() const
{
  return true;
}

bool BurnedRuins::isNeedRoadAccess() const
{
  return false;
}

void BurnedRuins::destroy()
{
  Building::destroy();
}

CollapsedRuins::CollapsedRuins() : Building(B_COLLAPSED_RUINS, Size(1) )
{
    _damageLevel = 1;

    _getAnimation().load( ResourceGroup::sprites, 1, 8 );
    _getAnimation().setOffset( Point( 14, 26 ) );
    _getAnimation().setFrameDelay( 4 );
    _getAnimation().setLoop( false );
    _fgPictures.resize(1);
}

void CollapsedRuins::burn()
{

}

void CollapsedRuins::build( CityPtr city, const TilePos& pos )
{
    Building::build( city, pos );
    //while burning can't remove it
    getTile().setFlag( Tile::tlTree, false );
    getTile().setFlag( Tile::tlBuilding, true );
    getTile().setFlag( Tile::tlRoad, false );
    setPicture( ResourceGroup::land2a, 111 + rand() % 8  );
}

bool CollapsedRuins::isWalkable() const
{
  return true;
}

bool CollapsedRuins::isNeedRoadAccess() const
{
  return false;
}


PlagueRuins::PlagueRuins() : Building( B_PLAGUE_RUINS, Size(1) )
{
  _fireLevel = 99;

  setPicture( Picture::load( ResourceGroup::land2a, 187) );
  _getAnimation().load( ResourceGroup::land2a, 188, 8 );
  _getAnimation().setOffset( Point( 14, 26 ) );
  _fgPictures.resize(2);
  _fgPictures[ 1 ] = Picture::load( ResourceGroup::sprites, 218 );
  _fgPictures[ 1 ].setOffset( 16, 32 );
}

void PlagueRuins::timeStep(const unsigned long time)
{
  _getAnimation().update( time );
  _fgPictures[ 0 ] = _getAnimation().getCurrentPicture();

  if (time % 16 == 0 )
  {
    if( _fireLevel > 0 )
    {
      _fireLevel -= 1;
      if( _fireLevel == 50 )
      {
        setPicture(Picture::load( ResourceGroup::land2a, 214));
        _getAnimation().clear();
        _getAnimation().load( ResourceGroup::land2a, 215, 8);
        _getAnimation().setOffset( Point( 14, 26 ) );
      }
      else if( _fireLevel == 25 )
      {
        setPicture( Picture::load( ResourceGroup::land2a, 223));
        _getAnimation().clear();
        _getAnimation().load(ResourceGroup::land2a, 224, 8);
        _getAnimation().setOffset( Point( 14, 18 ) );
      }
    }
    else
    {
      deleteLater();
      _getAnimation().clear();
      _fgPictures.clear();
    }
  }
}

void PlagueRuins::destroy()
{
  Building::destroy();

  BuildEvent::create( getTilePos(), B_BURNED_RUINS );
}

void PlagueRuins::applyService(ServiceWalkerPtr walker)
{

}

void PlagueRuins::burn()
{

}

void PlagueRuins::build( CityPtr city, const TilePos& pos )
{
  Building::build( city, pos );
  //while burning can't remove it
  getTile().setFlag( Tile::tlTree, false );
  getTile().setFlag( Tile::tlBuilding, false );
  getTile().setFlag( Tile::tlRoad, false );
  getTile().setFlag( Tile::tlRock, true );
}

bool PlagueRuins::isWalkable() const
{
  return (_fireLevel == 0);
}

bool PlagueRuins::isNeedRoadAccess() const
{
  return false;
}
