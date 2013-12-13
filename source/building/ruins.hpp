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

#ifndef __CAESARIA_RUINS_H_INCLUDE_
#define __CAESARIA_RUINS_H_INCLUDE_

#include "service.hpp"

class BurningRuins : public ServiceBuilding
{
public:
  BurningRuins();

  virtual void deliverService();
  virtual void timeStep(const unsigned long time);
  virtual void burn();
  virtual void build(PlayerCityPtr city, const TilePos& pos );
  virtual bool isWalkable() const;
  virtual void destroy();
  virtual int getMaxWorkers() const;

  virtual float evaluateService( ServiceWalkerPtr walker);
  virtual void applyService( ServiceWalkerPtr walker);
  virtual bool isNeedRoadAccess() const;
};

class BurnedRuins : public Building
{
public:
  BurnedRuins();

  virtual void timeStep(const unsigned long time);
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual void build(PlayerCityPtr city, const TilePos& pos );
  virtual bool isNeedRoadAccess() const;
  virtual void destroy();
};

typedef SmartPtr< BurningRuins > BurningRuinsPtr;

class CollapsedRuins : public Building
{
public:
  CollapsedRuins();

  virtual void burn();
  virtual void build(PlayerCityPtr city, const TilePos& pos );

  virtual bool isWalkable() const;
  virtual bool isNeedRoadAccess() const;
};

class PlagueRuins : public Building
{
public:
  PlagueRuins();

  virtual void timeStep(const unsigned long time);
  virtual void burn();
  virtual void build( PlayerCityPtr city, const TilePos& pos );
  virtual bool isWalkable() const;
  virtual void destroy();

  virtual void applyService(ServiceWalkerPtr walker);

  virtual bool isNeedRoadAccess() const;
};

#endif //__CAESARIA_RUINS_H_INCLUDE_
