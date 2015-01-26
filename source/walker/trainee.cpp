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
#include "city/helper.hpp"
#include "name_generator.hpp"
#include "objects/constants.hpp"
#include "core/priorities.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "pathway/pathway_helper.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_TRAINEEMAN_IN_WALKERFACTORY(walker::trainee, 0, trainee)

typedef Priorities<TileOverlay::Type> NecessaryBuildings;

class TraineeWalker::Impl
{
public:
  NecessaryBuildings necBuildings;  // list of buildings needing this trainee
  BuildingPtr base;
  BuildingPtr destination;
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
  case walker::actor:      _d->necBuildings << objects::theater
                                           << objects::amphitheater;  break;
  case walker::gladiator:  _d->necBuildings << objects::amphitheater
                                              << objects::colloseum;  break;
  case walker::lionTamer:  _d->necBuildings << objects::colloseum;  break;
  case walker::soldier:    _d->necBuildings << objects::military_academy
                                            << objects::fort_legionaries
                                            << objects::fort_horse
                                            << objects::fort_javelin
                                            << objects::tower;  break;
  case walker::charioteer:  _d->necBuildings << objects::hippodrome;  break;
  default: break;
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void TraineeWalker::_cancelPath()
{
  if( _d->destination.isValid() )
  {
    _d->destination->cancelTrainee( type() );
  }
}

void TraineeWalker::setBase(Building &originBuilding){  _d->destination = &originBuilding;}

void TraineeWalker::_computeWalkerPath( bool roadOnly )
{
  if( _d->base.isNull() )
  {
    deleteLater();
    return;
  }

  _d->maxNeed = 0;  // need of this trainee in buildings
 
  Pathway finalPath;
  city::Helper helper( _city() );

  BuildingList buildings;
  foreach( buildingType, _d->necBuildings )
  {
    BuildingList tmpBuildings = helper.find<Building>( *buildingType );
    buildings.insert( buildings.end(), tmpBuildings.begin(), tmpBuildings.end() );
  }

  TilesArray startArea = roadOnly ? _d->base->getAccessRoads() : _d->base->enterArea();

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
    Logger::warning( "Not need trainee walker from [%d,%d]", _d->base->pos().i(), _d->base->pos().j() );
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
    _d->destination = ptr_cast<Building>( droute.first );
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

void TraineeWalker::checkDestination(const TileOverlay::Type buildingType, Propagator &pathPropagator)
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
      _d->destination = building;
    }
  }
}

int TraineeWalker::value() const{ return 100; }

void TraineeWalker::send2City(BuildingPtr base, bool roadOnly )
{
  _d->base = base;
  _computeWalkerPath( roadOnly );

  if( !isDeleted() && _d->destination.isValid() )
  {
    _d->destination->reserveTrainee( type() );
    _city()->addWalker( this );
  }
}

void TraineeWalker::_reachedPathway()
{
  Walker::_reachedPathway();
  deleteLater();

  if( _d->destination.isValid() )
  {
    _d->destination->updateTrainee( this );
  }
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "originBldPos" ] = _d->base->pos();
  stream[ "destBldPos" ] = _d->destination->pos();
  stream[ "maxDistance" ] = _d->maxDistance;
  stream[ "traineeType" ] = type();
  stream[ "type" ] = (int)walker::trainee;
}

void TraineeWalker::load( const VariantMap& stream )
{
  Walker::load(stream);

  _d->base << _city()->getOverlay( stream.get( "originBldPos" ).toTilePos() );
  _d->destination << _city()->getOverlay( stream.get( "destBldPos" ).toTilePos() );
  _d->maxDistance = (int)stream.get( "maxDistance" );
  walker::Type wtype = (walker::Type)stream.get( "traineeType" ).toInt();

  _setType( wtype );
  _init( wtype );
}

TilePos TraineeWalker::places(Walker::Place type) const
{
  switch( type )
  {
  case plOrigin: return _d->base.isValid() ? _d->base->pos() : TilePos( -1, -1 );
  case plDestination: return _d->destination.isValid() ? _d->destination->pos() : TilePos( -1, -1 );
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
