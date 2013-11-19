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

#ifndef __OPENCAESAR3_WORKINGBUILDING_H_INCLUDED__
#define __OPENCAESAR3_WORKINGBUILDING_H_INCLUDED__

#include "building/building.hpp"

/** Building where people work */
class WorkingBuilding : public Building
{
public:
  WorkingBuilding(const TileOverlay::Type type, const Size& size);
  ~WorkingBuilding();

  void setMaxWorkers(const int maxWorkers);
  int getMaxWorkers() const;

  void setWorkers( const unsigned int currentWorkers );
  void addWorkers( const unsigned int workers );
  void removeWorkers( const unsigned int workers );

  int getWorkersCount() const;

  virtual void setActive(const bool value);  // if false then this building is stopped
  virtual bool isActive() const;

  virtual void destroy();

  virtual void timeStep(const unsigned long time);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void addWalker( WalkerPtr walker );
  virtual const WalkerList& getWalkers() const;

  virtual std::string getError() const;

protected:
  void _setError(const std::string& err);
  void _fireWorkers();

private:

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_WORKINGBUILDING_H_INCLUDED__
