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


#ifndef GFX_GL_ENGINE_HPP
#define GFX_GL_ENGINE_HPP

#include <gfx_engine.hpp>

#include <list>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

#include <picture.hpp>


// This is the OpenGL engine
// It does a dumb drawing from back to front, in a 2D projection, with no depth buffer
class GfxGlEngine : public GfxEngine
{

public:

   GfxGlEngine();
   virtual ~GfxGlEngine();
   virtual void init();
   virtual void exit();

   virtual void load_picture(Picture &ioPicture);
   virtual void unload_picture(Picture &ioPicture);

   virtual void init_frame();
   virtual void drawPicture(const Picture &picture, const int dx, const int dy);
   virtual void exit_frame();

private:
   SDL_Surface *_screen;
};

#endif
