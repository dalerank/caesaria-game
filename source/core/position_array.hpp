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
#include "variant_list.hpp"

class PointsArray : public std::vector<Point>
{
public:
  VariantList save() const
  {
    VariantList ret;
    for( auto& item : *this ) ret << item;
    return ret;
  }

  void move( const Point& offset )
  {
    for( auto&& p : *this ) p += offset;
  }

  PointsArray& operator<<(const Point& point)
  {
    push_back( point );
    return *this;
  }

  Point valueOrEmpty( unsigned int index ) const
  {
    return index < size() ? at( index ) : Point();
  }

  PointsArray& load( const VariantList& vl )
  {
    for( auto& item : vl ) { push_back( item ); }
    return *this;
  }
};

#endif  //_CAESARIA_POSITIONARRAY_INCLUDE_H_
