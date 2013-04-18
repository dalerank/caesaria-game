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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com



#include "oc3_walker.hpp"

#include <iostream>

#include "oc3_building_data.hpp"
#include "oc3_exception.hpp"
#include "oc3_scenario.hpp"
#include "oc3_walker_market_buyer.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_positioni.hpp"


std::map<WalkerType, Walker*> Walker::_mapWalkerByID;  // key=walkerType, value=instance

Walker::Walker()
{
   _action._action = WA_MOVE;
   _action._direction = D_NONE;
   _walkerType = WT_NONE;
   _walkerGraphic = WG_NONE;

   _animation = NULL;
   _animIndex = 0;
   _speed = 1;  // default speed
   _isDeleted = false;

   _midTileI = 7;
   _midTileJ = 7;
   _remainMoveI = 0;
   _remainMoveJ = 0;
};

Walker::~Walker()
{
  
}

WalkerType Walker::getType() const
{
   return _walkerType;
}

Walker* Walker::getInstance(const WalkerType walkerType)
{
   if (_mapWalkerByID.empty())
   {
      // first call to this method
      _mapWalkerByID[WT_SERVICE] = new ServiceWalker(S_MAX);  // dummy serviceType
      _mapWalkerByID[WT_MARKET_BUYER] = new MarketBuyer();
      _mapWalkerByID[WT_CART_PUSHER] = new CartPusher();
      //_mapWalkerByID[WT_IMMIGRANT] = new Immigrant();
      _mapWalkerByID[WT_TRAINEE] = new TraineeWalker(WTT_NONE);
   }

   std::map<WalkerType, Walker*>::iterator mapIt;
   mapIt = _mapWalkerByID.find(walkerType);
   Walker *res;

   if (mapIt == _mapWalkerByID.end())
   {
      // THROW("Unknown walker type:" << walkerType);
      res = NULL;
   }
   else
   {
      res = mapIt->second;
   }
   return res;
}


void Walker::timeStep(const unsigned long time)
{
   switch (_action._action)
   {
   case WA_MOVE:
      walk();
      break;

   default:
      break;
   }
}


bool Walker::isDeleted() const
{
   return _isDeleted;
}

void Walker::setIJ(const int i, const int j)
{
   _i = i;
   _j = j;

   _si = _midTileI;
   _sj = _midTileJ;

   _ii = 15*i + _si;
   _jj = 15*j + _sj;
}

int Walker::getI() const
{
   return _i;
}

int Walker::getJ() const
{
   return _j;
}

int Walker::getII() const
{
   return _ii;
}

int Walker::getJJ() const
{
   return _jj;
}

void Walker::setPathWay(PathWay &pathWay)
{
   _pathWay = pathWay;
   _pathWay.begin();

   onMidTile();
}

void Walker::setDestinationIJ(const int i, const int j)
{
   // Propagator pathPropagator;
   // pathPropagator.init();
   // Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   // TerrainTile &currentTerrain = tilemap.at(_i, _j).get_terrain();
   // Road* currentRoad = dynamic_cast<Road*> (currentTerrain.getOverlay());
   // TerrainTile &destTerrain = tilemap.at(i, j).get_terrain();
   // Road* destRoad = dynamic_cast<Road*> (destTerrain.getOverlay());
   // pathPropagator.getPath(*currentRoad, *destRoad, _pathWay);
   // _pathWay.begin();

   // onMidTile();
}

void Walker::setSpeed(const int speed)
{
   _speed = speed;
}

WalkerGraphicType Walker::getWalkerGraphic() const
{
   return _walkerGraphic;
}


// ioSI: subtile index, ioI: tile index, ioAmount: distance, iMidPos: subtile offset 0, oNewTile: true if tile change, oMidTile: true if on tile center
void Walker::inc(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile)
{
   int delta = ioAmount;
   if ((ioSI<iMidPos) && (ioSI+delta>=iMidPos))  // midpos is ahead and inside the current movement
   {
      // we will stop at the mid tile!
      delta = iMidPos - ioSI;
      oMidTile = true;
   }

   if (ioSI+delta>15)  // the start of next tile is inside the current movement
   {
      // we will stop at the beginning of the new tile!
      delta = 16 - ioSI;
      oNewTile = true;
      ioI += 1;  // next tile
      ioSI = ioSI - 15;
   }

   ioAmount -= delta;
   ioSI += delta;
}

