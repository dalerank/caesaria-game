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

#ifndef _CAESARIA_POSITIONARRAY_INCLUDE_H_
#define _CAESARIA_POSITIONARRAY_INCLUDE_H_

#include "position.hpp"

class VariantList;

class PointsArray : public std::vector<Point>
{
public:
  PointsArray() {}

  PointsArray(const std::vector<Point>& points);

  VariantList save() const;

  void move( const Point& offset );

  PointsArray& operator<<(const Point& point);

  Point valueOrEmpty( unsigned int index ) const;

  PointsArray& load( const VariantList& vl );
};

#endif  //_CAESARIA_POSITIONARRAY_INCLUDE_H_
