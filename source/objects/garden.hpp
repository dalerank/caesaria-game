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

#ifndef __CAESARIA_GARDEN_H_INCLUDED__
#define __CAESARIA_GARDEN_H_INCLUDED__

#include "objects/construction.hpp"

class Garden : public Construction
{
public:
  Garden();
  virtual void initTerrain(gfx::Tile& terrain);
  virtual bool isWalkable() const;
  virtual bool isFlat() const;
  virtual bool isNeedRoadAccess() const;
  virtual bool build(const CityAreaInfo &info);
  virtual void load(const VariantMap &stream);
  virtual void save(VariantMap &stream) const;
  virtual Desirability desirability() const;
  virtual std::string sound() const;
  virtual void destroy();
  virtual void setPicture(gfx::Picture picture);

  void update();
private:
  bool _flat;
};

#endif //__CAESARIA_GARDEN_H_INCLUDED__
