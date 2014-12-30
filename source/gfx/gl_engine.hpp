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


#ifndef GFX_GL_ENGINE_HPP
#define GFX_GL_ENGINE_HPP

#include "engine.hpp"
#include "picture.hpp"
#include "vfs/path.hpp"
#include "core/predefinitions.hpp"
#include "core/smartlist.hpp"
#include "core/variant.hpp"

// This is the OpenGL engine
// It does a dumb drawing from back to front, in a 2D projection, with no depth buffer
namespace gfx
{

#define CAESARIA_GL_RENDER

#ifdef CAESARIA_PLATFORM_ANDROID
#undef CAESARIA_GL_RENDER
#endif

#ifdef CAESARIA_GL_RENDER

class GlEngine : public Engine
{
public:
  GlEngine();
  virtual ~GlEngine();
  void init();
  void exit();
  void delay( const unsigned int msec );

  Picture* createPicture(const Size& size);
  virtual void loadPicture(Picture &ioPicture, bool streamed);
  virtual void unloadPicture(Picture &ioPicture);
  void deletePicture(Picture* pic);

  virtual void startRenderFrame();
  virtual void endRenderFrame();

  virtual void initViewport(int index, Size s);
  virtual void setViewport( int, bool render);
  virtual void drawViewport( int, Rect r);

  void draw(const Picture &picture, const int dx, const int dy, Rect* clipRect=0);
  void draw(const Picture &picture, const Point& pos, Rect* clipRect=0 );
  void draw(const Picture &picture, const Rect& src, const Rect& dst, Rect* clipRect=0 );
  void draw(const Pictures& pictures, const Point& pos, Rect* clipRect);
  void drawLine(const NColor &color, const Point &p1, const Point &p2);

  void setColorMask( int rmask, int gmask, int bmask, int amask );
  void resetColorMask();

  void createScreenshot( const std::string& filename );
  unsigned int fps() const;
  bool haveEvent( NEvent& event );

  Modes modes() const;

  Point cursorPos() const;
  Picture& screen();
  virtual void setFlag(int flag, int value);

private:
  void _createFramebuffer( unsigned int& id );
  void _initShaderProgramm(const char* vertSrc, const char* fragSrc,
                           unsigned int& vertexShader, unsigned int& fragmentShader, unsigned int& shaderProgram);

  class Impl;
  ScopedPtr<Impl> _d;

  Picture _screen;
  unsigned int _fps, _lastUpdateFps, _lastFps, _drawCall;
  float _rmask, _gmask, _bmask, _amask;  
};

#endif

}
#endif
