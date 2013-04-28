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


#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <string>
#include <map>
#include <list>
#include <set>

#include "oc3_tilemap.hpp"
#include "oc3_enums.hpp"
#include "oc3_good.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_animation.hpp"

class Widget;
class GuiInfoBox;

class LandOverlay : public Serializable
{
public:
   LandOverlay( const BuildingType type, const Size& size=Size(1));
   virtual ~LandOverlay();

   virtual void timeStep(const unsigned long time);  // perform one simulation step

   Tile& getTile() const;  // master tile, in case of multi-tile area
   int getSize() const;  // size in tiles (1=1x1, 2=2x2, ...)
   bool isDeleted() const;  // returns true if the overlay should be forgotten
   void deleteLater();
   virtual bool isWalkable() const;
   virtual LandOverlay* clone() const = 0;
   virtual void setTerrain( TerrainTile &terrain ) = 0;
   virtual void build( const TilePos& pos );
   virtual void destroy();  // handles the walkers

   // graphic
   void setPicture(Picture &picture);
   Picture& getPicture();
   std::vector<Picture*>& getForegroundPictures();

   // make a new info box (or NULL). The object will need to be deleted.
   virtual GuiInfoBox* makeInfoBox();
   std::string getName();  // title of the info box

   BuildingType getType() const;
   void setType(const BuildingType buildingType);

   void serialize(OutputSerialStream &stream);
   static LandOverlay& unserialize_all(InputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

protected:
   std::vector<Picture*> _fgPictures;
   Picture* _picture;
   Tile* _master_tile;  // left-most tile if multi-tile, or "this" if single-tile
   int _size;  // size in tiles
   bool _isDeleted;
   std::string _name;

   Animation _animation;  // basic animation (if any)
   int _animIndex; // current frame in the animation

   BuildingType _buildingType;
   //static std::map<BuildingType, LandOverlay*> _mapBuildingByID;  // key=buildingType, value=instance
};


class Construction : public LandOverlay
{
public:
   Construction( const BuildingType type, const Size& size);

   virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
   virtual void build( const TilePos& pos );
   virtual void burn();
   virtual void collapse();
   const PtrTilesList& getAccessRoads() const;  // return all road tiles adjacent to the construction
   virtual void computeAccessRoads();  
   virtual unsigned char getMaxDistance2Road() const; // virtual because HOUSE has different behavior
   virtual char getDesirabilityInfluence() const;

protected:
   std::list<Tile*> _accessRoads;
};

class Garden : public Construction
{
public:
  Garden();
  Garden* clone() const;
  void setTerrain(TerrainTile &terrain);  
  bool isWalkable() const;
};

class Road : public Construction
{
public:
  Road();
  Road* clone() const;
  
  virtual Picture& computePicture();

  void build(const TilePos& pos );
  void setTerrain(TerrainTile &terrain);
  bool canBuild(const TilePos& pos ) const;
  bool isWalkable() const;
};

class Plaza : public Road
{
public:
  Plaza();
  virtual Plaza* clone() const;
  virtual void setTerrain(TerrainTile &terrain);  
  virtual bool canBuild(const TilePos& pos ) const;
  virtual Picture& computePicture();
};

class ServiceWalker;
class Building : public Construction
{
public:
   Building(const BuildingType type, const Size& size=Size(1) );
   virtual void setTerrain(TerrainTile &terrain);

   virtual void timeStep(const unsigned long time);
   virtual void storeGoods(GoodStock &stock, const int amount = -1);
   // evaluate the given service
   virtual float evaluateService(ServiceWalker &walker);
   // handle service reservation
   void reserveService(const ServiceType service);
   void cancelService(const ServiceType service);
   virtual void applyService(ServiceWalker &walker);
   // evaluate the need for the given trainee
   virtual float evaluateTrainee(const WalkerTraineeType traineeType);  // returns >0 if trainee is needed
   void reserveTrainee(const WalkerTraineeType traineeType); // trainee will come
   void cancelTrainee(const WalkerTraineeType traineeType);  // trainee will not come
   void applyTrainee(const WalkerTraineeType traineeType); // trainee arrives

