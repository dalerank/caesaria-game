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
#include "game/path_finding.hpp"
#include "game/city.hpp"
#include "game/name_generator.hpp"
#include "building/constants.hpp"
#include "game/pathway_helper.hpp"

using namespace constants;

class TraineeWalker::Impl
{
public:
  BuildingPtr base;
  BuildingPtr destination;
};

TraineeWalker::TraineeWalker(PlayerCityPtr city, walker::Type traineeType)
  : Walker( city ), _d( new Impl )
{
  _setType( traineeType );
  _maxDistance = 30;

  _init( traineeType );
}

void TraineeWalker::_init(walker::Type traineeType)
{
  switch( traineeType )
  {
  case walker::actor:
    _setAnimation( gfx::actor );
    _buildingNeed.push_back(building::theater);
    _buildingNeed.push_back(building::amphitheater);
  break;

  case walker::gladiator:
    _setAnimation( gfx::gladiator );
    _buildingNeed.push_back(building::amphitheater);
    _buildingNeed.push_back(building::colloseum);
  break;

  case walker::tamer:
    _setAnimation( gfx::tamer );
    _buildingNeed.push_back(building::colloseum);
  break;

  case walker::charioter:
    _setAnimation( gfx::unknown );  // TODO
  break;

  case walker::soldier:
    _setAnimation( gfx::soldier );
    _buildingNeed.push_back( building::militaryAcademy );
    _buildingNeed.push_back( building::fortLegionaire );
  break;

  default:
  case walker::unknown:
  case walker::all:
    break;
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void TraineeWalker::_cancelPath()
{
  if( _d->destination.isValid() )
  {
    _d->destination->cancelTrainee( getType() );
  }
}

void TraineeWalker::setBase(Building &originBuilding)
{
  _d->destination = &originBuilding;
}

void TraineeWalker::_computeWalkerPath( bool roadOnly )
{
  _maxNeed = 0;  // need of this trainee in buildings
 
  Pathway finalPath;
  if( roadOnly )
  {
    Propagator pathPropagator( _getCity() );
    pathPropagator.init( _d->base.as<Construction>() );
    pathPropagator.setAllDirections( false );
    pathPropagator.propagate( _maxDistance );

    foreach( TileOverlay::Type buildingType, _buildingNeed )
    {
      checkDestination(buildingType, pathPropagator);
    }

    if( _d->destination.isValid() )
    {
      pathPropagator.getPath( _d->destination.as<Construction>(), finalPath );
    }
  }
  else
  {
    CityHelper helper( _getCity() );

    BuildingList buildings;
    foreach( TileOverlay::Type buildingType, _buildingNeed )
    {
      BuildingList tmpBuildings = helper.find<Building>( buildingType );
      buildings.insert( buildings.end(), tmpBuildings.begin(), tmpBuildings.end() );
    }

    _maxNeed = 0;
    Propagator::DirectRoute droute;
    TilePos startPos = _d->base->getTilePos();
    foreach( BuildingPtr bld, buildings )
    {
      Pathway way = PathwayHelper::create( startPos, bld.as<Construction>(), PathwayHelper::allTerrain );
      float curNeed = bld->evaluateTrainee( getType() );
      if( way.isValid() && _maxNeed < curNeed && way.getLength() < _maxDistance )
      {
        _maxNeed = curNeed;
        droute = std::make_pair( bld.as<Construction>(), way );
      }
    }

    if( droute.first.isValid() )
    {
      finalPath = droute.second;
      _d->destination = droute.first.as<Building>();
    }
  }

  if( finalPath.isValid() )
  {
    // some building needs that trainee!!!
    setIJ( finalPath.getOrigin().getIJ() );
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
  Propagator::Routes pathWayList = pathPropagator.getRoutes( buildingType );

  foreach( Propagator::Routes::value_type& item, pathWayList )
  {
    // for every building within range
    BuildingPtr building = item.first.as<Building>();

    float need = building->evaluateTrainee( getType() );
    if (need > _maxNeed)
    {
      _maxNeed = need;
      _d->destination = building;
    }
  }
}

void TraineeWalker::send2City( BuildingPtr base, bool roadOnly)
{
  _d->base = base;
  _computeWalkerPath( roadOnly );

  if( !isDeleted() && _d->destination.isValid() )
  {
    _d->destination->reserveTrainee( getType() );
    _getCity()->addWalker( this );
  }
}

void TraineeWalker::_reachedPathway()
{
  Walker::_reachedPathway();
  deleteLater();
  _d->destination->applyTrainee( getType() );
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "originBldPos" ] = _d->base->getTilePos();
  stream[ "destBldPos" ] = _d->destination->getTilePos();
  stream[ "maxDistance" ] = _maxDistance;
  stream[ "traineeType" ] = getType();
  stream[ "type" ] = (int)walker::trainee;
}

void TraineeWalker::load( const VariantMap& stream )
{
  Walker::load(stream);

  _init( getType() );

  CityHelper helper( _getCity() );
  _d->base = helper.find<Building>( building::any, stream.get( "originBldPos" ).toTilePos() );
  _d->destination = helper.find<Building>( building::any, stream.get( "destBldPos" ).toTilePos() );
  _maxDistance = (int)stream.get( "maxDistance" );
  walker::Type wtype = (walker::Type)stream.get( "traineeType" ).toInt();

  _setType( wtype );
  _init( wtype );
}

TraineeWalkerPtr TraineeWalker::create(PlayerCityPtr city, walker::Type traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( city, traineeType ) );
  ret->drop();
  return ret;
}
