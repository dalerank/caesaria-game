// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef SDL_FACADE_HPP
#define SDL_FACADE_HPP


#include <list>
#include "oc3_picture.hpp"
#include <string>


/* Provides usefull functions for SDL */
class SdlFacade
{
public:
  static SdlFacade& instance();

  // creates a surface with the same format than "display", for fast blitting
  SDL_Surface* createSurface(Uint32 flags, const int width, const int height, const SDL_Surface* display);

  // changes the pixels: perform a logical OR (used to transform black to some other color, without touching the alpha value)
  void color_or(SDL_Surface *img, const SDL_Color &color);
protected:
  static SdlFacade *_instance;
};

#endif //SDL_FACADE_HPP
