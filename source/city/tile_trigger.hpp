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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_TILETRIGGER_H_INCLUDED__
#define __CAESARIA_TILETRIGGER_H_INCLUDED__

#include "walker/walker.hpp"
#include "core/predefinitions.hpp"

/** This is an immigrant coming with his stuff */
class TileTrigger : public Walker
{
public:
  static WalkerPtr create( PlayerCityPtr city ); //need for walker manager

  virtual ~TileTrigger();

  virtual void timeStep(const unsigned long time);

  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);  

  virtual const gfx::Picture& getMainPicture();

protected:
  TileTrigger( PlayerCityPtr city );

  virtual bool _checkExeCondition();
  virtual void _updateAnimation( unsigned int time );
  virtual void _exec();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_TILETRIGGER_H_INCLUDED__
