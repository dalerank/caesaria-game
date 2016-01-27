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
// along with CaesarIA. If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_TRADEROUTE_FINDER_H_INCLUDED__
#define __CAESARIA_TRADEROUTE_FINDER_H_INCLUDED__

#include "core/smartptr.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"
#include "core/tilepos_array.hpp"

namespace world
{

class TraderouteFinder
{
public:
  typedef enum { waterOnly=0x1, terrainOnly=0x2 } Flags;

  TraderouteFinder( const EmpireMap& empiremap );

  bool findRoute( TilePos start, TilePos stop, Locations& way, int flags );
  void setTerrainType( TilePos pos, unsigned int type );

  ~TraderouteFinder();
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace world

#endif //__CAESARIA_TRADEROUTE_FINDER_H_INCLUDED__
