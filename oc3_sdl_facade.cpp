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



#include "oc3_sdl_facade.hpp"

#include "oc3_gfx_engine.hpp"
#include "oc3_exception.hpp"


SdlFacade* SdlFacade::_instance = NULL;

SdlFacade& SdlFacade::instance()
{
  if (_instance == NULL)
  {
    _instance = new SdlFacade();
    if (_instance == NULL) THROW("Memory error, cannot instantiate object");
  }
  return *_instance;
}


////////////////////////////
// IMAGE CREATION METHODS
////////////////////////////


SDL_Surface* SdlFacade::createSurface(Uint32 flags, const int width, const int height, const SDL_Surface* display)
{
   SDL_Surface* res;
   const SDL_PixelFormat& fmt = *(display->format);
   res = SDL_CreateRGBSurface(flags, width, height, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
   if (res == NULL) THROW("Cannot make surface, size=" << width << "x" << height);
   return res;
}