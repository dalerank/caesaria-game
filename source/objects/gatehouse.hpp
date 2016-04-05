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

#ifndef __CAESARIA_GATEHOUSE_H_INCLUDED__
#define __CAESARIA_GATEHOUSE_H_INCLUDED__

#include "building.hpp"

class Gatehouse : public Building
{
public:
  typedef enum { autoToggle, closed, opened } Mode;
  Gatehouse();

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

  virtual bool isWalkable() const;
  virtual void destroy();
  virtual void burn();
  virtual void timeStep(const unsigned long time);

  void setMode( Mode mode );
  void nextMode();
  Mode mode() const;

  virtual gfx::Renderer::PassQueue passQueue() const;
  virtual const gfx::Pictures& pictures(gfx::Renderer::Pass pass) const;

  virtual void initTerrain(gfx::Tile &terrain);
  virtual bool canBuild(const city::AreaInfo& areaInfo) const;
  virtual bool build(const city::AreaInfo &info);
  virtual Variant getProperty(const std::string& name) const;
private:
  bool _update( const city::AreaInfo& areaInfo );

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_GATEHOUSE_H_INCLUDED__
