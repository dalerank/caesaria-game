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

#include "trainee.hpp"
#include "gfx/tile.hpp"
#include "core/variant.hpp"
#include "pathway/path_finding.hpp"
#include "city/statistic.hpp"
#include "name_generator.hpp"
#include "objects/constants.hpp"
#include "core/priorities.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "objects/building.hpp"
#include "gfx/helper.hpp"
#include "pathway/pathway_helper.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_TRAINEEMAN_IN_WALKERFACTORY(walker::trainee, 0, trainee)

typedef Priorities<object::Type> NecessaryBuildings;

class TraineeWalker::Impl
{
public:
  NecessaryBuildings necBuildings;  // list of buildings needing this trainee
  TilePos baseLocation;
  TilePos destLocation;
  unsigned int maxDistance;
  float maxNeed;  // evaluates the need for that trainee
};

TraineeWalker::TraineeWalker(PlayerCityPtr city, walker::Type traineeType)
  : Human( city ), _d( new Impl )
{
  _setType( traineeType );
  _d->maxDistance = 30;
  _init( traineeType );
}

void TraineeWalker::_init(walker::Type traineeType)
{
  switch( traineeType )
  {
  case walker::actor:      _d->necBuildings << object::theater
                                           << object::amphitheater;  break;
  case walker::gladiator:  _d->necBuildings << object::amphitheater
                                              << object::colloseum;  break;
  case walker::lionTamer:  _d->necBuildings << object::colloseum;  break;
  case walker::soldier:    _d->necBuildings << object::military_academy
                                            << object::fort_legionaries
                                            << object::fort_horse
                                            << object::fort_javelin
                                            << object::tower;  break;
  case walker::charioteer:  _d->necBuildings << object::hippodrome;  break;
  default: break;
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void TraineeWalker::_cancelPath()
{
  BuildingPtr destination = receiver();
  if( destination.isValid() )
  {
    destination->cancelTrainee( type() );
  }
}

void TraineeWalker::setBase(BuildingPtr originBuilding)
{
  _d->baseLocation = originBuilding.isValid()
      ? originBuilding->pos()
      : gfx::tilemap::invalidLocation();
}

BuildingPtr TraineeWalker::receiver() const
{
  return ptr_cast<Building>( _city()->getOverlay( _d->destLocation ) );
}

void TraineeWalker::_computeWalkerPath( bool roadOnly )
{
  if( gfx::tilemap::isValidLocation( _d->baseLocation ) )
  {
    Logger::warning( "!!! WARNING: trainee walker baselocation is unaccessible" );
    deleteLater();
    return;
  }

  BuildingPtr base = ptr_cast<Building>( _city()->getOverlay( _d->baseLocation ) );
  if( !base.isValid() )
  {
    Logger::warning( "!!! WARNING: trainee walker base is null" );
    deleteLater();
    return;
  }

  _d->maxNeed = 0;  // need of this trainee in buildings
 
  Pathway finalPath;

  BuildingList buildings;
  foreach( buildingType, _d->necBuildings )
  {
    BuildingList tmpBuildings = city::statistic::findo<Building>( _city(), *buildingType );
    buildings.insert( buildings.end(), tmpBuildings.begin(), tmpBuildings.end() );
  }

  TilesArray startArea = roadOnly ? base->roadside() : base->enterArea();

  DirectRoute droute;
  _d->maxNeed = 0;
  unsigned int minDistance = _d->maxDistance;

  bool isNeedTrainee = false;
  foreach( it, buildings )
  {
    BuildingPtr bld = *it;
    float curNeed = bld->evaluateTrainee( type() );
    if( curNeed > 0 )
    {
      isNeedTrainee = true;
      break;
    }
  }

  if( !isNeedTrainee )
  {
    Logger::warning( "!!! WARNING: not need trainee walker from [%d,%d]", base->pos().i(), base->pos().j() );
    deleteLater();
    return;
  }

  foreach( itile, startArea )
  {
    TilePos startPos = (*itile)->pos();
    foreach( it, buildings )
    {
      BuildingPtr bld = *it;

      float curNeed = bld->evaluateTrainee( type() );
      if( _d->maxNeed < curNeed )
      {
        Pathway way = PathwayHelper::create( startPos, bld.object(),
                                             roadOnly ? PathwayHelper::roadOnly : PathwayHelper::allTerrain );
        if( way.isValid() && way.length() < minDistance )
        {
          _d->maxNeed = curNeed;
          droute = DirectRoute( bld.object(), way );
        }
      }
    }
  }

  if( droute.first.isValid() )
  {
    finalPath = droute.second;
    _d->destLocation = droute.first->pos();
  }

  if( finalPath.isValid() )
  {
    // some building needs that trainee!!!
    setPos( finalPath.startPos() );
    setPathway( finalPath );
  }
  else
  {
    // nobody needs him...
    deleteLater();
  }
}

void TraineeWalker::_checkDestination(const object::Type buildingType, Propagator &pathPropagator)
{
  DirectPRoutes pathWayList = pathPropagator.getRoutes( buildingType );

  foreach( item, pathWayList )
  {
    // for every building within range
    BuildingPtr building = ptr_cast<Building>( item->first );

    float need = building->evaluateTrainee( type() );
    if (need > _d->maxNeed)
    {
      _d->maxNeed = need;
      _d->destLocation = building->pos();
    }
  }
}

int TraineeWalker::value() const{ return 100; }

void TraineeWalker::send2City(BuildingPtr base, bool roadOnly )
{
  if( !base.isValid() )
  {
    Logger::warning( "!!! WARNING: trainee walker base is null" );
    deleteLater();
    return;
  }

  _d->baseLocation = base->pos();
  _computeWalkerPath( roadOnly );

  if( !isDeleted() && !gfx::tilemap::isValidLocation( _d->destLocation ) )
  {
    BuildingPtr dest = receiver();
    dest->reserveTrainee( type() );
    attach();
  }
}

void TraineeWalker::_reachedPathway()
{
  Walker::_reachedPathway();
  deleteLater();

  BuildingPtr dest = ptr_cast<Building>( _city()->getOverlay( _d->destLocation ) );
  if( dest.isValid() )
  {
    dest->updateTrainee( this );
  }
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, baseLocation );
  VARIANT_SAVE_ANY_D( stream, _d, destLocation );

  VARIANT_SAVE_ANY_D( stream, _d, maxDistance )
  stream[ "traineeType" ] = type();
  stream[ "type" ] = (int)walker::trainee;
}

void TraineeWalker::load( const VariantMap& stream )
{
  Walker::load(stream);

  VARIANT_LOAD_ANY_D( _d, baseLocation, stream )
  VARIANT_LOAD_ANY_D( _d, destLocation, stream )
  VARIANT_LOAD_ANY_D( _d, maxDistance, stream )
  walker::Type wtype = (walker::Type)stream.get( "traineeType" ).toInt();

  _setType( wtype );
  _init( wtype );
}

TilePos TraineeWalker::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->baseLocation;
  case plDestination: return _d->destLocation;
  default: break;
  }

  return Human::places( type );
}


TraineeWalker::~TraineeWalker(){}

TraineeWalkerPtr TraineeWalker::create(PlayerCityPtr city, walker::Type traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( city, traineeType ) );
  ret->drop();
  return ret;
}

WalkerPtr TraineeWalkerCreator::create(PlayerCityPtr city)
{
  return TraineeWalker::create( city, walker::trainee ).object();
}
