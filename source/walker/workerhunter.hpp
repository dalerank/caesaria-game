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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_WORKERSHUNTER_H_INCLUDE_
#define __CAESARIA_WORKERSHUNTER_H_INCLUDE_

#include "serviceman.hpp"
#include "city/industry.hpp"

class Recruter;
typedef SmartPtr<Recruter> RecruterPtr;

class Recruter : public ServiceWalker
{
public:
  static RecruterPtr create( PlayerCityPtr city );

  int needWorkers() const;

  void hireWorkers( const int workers );  
  void setPriority( const city::HirePriorities& priority );
  virtual void send2City( WorkingBuildingPtr building, const int workersNeeded );
  virtual void send2City( BuildingPtr base, int orders=goLowerService );
  void once(WorkingBuildingPtr building, const unsigned int workersNeed, unsigned int distance);
  void timeStep(const unsigned long time);

  virtual TilePos places(Place type) const;
  virtual unsigned int reachDistance() const;
  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual bool die();

protected:
  virtual void _centerTile();

private:
  Recruter( PlayerCityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif//__CAESARIA_WORKERSHUNTER_H_INCLUDE_
