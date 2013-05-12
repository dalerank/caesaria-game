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

#include "oc3_traineewalker.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"
#include "oc3_variant.hpp"

TraineeWalker::TraineeWalker(const WalkerTraineeType traineeType)
{
  _walkerType = WT_TRAINEE;
  _originBuilding = NULL;
  _destinationBuilding = NULL;
  _maxDistance = 30;

  init(traineeType);
}

void TraineeWalker::init(const WalkerTraineeType traineeType)
{
  _traineeType = traineeType;

  switch (_traineeType)
  {
  case WTT_ACTOR:
    _walkerGraphic = WG_ACTOR;
    _buildingNeed.push_back(B_THEATER);
    _buildingNeed.push_back(B_AMPHITHEATER);
    break;
  case WTT_GLADIATOR:
    _walkerGraphic = WG_GLADIATOR;
    _buildingNeed.push_back(B_AMPHITHEATER);
    _buildingNeed.push_back(B_COLLOSSEUM);
    break;
  case WTT_TAMER:
    _walkerGraphic = WG_TAMER;
    _buildingNeed.push_back(B_COLLOSSEUM);
    break;
  case WTT_CHARIOT:
    _walkerGraphic = WG_NONE;  // TODO
    break;
  case WTT_NONE:
  case WTT_MAX:
    break;
  }
}

void TraineeWalker::setOriginBuilding(Building &originBuilding)
{
  _originBuilding = &originBuilding;
}

int TraineeWalker::getType() const
{
  return _traineeType;
}

void TraineeWalker::computeWalkerPath()
{
  _maxNeed = 0;  // need of this trainee in buildings
  Propagator pathPropagator;
  pathPropagator.init( *_originBuilding.object() );
  pathPropagator.propagate(_maxDistance);

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
    setIJ( _pathWay.getOrigin().getIJ() );
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
  Propagator::ReachedBuldings pathWayList;
  pathPropagator.getReachedBuildings(buildingType, pathWayList);

  for( Propagator::ReachedBuldings::iterator pathWayIt= pathWayList.begin(); 
    pathWayIt != pathWayList.end(); ++pathWayIt)
  {
    // for every building within range
    BuildingPtr building = pathWayIt->first;

    float need = building->evaluateTrainee(_traineeType);
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
    _destinationBuilding->reserveTrainee(_traineeType);
    Scenario::instance().getCity().addWalker( WalkerPtr( this ) );
  }
}

void TraineeWalker::onDestination()
{
  Walker::onDestination();
  _isDeleted= true;
  _destinationBuilding->applyTrainee(_traineeType);
}

void TraineeWalker::save( VariantMap& stream ) const
{
  Walker::save( stream );
  stream[ "traineeType" ] = (int)_traineeType;
  stream[ "originBldI" ] = _originBuilding->getTile().getI();
  stream[ "originBldJ" ] = _originBuilding->getTile().getJ();
  stream[ "destBldI" ] = _destinationBuilding->getTile().getI();
  stream[ "destBldJ" ] = _destinationBuilding->getTile().getJ();
  stream[ "maxDistance" ] = _maxDistance;
}

void TraineeWalker::load( const VariantMap& stream )
{
//   Walker::unserialize(stream);
//   _traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
//   init(_traineeType);
// 
//   stream.read_objectID( (void**)_originBuilding.object() );
//   stream.read_objectID( (void**)_destinationBuilding.object() );
// 
//   _maxDistance = stream.read_int(2, 0, 65535);
}

TraineeWalkerPtr TraineeWalker::create( const WalkerTraineeType traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( traineeType ) );
  ret->drop();
  return ret;
}