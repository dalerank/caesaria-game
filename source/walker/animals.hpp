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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_ANIMAL_H_INCLUDED__
#define __CAESARIA_ANIMAL_H_INCLUDED__

#include "walker.hpp"
#include "core/predefinitions.hpp"

class Fish : public Walker
{
public:
  Fish( PlayerCityPtr city );
  virtual ~Fish();
};

class Animal : public Walker
{
public:
  Animal( PlayerCityPtr city );
  virtual ~Animal();

  virtual void send2City(const TilePos& start );

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  virtual std::string thoughts( Thought th) const;

protected:
  virtual void _findNewWay(const TilePos &start);

  class Impl;
  ScopedPtr< Impl > _d;
};

class Herbivorous : public Animal
{
public:
  virtual void send2City(const TilePos& start);

protected:
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual void _noWay();

  Herbivorous( constants::walker::Type type, PlayerCityPtr city );

private:
  int _noWayCount;
};

class Sheep : public Herbivorous
{
public:
  static WalkerPtr create( PlayerCityPtr city );

private:
  Sheep( PlayerCityPtr city );
};

class Zebra : public Herbivorous
{
public:
  static WalkerPtr create( PlayerCityPtr city );

private:
  Zebra( PlayerCityPtr city );
};

class Wolf : public Animal
{
public:
  static WalkerPtr create( PlayerCityPtr city );

  virtual void send2City(const TilePos& start);
  virtual bool die();
  virtual int agressive() const;
  virtual void timeStep(const unsigned long time);

protected:
  virtual void _reachedPathway();
  virtual void _centerTile();
  virtual void _brokePathway(TilePos pos);
  virtual void _findNewWay(const TilePos& start);

private:
  Wolf( PlayerCityPtr city );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__CAESARIA_ANIMAL_H_INCLUDED__
