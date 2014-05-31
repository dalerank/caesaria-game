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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_HOUSE_H_INCLUDED__
#define __CAESARIA_HOUSE_H_INCLUDED__

#include "objects/building.hpp"
#include "core/scopedptr.hpp"
#include "game/citizen_group.hpp"

class HouseSpecification;
class GoodStore;

class HouseLevel
{
public:
  typedef enum { smallHovel=1, bigTent,
                 smallHut, bigHut,
                 smallDomus, bigDomus,
                 smallMansion, bigMansion, //8
                 smallInsula, middleInsula, //10
                 bigInsula, beatyfullInsula, //12
                 smallVilla,  middleVilla,  bigVilla,  greatVilla,
                 smallPalace, middlePalace, bigPalace, greatPalace } ID;
  typedef enum { smallHovelSmlPic=1, smallHovelBigPic=5,
                 bigTentSmlPic=3, bigTentBigPic=6,
                 smallHutSmlPic=7, smallHutBigPic=11,
                 bigHutSmlPic=9, bigHutBigPic=12,
                 smallDomusSmlPic=13, smallDomusBigPic=17,
                 bigDomusSmlPic=15, bigDomusBigPic=18,
                 smallMansionSmlPic=19, smallMansionBigPic=23,
                 bigMansionSmlPic=21, bigMansionBigPic=24,
                 smallInsulaSmlPic=25, smallInsulaBigPic=29,
                 middleInsulaSmlPic=27, middleInsulaBigPic=30,
                 bigInsulaPic=31, beatyfullInsulaPic=33,
                 smallVillaPic=35, middleVillaPic=36,
                 bigVillaPic=37, greatVillaPic=38,
                 smallPalacePic=39, middlePalacePic=41,
                 bigPalacePic=43, greatPalacePic=44 } PicIndex;
};

class House : public Building
{
  friend class HouseSpecification;
public:
  enum { food=Construction::count, health, unemployed, morale };

  House( const int houseId=HouseLevel::smallHovel );

  virtual void timeStep(const unsigned long time);

  virtual GoodStore& goodStore();

  // return the current house level
  const HouseSpecification& spec() const;

  virtual void applyService(ServiceWalkerPtr walker);
  virtual float evaluateService(ServiceWalkerPtr walker);
  virtual void buyMarket(ServiceWalkerPtr walker);

  virtual void appendServiceValue(Service::Type srvc, float value );
  virtual bool hasServiceAccess( Service::Type service );
  virtual float getServiceValue( Service::Type service );
  virtual void setServiceValue(Service::Type service, float value );
  virtual gfx::TilesArray enterArea() const;

  virtual double getState( ParameterType param) const;

  int workersCount() const;

  bool isEducationNeed( Service::Type type ) const;
  bool isEntertainmentNeed( Service::Type type ) const;

  Desirability getDesirability() const;

  virtual void destroy();

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  int maxHabitants();
  void addHabitants( CitizenGroup& habitants );
  CitizenGroup remHabitants( int count );
  const CitizenGroup& habitants() const;

  float collectTaxes();
  DateTime getLastTaxation() const;

  std::string getEvolveInfo() const;

  virtual int getRoadAccessDistance() const;

  virtual bool isWalkable() const;
  virtual bool isFlat() const;

  virtual std::string sound() const;
  virtual std::string troubleDesc() const;

  bool isCheckedDesirability() const;

private:

  void _levelUp();
  void _levelDown();

  void _update();
  void _tryEvolve_1_to_11_lvl( int level, int startSmallPic, int startBigPic, const char desirability );
  void _tryEvolve_12_to_20_lvl(int level4grow, int startPic, int minSize, const char desirability);
  void _tryDegrage_11_to_2_lvl( int smallPic, int bigPic, const char desirability );
  void _tryDegrade_20_to_12_lvl(int startPicId, int size, const char desirability);
  void _makeOldHabitants();
  void _updateHabitants(const CitizenGroup& group);
  void _checkEvolve();
  void _updateTax();
  void _updateMorale();
  void _checkHomeless();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAEARIA_HOUSE_H_INCLUDED__
