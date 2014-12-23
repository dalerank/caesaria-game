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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_RIOTER_H_INCLUDE_
#define __CAESARIA_RIOTER_H_INCLUDE_

#include "human.hpp"
#include "objects/constants.hpp"

class Rioter : public Human
{
public:
  static RioterPtr create( PlayerCityPtr city );
  virtual ~Rioter();

  virtual void timeStep(const unsigned long time);
  void send2City( BuildingPtr bld );

  virtual bool die();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual int agressive() const;
  virtual void excludeAttack( constants::objects::Group group );

protected:
  Rioter( PlayerCityPtr city );
  virtual void _reachedPathway();
  virtual void _updateThoughts();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

class NativeRioter : public Rioter
{
public:
  static RioterPtr create( PlayerCityPtr city );

private:
  NativeRioter( PlayerCityPtr city );
};

#endif//__CAESARIA_RIOTER_H_INCLUDE_
