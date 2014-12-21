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

#ifndef __CAESARIA_HEALTHBUILDINGS_H_INCLUDED__
#define __CAESARIA_HEALTHBUILDINGS_H_INCLUDED__

#include "service.hpp"

class Doctor : public ServiceBuilding
{
public:
  Doctor();

  virtual unsigned int walkerDistance() const;
  virtual void deliverService();
};

class Hospital : public ServiceBuilding
{
public:
  Hospital();
};

class Baths : public ServiceBuilding
{
public:
  Baths();

  virtual void timeStep(const unsigned long time);
  virtual void deliverService();
  virtual unsigned int walkerDistance() const;
  virtual bool build(const CityAreaInfo &info);
  virtual bool mayWork() const;
protected:
  bool _haveReservorWater;
};

class Barber : public ServiceBuilding
{
public:
  Barber();

  virtual void deliverService();
  virtual unsigned int walkerDistance() const;
};

#endif //__CAESARIA_HEALTHBUILDINGS_H_INCLUDED__
