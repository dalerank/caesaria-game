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

#include "serviceman.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "game/city.hpp"
#include "game/path_finding.hpp"
#include "game/name_generator.hpp"
#include "core/stringhelper.hpp"
#include "game/tilemap.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "corpse.hpp"

using namespace constants;

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
  _setType( walker::serviceman );
  _setAnimation( gfx::unknown );
  _d->maxDistance = 5;  // TODO: _building.getMaxDistance() ?
  _d->service = service;

  _init(service);
}

void ServiceWalker::_init(const Service::Type service)
{
  _d->service = service;
  NameGenerator::NameType nameType = NameGenerator::male;

  switch (_d->service)
  {
  case Service::well:
  case Service::fontain:
  case Service::oracle:
    _setAnimation( gfx::unknown );
  break;
  
  case Service::religionNeptune:
  case Service::religionCeres:
  case Service::religionVenus:
  case Service::religionMars:
  case Service::religionMercury:
    _setAnimation( gfx::priest );
  break;
  
  case Service::engineer: _setAnimation( gfx::engineer ); _setType( walker::engineer ); break;
  case Service::doctor:   _setAnimation( gfx::doctor );   _setType( walker::doctor );   break;
  case Service::hospital: _setAnimation( gfx::doctor );   _setType( walker::surgeon );  break;
  case Service::barber:   _setAnimation( gfx::barber );   _setType( walker::barber );   break;
  case Service::baths:    _setAnimation( gfx::bathlady );     _setType( walker::bathlady ); break;
  case Service::school:   _setAnimation( gfx::scholar );                                  break;
  case Service::theater:  _setAnimation( gfx::actor );    _setType( walker::actor );    break;
  case Service::amphitheater:_setAnimation( gfx::gladiator ); _setType( walker::gladiator ); break;
  case Service::colloseum:_setAnimation( gfx::tamer );    _setType( walker::tamer );    break;
  case Service::hippodrome:_setAnimation( gfx::actor );   _setType( walker::charioter ); break;
  case Service::market: _setAnimation( gfx::marketlady ); nameType = NameGenerator::female; break;

  case Service::library:
  case Service::college:  _setAnimation( gfx::teacher );                              break;

  case Service::forum:
  case Service::senate:   _setAnimation(gfx:: taxCollector );                                    break;

  default:
  break;
  }

  setName( NameGenerator::rand( nameType ));
}

BuildingPtr ServiceWalker::getBase() const
{
  if( _d->base.isNull() )
  {
   Logger::warning( "ServiceBuilding is not initialized" );
  }

  return _d->base;
}

Service::Type ServiceWalker::getService() const
{
  return _d->service;
}

void ServiceWalker::_computeWalkerPath()
{  
  Propagator pathPropagator( _getCity() );
  pathPropagator.init( _d->base.as<Construction>() );
  Propagator::PathWayList pathWayList = pathPropagator.getWays(_d->maxDistance);

  float maxPathValue = 0.0;
  Pathway* bestPath = NULL;
  foreach( Pathway& current, pathWayList )
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
  setPathway(*bestPath);
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

float ServiceWalker::evaluatePath( Pathway& pathWay )
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

void ServiceWalker::reservePath(Pathway &pathWay)
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
  _computeWalkerPath();

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
  _init( srvcType );
  _d->maxDistance = stream.get( "maxDistance" ).toInt();

  TilePos basePos = stream.get( "base" ).toTilePos();
  TileOverlayPtr overlay = _getCity()->getTilemap().at( basePos ).getOverlay();

  _d->base = overlay.as<Building>();
  if( _d->base.isNull() )
  {
    Logger::warning( "Not found base building[%d,%d] for service walker", basePos.getI(), basePos.getJ() );
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

void ServiceWalker::die()
{
  int start=-1, stop=-1;
  std::string rcGroup;
  switch( _d->service )
  {
  case Service::engineer: start=1233; stop=1240; rcGroup=ResourceGroup::citizen1; break;

  case Service::religionNeptune:
  case Service::religionCeres:
  case Service::religionVenus:
  case Service::religionMars:
  case Service::religionMercury:
    start=305; stop=312; rcGroup=ResourceGroup::citizen1;
  break;

  case Service::doctor:
  case Service::hospital:
    start=913; stop=920; rcGroup=ResourceGroup::citizen3; break;
  break;

  case Service::barber: start=559; stop=566; rcGroup=ResourceGroup::citizen2; break;
  case Service::baths: start=201; stop=208; rcGroup = ResourceGroup::citizen1; break;
  case Service::school: start=817; stop=824; rcGroup = ResourceGroup::citizen1; break;

  case Service::library:
  case Service::college:
    start=1121; stop=1128; rcGroup = ResourceGroup::citizen3;
  break;

  case Service::theater: start=409; stop=416; rcGroup=ResourceGroup::citizen1; break;
  case Service::amphitheater: start=97; stop=104; rcGroup=ResourceGroup::citizen2; break;
  case Service::colloseum: start=513; stop=520; rcGroup=ResourceGroup::citizen1; break;
  case Service::hippodrome: break;

  case Service::market: start=921; stop=928; rcGroup=ResourceGroup::citizen1; break;

  case Service::forum:
  case Service::senate:
    start=713; stop=720; rcGroup = ResourceGroup::citizen1; break;
  break;

  default:
  break;
  }

  Walker::die();

  if( start >= 0 )
  {
    Corpse::create( _getCity(), getIJ(), rcGroup, start, stop );
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
