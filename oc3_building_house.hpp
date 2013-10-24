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

#ifndef __OPENCAESAR3_HOUSE_H_INCLUDED__
#define __OPENCAESAR3_HOUSE_H_INCLUDED__

#include "oc3_building.hpp"
#include "core/scopedptr.hpp"
#include "oc3_citizen_group.hpp"

class HouseLevelSpec;
class GoodStore;

class House : public Building
{
  friend class HouseLevelSpec;
public:
  enum { smallHovel=1, bigTent, smallHut, bigHut } Level;

  House( const int houseId=smallHovel );

  virtual void timeStep(const unsigned long time);

  virtual GoodStore& getGoodStore();

  // return the current house level
  const HouseLevelSpec& getLevelSpec() const;

  virtual void applyService(ServiceWalkerPtr walker);
  virtual float evaluateService(ServiceWalkerPtr walker);
  virtual void buyMarket(ServiceWalkerPtr walker);

  void appendServiceValue( Service::Type srvc, const int value );
  bool hasServiceAccess( Service::Type service );
  int getServiceValue( Service::Type service );
  void setServiceValue( Service::Type service, const int access );

  int getFoodLevel() const;
  int getHealthLevel() const;
  int getWorkersCount() const;

  const BuildingData::Desirability& getDesirabilityInfo() const;

  void levelUp();
  void levelDown();

  virtual void destroy();

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  int getMaxHabitants();
  void addHabitants( CitizenGroup& habitants );
  const CitizenGroup& getHabitants() const;

  int collectTaxes();

  std::string getUpCondition() const;

  int getMaxDistance2Road() const; 

  bool isWalkable() const;

private:

  void _update();
  void _tryUpdate_1_to_11_lvl( int level, int startSmallPic, int startBigPic, const char desirability );
  void _tryDegrage_11_to_2_lvl( int smallPic, int bigPic, const char desirability );

  class Impl;
  ScopedPtr< Impl > _d;
};

//operator need for std::set
inline bool operator<(HousePtr v1, HousePtr v2)
{
  return v1.object() < v2.object();
}

#endif //__OPENCAESAR3_HOUSE_H_INCLUDED__
