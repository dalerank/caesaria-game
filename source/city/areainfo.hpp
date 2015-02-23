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

#ifndef __CAESARIA_CITYAREAINFO_H_INCLUDED__
#define __CAESARIA_CITYAREAINFO_H_INCLUDED__

#include "predefinitions.hpp"
#include "gfx/predefinitions.hpp"

namespace city
{

struct AreaInfo
{
  PlayerCityPtr city;
  TilePos pos;
  const gfx::TilesArray& aroundTiles;
};

}//end namespace city

#endif //__CAESARIA_CITYAREAINFO_H_INCLUDED__
