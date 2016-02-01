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

#ifndef __CAESARIA_TRAINEEWALKER_H_INCLUDED__
#define __CAESARIA_TRAINEEWALKER_H_INCLUDED__

#include "human.hpp"
#include "objects/overlay.hpp"
#include "walkers_factory_creator.hpp"

class Propagator;

/** This walker goes to work */
class TraineeWalker : public Citizen
{
  WALKER_MUST_INITIALIZE_FROM_FACTORY
public:
  virtual int value() const;
  virtual void send2City( BuildingPtr base, bool roadOnly=true );
  void setBase(BuildingPtr building);
  BuildingPtr base() const;
  BuildingPtr receiver() const;

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual TilePos places(Place type) const;

  virtual ~TraineeWalker();
protected:
  TraineeWalker( PlayerCityPtr city, walker::Type traineeType=walker::trainee );
  void _computeWalkerPath( bool roadOnly );

  void _checkDestination(const object::Type buildingType, Propagator& pathPropagator);
  virtual void _reachedPathway();
  void _init( walker::Type traineeType);
  void _cancelPath();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class TraineeWalkerCreator : public WalkerCreator
{
public:
  virtual WalkerPtr create( PlayerCityPtr city );
};

#endif //__CAESARIA_TRAINEEWALKER_H_INCLUDED__
