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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_CONSTRUCTION_EXTENSION_H_INCLUDED__
#define __CAESARIA_CONSTRUCTION_EXTENSION_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "construction.hpp"

class ConstructionExtension : public ReferenceCounted
{
public:
  virtual void run( gfx::TileOverlayPtr parent, unsigned int time ) = 0;
  virtual bool isDeleted() const = 0;
};

class FactoryProgressUpdater : public ConstructionExtension
{
public:
  static ConstructionExtensionPtr assignTo( FactoryPtr factory, float value, int time );

  virtual void run( gfx::TileOverlayPtr parent, unsigned int time );
  virtual bool isDeleted() const;
private:
  FactoryProgressUpdater();

  float _value;
  int _time;
};

#endif //__CAESARIA_CONSTRUCTION_EXTENSION_H_INCLUDED__
