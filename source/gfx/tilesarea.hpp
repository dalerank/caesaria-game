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

#include "tile.hpp"

#include <vector>
#include <cstring>

namespace gfx
{

class TilesArea
{
private:
  TilePos leftBottomCorner;
  TilePos rightTopCorner;
  TilePos leftTopCorner;
  TilePos rightBottomCorner;
public:  

  TilesArea() : leftBottomCorner(-1, -1), rightTopCorner(-1, -1), leftTopCorner(-1, -1), rightBottomCorner(-1, -1){}

  TilesArea(TilePos start, TilePos end){
    int startI, startJ, endI, endJ;
    startI = std::min(start.i(), end.i());
    startJ = std::min(start.j(), end.j());
    endI = std::max(start.i(), end.i());
    endJ = std::max(start.j(), end.j());
    leftBottomCorner = TilePos(startI, startJ);
    rightTopCorner = TilePos(endI, endJ);
    leftTopCorner = TilePos(startI, endJ);
    rightBottomCorner = TilePos(endJ, startI);
  }

  TilesArea(const TilesArea& a)
  {
    leftBottomCorner = a.leftBottomCorner;
    rightTopCorner = a.rightTopCorner;
    leftTopCorner = a.leftTopCorner;
    rightBottomCorner = a.rightBottomCorner;
  }

  bool contain( TilePos tilePos ) const
  {
    return tilePos >= leftBottomCorner && tilePos <= rightTopCorner;
  }    

  TilePos leftUpCorner() const
  {
    return leftTopCorner;
  }

  TilePos rightDownCorner() const
  {
    return rightBottomCorner;
  }

  TilesArea& operator=(const TilesArea& a)
  {
    leftBottomCorner = a.leftBottomCorner;
    rightTopCorner = a.rightTopCorner;
    leftTopCorner = a.leftTopCorner;
    rightBottomCorner = a.rightBottomCorner;
  }

  TilePos random() const
  {
    return TilePos(math::random(rightTopCorner.i() - leftBottomCorner.i()) + leftBottomCorner.i(), 
                    math::random(rightTopCorner.j() - leftBottomCorner.j()) + leftBottomCorner.j());
  }

  static TilesArea GetArea(TilePos start, TilePos end){
    return TilesArea(start, end);
  }
};

}//end namespace

#endif //_CAESARIA_TILESAREA_INCLUDE_H_
