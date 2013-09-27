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
#include "oc3_tilemap.hpp"

class ServiceWalker::Impl
{
public:
  BuildingPtr base;
  Service::Type service;
  int maxDistance;
};

ServiceWalker::ServiceWalker( CityPtr city, const Service::Type service)
  : Walker( city ), _d( new Impl )
{
  _setType( WT_SERVICE );
  _setGraphic( WG_NONE );
  _d->maxDistance = 5;  // TODO: _building.getMaxDistance() ?
  _d->service = service;

  init(service);
}

void ServiceWalker::init(const Service::Type service)
{
  _d->service = service;
  NameGenerator::NameType nameType = NameGenerator::male;

  switch (_d->service)
  {
  case Service::well:
  case Service::fontain:
  case Service::S_TEMPLE_ORACLE:
    _setGraphic( WG_NONE );
  break;
  
  case Service::engineer:
     _setGraphic( WG_ENGINEER );
     _setType( WT_ENGINEER );
  break;

  case Service::S_TEMPLE_NEPTUNE:
  case Service::S_TEMPLE_CERES:
  case Service::S_TEMPLE_VENUS:
  case Service::S_TEMPLE_MARS:
  case Service::S_TEMPLE_MERCURE:
    _setGraphic( WG_PRIEST );
  break;
  
  case Service::doctor:
  case Service::hospital:
    _setGraphic( WG_DOCTOR );
    _setType( WT_DOCTOR );
  break;
  
  case Service::barber:
    _setGraphic( WG_BARBER );
  break;
  
  case Service::baths:
    _setGraphic( WG_BATH );
    _setType( WT_BATHLADY );
  break;
  
  case Service::school:
    _setGraphic( WG_CHILD );
  break;
  
  case Service::library:
  case Service::college:
    _setGraphic( WG_LIBRARIAN );
  break;
  
  case Service::theater:
    _setType( WT_ACTOR );
    _setGraphic( WG_ACTOR );
  break;

  case Service::amphitheater:
    _setType( WT_GLADIATOR );
    _setGraphic( WG_GLADIATOR );
  break;

  case Service::colloseum:
    _setType( WT_TAMER );
    _setGraphic( WG_TAMER );
  break;

  case Service::hippodrome:
    _setType( WT_CHARIOT );
    _setGraphic( WG_ACTOR );
  break;
  
  case Service::S_MARKET:
    _setGraphic( WG_MARKETLADY );
    nameType = NameGenerator::female;
  break;

  case Service::S_FORUM:
  case Service::S_SENATE:
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

Service::Type ServiceWalker::getService() const
{
  return _d->service;
}

void ServiceWalker::computeWalkerPath()
{
  std::list<PathWay> pathWayList;

  Propagator pathPropagator( _getCity() );
  pathPropagator.init( _d->base.as<Construction>() );
  pathPropagator.getWays(_d->maxDistance, pathWayList);

  float maxPathValue = 0.0;
  PathWay* bestPath = NULL;
  foreach( PathWay& current, pathWayList )
  {
    float pathValue = evaluatePath(current);
    if (pathValue > maxPathValue)
    {
      bestPath = &current;
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

void ServiceWalker::return2Base()
{
  if( !getPathway().isReverse() )
  {
    _getPathway().toggleDirection();
  }
}

ServiceWalker::ReachedBuildings ServiceWalker::getReachedBuildings(const TilePos& pos )
{
  ReachedBuildings res;

  int reachDistance = getReachDistance();
  TilePos start = pos - TilePos( reachDistance, reachDistance );
  TilePos stop = pos + TilePos( reachDistance, reachDistance );
  TilemapArea reachedTiles = _getCity()->getTilemap().getArea( start, stop );
  foreach( Tile* tile, reachedTiles )
  {
    BuildingPtr building = tile->getOverlay().as<Building>();
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
  ConstTilemapTiles& pathTileList = pathWay.getAllTiles();

  int distance = 0;
  float res = 0.0;
  for( ConstTilemapTiles::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
  {
    ServiceWalker::ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->getIJ() );
    foreach( BuildingPtr building, reachedBuildings )
    {
      std::pair<ServiceWalker::ReachedBuildings::iterator, bool> rc = doneBuildings.insert( building );
      if (rc.second == true)
      {
        // the building has not been evaluated yet
        res += building->evaluateService( ServiceWalkerPtr( this ) );
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
  ConstTilemapTiles& pathTileList = pathWay.getAllTiles();

  for( ConstTilemapTiles::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
  {
    ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->getIJ() );
    foreach( BuildingPtr building, reachedBuildings )
    {
      std::pair<ReachedBuildings::iterator, bool> rc = doneBuildings.insert( building );
      if (rc.second == true)
      {
        // the building has not been reserved yet
        building->reserveService(_d->service);
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
    _getCity()->addWalker( WalkerPtr( this ));
  }
}

void ServiceWalker::onNewTile()
{
  Walker::onNewTile();

  ReachedBuildings reachedBuildings = getReachedBuildings( getIJ() );
  foreach( BuildingPtr building, reachedBuildings )
  {
    building->applyService( ServiceWalkerPtr( this ) );
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
    computeDirection();
    go();
  }
}

void ServiceWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "service" ] = Variant( ServiceHelper::getName( _d->service ) );
  stream[ "base" ] = _d->base->getTile().getIJ();
  stream[ "maxDistance" ] = _d->maxDistance;
}

void ServiceWalker::load( const VariantMap& stream )
{
  Walker::load( stream );

  Service::Type srvcType = ServiceHelper::getType( stream.get( "service" ).toString() );
  init( srvcType );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();

  TilePos basePos = stream.get( "base" ).toTilePos();
  LandOverlayPtr overlay = _getCity()->getTilemap().at( basePos ).getOverlay();

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

ServiceWalkerPtr ServiceWalker::create( CityPtr city, const Service::Type service )
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
