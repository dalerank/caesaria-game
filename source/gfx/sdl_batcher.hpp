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

#ifndef __CAESARIA_GFX_SDL_BATCHER_H_INCLUDED__
#define __CAESARIA_GFX_SDL_BATCHER_H_INCLUDED__

#include "engine.hpp"

namespace gfx
{

class SdlBatcher
{
public:
  struct State
  {
    Picture texture;
    Rects   srcrects;
    Rects   dstrects;
    Rect    clip;
  };

  SdlBatcher();
  ~SdlBatcher();

  const State& current() const;

  bool append(const Picture& pic, const Point& pos, Rect *clip );
  bool append( const Picture& pic, const Rect& srcrect, const Rect& dstrect, Rect* clip );
  bool append( const Picture& pic, const Rects& srcrects, const Rects& dstrects, Rect* clip );

  void reset();
  bool finish();

  bool active() const;
  void setActive( bool value );

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

} //end namespace gfx

#endif //__CAESARIA_GFX_SDL_BACHER_H_INCLUDED__
