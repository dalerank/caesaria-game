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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_HOUSE_H_INCLUDED__
#define __CAESARIA_HOUSE_H_INCLUDED__

#include "objects/building.hpp"
#include "core/scopedptr.hpp"
#include "game/citizen_group.hpp"
#include "good/good.hpp"

class HouseSpecification;

class HouseLevel
{
public:
  typedef enum { vacantLot=0,
                 hovel=1, tent,
                 shack, hut, //4
                 domus, bigDomus, //6
                 mansion, bigMansion, //8
                 insula, middleInsula,   //10
                 bigInsula, beatyfullInsula, //12
                 smallVilla,  middleVilla,  bigVilla,  greatVilla,
                 smallPalace, middlePalace, bigPalace, greatPalace,
                 count } ID;

  typedef enum { maxSize2=2, maxSize3, maxSize4 } HouseMaxSize;
};

class House : public Building
{
  friend class HouseSpecification;
public:
  enum { food=Construction::paramCount, health, happiness, happinessBuff, healthBuff, settleLock };

  House( HouseLevel::ID level=HouseLevel::vacantLot );

  virtual void timeStep(const unsigned long time);

  virtual good::Store& goodStore();

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
  virtual bool build( const CityAreaInfo& info );

  virtual double state( ParameterType param) const;

  unsigned int workersCount() const;

  bool isEducationNeed( Service::Type type ) const;
  bool isEntertainmentNeed( Service::Type type ) const;
  bool isHealthNeed( Service::Type type ) const;

  Desirability desirability() const;

  virtual void destroy();

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  virtual void collapse();
  virtual void burn();

  unsigned int maxHabitants();
  void addHabitants( CitizenGroup& habitants );
  CitizenGroup remHabitants( int paramCount );
  const CitizenGroup& habitants() const;

  float collectTaxes();
  float taxesThisYear() const;
  void appendMoney( float money );
  DateTime lastTaxationDate() const;

  std::string evolveInfo() const;
  std::string levelName() const;

  virtual int roadAccessDistance() const;

  virtual bool isWalkable() const;
  virtual bool isFlat() const;

  virtual std::string sound() const;
  virtual std::string troubleDesc() const;

  bool isCheckedDesirability() const;

  void __debugChangeLevel( int change );

private:
  void _levelUp();
  void _levelDown();
  void _disaster();
  void _update(bool needChangeTexture);
  bool _tryEvolve_1_to_12_lvl(int level, int growSize, const char desirability );
  bool _tryEvolve_12_to_20_lvl(int level4grow, int minSize, const char desirability);
  void _tryDegrage_12_to_2_lvl( const char desirability );
  void _tryDegrade_20_to_12_lvl(int size, const char desirability);
  void _makeOldHabitants();
  void _updateHabitants(const CitizenGroup& group);
  void _checkEvolve();
  void _checkPatricianDeals();
  void _updateTax();
  void _updateCrime();
  void _checkHomeless();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAEARIA_HOUSE_H_INCLUDED__