// ioSI: subtile index, ioI: tile index, ioAmount: distance, iMidPos: subtile offset 0, oNewTile: true if tile change, oMidTile: true if on tile center
void Walker::dec(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile)
{
   int delta = ioAmount;
   if ((ioSI>iMidPos) && (ioSI-delta<=iMidPos))  // midpos is ahead and inside the current movement
   {
      // we will stop at the mid tile!
      delta = ioSI - iMidPos;
      oMidTile = true;
   }

   if (ioSI-delta<0)  // the start of next tile is inside the current movement
   {
      // we will stop at the beginning of the new tile!
      delta = ioSI+1;
      oNewTile = true;
      ioI -= 1;  // next tile
      ioSI = ioSI + 15;
   }

   ioAmount -= delta;
   ioSI -= delta;
}


void Walker::walk()
{
   if (D_NONE == _action._direction )
   {
      // nothing to do
      return;
   }
   _animIndex = (_animIndex+1) % 12;

   switch (_action._direction)
   {
   case D_NORTH:
   case D_SOUTH:
      _remainMoveJ += _speed;
      break;
   case D_EAST:
   case D_WEST:
      _remainMoveI += _speed;
      break;
   case D_NORTH_EAST:
   case D_SOUTH_WEST:
   case D_SOUTH_EAST:
   case D_NORTH_WEST:
      _remainMoveI += _speed*0.7f;
      _remainMoveJ += _speed*0.7f;
      break;
   default:
      THROW("Invalid move direction: " << _action._direction);
      break;
   }
   

   bool newTile = false;
   bool midTile = false;
   int amountI = int(_remainMoveI);
   int amountJ = int(_remainMoveJ);
   _remainMoveI -= amountI;
   _remainMoveJ -= amountJ;

   // std::cout << "walker step, amount :" << amount << std::endl;
   while (amountI+amountJ > 0)
   {
      switch (_action._direction)
      {
      case D_NORTH:
         inc(_sj, _j, amountJ, _midTileJ, newTile, midTile);
         break;
      case D_NORTH_EAST:
         inc(_sj, _j, amountJ, _midTileJ, newTile, midTile);
         inc(_si, _i, amountI, _midTileI, newTile, midTile);
         break;
      case D_EAST:
         inc(_si, _i, amountI, _midTileI, newTile, midTile);
         break;
      case D_SOUTH_EAST:
         dec(_sj, _j, amountJ, _midTileJ, newTile, midTile);
         inc(_si, _i, amountI, _midTileI, newTile, midTile);
         break;
      case D_SOUTH:
         dec(_sj, _j, amountJ, _midTileJ, newTile, midTile);
         break;
      case D_SOUTH_WEST:
         dec(_sj, _j, amountJ, _midTileJ, newTile, midTile);
         dec(_si, _i, amountI, _midTileI, newTile, midTile);
         break;
      case D_WEST:
         dec(_si, _i, amountI, _midTileI, newTile, midTile);
         break;
      case D_NORTH_WEST:
         inc(_sj, _j, amountJ, _midTileJ, newTile, midTile);
         dec(_si, _i, amountI, _midTileI, newTile, midTile);
         break;
      default:
         THROW("Invalid move direction: " << _action._direction);
         break;
      }

      if (newTile)
      {
         // walker is now on a new tile!
         onNewTile();
      }

      if (midTile)
      {
         // walker is now on the middle of the tile!
         onMidTile();
      }

      // if (midTile) std::cout << "walker mid tile" << std::endl;
      // if (newTile) std::cout << "walker new tile" << std::endl;
      // if (amount != 0) std::cout << "walker remaining step :" << amount << std::endl;
   }

   _ii = _i*15+_si;
   _jj = _j*15+_sj;
}


void Walker::onNewTile()
{
   // std::cout << "Walker is on a new tile! coord=" << _i << "," << _j << std::endl;
   Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
   Tile& currentTile = tilemap.at(_i, _j);
   if( !currentTile.get_terrain().isRoad() )
   {
      std::cout << "Walker at " << _i << "," << _j << " is not on a road!!!" << std::endl;
   }
}


void Walker::onMidTile()
{
   // std::cout << "Walker is on mid tile! coord=" << _i << "," << _j << std::endl;
   if (_pathWay.isDestination())
   {
      onDestination();
   }
   else
   {
      // compute the direction to reach the destination
      computeDirection();
   }
}


void Walker::onDestination()
{
   // std::cout << "Walker arrived at destination! coord=" << _i << "," << _j << std::endl;
   _action._action=WA_NONE;  // stop moving
   _animation=NULL;
}

void Walker::onNewDirection()
{
   _animation=NULL;  // need to fetch the new animation
}


