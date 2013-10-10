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

#include "oc3_gfx_engine.hpp"

#include <list>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>

#include "oc3_picture.hpp"

// This is the OpenGL engine
// It does a dumb drawing from back to front, in a 2D projection, with no depth buffer
class GfxGlEngine : public GfxEngine
{
public:
   GfxGlEngine();
   ~GfxGlEngine();
   void init();
   void exit();
   void delay( const unsigned int msec );

   virtual void loadPicture(Picture &ioPicture);
   virtual void unloadPicture(Picture &ioPicture);

   void init_frame();
   void drawPicture(const Picture &picture, const int dx, const int dy, Rect* clipRect=0);
   void drawPicture(const Picture &picture, const Point& pos, Rect* clipRect=0 );
   void exit_frame();

   void setTileDrawMask( int rmask, int gmask, int bmask, int amask );
   void resetTileDrawMask();

   void createScreenshot( const std::string& filename );
   unsigned int getFps() const;
   bool haveEvent( NEvent& event );

   std::vector<Size> getAvailableModes() const;

private:
   SDL_Surface *_screen;
};

#endif
