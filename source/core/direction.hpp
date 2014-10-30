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

#ifndef _CAESARIA_DIRECTION_INCLUDE_H_
#define _CAESARIA_DIRECTION_INCLUDE_H_

#include "enumerator.hpp"

namespace constants
{

typedef enum
{ 
  noneDirection=0,
  north,
  northWest,
  west,
  southWest,
  south,
  southEast,
  east,
  northEast,
  countDirection
} Direction;

class DirectionHelper : public EnumsHelper<Direction>
{
public:
  DirectionHelper() : EnumsHelper<Direction>( noneDirection )
  {
#define ADD_DIRECTION(a) append(a, "##"#a"##" );
    ADD_DIRECTION(noneDirection)
    ADD_DIRECTION(north)
    ADD_DIRECTION(northWest)
    ADD_DIRECTION(west)
    ADD_DIRECTION(southWest)
    ADD_DIRECTION(south)
    ADD_DIRECTION(southEast)
    ADD_DIRECTION(east)
    ADD_DIRECTION(northEast)
#undef ADD_DIRECTION
  }
};

} //end namespace constants

#endif  //_CAESARIA_DIRECTION_INCLUDE_H_
