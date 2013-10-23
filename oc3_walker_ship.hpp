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

#ifndef _OPENCAESAR_SHIP_INCLUDE_H_
#define _OPENCAESAR_SHIP_INCLUDE_H_

#include "oc3_walker.hpp"
#include "oc3_predefinitions.hpp"

class Ship : public Walker
{
public:
  Ship( CityPtr city );
  ~Ship();
};

class FishingBoat : public Ship
{
public:
  static FishingBoatPtr create( CityPtr city );

  void send2City( WharfPtr base, const TilePos& start);

  virtual void onDestination();
  virtual void onNewTile();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual void timeStep(const unsigned long time);
  void startCatch();

  bool isBusy() const;

private:
  FishingBoat( CityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif
