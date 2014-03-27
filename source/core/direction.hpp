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

} //end namespace constants

#endif  //_CAESARIA_DIRECTION_INCLUDE_H_