void Walker::computeDirection()
{
   DirectionType lastDirection = _action._direction;
   _action._direction = _pathWay.getNextDirection();

   if (lastDirection != _action._direction)
   {
      onNewDirection();
   }
}


DirectionType Walker::getDirection()
{
   return _action._direction;
}

void Walker::getPictureList(std::vector<Picture*> &oPics)
{
   oPics.clear();
   oPics.push_back(&getMainPicture());
}

Picture& Walker::getMainPicture()
{
   if (_animation == NULL)
   {
      const std::map<WalkerAction, Animation>& animMap = WalkerLoader::instance().getAnimationMap(getWalkerGraphic());
      std::map<WalkerAction, Animation>::const_iterator itAnimMap;
      if (_action._action == WA_NONE || _action._direction == D_NONE)
      {
         WalkerAction action;
         action._action = WA_MOVE;       // default action
         if (_action._direction == D_NONE)
         {
            action._direction = D_NORTH;  // default direction
         }
         else
         {
            action._direction = _action._direction;  // last direction of the walker
         }
         _animIndex = 0;  // first animation frame
         itAnimMap = animMap.find(action);
      }
      else
      {
         itAnimMap = animMap.find(_action);
      }

      _animation = &(itAnimMap->second);
   }

   return *_animation->getPictures()[_animIndex];
}

void Walker::serialize(OutputSerialStream &stream)
{
   stream.write_objectID(this);
   stream.write_int((int) _walkerType, 1, 0, WT_MAX);
   _pathWay.serialize(stream);
   stream.write_int((int) _action._action, 1, 0, WA_MAX);
   stream.write_int((int) _action._direction, 1, 0, D_MAX);
   stream.write_int(_i, 2, 0, 1000);
   stream.write_int(_j, 2, 0, 1000);
   stream.write_int(_si, 1, 0, 50);
   stream.write_int(_sj, 1, 0, 50);
   stream.write_int(_ii, 4, 0, 1000000);
   stream.write_int(_jj, 4, 0, 1000000);
   stream.write_int(_speed, 1, 0, 50);
   stream.write_int(_midTileI, 1, 0, 50);
   stream.write_int(_midTileJ, 1, 0, 50);
   stream.write_int(_animIndex, 1, 0, 50);
}

Walker& Walker::unserialize_all(InputSerialStream &stream)
{
   int objectID = stream.read_objectID();
   WalkerType walkerType = (WalkerType) stream.read_int(1, 0, WT_MAX);
   Walker *instance = getInstance(walkerType);
   Walker *res = instance->clone();
   res->unserialize(stream);
   stream.link(objectID, res);
   return *res;
}

void Walker::unserialize(InputSerialStream &stream)
{
   _pathWay.unserialize(stream);
   _action._action = (WalkerActionType) stream.read_int(1, 0, WA_MAX);
   _action._direction = (DirectionType) stream.read_int(1, 0, D_MAX);
   _i = stream.read_int(2, 0, 1000);
   _j = stream.read_int(2, 0, 1000);
   _si = stream.read_int(1, 0, 50);
   _sj = stream.read_int(1, 0, 50);
   _ii = stream.read_int(4, 0, 1000000);
   _jj = stream.read_int(4, 0, 1000000);
   _speed = stream.read_int(1, 0, 50);
   _midTileI = stream.read_int(1, 0, 50);
   _midTileJ = stream.read_int(1, 0, 50);
   _animIndex = stream.read_int(1, 0, 50);
}

TilePos Walker::getIJ() const
{
    return TilePos( _i, _j );
}

class Immigrant::Impl
{
public:
    Point destination;
};

Immigrant::Immigrant() : _d( new Impl )
{
   _walkerType = WT_IMMIGRANT;
   _walkerGraphic = WG_HOMELESS;
}

Immigrant* Immigrant::clone() const
{
   Immigrant* ret = new Immigrant();
   ret->_d->destination = _d->destination;
   return ret;
}

void Immigrant::assignPath( const Building& home )
{
    City& city = Scenario::instance().getCity();
    Tile& exitTile = city.getTilemap().at( city.getRoadExitI(), city.getRoadExitJ() );

    Road* exitRoad = dynamic_cast< Road* >( exitTile.get_terrain().getOverlay() );
    if( exitRoad )
    {
        Propagator pathfinder;
	    PathWay pathWay;
        pathfinder.init( const_cast< Building& >( home ) );
        bool findPath = pathfinder.getPath( *exitRoad, pathWay );
	    if( findPath )
	    {
		    setPathWay( pathWay );
		    setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());   
	    }
    }
    else
        _isDeleted = true;
}

