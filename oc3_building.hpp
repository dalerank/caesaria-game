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

//#include "oc3_tilemap.hpp"
#include "oc3_tileoverlay.hpp"
#include "oc3_enums.hpp"
#include "oc3_good.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_animation.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_predefinitions.hpp"
#include "oc3_service.hpp"
#include "oc3_building_data.hpp"

class Construction : public TileOverlay
{
public:
  Construction( const TileOverlayType type, const Size& size );

  virtual bool canBuild( CityPtr city, const TilePos& pos ) const;  // returns true if it can be built there
  virtual std::string getError() const;
  virtual void build( CityPtr city, const TilePos& pos );
  virtual void burn();
  virtual void collapse();
  virtual bool isNeedRoadAccess() const;
  virtual const TilemapTiles& getAccessRoads() const;  // return all road tiles adjacent to the construction
  virtual void computeAccessRoads();  
  virtual int  getMaxDistance2Road() const; // virtual because HOUSE has different behavior
  virtual const BuildingData::Desirability& getDesirabilityInfo() const;
  virtual void destroy();

protected:
  TilemapTiles _accessRoads;
};

class Building : public Construction
{
public:
   Building(const TileOverlayType type, const Size& size=Size(1) );
   virtual void initTerrain(Tile& terrain);

   virtual void timeStep(const unsigned long time);
   virtual void storeGoods(GoodStock &stock, const int amount = -1);
   // evaluate the given service
   virtual float evaluateService(ServiceWalkerPtr walker);
   // handle service reservation
   void reserveService(const Service::Type service);
   void cancelService(const Service::Type service);
   virtual void applyService( ServiceWalkerPtr walker);
   // evaluate the need for the given trainee
   virtual float evaluateTrainee(const WalkerType traineeType);  // returns >0 if trainee is needed
   void reserveTrainee(const WalkerType traineeType); // trainee will come
   void cancelTrainee(const WalkerType traineeType);  // trainee will not come
   void applyTrainee(const WalkerType traineeType); // trainee arrives

   float getDamageLevel();
   void  setDamageLevel(const float value);
   float getFireLevel();
   void  setFireLevel(const float value);

   void save( VariantMap& stream) const;
   void load( const VariantMap& stream);

protected:
   float _damageLevel;  // >100 => building is destroyed
   float _fireLevel;    // >100 => building catch fire
   float _damageIncrement;
   float _fireIncrement;
   typedef std::map<WalkerType, int> TraineeMap;
   std::set<Service::Type> _reservedServices;  // a serviceWalker is on the way
   TraineeMap _traineeMap;  // current level of trainees working in the building (0..200)
   std::set<WalkerType> _reservedTrainees;  // a trainee is on the way
};

//operator need for std::reset
inline bool operator<(BuildingPtr v1, BuildingPtr v2)
{
  return v1.object() < v2.object();
}

class SmallStatue : public Building
{
public:
  SmallStatue();
  bool isNeedRoadAccess() const;
};

class MediumStatue : public Building
{
public:
  MediumStatue();
  bool isNeedRoadAccess() const;
};

class BigStatue : public Building
{
public:
  BigStatue();
  bool isNeedRoadAccess() const;
};

class Shipyard : public Building
{
public:
  Shipyard();
};

class TriumphalArch : public Building
{
public:
  TriumphalArch();
};

class Dock : public Building
{
public:
  Dock();
  void timeStep(const unsigned long time);
};

#endif
