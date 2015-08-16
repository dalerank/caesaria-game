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

#ifndef __CAESARIA_DIRECTED_ACTION_H_INCLUDED__
#define __CAESARIA_DIRECTED_ACTION_H_INCLUDED__

#include "core/direction.hpp"

struct DirectedAction
{
  int action;
  Direction direction;

  DirectedAction(int a=0, Direction d=direction::none) : action(a), direction(d) {}

  bool valid() const { return direction::none != direction; }

  inline bool operator<( const DirectedAction &a ) const
  {
    return ( action != a.action
              ? action < a.action
              : direction < a.direction );
  }
};

#endif //__CAESARIA_DIRECTED_ACTION_H_INCLUDED__
