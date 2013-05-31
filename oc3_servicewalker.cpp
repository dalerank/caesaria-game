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

#include "oc3_servicewalker.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"
#include "oc3_city.hpp"

class ServiceWalker::Impl
{
public:
  City* city;
  BuildingPtr base;
  ServiceType service;
  int maxDistance;
};

ServiceWalker::ServiceWalker( City& city, const ServiceType service) 
: _d( new Impl )
{
  _walkerType = WT_SERVICE;
  _walkerGraphic = WG_NONE;
  _d->maxDistance = 5;  // TODO: _building.getMaxDistance() ?
  _d->service = service;
  _d->city = &city;

  init(service);
}

void ServiceWalker::init(const ServiceType service)
{
  _d->service = service;

  switch (_d->service)
  {
  case S_WELL:
  case S_FOUNTAIN:
  case S_SENATE:
  case S_TEMPLE_ORACLE:
    _walkerGraphic = WG_NONE;
    break;
  case S_ENGINEER:
    _walkerGraphic = WG_ENGINEER;
    break;
 /*
     case S_PREFECT:
        _walkerGraphic = WG_PREFECT;
        break;*/
    
  case S_TEMPLE_NEPTUNE:
  case S_TEMPLE_CERES:
  case S_TEMPLE_VENUS:
  case S_TEMPLE_MARS:
  case S_TEMPLE_MERCURE:
    _walkerGraphic = WG_PRIEST;
    break;
  case S_DOCTOR:
  case S_HOSPITAL:
    _walkerGraphic = WG_DOCTOR;
    break;
  case S_BARBER:
    _walkerGraphic = WG_BARBER;
    break;
  case S_BATHS:
    _walkerGraphic = WG_BATH;
    break;
  case S_SCHOOL:
    _walkerGraphic = WG_CHILD;
    break;
  case S_LIBRARY:
  case S_COLLEGE:
    _walkerGraphic = WG_LIBRARIAN;
    break;
  case S_THEATER:
  case S_AMPHITHEATER:
  case S_HIPPODROME:
  case S_COLLOSSEUM:
    _walkerGraphic = WG_ACTOR;
    break;
  case S_MARKET:
    _walkerGraphic = WG_MARKETLADY;
    break;
  case S_FORUM:
    _walkerGraphic = WG_TAX;
    break;
  case S_MAX:
    break;
  }
}

BuildingPtr ServiceWalker::getBase() const
{
  if( _d->base.isNull() ) 
  {
    _OC3_DEBUG_BREAK_IF( "ServiceBuilding is not initialized" );
  }

  return _d->base;
}

ServiceType ServiceWalker::getService()
{
  return _d->service;
}

void ServiceWalker::computeWalkerPath()
{
  std::list<PathWay> pathWayList;

  Propagator pathPropagator;
  pathPropagator.init( *_d->base.object() );
  pathPropagator.getAllPaths(_d->maxDistance, pathWayList);

  float maxPathValue = 0.0;
  PathWay* bestPath = NULL;
  for( std::list<PathWay>::iterator itPath = pathWayList.begin(); 
    itPath != pathWayList.end(); ++itPath)
  {
    PathWay &path = *itPath;
    float pathValue = evaluatePath(path);
    if (pathValue > maxPathValue)
    {
      bestPath = &path;
      maxPathValue = pathValue;
    }
  }

  if (bestPath == NULL)
  {
    // no good path
    deleteLater();
    return;
  }

  reservePath(*bestPath);
  setPathWay(*bestPath);
  setIJ(_pathWay.getOrigin().getIJ() );
}

unsigned int ServiceWalker::getReachDistance() const
{
  return 2;
}

