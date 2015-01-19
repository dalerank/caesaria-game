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

#ifndef __CAESARIA_SERVICEWALKER_H_INCLUDED__
#define __CAESARIA_SERVICEWALKER_H_INCLUDED__

#include "human.hpp"
#include "pathway/predefinitions.hpp"
#include "walkers_factory_creator.hpp"

/** This walker gives a service to buildings along the road */
class ServiceWalker : public Human
{
public:
  typedef enum { goLowerService=0x1, anywayWhenFailed=0x2, enterLastHouse=0x4 } Order;
  typedef std::set<BuildingPtr> ReachedBuildings;

  static ServiceWalkerPtr create( PlayerCityPtr city, const Service::Type service );

  Service::Type serviceType() const;
  BuildingPtr base() const;

  void setBase( BuildingPtr base );

  virtual void send2City( BuildingPtr base, int orders=goLowerService );
  virtual float serviceValue() const;
  virtual TilePos places(Place type) const;

  // evaluates the service demand on the given pathWay
  float evaluatePath( PathwayPtr pathWay);
  ReachedBuildings getReachedBuildings( const TilePos& pos );

  virtual unsigned int reachDistance() const;
  void setReachDistance( unsigned int value );

  virtual void return2Base();
  void setMaxDistance( const int distance );

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void setPathway(const Pathway& pathway);
  virtual bool die();

  virtual void initialize(const VariantMap &options);
  virtual ~ServiceWalker();

protected:
  virtual void _reachedPathway();
  virtual void _brokePathway(TilePos pos);
  virtual void _noWay();
  virtual void _centerTile();  // called when the walker is on a new tile

protected:
  ServiceWalker( PlayerCityPtr city, const Service::Type service );

  void _init(const Service::Type service);
  void _computeWalkerPath(int orders);
  void _reservePath(const Pathway& pathWay);
  void _updatePathway(const Pathway& pathway);
  void _updatePathway(PathwayPtr pathway);
  void _cancelPath();
  void _addObsoleteOverlay( gfx::TileOverlay::Type type );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class ServicemanCreator : public WalkerCreator
{
public:
  virtual WalkerPtr create( PlayerCityPtr city );
  ServicemanCreator( const Service::Type type )  { serviceType = type;  }
  Service::Type serviceType;
};

#define REGISTER_SERVICEMAN_IN_WALKERFACTORY(type,service,a) \
namespace { \
struct Registrator_##a { Registrator_##a() { WalkerManager::instance().addCreator( type, new ServicemanCreator( service ) ); }}; \
static Registrator_##a rtor_##a; \
}

#endif //__CAESARIA_SERVICEWALKER_H_INCLUDED__
