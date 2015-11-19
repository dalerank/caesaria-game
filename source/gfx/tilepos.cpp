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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "tilepos.hpp"

#ifndef CAESARIA_DISABLED_TILEMAPCONFIG
#include "tilemap_config.hpp"
#endif

TilePos::TilePos(const int i, const int j) : Vector2<int>( i, j ) {}

TilePos::TilePos(const TilePos& other) : Vector2<int>( other._x, other._y ) {}

TilePos::TilePos() : Vector2<int>( 0, 0 ) {}

float TilePos::distanceFrom(const TilePos& other) const { return getDistanceFrom( other );}

int TilePos::getDistanceFromSQ(const TilePos& other) const { return Vector2<int>::getDistanceFromSQ(other);}


#ifndef CAESARIA_DISABLED_TILEMAPCONFIG
Point TilePos::toScreenCoordinates() const
{
  return Point( gfx::tilemap::cellSize().width() * (_y+_x),
                gfx::tilemap::cellSize().height()* (_y-_x) );
}
#endif

double TilePos::getAngleICW() const { return getAngle(); }

TilePos TilePos::fit(const TilePos& lur, const TilePos& tbr) const
{
  TilePos ret = *this;
  ret._x = math::clamp( ret._x, lur._x, tbr._x );
  ret._y = math::clamp( ret._y, lur._y, tbr._y );

  return ret;
}

TilePos TilePos::nextStep(const TilePos& dst) const
{
  return *this + TilePos( math::signnum( dst.i() - i() ),
                          math::signnum( dst.j() - j() ) );
}

unsigned int TilePos::hash() const { return (_x << 16) + _y; }