ServiceWalker::ReachedBuildings ServiceWalker::getReachedBuildings(const TilePos& pos )
{
  ReachedBuildings res;

  int reachDistance = getReachDistance();
  TilePos start = pos - TilePos( reachDistance, reachDistance );
  TilePos stop = pos + TilePos( reachDistance, reachDistance );
  std::list<Tile*> reachedTiles = _d->city->getTilemap().getFilledRectangle( start, stop );
  for (std::list<Tile*>::iterator itTile = reachedTiles.begin(); itTile != reachedTiles.end(); ++itTile)
  {
    TerrainTile& terrain = (*itTile)->get_terrain();

    BuildingPtr building = terrain.getOverlay().as<Building>();
    if( building.isValid() )
    {
      res.insert(building);
    }
  }

  return res;
}

float ServiceWalker::evaluatePath( PathWay& pathWay )
{
  // evaluate all buildings along the path
  ServiceWalker::ReachedBuildings doneBuildings;  // list of evaluated building: don't do them again
  PtrTilesList& pathTileList = pathWay.getAllTiles();

  int distance = 0;
  float res = 0.0;
  for (PtrTilesList::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
  {
    ServiceWalker::ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->getIJ() );
    for (ServiceWalker::ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
    {
      std::pair<ServiceWalker::ReachedBuildings::iterator, bool> rc = doneBuildings.insert( *itBuilding );
      if (rc.second == true)
      {
        // the building has not been evaluated yet
        res += (*itBuilding)->evaluateService( ServiceWalkerPtr( this ) );
      }
    }
    distance++;
  }

  // std::cout << "evaluate path ";
  // pathWay.prettyPrint();
  // std::cout << " = " << res << std::endl;

  return res;
}

void ServiceWalker::reservePath(PathWay &pathWay)
{
  // reserve all buildings along the path
  ReachedBuildings doneBuildings;  // list of evaluated building: don't do them again
  std::list<Tile*>& pathTileList = pathWay.getAllTiles();

  for (std::list<Tile*>::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
  {
    ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->getIJ() );
    for (ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
    {
      std::pair<ReachedBuildings::iterator, bool> rc = doneBuildings.insert( *itBuilding );
      if (rc.second == true)
      {
        // the building has not been reserved yet
        (*itBuilding)->reserveService(_d->service);
      }
    }
  }
}

void ServiceWalker::send2City( BuildingPtr base )
{
  setBase( base );
  computeWalkerPath();

  if( !isDeleted() )
    _d->city->addWalker( WalkerPtr( this ));
}

void ServiceWalker::onNewTile()
{
  Walker::onNewTile();

  ReachedBuildings reachedBuildings = getReachedBuildings( getIJ() );
  for (ReachedBuildings::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
  {
    (*itBuilding)->applyService( ServiceWalkerPtr( this ) );
  }
}

void ServiceWalker::onDestination()
{
  Walker::onDestination();
  if (_pathWay.isReverse())
  {
    // walker is back in the market
    _isDeleted= true;
  }
  else
  {
    // walker finished service => get back to service building
    _pathWay.rbegin();
    _action._action=WA_MOVE;
    computeDirection();
  }
}

void ServiceWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "type" ] = (int)_d->service;
  stream[ "base" ] = _d->base->getTile().getIJ();
  stream[ "maxDistance" ] = _d->maxDistance;
}

void ServiceWalker::load( const VariantMap& stream )
{
//   Walker::unserialize(stream);
//   _service = (ServiceType) stream.read_int(1, 0, S_MAX);
//   init(_service);
// 
//   stream.read_objectID((void**)&_base);
//   _maxDistance = stream.read_int(2, 0, 65535);
}

void ServiceWalker::setMaxDistance( const int distance )
{
  _d->maxDistance = distance;
}

float ServiceWalker::getServiceValue() const
{
  return 100;
}

ServiceWalkerPtr ServiceWalker::create( City& city, const ServiceType service )
{
  ServiceWalkerPtr ret( new ServiceWalker( city, service ) );
  ret->drop();
  return ret;
}

ServiceWalker::~ServiceWalker()
{

}

void ServiceWalker::setBase( BuildingPtr base )
{
  _d->base = base;
}

City& ServiceWalker::_getCity() const
{
  if( !_d->city )
  {
    _OC3_DEBUG_BREAK_IF( true && "city for walker is null" ); 
  }

  return *_d->city;
}