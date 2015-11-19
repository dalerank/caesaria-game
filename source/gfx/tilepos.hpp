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

#ifndef __CAESARIA_TILEPOS_H_INCLUDED__
#define __CAESARIA_TILEPOS_H_INCLUDED__

#include "core/position.hpp"

class TilePos : Vector2<int>
{
public:
  struct Neighbors
  {
    const TilePos& pos;
    inline TilePos north() const { return TilePos( pos.x(), pos.y()+1 ); }
    inline TilePos northeast() const { return TilePos( pos.x()+1, pos.y()+1 ); }
    inline TilePos east() const { return TilePos( pos.x()+1, pos.y() ); }
    inline TilePos southeast() const { return TilePos( pos.x()+1, pos.y()-1 ); }
    inline TilePos south() const { return TilePos( pos.x(), pos.y()-1 ); }
    inline TilePos southwest() const { return TilePos( pos.x()-1, pos.y()-1 ); }
    inline TilePos west() const { return TilePos( pos.x()-1, pos.y() ); }
    inline TilePos northwest() const { return TilePos( pos.x()-1, pos.y()+1 ); }
  };

  TilePos( const int i, const int j );
  TilePos( const TilePos& other );
  TilePos();

  inline int i() const { return _x; }
  inline int j() const { return _y; }
  inline int z() const { return _y - _x; }

  inline int& ri() { return _x; }
  inline int& rj() { return _y; }

  Neighbors nb() const { return Neighbors{*this}; }

  inline void setI( const int i ) { _x = i; }
  inline void setJ( const int j ) { _y = j; }
  inline void set(int nx, int ny) { _x = nx; _y = ny; }

  float distanceFrom( const TilePos& other ) const;
  int getDistanceFromSQ(const TilePos& other) const;

  TilePos& operator=(const TilePos& other) { set( other._x, other._y ); return *this; }
  TilePos& operator+=(const TilePos& other) { set( _x + other._x, _y + other._y ); return *this; }
  TilePos& operator-=(const TilePos& other) { set( _x - other._x, _y - other._y ); return *this; }
  TilePos operator+(const TilePos& other) const { return TilePos( _x + other._x, _y + other._y ); }
  TilePos operator-(const TilePos& other) const { return TilePos( _x - other._x, _y - other._y ); }
  TilePos operator*(int v) const { return TilePos( _x * v, _y * v ); }
  TilePos operator-() const { return TilePos( -_x, -_y ); }
  TilePos operator/(int v) const { return TilePos( _x / v, _y / v); }
  bool operator==(const TilePos& other) const{ return (_x == other._x) && ( _y == other._y ); }
  bool operator!=(const TilePos& other) const{ return (_x != other._x ) || ( _y != other._y ); }
  bool operator<(const TilePos& other) const{ return (_x<other._x) || (_x==other._x && _y<other._y); }

  Point toScreenCoordinates() const;

  double getAngleICW() const;

  TilePos fit( const TilePos& lur, const TilePos& tbr ) const;

  TilePos nextStep( const TilePos& dst ) const;
};

#endif //__CAESARIA_TILEPOS_H_INCLUDED__
