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


#ifndef WALKER_HPP
#define WALKER_HPP

#include <string>
#include <memory>

#include "building.hpp"
#include "service_building.hpp"
#include "tilemap.hpp"
#include "picture.hpp"
#include "enums.hpp"
#include "pic_loader.hpp"
#include "path_finding.hpp"
#include "serializer.hpp"


class Walker : public Serializable
{
public:
   Walker();
   virtual ~Walker();

   virtual void timeStep(const unsigned long time);  // performs one simulation step
   WalkerType getType() const;
   static Walker* getInstance(const WalkerType walkerType);  // get an instance of the given type
   virtual Walker* clone() const = 0;

   // position and movement
   int getI() const;
   TilePos getIJ() const;
   int getJ() const;
   int getII() const;
   int getJJ() const;
   void setIJ(const int i, const int j);
   void setPathWay(PathWay &pathWay);
   void setDestinationIJ(const int i, const int j);
   void setSpeed(const int speed);
   virtual void onNewTile();  // called when the walker is on a new tile
   virtual void onMidTile();  // called when the walker is on the middle of a tile
   virtual void onDestination();  // called when the walker is at his destination
   virtual void onNewDirection(); // called when the walker changes direction
   void computeDirection();
   void walk();

   DirectionType getDirection();

   void serialize(OutputSerialStream &stream);
   static Walker& unserialize_all(InputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

   // graphic
   WalkerGraphicType getWalkerGraphic() const;
   virtual void getPictureList(std::vector<Picture*> &oPics);
   Picture& getMainPicture();

   // state
   bool isDeleted() const;  // returns true if the walker should be forgotten

protected:
   WalkerType _walkerType;
   WalkerGraphicType _walkerGraphic;
   bool _isDeleted;
   PathWay _pathWay;
   WalkerAction _action;

private:
   /* useful method for subtile movement computation
      si   = subtile coordinate in the current tile
      i    = tile coordinate
      amount = amount of the increase, returns remaining movement if any
      midPos = position of the midtile (so that walkers are not all exactly on the middle of the tile)
      newTile = return true if we are now on a new tile
      midTile = return true if we got above the treshold
    */
   void inc(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile);
   void dec(int &ioSI, int &ioI, int &ioAmount, const int iMidPos, bool &oNewTile, bool &oMidTile);

private:
   int _i, _j;   // tile coordinate
   int _si, _sj; // subtile coordinate in the current tile: 0..15
   int _ii, _jj; // subtile coordinate across all tiles: 0..15*mapsize (ii=15*i+si)
   int _speed;
   int _midTileI, _midTileJ;  // subtile coordinate in the current tile, at starting position

   const Animation *_animation;  // current animation
   int _animIndex;  // current frame in the animation

   static std::map<WalkerType, Walker*> _mapWalkerByID;  // key=walkerType, value=instance
};


/** This is an immigrant coming with his stuff */
class Immigrant : public Walker
{
public:
    virtual Immigrant* clone() const;

    static Immigrant* create( const Building& startPoint );
    
    void onDestination();
    ~Immigrant();
private:
    Immigrant();
 
    void assignPath( const Building& home );

    class Impl;
    std::auto_ptr< Impl > _d;
};

/** Soldier, friend or foo */
class Soldier : public Walker
{
public:
   Soldier();
   virtual Soldier* clone() const;

private:

};

/** This walker gives a service to buildings along the road */
class ServiceWalker : public Walker
{
public:
   ServiceWalker(const ServiceType service);
   virtual ServiceWalker* clone() const;
   void init(const ServiceType service);

   ServiceType getService();
   void setServiceBuilding(ServiceBuilding &building);
   ServiceBuilding &getServiceBuilding();

   void computeWalkerPath();
   void start();
   virtual void onDestination();
   virtual void onNewTile();  // called when the walker is on a new tile

   // evaluates the service demand on the given pathWay
   float evaluatePath(PathWay &pathWay);
   void reservePath(PathWay &pathWay);
   std::set<Building*> getReachedBuildings(const TilePos& pos );

   void setMaxDistance( const int distance );

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   ServiceType _service;
   ServiceBuilding *_building;
   int _maxDistance;
};


/** This walker goes to work */
class TraineeWalker : public Walker
{
public:
   TraineeWalker(const WalkerTraineeType traineeType);
   virtual TraineeWalker* clone() const;
   void init(const WalkerTraineeType traineeType);
   WalkerTraineeType getType() const;

   void checkDestination(const BuildingType buildingType, Propagator &pathPropagator);
   void start();
   void setOriginBuilding(Building &building);
   void computeWalkerPath();

   virtual void onDestination();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   WalkerTraineeType _traineeType;
   Building *_originBuilding;
   Building *_destinationBuilding;
   int _maxDistance;

   std::list<BuildingType> _buildingNeed;  // list of buildings needing this trainee
   float _maxNeed;  // evaluates the need for that trainee
};


#endif
