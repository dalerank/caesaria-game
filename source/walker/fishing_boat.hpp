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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_FISHING_BOAT_INCLUDE_H_
#define _CAESARIA_FISHING_BOAT_INCLUDE_H_

#include "ship.hpp"

class FishingBoat : public Ship
{
public:
  typedef enum { go2fishplace, catchFish, back2base, finishCatch, unloadFish, ready2Catch, wait } State;

  static FishingBoatPtr create( PlayerCityPtr city );

  void send2city( CoastalFactoryPtr base, TilePos start);

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual void timeStep(const unsigned long time);
  void startCatch();
  void return2base();

  void setBase( CoastalFactoryPtr base );
  State state() const;

  bool isBusy() const;
  int fishQty() const;
  int fishMax() const;
  void addFish( int qty );

  virtual bool die();
protected:
  virtual void _reachedPathway();

private:
  FishingBoat( PlayerCityPtr city );

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //_CAESARIA_FISHING_BOAT_INCLUDE_H_
