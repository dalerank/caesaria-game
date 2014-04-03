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

#include "engine.hpp"
#include "picture.hpp"
#include "core/scopedptr.hpp"

// This is the SDL engine
// It does a dumb drawing from back to front
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

  virtual void setTileDrawMask( int rmask, int gmask, int bmask, int amask );
  virtual void resetTileDrawMask();

  // deletes a picture (deallocate memory)
  virtual void deletePicture( Picture* pic );
  virtual void loadPicture(Picture &ioPicture);
  virtual void unloadPicture(Picture& ioPicture);
  virtual void drawPicture(const Picture &picture, const int dx, const int dy, Rect* clipRect=0);
  virtual void drawPicture(const Picture &picture, const Point& pos, Rect* clipRect=0 );
  // creates a picture with the given size, it will need to be loaded by the graphic engine
  virtual Picture* createPicture(const Size& size);

  virtual unsigned int fps() const;
  virtual void createScreenshot( const std::string& filename );

  virtual Modes getAvailableModes() const;
  virtual Point mousePos() const;

//non virtual functions: need to debug functional
  Picture& getScreen();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gfx
#endif //_CAESARIA_SDL_ENGINE_H_INCLUDE_
