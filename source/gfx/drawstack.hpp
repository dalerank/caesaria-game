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

#ifndef _CAESARIA_DRAWSTACK_INCLUDE_H_
#define _CAESARIA_DRAWSTACK_INCLUDE_H_

#include "picture.hpp"
#include "engine.hpp"

namespace gfx
{

class DrawStack
{
public:
  struct DrawInstruction
  {
    Picture pic;
    Point offset;
  };

  std::vector<DrawInstruction> stack;

  void draw(Engine& e, const Point& pos) const;
  void add( const Picture& pic, const Point& offset );
  void clear();
};

} //end namespace gfx
#endif //_CAESARIA_DRAWSTACK_INCLUDE_H_
