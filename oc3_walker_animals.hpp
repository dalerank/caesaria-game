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

#ifndef __OPENCAESAR3_ANIMAL_H_INCLUDED__
#define __OPENCAESAR3_ANIMAL_H_INCLUDED__

#include "oc3_walker.hpp"
#include "core/predefinitions.hpp"

/** This is an immigrant coming with his stuff */
class Animal : public Walker
{
public:
  Animal( CityPtr city );
  ~Animal();

  virtual void send2City(const TilePos& start );

  void save(VariantMap& stream) const;
  void load(const VariantMap& stream);

protected:
  void _findNewWay(const TilePos &start);

  class Impl;
  ScopedPtr< Impl > _d;
};

class Sheep : public Animal
{
public:
  static WalkerPtr create( CityPtr city );

  virtual void send2City(const TilePos& start);
  virtual void onDestination();
  virtual void onNewTile();

private:
  Sheep( CityPtr city );
};

#endif //__OPENCAESAR3_ANIMAL_H_INCLUDED__
