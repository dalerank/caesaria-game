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

#ifndef __CAESARIA_RUINS_H_INCLUDE_
#define __CAESARIA_RUINS_H_INCLUDE_

#include "building.hpp"
#include "constants.hpp"

class Ruins : public Building
{
public:
  Ruins( constants::objects::Type type );
  void setInfo( std::string parent ) { _parent = parent; }
  std::string info() const { return _parent; }

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);
  void afterBuild() { _alsoBuilt=false; }

protected:
  std::string _parent;
  bool _alsoBuilt;
};

class BurningRuins : public Ruins
{
public:
  BurningRuins();

  virtual void timeStep(const unsigned long time);
  virtual void burn();
  virtual bool build(const CityAreaInfo &info);
  virtual bool isWalkable() const;
  virtual bool isDestructible() const;
  virtual void destroy();
  virtual bool isFlat() const { return false; }
  virtual void collapse();
  virtual bool canDestroy() const;

  virtual float evaluateService( ServiceWalkerPtr walker);
  virtual void applyService( ServiceWalkerPtr walker);
  virtual bool isNeedRoadAccess() const;
};

class BurnedRuins : public Ruins
{
public:
  BurnedRuins();

  virtual void timeStep(const unsigned long time);
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool build(const CityAreaInfo &info);
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
};

typedef SmartPtr< BurningRuins > BurningRuinsPtr;
typedef SmartPtr< BurnedRuins > BurnedRuinsPtr;

class CollapsedRuins : public Ruins
{
public:
  CollapsedRuins();

  virtual void burn();
  virtual bool build(const CityAreaInfo &info);
  virtual void collapse();

  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool isNeedRoadAccess() const;
};

class PlagueRuins : public Ruins
{
public:
  PlagueRuins();

  virtual void timeStep(const unsigned long time);
  virtual void burn();
  virtual bool isDestructible() const;
  virtual bool build( const CityAreaInfo& info );
  virtual bool isWalkable() const;
  virtual void destroy();

  virtual void applyService(ServiceWalkerPtr walker);

  virtual bool isNeedRoadAccess() const;
};

#endif //__CAESARIA_RUINS_H_INCLUDE_
