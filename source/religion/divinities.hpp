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

#ifndef __CAESARIA_DIVINITIES_H_INCLUDED__
#define __CAESARIA_DIVINITIES_H_INCLUDED__

#include "romedivinity.hpp"
#include "gfx/picture.hpp"
#include "game/service.hpp"

namespace religion
{

class RomeDivinityBase : public RomeDivinity
{
public:
  void load( const VariantMap& vm );

  void assignFestival( int type );

  virtual VariantMap save() const;

  virtual std::string getName() const { return _name; }
  virtual std::string getShortDescription() const { return _shortDesc; }
  virtual Service::Type getServiceType() const { return _service; }
  virtual const Picture& getPicture() const { return _pic; }
  virtual float getRelation() const { return _relation; }
  virtual float getDefaultDecrease() const { return 2.f; }
  virtual DateTime getLastFestivalDate() const { return _lastFestival; }

  virtual void updateRelation( float income, PlayerCityPtr city );

  virtual std::string getMoodDescription() const;

  RomeDivinityBase();

  virtual void setInternalName(const std::string &newName);

protected:
  std::string _name;
  Service::Type _service;
  std::string _shortDesc;
  DateTime _lastFestival;
  DateTime _lastActionDate;
  float _relation;
  Picture _pic;
  StringArray _moodDescr;
};

class RomeDivinityCeres : public RomeDivinityBase
{
public:
  static RomeDivinityPtr create();
  virtual void updateRelation(float income, PlayerCityPtr city);
};

class RomeDivinityNeptune : public RomeDivinityBase
{
public:
  static RomeDivinityPtr create();
  virtual void updateRelation(float income, PlayerCityPtr city);
};

class RomeDivinityMercury : public RomeDivinityBase
{
public:
  static RomeDivinityPtr create();
  virtual void updateRelation(float income, PlayerCityPtr city);
};

}//end namespace religion

#endif //__CAESARIA_DIVINITIES_H_INCLUDED__
