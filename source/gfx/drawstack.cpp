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

#include "drawstack.hpp"
#include "core/foreach.hpp"

namespace gfx
{

void DrawStack::draw( Engine& e, const Point& pos) const
{
  foreach( i, stack )
  {
    e.draw( (*i).pic, pos + (*i).offset );
  }
}

void DrawStack::add(const Picture& pic, const Point& offset)
{
  stack.push_back( DrawInstruction() );
  stack.back().pic = pic;
  stack.back().offset = offset;
}

void DrawStack::clear()
{
  stack.clear();
}

}//end namespace gfx