   float getDamageLevel();
   void setDamageLevel(const float value);
   float getFireLevel();
   void setFireLevel(const float value);

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

protected:
   float _damageLevel;  // >100 => building is destroyed
   float _fireLevel;    // >100 => building catch fire
   float _damageIncrement;
   float _fireIncrement;
   std::set<ServiceType> _reservedServices;  // a serviceWalker is on the way
   std::map<WalkerTraineeType, int> _traineeMap;  // current level of trainees working in the building (0..200)
   std::set<WalkerTraineeType> _reservedTrainees;  // a trainee is on the way
};


/** Building where people work */
class WorkingBuilding : public Building
{
public:
   WorkingBuilding(const BuildingType type, const Size& size);

   void setMaxWorkers(const int maxWorkers);
   int getMaxWorkers() const;

   void setWorkers(const int currentWorkers);
   void addWorkers( const int workers );

   int getWorkers() const;

   void setActive(const bool value);  // if false then this building is stopped
   bool isActive();

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   int _currentWorkers;
   int _maxWorkers;
   bool _isActive;
};


class Granary: public WorkingBuilding
{
public:
   Granary();
   Granary* clone() const;

   void timeStep(const unsigned long time);
   void computePictures();
   SimpleGoodStore& getGoodStore();
   virtual GuiInfoBox* makeInfoBox( Widget* parent );

   void serialize(OutputSerialStream &stream);
   void unserialize(InputSerialStream &stream);

private:
   SimpleGoodStore _goodStore;
};

class GovernorsHouse  : public WorkingBuilding
{
public:
  GovernorsHouse();
  GovernorsHouse* clone() const;
  
//  void serialize(OutputSerialStream &stream);
//  void unserialize(InputSerialStream &stream);
};

class GovernorsVilla  : public WorkingBuilding
{
public:
  GovernorsVilla();
  GovernorsVilla* clone() const;
  
//  void serialize(OutputSerialStream &stream);
//  void unserialize(InputSerialStream &stream);
};

class GovernorsPalace : public WorkingBuilding
{
public:
  GovernorsPalace();
  GovernorsPalace* clone() const;
  
//  void serialize(OutputSerialStream &stream);
//  void unserialize(InputSerialStream &stream);
};


class SmallStatue : public Building
{
public:
  SmallStatue();
  SmallStatue* clone() const;
  
};

class MediumStatue : public Building
{
public:
  MediumStatue();
  MediumStatue* clone() const;
  
};

class BigStatue : public Building
{
public:
  BigStatue();
  BigStatue* clone() const;
  
};

class Academy : public WorkingBuilding
{
public:
  Academy();
  Academy* clone() const;
};

class Barracks : public WorkingBuilding
{
public:
  Barracks();
  Barracks* clone() const;
};

class Shipyard : public Building
{
public:
  Shipyard();
  Shipyard* clone() const;
};

class TriumphalArch : public Building
{
public:
  TriumphalArch();
  TriumphalArch* clone() const;
};

class MissionPost : public WorkingBuilding
{
public:
  MissionPost();
  MissionPost* clone() const;
};

class Dock : public Building
{
public:
  Dock();
  Dock* clone() const;
  void timeStep(const unsigned long time);
};

class NativeBuilding : public Building
{
public:
  NativeBuilding( const BuildingType type, const Size& size );
  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);
  virtual GuiInfoBox* makeInfoBox( Widget* parent );
};

class NativeHut : public NativeBuilding
{
public:
  NativeHut();
  NativeHut* clone() const;
  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);
  //virtual GuiInfoBox* makeInfoBox();  
};

class NativeField  : public NativeBuilding
{
public:
  NativeField();
  NativeField* clone() const;  
  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);
  //virtual GuiInfoBox* makeInfoBox();
};

class NativeCenter : public NativeBuilding
{
public:
  NativeCenter();
  NativeCenter* clone() const;  
  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);
  //virtual GuiInfoBox* makeInfoBox();
};

class FortLegionnaire : public Building
{
public:  
  FortLegionnaire();
  FortLegionnaire* clone() const;
};

class FortJaveline : public Building
{
public:  
  FortJaveline();
  FortJaveline* clone() const;
};

class FortMounted : public Building
{
public:  
  FortMounted();
  FortMounted* clone() const;
};


#endif
