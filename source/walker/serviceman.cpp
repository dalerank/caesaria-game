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

#include "serviceman.hpp"
#include "gfx/tile.hpp"
#include "core/variant_map.hpp"
#include "objects/service.hpp"
#include "helper.hpp"
#include "city/city.hpp"
#include "pathway/path_finding.hpp"
#include "pathway/pathway_helper.hpp"
#include "name_generator.hpp"
#include "core/utils.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "corpse.hpp"
#include "core/foreach.hpp"
#include "gfx/helper.hpp"
#include "city/states.hpp"
#include "gfx/tilearea.hpp"
#include "walkers_factory.hpp"

using namespace gfx;

REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::priest,   Service::religionCeres, priest )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::doctor,   Service::doctor, doctor )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::bathlady, Service::baths, bathlady )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::actor,    Service::theater, actor )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::gladiator,Service::amphitheater, gladiator )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::barber,   Service::barber, barber )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::surgeon,  Service::hospital, surgeon )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::scholar,  Service::school, scholar )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::librarian,Service::library, library )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::teacher,  Service::academy, teacher )
REGISTER_SERVICEMAN_IN_WALKERFACTORY( walker::missioner,Service::missionary, missioner )

namespace {
  const unsigned int defaultServiceDistance = 5;
}

class ServiceWalker::Impl
{
public:
  TilePos basePos;
  TilePos lastHousePos;
  Service::Type service;
  Propagator::ObsoleteOverlays obsoleteOvs;
  unsigned int reachDistance;
  unsigned int maxDistance;
};

ServiceWalker::ServiceWalker(PlayerCityPtr city, const Service::Type service)
  : Human( city ), _d( new Impl )
{
  _setType( walker::serviceman );
  _setNation( city->states().nation );
  _d->maxDistance = defaultServiceDistance;
  _d->service = service;
  _d->reachDistance = 2;
  _d->lastHousePos = gfx::tilemap::invalidLocation();

  _init(service);
}

void ServiceWalker::_init(const Service::Type service)
{
  _d->service = service;
  NameGenerator::NameType nameType = NameGenerator::male;

  switch (_d->service)
  {
  case Service::well:
  case Service::fountain:
  case Service::oracle:
    //_setAnimation( gfx::unknown );
  break;
  
  case Service::religionNeptune:
  case Service::religionCeres:
  case Service::religionVenus:
  case Service::religionMars:
  case Service::religionMercury:
    _setType( walker::priest );
  break;
  
  case Service::engineer:  _setType( walker::engineer ); break;
  case Service::doctor:    _setType( walker::doctor );   break;
  case Service::hospital:  _setType( walker::surgeon );  break;
  case Service::barber:    _setType( walker::barber );   break;
  case Service::baths:     _setType( walker::bathlady ); break;
  case Service::school:    _setType( walker::scholar);   break;
  case Service::theater:   _setType( walker::actor );    break;
  case Service::amphitheater: _setType( walker::gladiator ); break;
  case Service::colloseum:  _setType( walker::lionTamer );    break;
  case Service::hippodrome: _setType( walker::charioteer ); break;
  case Service::market:     _setType( walker::marketLady ); nameType = NameGenerator::female; break;
  case Service::missionary: _setType( walker::missioner ); break;

  case Service::library:
  case Service::academy:    _setType( walker::teacher ); break;

  case Service::forum:
  case Service::senate:     _setType( walker::taxCollector); break;

  default:
  break;
  }

  if( name().empty() )
    setName( NameGenerator::rand( nameType ));
}

const TilePos &ServiceWalker::baseLocation() const { return _d->basePos; }
Service::Type ServiceWalker::serviceType() const {  return _d->service; }

void ServiceWalker::_computeWalkerPath( int orders )
{  
  if( orders == noOrders )
  {
    orders = goLowerService;
  }

  ConstructionPtr ctr = ptr_cast<Construction>( _city()->getOverlay( _d->basePos ) );
  Propagator pathPropagator( _city() );

  pathPropagator.init( ctr );
  pathPropagator.setAllDirections( Propagator::nwseDirections );
  pathPropagator.setObsoleteOverlays( _d->obsoleteOvs );

  PathwayList pathWayList = pathPropagator.getWays(_d->maxDistance);

  float maxPathValue = 0.0;
  PathwayPtr bestPath;

  if( (orders & goLowerService) == goLowerService )
  {
    foreach( current, pathWayList )
    {
      float pathValue = evaluatePath( *current );
      if(pathValue > maxPathValue)
      {
        bestPath = *current;
        maxPathValue = pathValue;
      }
    }
  }

  if( !bestPath.isValid()
      && pathWayList.size() > 0
      && ( (orders & anywayWhenFailed ) == anywayWhenFailed ) )
  {
    bestPath = pathWayList.random();
  }

  if( !bestPath.isValid() )
  {
    //no good path
    deleteLater();
    return;
  }

  if( (orders & enterLastHouse) == enterLastHouse )
  {
    const TilesArray& tiles = bestPath->allTiles();
    foreach( itTile, tiles )
    {
      ServiceWalker::ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->pos() );
      foreach( it, reachedBuildings )
      {
        if( (*it)->type() == object::house )
        {
          _d->lastHousePos = (*itTile)->pos();
        }
      }
    }
  }

  setPos( bestPath->startPos() );
  setPathway( *bestPath.object() );
}

