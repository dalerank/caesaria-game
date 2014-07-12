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

#include "gl_engine.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include "core/logger.hpp"
#include "core/exception.hpp"
#include "picture.hpp"
#include "core/position.hpp"
#include "core/eventconverter.hpp"
#include "core/foreach.hpp"

#ifdef CAESARIA_PLATFORM_ANDROID
#include <GLES/gl.h>
#define glOrtho glOrthof
#else
#include <SDL_opengl.h>
#endif

#ifdef GL_EXT_PROTOTYPES
#ifdef CAESARIA_PLATFORM_ANDROID
#include <GLES/glext.h>
#else
#include <GL/glext.h>
#endif
#define glGenFramebuffers  glGenFramebuffersEXT
#define glGenTextures     glGenTexturesEXT
#define glGenRendererbuffers glGenRendererbuffersEXT
#define glBindFramebuffer glBindFramebufferEXT
#define glBindRenderbuffer glBindRendererbufferEXT
#define glRenderbufferStorage glRenderbufferStorageEXT
#define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
#define glCheckFramebufferStatus glCheckFramebufferEXT
#else
#define NO_FRAME_BUFFER
#endif

#include "core/font.hpp"
#include "pictureconverter.hpp"
#include "core/stringhelper.hpp"
#include "core/time.hpp"
#include "IMG_savepng.h"

namespace gfx{


GlEngine::GlEngine() : Engine()
{
}

GlEngine::~GlEngine() {}

void GlEngine::init()
{
  setFlag( Engine::effects, 1 );
  _rmask = _gmask = _bmask = _amask = 1.f;
  int rc;
  rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());
  rc = TTF_Init();
  if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

  SDL_Surface* screen = SDL_SetVideoMode( _srcSize.width(), _srcSize.height(), 16, SDL_OPENGL );
   _screen.init( screen, Point() );
  if( screen == NULL )
  {
    THROW("Unable to set video mode: " << SDL_GetError());
  }

  glEnable( GL_TEXTURE_2D );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glViewport( 0, 0, _srcSize.width(), _srcSize.height() );
  glClear(GL_COLOR_BUFFER_BIT); // black screen
  glDisable(GL_DEPTH_TEST); // no depth test
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, _srcSize.width(), _srcSize.height(), 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Displacement trick for exact pixelization
  glTranslatef(0.375, 0.375, 0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  Logger::warning( "GrafixEngine: set caption");
  SDL_WM_SetCaption( "CaesarIA: OGL "CAESARIA_VERSION, 0 );

  //!!!!!
  if( getFlag( Engine::effects ) > 0 )
  {
    _initFramebuffer();
   // _initShaderProgram(screenVertexSource, screenFragmentSource, screenVertexShader, screenFragmentShader, screenShaderProgram);
  }
}

void GlEngine::_initShaderProgramm( const char* vertSrc, const char* fragSrc,
                                    unsigned int& vertexShader, unsigned int& fragmentShader, unsigned int& shaderProgram)
{
  // Create and compile the vertex shader
  /*vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertSrc, NULL);
  glCompileShader(vertexShader);

  // Create and compile the fragment shader
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragSrc, NULL);
  glCompileShader(fragmentShader);

  // Link the vertex and fragment shader into a shader program
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);*/
}


void GlEngine::exit()
{
   TTF_Quit();
   SDL_Quit();
}

void GlEngine::deletePicture( Picture* pic )
{
  if( pic )
    unloadPicture( *pic );
}

Picture* GlEngine::createPicture(const Size& size )
{
  SDL_Surface* img = SDL_CreateRGBSurface( 0, size.width(), size.height(), 32,
                                           0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );

  Logger::warningIf( NULL == img, StringHelper::format( 0xff, "GlEngine:: can't make surface, size=%dx%d", size.width(), size.height() ) );

  Picture *pic = new Picture();
  pic->init(img, Point( 0, 0 ));  // no offset

  return pic;
}

Picture& GlEngine::screen(){  return _screen; }

void GlEngine::_initFramebuffer()
{
#ifndef NO_FRAME_BUFFER

#ifndef GL_DRAW_FRAMEBUFFER
  #define GL_DRAW_FRAMEBUFFER               0x8CA9
#endif

  glGenFramebuffers(1, &_framebuffer);
  glGenTextures(1, &_colorbuffer);
  glGenRenderbuffers(1, &_depthbuffer);

  glBindFramebuffer(GL_FRAMEBUFFER_EXT, _framebuffer);

  glBindTexture(GL_TEXTURE_2D, _colorbuffer);
  glTexImage2D(	GL_TEXTURE_2D,
                0,
                GL_RGBA,
                _srcSize.width(), _srcSize.height(),
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, _colorbuffer, 0);

  glBindRenderbuffer(GL_RENDERBUFFER_EXT, _depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _srcSize.width(), _srcSize.height());
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depthbuffer);
  int st1 = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  if(st1==GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    Logger::warning( "Init framebuffer so good ");
  }
  else
  {
     if(st1==GL_FRAMEBUFFER_UNSUPPORTED_EXT)
     {
       setFlag( effects, 0 );
       Logger::warning("Init framebuffer failde");
     }
  }
#endif
}

