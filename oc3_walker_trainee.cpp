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

#include "oc3_walker_trainee.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"
#include "oc3_path_finding.hpp"
#include "oc3_city.hpp"
#include "oc3_name_generator.hpp"

TraineeWalker::TraineeWalker(const WalkerType traineeType)
{
  _setType( traineeType );
  _originBuilding = NULL;
  _destinationBuilding = NULL;
  _maxDistance = 30;

  init( traineeType );
}

void TraineeWalker::init(const WalkerType traineeType)
{
  switch (traineeType)
  {
  case WT_ACTOR:
    _setGraphic( WG_ACTOR );
    _buildingNeed.push_back(B_THEATER);
    _buildingNeed.push_back(B_AMPHITHEATER);
  break;

  case WT_GLADIATOR:
    _setGraphic( WG_GLADIATOR );
    _buildingNeed.push_back(B_AMPHITHEATER);
    _buildingNeed.push_back(B_COLLOSSEUM);
  break;

  case WT_TAMER:
    _setGraphic( WG_TAMER );
    _buildingNeed.push_back(B_COLLOSSEUM);
  break;

  case WT_CHARIOT:
    _setGraphic( WG_NONE );  // TODO
  break;

  default:
  case WT_NONE:
  case WT_MAX:
    break;
  }

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void TraineeWalker::setOriginBuilding(Building &originBuilding)
{
  _originBuilding = &originBuilding;
}

void TraineeWalker::computeWalkerPath()
{
  _maxNeed = 0;  // need of this trainee in buildings
 
  Propagator pathPropagator( _city );
  pathPropagator.init( _originBuilding.as<Construction>() );
  pathPropagator.propagate( _maxDistance );

  for (std::list<BuildingType>::iterator itType = _buildingNeed.begin(); itType != _buildingNeed.end(); ++itType)
  {
    BuildingType buildingType = *itType;
    checkDestination(buildingType, pathPropagator);
  }

  if( _destinationBuilding != NULL )
  {
    // some building needs that trainee!
    // std::cout << "trainee sent!" << std::endl;
    PathWay pathWay;
    pathPropagator.getPath( _destinationBuilding, pathWay);
    setPathWay( pathWay );
    setIJ( _getPathway().getOrigin().getIJ() );
  }
  else
  {
    // nobody needs him...
    // std::cout << "trainee suicide!" << std::endl;
    deleteLater();
  }
}


void TraineeWalker::checkDestination(const BuildingType buildingType, Propagator &pathPropagator)
{
  Propagator::Routes pathWayList;
  pathPropagator.getRoutes(buildingType, pathWayList);

  for( Propagator::Routes::iterator pathWayIt= pathWayList.begin(); 
    pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every building within range
    BuildingPtr building = pathWayIt->first;

    float need = building->evaluateTrainee( (WalkerType)getType() );
    if (need > _maxNeed)
    {
      _maxNeed = need;
      _destinationBuilding = building;
    }
  }
}


void TraineeWalker::send2City()
{
  computeWalkerPath();

  if( !isDeleted() )
  {
    _destinationBuilding->reserveTrainee( (WalkerType)getType() );
    _city->addWalker( WalkerPtr( this ) );
  }
}

void TraineeWalker::onDestination()
{
  Walker::onDestination();
  deleteLater();
  _destinationBuilding->applyTrainee( (WalkerType)getType() );
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "originBldPos" ] = _originBuilding->getTile().getIJ();
  stream[ "destBldPos" ] = _destinationBuilding->getTile().getIJ();
  stream[ "maxDistance" ] = _maxDistance;
}

void TraineeWalker::load( const VariantMap& stream )
{
  Walker::load(stream);

  init( (WalkerType)getType() );

  CityHelper helper( _city );
  _originBuilding = helper.getBuilding<Building>( stream.get( "originBldPos" ).toTilePos() );
  _destinationBuilding = helper.getBuilding<Building>( stream.get( "destBldPos" ).toTilePos() );
  _maxDistance = (int)stream.get( "maxDistance" );
}

TraineeWalkerPtr TraineeWalker::create(CityPtr city, const WalkerType traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( traineeType ) );
  ret->_city = city;
  ret->drop();
  return ret;
}
