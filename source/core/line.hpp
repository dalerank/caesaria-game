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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_LINE_INCLUDE_H__
#define __CAESARIA_LINE_INCLUDE_H__

#include "position.hpp"
#include "color.hpp"

class Line
{
public:
  Point begin, end;
  NColor color;
};

class Lines : public std::vector<Line>
{
public:
  void add( const NColor& color, const Point& p1, const Point& p2 )
  {
    Line a = { p1, p2, color };
    push_back( a );
  }

  Lines& append(const Point& p1, const Point& p2)
  {
    this->push_back({p1,p2});
    return *this;
  }

  std::vector<Point> points() const
  {
    std::vector<Point> pnts;
    for (const auto& line : *this)
    {
      pnts.push_back(line.begin);
      pnts.push_back(line.end);
    }

    return pnts;
  }
};

#endif //__CAESARIA_LINE_INCLUDE_H__
