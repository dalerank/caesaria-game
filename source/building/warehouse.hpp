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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef WAREHOUSE_HPP
#define WAREHOUSE_HPP

#include "working.hpp"
#include "game/enums.hpp"
#include "game/good.hpp"
#include "core/position.hpp"

class GoodStore;

class Warehouse: public WorkingBuilding
{
  friend class WarehouseStore;

public:
  Warehouse();
  void init();

  virtual void timeStep(const unsigned long time);
  void computePictures();
  GoodStore& getGoodStore();
  
  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

private:
  void _resolveDevastationMode();

  class Impl;
  ScopedPtr< Impl > _d;
};



#endif
