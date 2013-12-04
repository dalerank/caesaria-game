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

#include "walker.hpp"

class Propagator;

/** This walker goes to work */
class TraineeWalker : public Walker
{
public:
  static TraineeWalkerPtr create( PlayerCityPtr city, constants::walker::Type traineeType );

  void checkDestination(const TileOverlay::Type buildingType, Propagator& pathPropagator);
  void send2City( BuildingPtr base, bool roadOnly=true );
  void setBase(Building &building);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

protected:
  TraineeWalker( PlayerCityPtr city, constants::walker::Type traineeType);
  void _computeWalkerPath( bool roadOnly );

  virtual void _reachedPathway();
  void _init(constants::walker::Type traineeType);
  void _cancelPath();

private:
  int _maxDistance;

  std::list<TileOverlay::Type> _buildingNeed;  // list of buildings needing this trainee
  float _maxNeed;  // evaluates the need for that trainee

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_TRAINEEWALKER_H_INCLUDED__