void GlEngine::_drawFramebuffer()
{
#ifndef NO_FRAME_BUFFER
  // Bind default framebuffer and draw contents of our framebuffer
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // black screen

  float x0 =0;
  float x1 = x0+_srcSize.width();
  float y0 = 0;
  float y1 = y0+_srcSize.height();

  glBindTexture(GL_TEXTURE_2D, _colorbuffer);

  glBegin( GL_QUADS );

  glTexCoord2i( 0, 1 );
  glVertex2f( x0, y0 );

  glTexCoord2i( 0, 0 );
  glVertex2f( x0, y1 );

  glTexCoord2i( 1, 0 );
  glVertex2f( x1, y1 );

  glTexCoord2i( 1, 1 );
  glVertex2f( x1, y0 );

  glEnd();
#endif
}

Point GlEngine::cursorPos() const
{
  int x,y;
  SDL_GetMouseState(&x,&y);

  return Point( x, y );
}

void GlEngine::unloadPicture(Picture& ioPicture)
{
  GLuint& texture( ioPicture.textureID() );
  glDeleteTextures(1, &texture );
  SDL_FreeSurface(ioPicture.surface());
  texture = 0;

  ioPicture = Picture();
}

void GlEngine::loadPicture(Picture& ioPicture)
{
   GLuint& texture( ioPicture.textureID() );

   SDL_Surface* pot_surface = SDL_DisplayFormatAlpha( ioPicture.surface() );
   SDL_SetAlpha( pot_surface, 0, 0 );

   SDL_FreeSurface( ioPicture.surface() );

   ioPicture.init( pot_surface, ioPicture.offset() );

   GLenum texture_format;
   GLint nOfColors;

   // get the number of channels in the SDL surface
   nOfColors = pot_surface->format->BytesPerPixel;
   if (nOfColors == 4)     // contains an alpha channel
   {
#ifdef CAESARIA_PLATFORM_ANDROID
      texture_format = GL_RGBA;
#else
      if (pot_surface->format->Rmask == 0x000000ff)
         texture_format = GL_RGBA;
      else
         texture_format = GL_BGRA;
#endif
   }
   else if (nOfColors == 3)     // no alpha channel
   {
#ifdef CAESARIA_PLATFORM_ANDROID
      texture_format = GL_RGB;
#else
      if (pot_surface->format->Rmask == 0x000000ff)
         texture_format = GL_RGB;
      else
         texture_format = GL_BGR;
#endif
   }
   else
   {
      THROW("Invalid image format");
   }

   if (texture == 0)
   {
      // the picture has no texture ID!
      // generate a texture ID
      glGenTextures( 1, &texture );
   }

   // Bind the texture object
   glBindTexture( GL_TEXTURE_2D, texture );

   // Set the texture's stretching properties
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  // Edit the texture object's image data using the information SDL_Surface gives us
  glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, pot_surface->w, pot_surface->h, 0,
                texture_format, GL_UNSIGNED_BYTE, pot_surface->pixels );
}


void GlEngine::startRenderFrame()
{
#ifndef NO_FRAME_BUFFER
  if( getFlag( Engine::effects ) > 0 )
  {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _framebuffer);
  }
#endif

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // black screen
}

