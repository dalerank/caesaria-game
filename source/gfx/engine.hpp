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

#ifndef __CAESARIA_GFX_ENGINE_H_INCLUDE__
#define __CAESARIA_GFX_ENGINE_H_INCLUDE__

#include "picturesarray.hpp"
#include "core/size.hpp"
#include <map>

struct NEvent;

namespace gfx
{

class Engine
{
public:
  typedef Size Mode;
  typedef std::vector<Size> Modes;

  typedef enum { fullscreen=0, debugInfo, effects } Flags;
  static Engine& instance();

  Engine();
  virtual ~Engine();
  virtual void init() = 0;
  virtual void exit() = 0;
  virtual void delay( const unsigned int msec ) = 0;
  virtual bool haveEvent( NEvent& event ) = 0;

  void setScreenSize( Size size );
  Size virtualSize() const;
  Size screenSize() const;

  bool isFullscreen() const;
  void setFullscreen(bool enabled );

  virtual void setFlag( int flag, int value );
  virtual int getFlag( int flag ) const;

  virtual void loadPicture( Picture& ioPicture, bool streaming ) = 0;
  virtual void unloadPicture( Picture& ioPicture) = 0;

  virtual void initViewport( int, Size s) = 0;
  virtual void setViewport( int, bool render) = 0;
  virtual void drawViewport( int, Rect r) = 0;

  virtual void startRenderFrame() = 0;  // start a new frame
  virtual void endRenderFrame() = 0;  // display the frame

  virtual void draw(const Picture& pic, const int dx, const int dy, Rect* clipRect=0 ) = 0;
  virtual void draw(const Picture& pic, const Point& pos, Rect* clipRect=0 ) = 0;
  virtual void draw(const Picture& pic, const Rect& srcRect, const Rect& dstRect, Rect* clipRect=0 ) = 0;
  virtual void draw(const Pictures& pic, const Point& pos, Rect* clipRect=0 ) = 0;

  virtual void drawLine( const NColor& color, const Point& p1, const Point& p2 ) = 0;

  virtual void setColorMask( int rmask, int gmask, int bmask, int amask ) = 0;
  virtual void resetColorMask() = 0;
  
  virtual void deletePicture( Picture* pic ) = 0;

  virtual void createScreenshot( const std::string& filename ) = 0;
  virtual unsigned int fps() const = 0;
  virtual Modes modes() const = 0;
  virtual Point cursorPos() const = 0;
  virtual Picture& screen() = 0;

protected:
  static Engine* _instance;

  Size _srcSize, _virtualSize;
  std::map< int, int > _flags;
};

}//end namespace gfx
#endif //__CAESARIA_GFX_ENGINE_H_INCLUDE__
