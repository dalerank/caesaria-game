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

#include "oc3_walker_service.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"
#include "oc3_city.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_name_generator.hpp"
#include "oc3_stringhelper.hpp"

class ServiceWalker::Impl
{
public:
  CityPtr city;
  BuildingPtr base;
  ServiceType service;
  int maxDistance;
};

ServiceWalker::ServiceWalker( CityPtr city, const ServiceType service) 
: _d( new Impl )
{
  _setType( WT_SERVICE );
  _setGraphic( WG_NONE );
  _d->maxDistance = 5;  // TODO: _building.getMaxDistance() ?
  _d->service = service;
  _d->city = city;

  init(service);
}

void ServiceWalker::init(const ServiceType service)
{
  _d->service = service;
  NameGenerator::NameType nameType = NameGenerator::male;

  switch (_d->service)
  {
  case S_WELL:
  case S_FOUNTAIN:
  case S_TEMPLE_ORACLE:
    _setGraphic( WG_NONE );
  break;
  
  case S_ENGINEER:
     _setGraphic( WG_ENGINEER );
     _setType( WT_ENGINEER );
  break;

  case S_TEMPLE_NEPTUNE:
  case S_TEMPLE_CERES:
  case S_TEMPLE_VENUS:
  case S_TEMPLE_MARS:
  case S_TEMPLE_MERCURE:
    _setGraphic( WG_PRIEST );
  break;
  
  case S_DOCTOR:
  case S_HOSPITAL:
    _setGraphic( WG_DOCTOR );
    _setType( WT_DOCTOR );
  break;
  
  case S_BARBER:
    _setGraphic( WG_BARBER );
  break;
  
  case S_BATHS:
    _setGraphic( WG_BATH );
  break;
  
  case S_SCHOOL:
    _setGraphic( WG_CHILD );
  break;
  
  case S_LIBRARY:
  case S_COLLEGE:
    _setGraphic( WG_LIBRARIAN );
  break;
  
  case S_THEATER:
  case S_AMPHITHEATER:
  case S_HIPPODROME:
  case S_COLLOSSEUM:
    _setGraphic( WG_ACTOR );
  break;
  
  case S_MARKET:
    _setGraphic( WG_MARKETLADY );
    nameType = NameGenerator::female;
  break;

  case S_FORUM:
  case S_SENATE:
    _setGraphic( WG_TAX );
  break;

  default:
  break;
  }

  setName( NameGenerator::rand( nameType ));
}

BuildingPtr ServiceWalker::getBase() const
{
  _OC3_DEBUG_BREAK_IF( _d->base.isNull() && "ServiceBuilding is not initialized" );

  return _d->base;
}

ServiceType ServiceWalker::getService()
{
  return _d->service;
}

void ServiceWalker::computeWalkerPath()
{
  std::list<PathWay> pathWayList;

  Propagator pathPropagator( _d->city );
  pathPropagator.init( _d->base.as<Construction>() );
  pathPropagator.getWays(_d->maxDistance, pathWayList);

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
  setIJ( _getPathway().getOrigin().getIJ() );
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
  PtrTilesArea reachedTiles = _d->city->getTilemap().getFilledRectangle( start, stop );
  for( PtrTilesArea::iterator itTile = reachedTiles.begin(); itTile != reachedTiles.end(); ++itTile)
  {
    TerrainTile& terrain = (*itTile)->getTerrain();

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
  ConstPtrTilesList& pathTileList = pathWay.getAllTiles();

  int distance = 0;
  float res = 0.0;
  for( ConstPtrTilesList::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
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
  ConstPtrTilesList& pathTileList = pathWay.getAllTiles();

  for( ConstPtrTilesList::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
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
  {
    _d->city->addWalker( WalkerPtr( this ));
  }
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
  if (_getPathway().isReverse())
  {
    // walker is back in the market
    deleteLater();
  }
  else
  {
    // walker finished service => get back to service building
    _getPathway().rbegin();
    _setAction( WA_MOVE );
    computeDirection();
  }
}

void ServiceWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "service" ] = (int)_d->service;
  stream[ "base" ] = _d->base->getTile().getIJ();
  stream[ "maxDistance" ] = _d->maxDistance;
}

void ServiceWalker::load( const VariantMap& stream )
{
  Walker::load( stream );

  init( (ServiceType)stream.get( "service" ).toInt() );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();

  TilePos basePos = stream.get( "base" ).toTilePos();
  LandOverlayPtr overlay = _d->city->getTilemap().at( basePos ).getTerrain().getOverlay();

  _d->base = overlay.as<Building>();
  if( _d->base.isNull() )
  {
    StringHelper::debug(  0xff, "Not found base building[%d,%d] for service walker", basePos.getI(), basePos.getJ() );
  }
  else
  {
    WorkingBuildingPtr wrk = _d->base.as<WorkingBuilding>();
    if( wrk.isValid() )
    {
      wrk->addWalker( this );
    }
  }
}

void ServiceWalker::setMaxDistance( const int distance )
{
  _d->maxDistance = distance;
}

float ServiceWalker::getServiceValue() const
{
  return 100;
}

ServiceWalkerPtr ServiceWalker::create( CityPtr city, const ServiceType service )
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

CityPtr ServiceWalker::_getCity() const
{
  _OC3_DEBUG_BREAK_IF( _d->city.isNull() && "city for walker is null" ); 
  return _d->city;
}