void Immigrant::onDestination()
{  
    _isDeleted = true;
}

Immigrant* Immigrant::create( const Building& startPoint )
{
    Immigrant* newImmigrant = new Immigrant();
    newImmigrant->assignPath( startPoint );
    return newImmigrant;
}

Immigrant::~Immigrant()
{

}

Soldier::Soldier()
{
   _walkerType = WT_SOLDIER;
   _walkerGraphic = WG_HORSEMAN;
}

Soldier* Soldier::clone() const
{
   return new Soldier(*this);
}

ServiceWalker::ServiceWalker(const ServiceType service)
{
   _walkerType = WT_SERVICE;
   _building = NULL;
   _maxDistance = 5;  // TODO: _building.getMaxDistance() ?

   init(service);
}

void ServiceWalker::init(const ServiceType service)
{
  _service = service;

  switch (_service)
  {
  case S_WELL:
  case S_FOUNTAIN:
  case S_SENATE:
  case S_TEMPLE_ORACLE:
    _walkerGraphic = WG_NONE;
    break;
  case S_ENGINEER:
    _walkerGraphic = WG_ENGINEER;
    break;
  case S_PREFECT:
    _walkerGraphic = WG_PREFECT;
    break;
  case S_TEMPLE_NEPTUNE:
  case S_TEMPLE_CERES:
  case S_TEMPLE_VENUS:
  case S_TEMPLE_MARS:
  case S_TEMPLE_MERCURE:
    _walkerGraphic = WG_PRIEST;
    break;
  case S_DOCTOR:
  case S_HOSPITAL:
    _walkerGraphic = WG_DOCTOR;
    break;
  case S_BARBER:
    _walkerGraphic = WG_BARBER;
    break;
  case S_BATHS:
    _walkerGraphic = WG_BATH;
    break;
  case S_SCHOOL:
    _walkerGraphic = WG_CHILD;
    break;
  case S_LIBRARY:
  case S_COLLEGE:
    _walkerGraphic = WG_LIBRARIAN;
    break;
  case S_THEATER:
  case S_AMPHITHEATER:
  case S_HIPPODROME:
  case S_COLLOSSEUM:
    _walkerGraphic = WG_ACTOR;
    break;
  case S_MARKET:
    _walkerGraphic = WG_CHILD;
    break;
  case S_FORUM:
    _walkerGraphic = WG_TAX;
    break;
  case S_MAX:
    break;
  }
}

ServiceWalker* ServiceWalker::clone() const
{
   return new ServiceWalker(*this);
}

void ServiceWalker::setServiceBuilding(ServiceBuilding &building)
{
   _building = &building;
}

ServiceBuilding &ServiceWalker::getServiceBuilding()
{
   if (_building == NULL) THROW("ServiceBuilding is not initialized");
   return *_building;
}

ServiceType ServiceWalker::getService()
{
   return _service;
}

void ServiceWalker::computeWalkerPath()
{
   std::list<PathWay> pathWayList;

   Propagator pathPropagator;
   pathPropagator.init(*_building);
   pathPropagator.getAllPaths(_maxDistance, pathWayList);

   float maxPathValue = 0.0;
   PathWay* bestPath = NULL;
   for (std::list<PathWay>::iterator itPath = pathWayList.begin(); itPath != pathWayList.end(); ++itPath)
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
      _isDeleted = true;
      return;
   }

   reservePath(*bestPath);
   setPathWay(*bestPath);
   setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());
   Scenario::instance().getCity().getWalkerList().push_back(this);
}

std::set<Building*> ServiceWalker::getReachedBuildings(const TilePos& pos )
{
   std::set<Building*> res;

   int reachDistance = 2;
   TilePos start = pos - TilePos( reachDistance, reachDistance );
   TilePos stop = pos + TilePos( reachDistance, reachDistance );
   std::list<Tile*> reachedTiles = Scenario::instance().getCity().getTilemap().getFilledRectangle( start, stop );
   for (std::list<Tile*>::iterator itTile = reachedTiles.begin(); itTile != reachedTiles.end(); ++itTile)
   {
      TerrainTile& terrain = (*itTile)->get_terrain();
      if( terrain.isBuilding() )
      {
         Building* building = dynamic_cast<Building*>( terrain.getOverlay() );
         if( building )
         {
            res.insert(building);
         }
      }
   }

   return res;
}

