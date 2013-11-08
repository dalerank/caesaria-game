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

#ifndef __OPENCAESAR3_PROTESTOR_H_INCLUDE_
#define __OPENCAESAR3_PROTESTOR_H_INCLUDE_

#include "walker.hpp"

class Protestor;
typedef SmartPtr<Protestor> RioterPtr;

class Protestor : public Walker
{
public:
  static RioterPtr create( CityPtr city );
  virtual ~Protestor();

  virtual void onNewTile();
  virtual void onDestination();
  virtual void timeStep(const unsigned long time);
  void send2City( HousePtr house );

  virtual void die();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

private:
  Protestor( CityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif//__OPENCAESAR3_PROTESTOR_H_INCLUDE_
