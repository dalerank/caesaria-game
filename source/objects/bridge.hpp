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

#ifndef __CAESARIA_BRIDGE_H_INCLUDED__
#define __CAESARIA_BRIDGE_H_INCLUDED__

#include "construction.hpp"
#include "gfx/tilearea.hpp"

class Bridge : public Construction
{
public:
  class Area : public gfx::TileArea
  {
  public:
    Area( const gfx::Tilemap& tmap, const TilePos& leftup, const TilePos& rightdown )
      : gfx::TileArea( tmap, leftup, rightdown )
    {
    }

    void cropCorners()
    {
      pop_back();
      pop_front();
    }
  };

  Bridge( const object::Type type );
};

#endif //__CAESARIA_ACADEMY_H_INCLUDED__
