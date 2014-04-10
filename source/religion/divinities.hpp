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

namespace rome
{

class RomeDivinity : public Divinity
{
public:
  void load( const VariantMap& vm );

  void assignFestival( int type );

  virtual VariantMap save() const;

  virtual std::string name() const { return _name; }
  virtual std::string shortDescription() const { return _shortDesc; }
  virtual Service::Type serviceType() const { return _service; }
  virtual const gfx::Picture& picture() const { return _pic; }
  virtual float relation() const { return _relation; }
  virtual float getDefaultDecrease() const { return 2.f; }
  virtual DateTime lastFestivalDate() const { return _lastFestival; }

  virtual void updateRelation( float income, PlayerCityPtr city );

  virtual std::string moodDescription() const;

  RomeDivinity();

  virtual void setInternalName(const std::string &newName);
  virtual std::string internalName() const;

protected:
  std::string _name;
  Service::Type _service;
  std::string _shortDesc;
  DateTime _lastFestival;
  DateTime _lastActionDate;
  float _relation;
  gfx::Picture _pic;
  StringArray _moodDescr;
};

class Mercury : public RomeDivinity
{
public:
  static DivinityPtr create();
  virtual void updateRelation(float income, PlayerCityPtr city);
};

}//end namespace rome

}//end namespace religion

#endif //__CAESARIA_DIVINITIES_H_INCLUDED__
