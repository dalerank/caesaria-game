#include "oc3_traineewalker.hpp"
#include "oc3_scenario.hpp"

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

  if (_destinationBuilding != NULL)
  {
    // some building needs that trainee!
    // std::cout << "trainee sent!" << std::endl;
    PathWay pathWay;
    pathPropagator.getPath( _destinationBuilding, pathWay);
    setPathWay(pathWay);
    setIJ( _pathWay.getOrigin().getIJ() );
  }
  else
  {
    // nobody needs him...
    // std::cout << "trainee suicide!" << std::endl;
    _isDeleted = true;
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

  _destinationBuilding->reserveTrainee(_traineeType);
  Scenario::instance().getCity().addWalker( WalkerPtr( this ) );
}

void TraineeWalker::onDestination()
{
  Walker::onDestination();
  _isDeleted= true;
  _destinationBuilding->applyTrainee(_traineeType);
}

void TraineeWalker::serialize(OutputSerialStream &stream)
{
  Walker::serialize(stream);
  stream.write_int((int) _traineeType, 1, 0, WTT_MAX);
  stream.write_objectID( _originBuilding.object() );
  stream.write_objectID( _destinationBuilding.object() );
  stream.write_int(_maxDistance, 2, 0, 65535);
}

void TraineeWalker::unserialize(InputSerialStream &stream)
{
  Walker::unserialize(stream);
  _traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
  init(_traineeType);

  stream.read_objectID( (void**)_originBuilding.object() );
  stream.read_objectID( (void**)_destinationBuilding.object() );

  _maxDistance = stream.read_int(2, 0, 65535);
}

TraineeWalkerPtr TraineeWalker::create( const WalkerTraineeType traineeType )
{
  TraineeWalkerPtr ret( new TraineeWalker( traineeType ) );
  ret->drop();
  return ret;
}