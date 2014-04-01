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

#ifndef __CAESARIA_ANIMAL_H_INCLUDED__
#define __CAESARIA_ANIMAL_H_INCLUDED__

#include "walker.hpp"
#include "core/predefinitions.hpp"

/** This is an immigrant coming with his stuff */
class Animal : public Walker
{
public:
  Animal( PlayerCityPtr city );
  ~Animal();

  virtual void send2City(const TilePos& start );

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

protected:
  void _findNewWay(const TilePos &start);

  class Impl;
  ScopedPtr< Impl > _d;
};

class Sheep : public Animal
{
public:
  static WalkerPtr create( PlayerCityPtr city );

  virtual void send2City(const TilePos& start);
  virtual void die();

protected:
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);

private:
  Sheep( PlayerCityPtr city );
};

#endif //__CAESARIA_ANIMAL_H_INCLUDED__
