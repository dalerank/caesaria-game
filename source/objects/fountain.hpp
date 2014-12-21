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

#ifndef __CAESARIA_FOUNTAIN_INCLUDED__
#define __CAESARIA_FOUNTAIN_INCLUDED__

#include "core/position.hpp"
#include "service.hpp"

class Fountain : public ServiceBuilding
{
public:
  Fountain();

  virtual bool build(const CityAreaInfo &info);
  virtual bool canBuild( const CityAreaInfo& areaInfo ) const;
  virtual void deliverService();
  virtual void timeStep(const unsigned long time);
  virtual bool isNeedRoadAccess() const;
  virtual bool haveReservoirAccess() const;
  virtual void destroy();
  virtual bool mayWork() const;
  unsigned int fillRange() const;

  virtual void load( const VariantMap& stream);
  virtual void save(VariantMap &stream) const;
private:
  class Impl;
  ScopedPtr<Impl> _d;

  void _initAnimation();
};

#endif //__CAESARIA_FOUNTAIN_INCLUDED__