void GlEngine::endRenderFrame()
{ 
  if( getFlag( debugInfo ) )
  {
    static Font _debugFont = Font::create( FONT_2 );
    static Picture* pic = Picture::create( Size( 160, 30 ));
    std::string debugText = StringHelper::format( 0xff, "fps:%d call:%d", _lastFps, _drawCall );
    _debugFont.draw( *pic, debugText, 0, 0, true );
    draw( *pic, _screen.width() / 2, 2 );
  }

  if( getFlag( Engine::effects ) > 0 )
  {
    _drawFramebuffer();
  }

  SDL_GL_SwapBuffers(); //Refresh the screen
  _fps++;

  if( DateTime::elapsedTime() - _lastUpdateFps > 1000 )
  {
    _lastUpdateFps = DateTime::elapsedTime();
    _lastFps = _fps;
    _fps = 0;
  }

  _drawCall = 0;
}

void GlEngine::draw(const Picture &picture, const int dx, const int dy, Rect* clipRect)
{
   const GLuint& aTextureID( picture.textureID() );
   if( aTextureID == 0 )
     return;

   _drawCall++;
   float x0 = (float)( dx+picture.offset().x());
   float x1 = x0+picture.width();
   float y0 = (float)(dy-picture.offset().y());
   float y1 = y0+picture.height();

   // Bind the texture to which subsequent calls refer to
   glBindTexture( GL_TEXTURE_2D, aTextureID);
   // Set the texture's stretching properties

#ifndef CAESARIA_PLATFORM_ANDROID
  glBegin( GL_QUADS );

  //Bottom-left vertex (corner)
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2i( 0, 0 ); glVertex2f( x0, y0 );   
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2i( 1, 0 ); glVertex2f( x1, y0 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2i( 1, 1 ); glVertex2f( x1, y1 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2i( 0, 1 ); glVertex2f( x0, y1 );

  glEnd();
#else
  GLfloat vtx1[] = {
    x0, y0,
    x1, y0,
    x1, y1,
    x0, y1
  };

  GLfloat tex1[] = {
    0, 0,
    1, 0,
    1, 1,
    0, 1
  };

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glVertexPointer(3,GL_FLOAT, 0, vtx1 );
  glTexCoordPointer(2, GL_FLOAT, 0, tex1 );
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}

void GlEngine::draw(const Pictures& pictures, const Point& pos, Rect* clipRect)
{
  foreach( it, pictures )
  {
    draw( *it, pos, clipRect );
  }
}

void GlEngine::draw( const Picture &picture, const Point& pos, Rect* clipRect )
{
  draw( picture, pos.x(), pos.y() );
}

void GlEngine::setColorMask( int rmask, int gmask, int bmask, int amask )
{
  _rmask = (rmask ? 1.f : 0.f);
  _gmask = (gmask ? 1.f : 0.f);
  _bmask = (bmask ? 1.f : 0.f);
  _amask = (amask ? 1.f : 0.f);
}

void GlEngine::resetColorMask()
{
  _rmask = _gmask = _bmask = _amask = 1.f;
}

void GlEngine::createScreenshot( const std::string& filename )
{
  Picture* screen = createPicture( screenSize() );
#ifdef CAESARIA_PLATFORM_ANDROID
  glReadPixels( 0, 0, screenSize().width(), screenSize().height(), GL_RGBA, GL_UNSIGNED_BYTE, screen->surface()->pixels);
#else
  glReadPixels( 0, 0, screenSize().width(), screenSize().height(), GL_BGRA, GL_UNSIGNED_BYTE, screen->surface()->pixels);
#endif

  PictureConverter::flipVertical( *screen );

  IMG_SavePNG( filename.c_str(), screen->surface(), -1 );

  deletePicture( screen );
  delete screen;
}

unsigned int GlEngine::fps() const {  return _fps; }

void GlEngine::delay( const unsigned int msec )
{
  SDL_Delay( msec );
}

bool GlEngine::haveEvent( NEvent& event )
{
  SDL_Event sdlEvent;

  if( SDL_PollEvent(&sdlEvent) )
  {
    event = EventConverter::instance().get( sdlEvent );
    return true;
  }

  return false;
}

Engine::Modes GlEngine::modes() const
{
  return Modes();
}

}
