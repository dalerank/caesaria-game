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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_GRANARY_H_INCLUDED__
#define __CAESARIA_GRANARY_H_INCLUDED__

#include "working.hpp"

namespace good
{
  class Store;
}

class Granary : public WorkingBuilding
{
public:
  Granary();

  virtual void timeStep(const unsigned long time);
  void computePictures();
  good::Store& store();

  virtual void initTerrain(gfx::Tile& terrain);
  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);
  virtual bool isWalkable() const;

  virtual void destroy();
  virtual std::string troubleDesc() const;

  virtual gfx::Renderer::PassQueue passQueue() const;
  virtual const gfx::Pictures& pictures(gfx::Renderer::Pass pass) const;

protected:
  virtual void _updateAnimation(const unsigned long time);

private:
  void _tryDevastateGranary();
  bool _trySendGoods(good::Product gtype, int qty);
  void _resolveDeliverMode();

  class Impl;
  ScopedPtr< Impl > _d;
};


#endif //__CAESARIA_GRANARY_H_INCLUDED__
