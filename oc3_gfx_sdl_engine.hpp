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


#ifndef GFX_SDL_ENGINE_HPP
#define GFX_SDL_ENGINE_HPP

#include "oc3_gfx_engine.hpp"
#include "oc3_picture.hpp"
#include "oc3_scopedptr.hpp"


// This is the SDL engine
// It does a dumb drawing from back to front
class GfxSdlEngine : public GfxEngine
{
public:
  GfxSdlEngine();
  ~GfxSdlEngine();
  void init();
  void exit();
  void delay( const unsigned int msec );
  bool haveEvent( NEvent& event );

  void loadPicture(Picture &ioPicture);
  void unloadPicture(Picture& ioPicture);

  void startRenderFrame();
  void drawPicture(const Picture &picture, const int dx, const int dy, Rect* clipRect=0);
  void drawPicture(const Picture &picture, const Point& pos, Rect* clipRect=0 );
  void endRenderFrame();

  void setFlag( int flag, int value );

  void setTileDrawMask( int rmask, int gmask, int bmask, int amask );
  void resetTileDrawMask();

  // deletes a picture (deallocate memory)
  void deletePicture( Picture* pic );

  // creates a picture with the given size, it will need to be loaded by the graphic engine
  Picture* createPicture(const Size& size);

  Picture& getScreen();

  unsigned int getFps() const;
  void createScreenshot( const std::string& filename );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif