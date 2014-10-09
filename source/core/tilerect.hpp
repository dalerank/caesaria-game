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

#ifndef _CAESARIA_TILESAREA_INCLUDE_H_
#define _CAESARIA_TILESAREA_INCLUDE_H_

#include "position.hpp"

class TileRect
{
public:  

  TileRect();

  TileRect(TilePos start, TilePos end);

  TileRect(const TileRect& a);

  bool contain( TilePos tilePos ) const;

  TilePos leftUpCorner() const;

  TilePos rightDownCorner() const;

  TileRect& operator=(const TileRect& a);

  TilePos random() const;
private:
  TilePos _leftBottomCorner;
  TilePos _rightTopCorner;
  TilePos _leftTopCorner;
  TilePos _rightBottomCorner;
};

#endif //_CAESARIA_TILESAREA_INCLUDE_H_
