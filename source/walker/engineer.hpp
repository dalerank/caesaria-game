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

#ifndef __CAESARIA_ENGINEER_H_INCLUDED__
#define __CAESARIA_ENGINEER_H_INCLUDED__

#include "serviceman.hpp"
#include "core/predefinitions.hpp"

/** This is an immigrant coming with his stuff */
class Engineer : public ServiceWalker
{
public:
  static WalkerPtr create( PlayerCityPtr city ); //need for walker manager

  virtual ~Engineer();

  virtual std::string thoughts(Thought th) const;

protected:
  virtual void _centerTile();

  Engineer( PlayerCityPtr city );
  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__OPENCAESAR3_CORPSE_H_INCLUDED__