float ServiceWalker::evaluatePath(PathWay &pathWay)
{
   // evaluate all buildings along the path
   std::set<Building*> doneBuildings;  // list of evaluated building: don't do them again
   std::list<Tile*>& pathTileList = pathWay.getAllTiles();

   int distance = 0;
   float res = 0.0;
   for (std::list<Tile*>::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
   {
      std::set<Building*> reachedBuildings = getReachedBuildings( (*itTile)->getIJ() );
      for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
      {
         std::pair<std::set<Building*>::iterator, bool> rc = doneBuildings.insert( *itBuilding );
         if (rc.second == true)
         {
            // the building has not been evaluated yet
            res += (*itBuilding)->evaluateService(*this);
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
   std::set<Building*> doneBuildings;  // list of evaluated building: don't do them again
   std::list<Tile*>& pathTileList = pathWay.getAllTiles();

   for (std::list<Tile*>::iterator itTile = pathTileList.begin(); itTile != pathTileList.end(); ++itTile)
   {
      std::set<Building*> reachedBuildings = getReachedBuildings( (*itTile)->getIJ() );
      for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
      {
         std::pair<std::set<Building*>::iterator, bool> rc = doneBuildings.insert( *itBuilding );
         if (rc.second == true)
         {
            // the building has not been reserved yet
            (*itBuilding)->reserveService(_service);
         }
      }
   }
}

void ServiceWalker::start()
{
   computeWalkerPath();
}

void ServiceWalker::onNewTile()
{
   Walker::onNewTile();

   std::set<Building*> reachedBuildings = getReachedBuildings( getIJ() );
   for (std::set<Building*>::iterator itBuilding = reachedBuildings.begin(); itBuilding != reachedBuildings.end(); ++itBuilding)
   {
      (*itBuilding)->applyService(*this);
   }
}

void ServiceWalker::onDestination()
{
   Walker::onDestination();
   if (_pathWay.isReverse())
   {
      // walker is back in the market
      _isDeleted= true;
   }
   else
   {
      // walker finished service => get back to service building
      _pathWay.rbegin();
      _action._action=WA_MOVE;
      computeDirection();
   }
}

void ServiceWalker::serialize(OutputSerialStream &stream)
{
   Walker::serialize(stream);
   stream.write_int((int) _service, 1, 0, S_MAX);
   stream.write_objectID(_building);
   stream.write_int(_maxDistance, 2, 0, 65535);
}

void ServiceWalker::unserialize(InputSerialStream &stream)
{
   Walker::unserialize(stream);
   _service = (ServiceType) stream.read_int(1, 0, S_MAX);
   init(_service);

   stream.read_objectID((void**)&_building);
   _maxDistance = stream.read_int(2, 0, 65535);
}

void ServiceWalker::setMaxDistance( const int distance )
{
    _maxDistance = distance;
}

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

TraineeWalker* TraineeWalker::clone() const
{
   return new TraineeWalker(*this);
}

void TraineeWalker::setOriginBuilding(Building &originBuilding)
{
   _originBuilding = &originBuilding;
}

WalkerTraineeType TraineeWalker::getType() const
{
   return _traineeType;
}

void TraineeWalker::computeWalkerPath()
{
   _maxNeed = 0;  // need of this trainee in buildings
   Propagator pathPropagator;
   pathPropagator.init(*_originBuilding);
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
      pathPropagator.getPath(*_destinationBuilding, pathWay);
      setPathWay(pathWay);
      setIJ(_pathWay.getOrigin().getI(), _pathWay.getOrigin().getJ());
      Scenario::instance().getCity().getWalkerList().push_back(this);
      _destinationBuilding->reserveTrainee(_traineeType);
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
   std::map<Building*, PathWay> pathWayList;
   pathPropagator.getReachedBuildings(buildingType, pathWayList);

   for (std::map<Building*, PathWay>::iterator pathWayIt= pathWayList.begin(); pathWayIt != pathWayList.end(); ++pathWayIt)
   {
      // for every building within range
      Building& building = *(pathWayIt->first);

      float need = building.evaluateTrainee(_traineeType);
      if (need > _maxNeed)
      {
         _maxNeed = need;
         _destinationBuilding = &building;
      }
   }
}


void TraineeWalker::start()
{
   computeWalkerPath();
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
   stream.write_objectID(_originBuilding);
   stream.write_objectID(_destinationBuilding);
   stream.write_int(_maxDistance, 2, 0, 65535);
}

void TraineeWalker::unserialize(InputSerialStream &stream)
{
   Walker::unserialize(stream);
   _traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
   init(_traineeType);

   stream.read_objectID((void**)&_originBuilding);
   stream.read_objectID((void**)&_destinationBuilding);
   _maxDistance = stream.read_int(2, 0, 65535);
}

