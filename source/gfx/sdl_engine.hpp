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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com


#ifndef _CAESARIA_SDL_ENGINE_H_INCLUDE_
#define _CAESARIA_SDL_ENGINE_H_INCLUDE_

#include <SDL_surface.h>
#include <SDL_video.h>
#include "engine.hpp"
#include "picture.hpp"
#include "core/scopedptr.hpp"

// This is the SDL engine
namespace gfx
{

class SdlEngine : public Engine
{
public:
  SdlEngine();
  virtual ~SdlEngine();

  virtual void init();
  virtual void exit();
  virtual void delay( const unsigned int msec );
  virtual bool haveEvent( NEvent& event );

  virtual void startRenderFrame();
  virtual void endRenderFrame();

  virtual void setFlag( int flag, int value );

  virtual void setColorMask( int rmask, int gmask, int bmask, int amask );
  virtual void resetColorMask();

  virtual void initViewport( int, Size s);
  virtual void setViewport( int, bool render);
  virtual void drawViewport( int, Rect r);

  // deletes a picture (deallocate memory)
  virtual void deletePicture(Picture* pic);
  virtual void loadPicture(Picture& ioPicture, bool streaming);
  virtual void unloadPicture(Picture& ioPicture);

  virtual void draw(const Picture& picture, const int dx, const int dy, Rect* clipRect=0);
  virtual void draw(const Picture& picture, const Point& pos, Rect* clipRect=0 );
  virtual void draw(const Pictures& pictures, const Point& pos, Rect* clipRect=0 );
  virtual void draw(const Picture& pic, const Rect& srcRect, const Rect& dstRect, Rect* clipRect=0 );

  virtual void drawLine(const NColor &color, const Point &p1, const Point &p2);

  virtual unsigned int fps() const;
  virtual void createScreenshot( const std::string& filename );

  virtual Modes modes() const;
  virtual Point cursorPos() const;

  virtual Picture& screen();
  virtual unsigned int format() const;

  virtual void debug( const std::string& text, const Point& pos );

protected:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gfx
#endif //_CAESARIA_SDL_ENGINE_H_INCLUDE_
