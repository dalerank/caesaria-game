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

#ifndef __CAESARIA_WALKER_PREFECT_H_INCLUDED__
#define __CAESARIA_WALKER_PREFECT_H_INCLUDED__

#include "serviceman.hpp"
#include "objects/prefecture.hpp"

class Prefect : public ServiceWalker
{
public:
  typedef enum { patrol=0,
                 findFire, go2fire, fightFire,
                 go2enemy, fightEnemy,
                 doNothing } SbAction;

  static PrefectPtr create( PlayerCityPtr city );

  virtual void timeStep(const unsigned long time);
  virtual float serviceValue() const;

  virtual void load( const VariantMap& stream );
  virtual void save( VariantMap& stream ) const;

  virtual void send2City( PrefecturePtr prefecture, Prefect::SbAction, int water=0 );
  virtual void send2City( BuildingPtr base, int orders=goLowerService );
  virtual void acceptAction(Action action, TilePos pos);
  virtual bool die();
  virtual void initialize(const VariantMap &options);

  virtual std::string thoughts(Thought th) const;
  virtual TilePos places(Place type) const;

  virtual ~Prefect();

protected:
  virtual void _noWay();
  virtual void _centerTile();
  virtual void _reachedPathway();

protected:
  Prefect( PlayerCityPtr city );

  WalkerPtr _looks4Enemy( const int range);
  bool _looks4Fire( ReachedBuildings& buildings, TilePos& pos );
  bool _checkPath2NearestFire( const ReachedBuildings& buildings );
  void _serveBuildings( ReachedBuildings& reachedBuildings );
  void _back2Prefecture();
  void _back2Patrol();
  void _setSubAction(const SbAction action );
  bool _figthFire();
  bool _findFire();
  virtual void _brokePathway(TilePos pos);

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_WALKER_PREFECT_H_INCLUDED__
