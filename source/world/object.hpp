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

#ifndef __CAESARIA_EMPIRE_OBJECT_H_INCLUDED__
#define __CAESARIA_EMPIRE_OBJECT_H_INCLUDED__

#include "core/position.hpp"
#include "gfx/picture.hpp"
#include "predefinitions.hpp"

namespace world
{

class Object : public ReferenceCounted
{
public:
  static ObjectPtr create( Empire& empire );

  virtual EmpirePtr getEmpire() const;
  virtual std::string getName() const;
  virtual Point getLocation() const;
  virtual void setLocation( const Point& location );
  virtual gfx::Picture getPicture() const;
  virtual void setPicture( gfx::Picture pic );
  virtual VariantMap save() const;
  virtual void load( const VariantMap& stream );

  virtual ~Object();

private:
  Object();

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace world

#endif //__CAESARIA_EMPIRE_OBJECT_H_INCLUDED__