void ServiceWalker::_cancelPath()
{
  TilesArray pathTileList = pathway().allTiles();

  foreach( tile, pathTileList )
  {
    ReachedBuildings reachedBuildings = getReachedBuildings( (*tile)->pos() );
    foreach( b, reachedBuildings )
    {
      // the building has not been reserved yet
       (*b)->cancelService( _d->service );
    }
  }
}

void ServiceWalker::_addObsoleteOverlay(object::Type type) { _d->obsoleteOvs.insert( type ); }
unsigned int ServiceWalker::reachDistance() const { return _d->reachDistance;}
void ServiceWalker::setReachDistance(unsigned int value) { _d->reachDistance = value;}

void ServiceWalker::return2Base()
{
  if( !_pathway().isReverse() )
  {
    _pathway().toggleDirection();
  }
}

ServiceWalker::ReachedBuildings ServiceWalker::getReachedBuildings(const TilePos& pos )
{
  ReachedBuildings res;

  TilesArea reachedTiles( _city()->tilemap(), pos, reachDistance() );
  foreach( it, reachedTiles )
  {
    BuildingPtr building = ptr_cast<Building>( (*it)->overlay() );
    if( building.isValid() )
    {
      res.insert(building);
    }
  }

  return res;
}

float ServiceWalker::evaluatePath( PathwayPtr pathWay )
{
  // evaluate all buildings along the path
  ServiceWalker::ReachedBuildings doneBuildings;  // list of evaluated building: don't do them again
  const TilesArray& pathTileList = pathWay->allTiles();

  int distance = 0;
  float res = 0.0;
  foreach( itTile, pathTileList )
  {
    ServiceWalker::ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->pos() );
    foreach( it, reachedBuildings )
    {
      std::pair<ServiceWalker::ReachedBuildings::iterator, bool> rc = doneBuildings.insert( *it );
      if (rc.second == true)
      {
        // the building has not been evaluated yet
        int oneTileValue = (*it)->evaluateService( ServiceWalkerPtr( this ) );
        // mul serviceValue for buildingSize, need for more effectively count of path result
        res += (oneTileValue * (*it)->size().area() );
      }
    }
    distance++;
  }

  return res;
}

void ServiceWalker::_reservePath( const Pathway& pathWay)
{
  // reserve all buildings along the path
  ReachedBuildings doneBuildings;  // list of evaluated building: don't do them again
  const TilesArray& pathTileList = pathWay.allTiles();

  foreach( itTile, pathTileList )
  {
    ReachedBuildings reachedBuildings = getReachedBuildings( (*itTile)->pos() );
    foreach( it, reachedBuildings )
    {
      std::pair<ReachedBuildings::iterator, bool> rc = doneBuildings.insert( *it );
      if (rc.second == true)
      {
        // the building has not been reserved yet
        (*it)->reserveService(_d->service);
      }
    }
  }
}

void ServiceWalker::_updatePathway( PathwayPtr pathway)
{
  _updatePathway( pathway.isValid() ? *pathway.object() : Pathway() );
}

void ServiceWalker::_updatePathway( const Pathway& pathway)
{
  _cancelPath();

  Walker::_updatePathway( pathway );
  _reservePath( pathway );
}

void ServiceWalker::send2City(BuildingPtr base, int orders)
{
  if( base.isNull() )
  {
    Logger::warning( "WARNING !!!: Try send from unexist base " );
    return;
  }

  ServiceBuildingPtr servBuilding = ptr_cast<ServiceBuilding>( base );

  if( servBuilding.isValid() && _d->maxDistance <= defaultServiceDistance )
  {
    Logger::warning( "WARNING !!!: Base have short distance for walker. Parent [%d,%d] ", base->pos().i(), base->pos().j() );
    setMaxDistance( servBuilding->walkerDistance() );
  }

  if( !servBuilding.isValid() )
  {
    Logger::warning( "WARNING !!!: ServiceWalker send not from service building. Parent [%d,%d] ", base->pos().i(), base->pos().j() );
  }

  setBase( base );
  _computeWalkerPath( orders );

  attach();
}

