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
#include "gfx/tile.hpp"
#include "game/service.hpp"
#include "pathway/predefinitions.hpp"
#include "objects/overlay.hpp"
#include "walkers_factory_creator.hpp"

/** This walker gives a service to buildings along the road */
template<class T>
class SmartSet
{
public:
  typedef std::set<SmartPtr<T>> Collection;

  typedef typename Collection::iterator iterator;
  typedef typename Collection::const_iterator const_iterator;
  typedef typename Collection::reference reference;
  typedef typename Collection::const_reference const_reference;
  typedef typename Collection::reverse_iterator reverse_iterator;
  typedef typename Collection::const_reverse_iterator const_reverse_iterator;

  const_iterator begin() const { return _data.begin(); }
  const_reverse_iterator rbegin() const { return _data.rbegin(); }
  reverse_iterator rbegin() { return _data.rbegin(); }
  const_reverse_iterator rend() const { return _data.rend(); }
  reverse_iterator rend() { return _data.rend(); }
  iterator erase(iterator it) { return _data.erase(it); }
  iterator begin() { return _data.begin(); }
  iterator end() { return _data.end(); }
  const_iterator end() const { return _data.end(); }
  size_t size() const { return _data.size(); }
  bool empty() const { return _data.empty(); }
  void clear() { _data.clear(); }
  reference front() { return _data.front(); }
  const_reference front() const { return _data.front(); }
  reference back() { return _data.back(); }

  void addIfValid( SmartPtr<T> building )
  {
    if (building.isValid())
      _data.insert(building);
  }

  bool insert(SmartPtr<T> t) {
    return _data.insert(t).second;
  }

  template<class Dst>
  SmartSet<Dst> select() const
  {
    SmartSet<Dst> ret;
    for (auto i : _data) {
      ret.addIfValid( ptr_cast<Dst>( i ) );
    }

    return ret;
  }

  SmartList<T> toList() const
  {
    SmartList<T> ret;
    for (auto i : _data) {
      ret.push_back( i );
    }

    return ret;
  }

  template<class Dst>
  bool contain() const
  {
    for (const auto& i : _data) {
      if (is_kind_of<Dst>(i))
        return true;
    }

    return false;
  }

  template<class Dst>
  bool firstOf() const
  {
    for (auto i : _data) {
      SmartPtr<Dst> ret = ptr_cast<Dst>( i );
      if (ret.isValid())
        return ret;
    }

    return SmartPtr<Dst>();
  }
protected:
  Collection _data;
};

class ReachedBuildings : public SmartSet<Building>
{
public:
  bool contain( object::Type type ) const;
  BuildingPtr firstOf( object::Type type ) const;

  void cancelService( Service::Type service );

  template<class Dst>
  bool firstOf( object::Type type ) const
  {
    BuildingPtr ret = firstOf( type );
    return ptr_cast<Dst>( ret );
  }
};

class ServiceWalker : public Citizen
{
  WALKER_MUST_INITIALIZE_FROM_FACTORY
public:
  typedef enum { noOrders=0, goServiceMaximum=0x1, anywayWhenFailed=0x2, enterLastHouse=0x4, goServiceMinimum=0x8 } Order;

  Service::Type serviceType() const;
  const TilePos& baseLocation() const;

  void setBase( BuildingPtr base );
  BuildingPtr base() const;

  template<class T>
  void setBase( SmartPtr<T> base )
  {
    setBase( ptr_cast<Building>( base ) );
  }

  virtual void send2City( BuildingPtr base, int orders=goServiceMaximum );
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
  ServiceWalker( PlayerCityPtr city, const Service::Type service=Service::srvCount );

  void _init(const Service::Type service);
  void _computeWalkerPath(int orders);
  void _reservePath(const Pathway& pathWay);
  void _updatePathway(const Pathway& pathway);
  void _updatePathway(PathwayPtr pathway);
  void _cancelPath();
  void _addObsoleteOverlay( object::Type type );

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
