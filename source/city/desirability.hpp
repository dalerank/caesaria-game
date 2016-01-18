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
// Copyright 2012-2015 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_DESIRABILITY_H_INCLUDED__
#define __CAESARIA_DESIRABILITY_H_INCLUDED__

#include "city/predefinitions.hpp"
#include "objects/predefinitions.hpp"

struct Desirability
{
  int base  = 0;
  int range = 0;
  int step  = 0;

  enum { off=false, on=true };
  static void update( PlayerCityPtr r, OverlayPtr ov, bool on );
  static const Desirability& invalid();
};

#endif //__CAESARIA_DESIRABILTY_H_INCLUDED__