void ServiceWalker::_centerTile()
{
  Walker::_centerTile();

  ReachedBuildings reachedBuildings = getReachedBuildings( pos() );

  foreach( b, reachedBuildings ) { (*b)->applyService( this ); }

  ServiceBuildingPtr servBuilding = ptr_cast<ServiceBuilding>( _city()->getOverlay( baseLocation() ) );
  if( servBuilding.isValid() )
  {
    servBuilding->buildingsServed( reachedBuildings, this );
  }

  if( _d->lastHousePos == pos() )
  {
    deleteLater();
  }
}

void ServiceWalker::_reachedPathway()
{
  Walker::_reachedPathway();
  if( _pathway().isReverse())
  {
    // walker is back in the market
    deleteLater();
  }
  else
  {
    // walker finished service => get back to service building    
    _pathway().move( Pathway::reverse );
    _computeDirection();
    go();
  }
}

void ServiceWalker::_brokePathway(TilePos p)
{
  Walker::_brokePathway( p );
  ConstructionPtr ctr = ptr_cast<Construction>( _city()->getOverlay( baseLocation() ) );
  if( ctr.isValid() )
  {
    Pathway way = PathwayHelper::create( pos(), ctr, PathwayHelper::roadFirst );
    if( way.isValid() )
    {
      _updatePathway( way );
      go();
      return;
    }
  }

  die();
}

void ServiceWalker::_noWay()
{
  TilesArray area;

  ConstructionPtr base = ptr_cast<Construction>( _city()->getOverlay( baseLocation() ));
  if( base.isValid() )
    area = base->enterArea();

  if( area.contain( pos() ) )
  {
    deleteLater();
    return;
  }

  die();
}

void ServiceWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "service" ] = Variant( ServiceHelper::getName( _d->service ) );
  VARIANT_SAVE_ANY_D( stream, _d, basePos )
  VARIANT_SAVE_ANY_D( stream, _d, maxDistance )
  VARIANT_SAVE_ANY_D( stream, _d, reachDistance )
  VARIANT_SAVE_ANY_D( stream, _d, lastHousePos )
}

void ServiceWalker::load( const VariantMap& stream )
{
  Walker::load( stream );

  Service::Type srvcType = ServiceHelper::getType( stream.get( "service" ).toString() );
  _init( srvcType );
  VARIANT_LOAD_ANY_D( _d, maxDistance, stream )
  VARIANT_LOAD_ANY_D( _d, reachDistance, stream )
  VARIANT_LOAD_ANY_D( _d, lastHousePos, stream )

  VARIANT_LOAD_ANY_D( _d, basePos, stream )
  OverlayPtr overlay = _city()->getOverlay( _d->basePos );

  if( overlay.isNull() )
  {
    Logger::warning( "Not found base building[%d,%d] for service walker", _d->basePos.i(), _d->basePos.j() );
  }
  else
  {
    WorkingBuildingPtr wrk = ptr_cast<WorkingBuilding>( overlay );
    if( wrk.isValid() )
    {
      wrk->addWalker( this );
    }
  }
}

void ServiceWalker::setPathway( const Pathway& pathway)
{
  _cancelPath();

  Walker::setPathway( pathway );
  _reservePath( pathway );
}

bool ServiceWalker::die()
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
  case Service::academy:
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

  _cancelPath();
  Walker::die();

  if( start >= 0 )
  {
    Corpse::create( _city(), pos(), rcGroup, start, stop );
    return true;
  }

  return false;
}

void ServiceWalker::initialize(const VariantMap& options)
{
  Human::initialize( options );

  VariantList oboletesOvs = options.get( "obsoleteOverlays" ).toList();
  foreach( it, oboletesOvs )
  {
    object::Type ovType = object::toType( it->toString() );
    if( ovType != object::unknown )
      _addObsoleteOverlay( ovType );
  }
}

void ServiceWalker::setMaxDistance( const int distance ) { _d->maxDistance = distance; }
float ServiceWalker::serviceValue() const { return 100; }

TilePos ServiceWalker::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->basePos;
  default: break;
  }

  return Human::places( type );
}

ServiceWalkerPtr ServiceWalker::create(PlayerCityPtr city, const Service::Type service )
{
  ServiceWalkerPtr ret( new ServiceWalker( city, service ) );
  ret->initialize( WalkerHelper::getOptions( ret->type() ) );
  ret->drop();
  return ret;
}

ServiceWalker::~ServiceWalker() {}
void ServiceWalker::setBase( BuildingPtr base ) { _d->basePos = base.isValid() ? base->pos() : gfx::tilemap::invalidLocation(); }
WalkerPtr ServicemanCreator::create(PlayerCityPtr city) { return ServiceWalker::create( city, serviceType ).object();  }
