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

#include "position_array.hpp"
#include "variant_list.hpp"

PointsArray::PointsArray(const std::vector<Point>& points)
{
  resize(points.size());
  std::copy(points.begin(),points.end(),this->begin());
}

VariantList PointsArray::save() const
{
  VariantList ret;
  for( auto& item : *this ) ret << item;
  return ret;
}

void PointsArray::move(const Point& offset)
{
  for( auto&& p : *this ) p += offset;
}

PointsArray& PointsArray::operator<<(const Point& point)
{
  push_back( point );
  return *this;
}

Point PointsArray::valueOrEmpty(unsigned int index) const
{
  return index < size() ? at( index ) : Point();
}

PointsArray&PointsArray::load(const VariantList& vl)
{
  for( const auto& item : vl ) { push_back( item ); }
  return *this;
}
