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

#ifndef __CAESARIA_WORKINGBUILDING_H_INCLUDED__
#define __CAESARIA_WORKINGBUILDING_H_INCLUDED__

#include "objects/building.hpp"

/** Building where people work */
class WorkingBuilding : public Building
{
public:
  WorkingBuilding(const TileOverlay::Type type, const Size& size);
  virtual ~WorkingBuilding();

  void setMaxWorkers(const int maxWorkers);
  int maxWorkers() const;

  void setWorkers( const unsigned int currentWorkers );
  void addWorkers( const unsigned int workers );
  void removeWorkers( const unsigned int workers );

  int numberWorkers() const;

  virtual bool mayWork() const;

  virtual void setActive(const bool value);  // if false then this building is stopped
  virtual bool isActive() const;

  virtual void destroy();

  virtual void timeStep(const unsigned long time);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  virtual void addWalker( WalkerPtr walker );
  virtual const WalkerList& walkers() const;

  virtual std::string errorDesc() const;
  virtual std::string workersProblemDesc() const;
  virtual std::string workersStateDesc() const;
  virtual std::string troubleDesc() const;

protected:
  void _setError(const std::string& err);
  void _fireWorkers();
  void _updateAnimation( const unsigned int time );
  void _setClearAnimationOnStop( bool value );

private:

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_WORKINGBUILDING_H_INCLUDED__
