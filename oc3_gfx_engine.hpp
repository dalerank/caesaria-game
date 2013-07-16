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


#ifndef GFX_ENGINE_HPP
#define GFX_ENGINE_HPP

#include "oc3_picture.hpp"
#include "oc3_size.hpp"

struct NEvent;

class GfxEngine
{
public:
  static GfxEngine& instance();

  GfxEngine();
  virtual ~GfxEngine();
  virtual void init() = 0;
  virtual void exit() = 0;
  virtual void delay( const unsigned int msec ) = 0;
  virtual bool haveEvent( NEvent& event ) = 0;

  
  void setScreenSize(const int width, const int height);
  Size getScreenSize() const;
  int getScreenWidth() const;
  int getScreenHeight() const;

  virtual void setFlag( int flag, int value );

  virtual void loadPictures( const PicturesArray& ioPictures);
  virtual void loadPicture( Picture& ioPicture) = 0;
  virtual void unloadPicture( Picture& ioPicture) = 0;

  virtual void startRenderFrame() = 0;  // start a new frame
  virtual void endRenderFrame() = 0;  // display the frame

  virtual void drawPicture(const Picture &pic, const int dx, const int dy, Rect* clipRect=0 ) = 0;
  virtual void drawPicture(const Picture &pic, const Point& pos, Rect* clipRect=0 ) = 0;

  virtual void setTileDrawMask( int rmask, int gmask, int bmask, int amask ) = 0;
  virtual void resetTileDrawMask() = 0;
  
  virtual void deletePicture( Picture* pic ) = 0;
  // creates a picture with the given size, it will need to be loaded by the graphic engine
  virtual Picture* createPicture(const Size& size ) = 0;

  virtual void createScreenshot( const std::string& filename ) = 0;
  virtual unsigned int getFps() const = 0;

protected:
  static GfxEngine* _instance;

  int _screen_width;   // screen width
  int _screen_height;  // screen height
};

#endif
