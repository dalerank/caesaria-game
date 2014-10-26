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

#ifndef _CAESARIA_HUMAN_H_INCLUDE_
#define _CAESARIA_HUMAN_H_INCLUDE_

#include "walker.hpp"

class Human : public Walker
{
public:  
  virtual constants::walker::Nation nation() const;
  virtual ~Human();

protected:
  Human( PlayerCityPtr city );
  void _setNation( constants::walker::Nation nation );

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_WALKER_H_INCLUDE_
