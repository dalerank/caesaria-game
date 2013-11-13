// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_RUINS_H_INCLUDE_
#define __OPENCAESAR3_RUINS_H_INCLUDE_

#include "service.hpp"

class BurningRuins : public ServiceBuilding
{
public:
  BurningRuins();

  void deliverService();
  void timeStep(const unsigned long time);
  void burn();
  void build(PlayerCityPtr city, const TilePos& pos );
  bool isWalkable() const;
  void destroy();

  float evaluateService( ServiceWalkerPtr walker);
  void applyService( ServiceWalkerPtr walker);
  bool isNeedRoadAccess() const;
};

class BurnedRuins : public Building
{
public:
  BurnedRuins();

  void timeStep(const unsigned long time);
  bool isWalkable() const;
  void build(PlayerCityPtr city, const TilePos& pos );
  bool isNeedRoadAccess() const;
  void destroy();
};

typedef SmartPtr< BurningRuins > BurningRuinsPtr;

class CollapsedRuins : public Building
{
public:
    CollapsedRuins();

    void burn();
    void build(PlayerCityPtr city, const TilePos& pos );

    bool isWalkable() const;
    bool isNeedRoadAccess() const;
};

class PlagueRuins : public Building
{
public:
  PlagueRuins();

  void timeStep(const unsigned long time);
  void burn();
  void build( PlayerCityPtr city, const TilePos& pos );
  bool isWalkable() const;
  void destroy();

  void applyService(ServiceWalkerPtr walker);

  bool isNeedRoadAccess() const;
};

#endif
