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

#include "tilerect.hpp"

static bool operator<=(const TilePos& a, const TilePos& b) { return (a.i()<b.i()) || (a.i() == b.i() && a.j() <= b.j()); }
static bool operator>=(const TilePos& a, const TilePos& b) { return (a.i()>b.i()) || (a.i() == b.i() && a.j() >= b.j()); }

TileRect::TileRect() : _leftBottomCorner(-1, -1), _rightTopCorner(-1, -1), _leftTopCorner(-1, -1), _rightBottomCorner(-1, -1){}


TileRect::TileRect(TilePos start, TilePos end){
  int startI, startJ, endI, endJ;
  startI = std::min(start.i(), end.i());
  startJ = std::min(start.j(), end.j());
  endI = std::max(start.i(), end.i());
  endJ = std::max(start.j(), end.j());
  _leftBottomCorner = TilePos(startI, startJ);
  _rightTopCorner = TilePos(endI, endJ);
  _leftTopCorner = TilePos(startI, endJ);
  _rightBottomCorner = TilePos(endJ, startI);
}


TileRect::TileRect(const TileRect& a)
{
  _leftBottomCorner = a._leftBottomCorner;
  _rightTopCorner = a._rightTopCorner;
  _leftTopCorner = a._leftTopCorner;
  _rightBottomCorner = a._rightBottomCorner;
}

bool TileRect::contain(TilePos tilePos) const
{
  return tilePos >= _leftBottomCorner && tilePos <= _rightTopCorner;
}


TilePos TileRect::leftUpCorner() const
{
  return _leftTopCorner;
}


TilePos TileRect::rightDownCorner() const
{
  return _rightBottomCorner;
}


TileRect& TileRect::operator=(const TileRect& a)
{
  _leftBottomCorner = a._leftBottomCorner;
  _rightTopCorner = a._rightTopCorner;
  _leftTopCorner = a._leftTopCorner;
  _rightBottomCorner = a._rightBottomCorner;
  return *this;
}

TilePos TileRect::random() const
{
  return TilePos(math::random(_rightTopCorner.i() - _leftBottomCorner.i()) + _leftBottomCorner.i(),
                 math::random(_rightTopCorner.j() - _leftBottomCorner.j()) + _leftBottomCorner.j());
}
