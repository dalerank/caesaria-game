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

#ifndef __CAESARIA_DRAW_STATE_H_INCLUDED__
#define __CAESARIA_DRAW_STATE_H_INCLUDED__

#include "picture.hpp"
#include "core/rect_array.hpp"

namespace gfx
{

class Engine;
class Batch;
class Pictures;

class DrawState
{
public:
  DrawState(Engine& painter);
  DrawState(Engine& painter, const Point& lefttop, Rect* clip );

  inline bool isOk() const { return _ok; }

  DrawState& draw( const Picture& picture );
  DrawState& draw( const Pictures& pics );
  DrawState& draw( const Picture& picture, const Point& offset );
  DrawState& draw( const Batch& batch );
  DrawState& fallback( const Batch& batch );
  DrawState& fallback( const Pictures& pics );
  DrawState& fallback( const Pictures& pics, const Rects& dstRects );

private:
  Engine& _painter;
  bool _ok = false;
  Point _lefttop;
  Rect* _clip = nullptr;
};

}//end namespace gfx

#endif //__CAESARIA_DRAW_STATE_H_INCLUDED__
