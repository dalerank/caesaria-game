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

#ifndef _CAESARIA_PICTUREIMPL_HPP_INCLUDE_
#define _CAESARIA_PICTUREIMPL_HPP_INCLUDE_

#include "core/referencecounted.hpp"
#include "core/rectangle.hpp"
#include <SDL.h>

namespace gfx
{
  
class PictureImpl : public ReferenceCounted
{
public:
  SDL_Surface* surface;
  SDL_Texture* texture;  // for SDL surface
  std::string name; // for game save
  unsigned int opengltx;
  Rect orect;

  PictureImpl& operator=(const Impl& other)
  {
    surface = other.surface;
    texture = other.texture;
    name = other.name;
    opengltx = other.opengltx;
    orect = other.orect;
  }

  ~PictureImpl()
  {
    if( surface ) SDL_FreeSurface( surface );
    if( texture ) SDL_DestroyTexture( texture );
    surface = 0;
    texture = 0;
  }
};

}//end namespace gfx

#endif //_CAESARIA_PICTUREIMPL_HPP_INCLUDE_
