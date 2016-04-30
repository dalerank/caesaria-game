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

PointsArray& PointsArray::push_back(const Point& p) { _points.push_back(p); return *this; }
Point& PointsArray::operator[](size_t index) { return _points[index]; }
const Point& PointsArray::operator[](size_t index) const { return _points[index]; }
PointsArray::Collection::const_iterator PointsArray::begin() const { return _points.begin(); }
PointsArray::Collection::iterator PointsArray::begin() { return _points.begin(); }
PointsArray::Collection::iterator PointsArray::end() { return _points.end(); }
PointsArray::Collection::const_iterator PointsArray::end() const { return _points.end(); }
size_t PointsArray::size() const { return _points.size(); }
void PointsArray::resize(size_t newSize) { _points.resize(newSize); }
bool PointsArray::empty() const { return _points.empty(); }
void PointsArray::clear() { _points.clear(); }
Point& PointsArray::front() { return _points.front(); }

PointsArray::PointsArray(const std::vector<Point>& points)
{
  _points.resize(points.size());
  std::copy(points.begin(),points.end(),_points.begin());
}

VariantList PointsArray::save() const
{
  VariantList ret;
  for (auto& item : _points) ret << item;
  return ret;
}

void PointsArray::move(const Point& offset)
{
  for (auto& p : _points) p += offset;
}

PointsArray& PointsArray::operator<<(const Point& point)
{
  _points.push_back(point);
  return *this;
}

Point PointsArray::valueOrEmpty(unsigned int index) const
{
  return index < _points.size() ? _points.at(index) : Point();
}

PointsArray& PointsArray::load(const VariantList& vl)
{
  for (const auto& item : vl) { _points.push_back( item ); }
  return *this;
}
