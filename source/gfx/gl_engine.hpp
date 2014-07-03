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
// Copyright 2012-2014 Gregoire Athanase, gathanase@gmail.com


#ifndef GFX_GL_ENGINE_HPP
#define GFX_GL_ENGINE_HPP

#include "engine.hpp"

#include <list>
#include <vector>

#include "picture.hpp"

// This is the OpenGL engine
// It does a dumb drawing from back to front, in a 2D projection, with no depth buffer
namespace gfx
{

class GlEngine : public Engine
{
public:
   GlEngine();
   virtual ~GlEngine();
   void init();
   void exit();
   void delay( const unsigned int msec );

   Picture* createPicture(const Size& size);
   virtual void loadPicture(Picture &ioPicture);
   virtual void unloadPicture(Picture &ioPicture);
   void deletePicture(Picture* pic);

   void startRenderFrame();
   void draw(const Picture &picture, const int dx, const int dy, Rect* clipRect=0);
   void draw(const Picture &picture, const Point& pos, Rect* clipRect=0 );
   void draw(const Pictures& pictures, const Point& pos, Rect* clipRect);
   void endRenderFrame();

   void setColorMask( int rmask, int gmask, int bmask, int amask );
   void resetColorMask();

   void createScreenshot( const std::string& filename );
   unsigned int fps() const;
   bool haveEvent( NEvent& event );

   Modes modes() const;

   Point cursorPos() const;
   Picture& screen();
private:
   Picture _screen;
   unsigned int _fps, _lastUpdateFps, _lastFps, _drawCall;
   float _rmask, _gmask, _bmask, _amask;
};

}
#endif
