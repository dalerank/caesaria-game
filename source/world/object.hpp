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
#include "gfx/picturesarray.hpp"
#include "predefinitions.hpp"
#include "gfx/animation.hpp"

namespace world
{

class Object : public ReferenceCounted
{
public:
  static ObjectPtr create( EmpirePtr empire );

  virtual bool isDeleted() const;
  virtual std::string type() const;
  virtual void timeStep(const unsigned int time);
  virtual EmpirePtr empire() const;
  virtual std::string name() const;
  virtual void setName( const std::string& name );
  virtual Point location() const;
  virtual void addObject( ObjectPtr );
  virtual void setLocation( const Point& location );
  virtual gfx::Picture picture() const;
  virtual const gfx::Pictures& pictures() const;
  virtual void setPicture( gfx::Picture pic );
  virtual bool isMovable() const;

  virtual void save( VariantMap& stream ) const;
  virtual void load( const VariantMap& stream );

  virtual void attach();

  virtual ~Object();

  void deleteLater();

protected:
  Object(EmpirePtr empire );
  gfx::Animation& _animation();
  gfx::Pictures& _pictures();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace world

#endif //__CAESARIA_EMPIRE_OBJECT_H_INCLUDED__
