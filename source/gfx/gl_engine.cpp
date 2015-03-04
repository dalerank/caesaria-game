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

#ifdef CAESARIA_GL_RENDER

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <SDL.h>
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "core/exception.hpp"
#include "picture.hpp"
#include "core/position.hpp"
#include "core/eventconverter.hpp"
#include "core/foreach.hpp"
#include "vfs/file.hpp"
#include "game/settings.hpp"
#include "core/saveadapter.hpp"
#include <SDL_ttf.h>


#ifndef CAESARIA_PLATFORM_WIN
  #define GL_GLEXT_PROTOTYPES
#endif

#ifdef CAESARIA_PLATFORM_ANDROID
  #define glOrtho glOrthof
  #undef CAESARIA_USE_FRAMEBUFFER
  #include <SDL_opengles.h>
  #define USE_GLES
#else
  #include <SDL_opengl.h>
#endif

#ifdef CAESARIA_USE_FRAMEBUFFER
  #ifndef GL_GLEXT_PROTOTYPES
    #define ASSIGNGLFUNCTION(type,name) name = (type)wglGetProcAddress( #name );
    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLUNIFORM1IPROC glUniform1i;
    PFNGLUNIFORM1FPROC glUniform1f;
    PFNGLUNIFORM2IPROC glUniform2i;
    PFNGLUNIFORM2FPROC glUniform2f;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
    PFNGLGENTEXTURESEXTPROC glGenTexturesEXT;
    PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
    PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
    PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
    PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
    PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
    PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
    PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
  #endif

  #ifndef CAESARIA_PLATFORM_MACOSX
    #define glGenFramebuffers         glGenFramebuffersEXT
    #define glGenTextures             glGenTexturesEXT
    #define glGenRenderbuffers        glGenRenderbuffersEXT
    #define glBindFramebuffer         glBindFramebufferEXT
    #define glBindRenderbuffer        glBindRenderbufferEXT
    #define glRenderbufferStorage     glRenderbufferStorageEXT
    #define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
    #define glCheckFramebufferStatus  glCheckFramebufferStatusEXT
    #define glFramebufferTexture2D    glFramebufferTexture2DEXT
  #endif
#else
  #undef CAESARIA_USE_SHADERS
#endif

#include "core/font.hpp"
#include "pictureconverter.hpp"
#include "core/utils.hpp"
#include "core/time.hpp"
#include "IMG_savepng.h"

namespace gfx{

#ifdef CAESARIA_USE_FRAMEBUFFER

static const char* screenVertexSource = "varying vec2 vTexCoord; \n"
"void main(void) \n"
"{ \n"
   "vTexCoord = gl_MultiTexCoord0; \n"
   "gl_Position = ftransform(); \n"
"} \n";

PREDEFINE_CLASS_SMARTLIST(PostprocFilter,List)
typedef PostprocFilterList Effects;

class PostprocFilter : public ReferenceCounted
{
public:
  static PostprocFilterPtr create();
  void setVariables( const VariantMap& variables );
  void loadProgramm( vfs::Path fragmentShader );

  void setUniformVar( const std::string& name, const Variant& var );

  void begin();
  void bindTexture();
  void end();

private:
  PostprocFilter();
  void _log( unsigned int program );
  unsigned int _program;
  unsigned int _vertexShader, _fragmentShader;
  VariantMap _variables;
};

class EffectManager : public ReferenceCounted
{
public:
  EffectManager();
  void load( vfs::Path effectModel );

  Effects& effects();

private:
  Effects _effects;
};

class FrameBuffer : public ReferenceCounted
{
public:
  FrameBuffer();
  void initialize( Size size );

  void begin();
  void end();

  void draw();
  void draw( Effects& effects );

  bool isOk() const;

private:
  void _createFramebuffer( unsigned int& id );

  unsigned int _framebuffer, _framebuffer2;
  Size _size;
  bool _isOk;
};

void FrameBuffer::_createFramebuffer( unsigned int& id )
{
  unsigned int colorbuffer, depthbuffer;

  if( id != 0 )
  {
    glDeleteTextures( 1, &id );
    id = 0;
  }

#ifndef GL_DRAW_FRAMEBUFFER
  #define GL_DRAW_FRAMEBUFFER               0x8CA9
#endif

  glGenFramebuffers(1, &id);
  glGenTextures(1, &colorbuffer);
  glGenRenderbuffers(1, &depthbuffer);

  glBindFramebuffer(GL_FRAMEBUFFER_EXT, id);

  glBindTexture(GL_TEXTURE_2D, colorbuffer);
  glTexImage2D(	GL_TEXTURE_2D,
                0,
                GL_RGBA,
                _size.width(), _size.height(),
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorbuffer, 0);

  glBindRenderbuffer(GL_RENDERBUFFER_EXT, depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, _size.width(), _size.height());
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
  int st1 = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  if(st1==GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    Logger::warning( "FrameBuffer:: init framebuffer so good ");
  }
  else
  {
     if(st1==GL_FRAMEBUFFER_UNSUPPORTED_EXT)
     {
       _isOk = false;
       Logger::warning("FrameBuffer:: init framebuffer failed");
     }
  }
}

void FrameBuffer::draw()
{
  if( !_isOk )
    return;
  // Bind default framebuffer and draw contents of our framebuffer

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // black screen

  float x0 =0;
  float x1 = x0+_size.width();
  float y0 = 0;
  float y1 = y0+_size.height();

  glBindTexture(GL_TEXTURE_2D, _framebuffer);

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
}

FrameBuffer::FrameBuffer()
{
  _framebuffer = 0;
  _framebuffer2 = 0;
  _isOk = true;
}

void FrameBuffer::initialize(Size size)
{
  _size = size;
  _createFramebuffer( _framebuffer );
  _createFramebuffer( _framebuffer2 );
}

void FrameBuffer::begin()
{
  if( _isOk )
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, _framebuffer);
}

bool FrameBuffer::isOk() const { return _isOk; }

void FrameBuffer::draw( Effects& effects )
{
  foreach( it, effects )
  {
    // Bind default framebuffer and draw contents of our framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _framebuffer2 );
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // black screen

    float x0 =0;
    float x1 = x0+_size.width();
    float y0 = 0;
    float y1 = y0+_size.height();

    glBindTexture(GL_TEXTURE_2D, _framebuffer);

    (*it)->begin();
    (*it)->bindTexture();

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

    (*it)->end();

    std::swap( _framebuffer, _framebuffer2 );
  }
}

PostprocFilterPtr PostprocFilter::create()
{
  PostprocFilterPtr ret( new PostprocFilter() );
  ret->drop();

  return ret;
}

void PostprocFilter::setVariables(const VariantMap& variables)
{
  foreach( i, variables )
  {
    if( i->second.type() == Variant::List )
    {
      VariantList vl = i->second.toList();
      switch( vl.size() )
      {
      case 1: _variables[ i->first ] = i->second;
      case 2:
      {
        PointF p( vl.get( 0 ).toFloat(), vl.get( 1 ).toFloat() );
        if( vl.get( 0).type() == Variant::Int  ) { _variables[ i->first ] = p.toPoint(); }
        else { _variables[ i->first ] = p; }
      }
      case 3: break;
      }
    }
    else
    {
      _variables[ i->first ] = i->second;
    }
  }
}

void PostprocFilter::loadProgramm(vfs::Path fragmentShader)
{
#ifdef CAESARIA_USE_SHADERS
  vfs::NFile file = vfs::NFile::open( fragmentShader );
  if( !file.isOpen() )
    return;

  // Create and compile the vertex shader
  _vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(_vertexShader, 1, &screenVertexSource, NULL);
  glCompileShader(_vertexShader);

  GLint isCompiled = 0;
  glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
    _log( _vertexShader );
    glDeleteShader(_vertexShader); //Don't leak the shader.
    return;
  }

  // Create and compile the fragment shader
  _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  std::string vertStr( file.readAll().toString() );

  const char* fragSrc = vertStr.c_str();
  glShaderSource( _fragmentShader, 1, &fragSrc, NULL);
  glCompileShader(_fragmentShader);

  isCompiled = 0;
  glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
    _log(_fragmentShader); //Don't leak the shader.
    glDeleteShader(_fragmentShader); //Don't leak the shader.
    return;
  }

  // Link the vertex and fragment shader into a shader program
  _program = glCreateProgram();
  glAttachShader(_program, _vertexShader);
  glAttachShader(_program, _fragmentShader);
  glLinkProgram(_program);

  GLint linked;
  glGetProgramiv(_program, GL_LINK_STATUS, &linked);
  if( linked )
  {
    Logger::warning( "GlEngine: sucessful link shader program " + fragmentShader.toString() );
  }
#endif
}

void PostprocFilter::setUniformVar(const std::string& name, const Variant& var)
{
  GLint ii = glGetUniformLocation( _program, name.c_str() );

  switch( var.type() )
  {
  case Variant::Int: glUniform1i( ii, var.toInt() ); break;
  case Variant::Float: glUniform1f( ii, var.toFloat() ); break;
  case Variant::NPoint: { Point p = var.toPoint(); glUniform2i( ii, p.x(), p.y() ); } break;
  case Variant::NPointF: { PointF p = var.toPointF(); glUniform2f( ii, p.x(), p.y() ); } break;

  default: break;
  }
}

void PostprocFilter::begin()
{
  glUseProgram( _program );

  foreach( i, _variables )
    setUniformVar( i->first, i->second );
}


void PostprocFilter::end() { glUseProgram( 0 ); }

PostprocFilter::PostprocFilter() {}

void PostprocFilter::_log( unsigned int program )
{
  GLint maxLength = 0;
  glGetShaderiv(program, GL_INFO_LOG_LENGTH, &maxLength);

  //The maxLength includes the NULL character
  std::vector<char> errorLog(maxLength);
  glGetShaderInfoLog(program, maxLength, &maxLength, &errorLog[0]);

  Logger::warning( errorLog.data() );
}

EffectManager::EffectManager() {}

void EffectManager::load(vfs::Path effectModel)
{
  VariantMap stream = config::load( effectModel );

  VariantMap technique = stream.get( CAESARIA_STR_EXT(technique) ).toMap();

  foreach( pass, technique )
  {
    VariantMap variables = pass->second.toMap();
    PostprocFilterPtr effect = PostprocFilter::create();
    std::string shaderFile = variables.get( "shader" ).toString();
    variables.erase( "shader" );

    effect->loadProgramm( game::Settings::rcpath( shaderFile ) );
    effect->setVariables( variables );

    _effects.push_back( effect );
  }
}

Effects& EffectManager::effects() { return _effects; }

void PostprocFilter::bindTexture()
{
  GLint ii = glGetUniformLocation( _program, "tex");
  glUniform1i( ii, 0 );
}

#endif

class GlEngine::Impl
{
public:
  SDL_GLContext context;
  SDL_Window* window;
  Size viewportSize;
  bool useViewport;

#ifdef CAESARIA_USE_FRAMEBUFFER
  FrameBuffer fb;
  EffectManager effects;
#endif

  PictureRef fpsText;
  Font debugFont;

public:
  void throwIfnoWindow()
  {
    if (window == NULL)
    {
      Logger::warning( utils::format( 0xff, "CRITICAL!!! Unable to create SDL-window: %s", SDL_GetError() ) );
      THROW("Failed to create window");
    }
  }
};

GlEngine::GlEngine() : Engine(), _d( new Impl )
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

  Logger::warning( utils::format( 0xff, "SDLGraficEngine: set mode %dx%d",  _srcSize.width(), _srcSize.height() ) );

#ifdef USE_GLES
  //_srcSize = Size( mode.w, mode.h );
  Logger::warning( utils::format( 0xff, "SDLGraficEngine: Android set mode %dx%d",  _srcSize.width(), _srcSize.height() ) );

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);

  _d->window = SDL_CreateWindow( "CaesarIA:android", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _srcSize.width(), _srcSize.height(),
           SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

  Logger::warning("SDLGraficEngine:Android init successfull");

  _d->throwIfnoWindow();

  SDL_SetWindowFullscreen(_d->window, SDL_TRUE );
  _d->context = SDL_GL_CreateContext(_d->window);

#else
  unsigned int flags = SDL_WINDOW_OPENGL;
  if(isFullscreen())
  {
    _d->window = SDL_CreateWindow("CaesariA",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        0, 0,
        flags | SDL_WINDOW_FULLSCREEN_DESKTOP);

  }
  else
  {
    _d->window = SDL_CreateWindow("CaesariA",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _srcSize.width(), _srcSize.height(),
        flags);
  }

  _d->throwIfnoWindow();
  _d->viewportSize = _srcSize;
  _virtualSize = _srcSize;
  _d->useViewport = false;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  _d->context = SDL_GL_CreateContext(_d->window);

  Logger::warning("SDLGraficEngine: init successfull");
#endif

#ifdef CAESARIA_USE_FRAMEBUFFER
  #ifndef GL_GLEXT_PROTOTYPES
    ASSIGNGLFUNCTION(PFNGLCREATESHADERPROC,glCreateShader)
    ASSIGNGLFUNCTION(PFNGLSHADERSOURCEPROC,glShaderSource)
    ASSIGNGLFUNCTION(PFNGLCOMPILESHADERPROC,glCompileShader)
    ASSIGNGLFUNCTION(PFNGLGETSHADERIVPROC,glGetShaderiv)
    ASSIGNGLFUNCTION(PFNGLUSEPROGRAMPROC,glUseProgram)
    ASSIGNGLFUNCTION(PFNGLUNIFORM1IPROC,glUniform1i)
    ASSIGNGLFUNCTION(PFNGLUNIFORM1FPROC,glUniform1f)
    ASSIGNGLFUNCTION(PFNGLUNIFORM2IPROC,glUniform2i)
    ASSIGNGLFUNCTION(PFNGLUNIFORM2FPROC,glUniform2f)
    ASSIGNGLFUNCTION(PFNGLGETUNIFORMLOCATIONPROC,glGetUniformLocation)
    ASSIGNGLFUNCTION(PFNGLGETSHADERINFOLOGPROC,glGetShaderInfoLog)
    ASSIGNGLFUNCTION(PFNGLDELETESHADERPROC,glDeleteShader)
    ASSIGNGLFUNCTION(PFNGLCREATEPROGRAMPROC,glCreateProgram)
    ASSIGNGLFUNCTION(PFNGLATTACHSHADERPROC,glAttachShader)
    ASSIGNGLFUNCTION(PFNGLLINKPROGRAMPROC,glLinkProgram)
    ASSIGNGLFUNCTION(PFNGLGETPROGRAMIVPROC,glGetProgramiv)
    ASSIGNGLFUNCTION(PFNGLGENFRAMEBUFFERSEXTPROC,glGenFramebuffersEXT)
    ASSIGNGLFUNCTION(PFNGLGENTEXTURESEXTPROC,glGenTexturesEXT)
    ASSIGNGLFUNCTION(PFNGLGENRENDERBUFFERSEXTPROC,glGenRenderbuffersEXT)
    ASSIGNGLFUNCTION(PFNGLBINDFRAMEBUFFEREXTPROC,glBindFramebufferEXT)
    ASSIGNGLFUNCTION(PFNGLFRAMEBUFFERTEXTURE2DEXTPROC,glFramebufferTexture2DEXT)
    ASSIGNGLFUNCTION(PFNGLBINDRENDERBUFFEREXTPROC,glBindRenderbufferEXT)
    ASSIGNGLFUNCTION(PFNGLRENDERBUFFERSTORAGEEXTPROC,glRenderbufferStorageEXT)
    ASSIGNGLFUNCTION(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC,glFramebufferRenderbufferEXT)
    ASSIGNGLFUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC,glCheckFramebufferStatusEXT)
  #endif //GL_GLEXT_PROTOTYPES
#endif //CAESARIA_USE_FRAMEBUFFER

  SDL_DisplayMode mode;
  SDL_GetCurrentDisplayMode(0, &mode);
  Logger::warning( "Screen bpp: %d", SDL_BITSPERPIXEL(mode.format));
  Logger::warning( "Vendor     : %s", glGetString(GL_VENDOR));
  Logger::warning( "Renderer   : %s", glGetString(GL_RENDERER));
  Logger::warning( "Version    : %s", glGetString(GL_VERSION));
  Logger::warning( "Extensions : %n", glGetString(GL_EXTENSIONS));

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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Logger::warning( "GrafixEngine: set caption");
  std::string versionStr = utils::format(0xff, "CaesarIA: OpenGL %d.%d R%d [%s:%s]",
                                                 CAESARIA_VERSION_MAJOR, CAESARIA_VERSION_MINOR, CAESARIA_VERSION_REVSN,
                                                 CAESARIA_PLATFORM_NAME, CAESARIA_COMPILER_NAME );
  SDL_SetWindowTitle( _d->window, versionStr.c_str() );

  //!!!!!
#ifdef CAESARIA_USE_FRAMEBUFFER
  if( getFlag( Engine::effects ) > 0 )
  {
    _d->fb.initialize( _srcSize );
    _d->effects.load( SETTINGS_RC_PATH( opengl_opts ) );
  }
#endif

  _d->fpsText.reset( Picture::create( Size( 200, 20 ), 0, true ));
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

void GlEngine::setFlag( int flag, int value )
{
  Engine::setFlag( flag, value );

  if( flag == debugInfo )
  {
    _d->debugFont = Font::create( FONT_2 );
  }
}

Picture* GlEngine::createPicture( const Size& size )
{
  SDL_Surface* img = SDL_CreateRGBSurface( 0, size.width(), size.height(), 32,
                                           0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );

  Logger::warningIf( NULL == img, utils::format( 0xff, "GlEngine:: can't make surface, size=%dx%d", size.width(), size.height() ) );

  Picture *pic = new Picture();
  pic->init( 0, img, 0 );  // no offset

  return pic;
}

Picture& GlEngine::screen(){  return _screen; }

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

/*static int power_of_2(int input)
{
    int value = 1;

    while (value < input)
    {
        value <<= 1;
    }
    return value;
}*/

void GlEngine::loadPicture(Picture& ioPicture, bool streamed)
{
  GLuint& texture( ioPicture.textureID() );

  SDL_Surface* surface = ioPicture.surface(); //SDL_DisplayFormatAlpha( ioPicture.surface() );
  //SDL_SetAlpha( surface, 0, 0 );

  //SDL_FreeSurface( ioPicture.surface() );

  //ioPicture.init( 0, ioPicture.surface(),  );

  GLenum texture_format;
  GLint nOfColors;

  // get the number of channels in the SDL surface
  nOfColors = surface->format->BytesPerPixel;
  if (nOfColors == 4)     // contains an alpha channel
  {
#ifdef USE_GLES
      texture_format = GL_RGBA;
#else
    if (surface->format->Rmask == 0x000000ff)
       texture_format = GL_RGBA;
    else
       texture_format = GL_BGRA;
#endif
  }
  else if (nOfColors == 3)     // no alpha channel
  {
#ifdef USE_GLES
      texture_format = GL_RGB;
#else
    if (surface->format->Rmask == 0x000000ff)
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
  glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  // Edit the texture object's image data using the information SDL_Surface gives us
#ifdef USE_GLES
  int texture_w = power_of_2(surface->w);
  int texture_h = power_of_2(surface->h);

  glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, texture_w, texture_h, 0,
                texture_format, GL_UNSIGNED_BYTE, surface->pixels );
#else
  glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                texture_format, GL_UNSIGNED_BYTE, surface->pixels );
#endif
}

void GlEngine::startRenderFrame()
{
#ifdef CAESARIA_USE_FRAMEBUFFER
  if( getFlag( Engine::effects ) > 0 )
  {
    _d->fb.begin();
  }
#endif
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // black screen
}

void GlEngine::endRenderFrame()
{
  if( getFlag( Engine::debugInfo ) )
  {
    std::string debugText = utils::format( 0xff, "fps:%d call:%d", _lastFps, _drawCall );
    _d->fpsText->fill( 0, Rect() );
    _d->debugFont.draw( *_d->fpsText, debugText, Point( 0, 0 ) );
    draw( *_d->fpsText, Point( _srcSize.width() / 2, 2 ) );
  }

#ifdef CAESARIA_USE_FRAMEBUFFER
  if( getFlag( Engine::effects ) > 0 )
  {
    _d->fb.draw( _d->effects.effects() );
    _d->fb.draw();
  }
#endif

  SDL_GL_SwapWindow(_d->window);
  _fps++;

  if( DateTime::elapsedTime() - _lastUpdateFps > 1000 )
  {
    _lastUpdateFps = DateTime::elapsedTime();
    _lastFps = _fps;
    _fps = 0;
  }

  _drawCall = 0;
}

void GlEngine::initViewport(int index, Size s)
{
#ifdef CAESARIA_USE_FRAMEBUFFER
  _d->viewportSize = s;
#endif
}

void GlEngine::setViewport(int, bool render)
{
  _d->useViewport = render;
}

void GlEngine::drawViewport(int, Rect r)
{

}

void GlEngine::draw(const Picture& picture, const int dx, const int dy, Rect* clipRect)
{
  const GLuint& aTextureID( picture.textureID() );
  if( aTextureID == 0 )
    return;

  _drawCall++;

  const Rect& orect = picture.originRect();
  Size picSize = orect.size();
  const Point& offset = picture.offset();

  PointF scale( 1.f, 1.f );

  if( _d->useViewport )
  {
    scale = PointF( _d->viewportSize.width() / (float)_srcSize.width(),
                    _d->viewportSize.height() / (float)_srcSize.height() );
  }

  float x0 = (float)( dx + offset.x() ) * scale.x();
  float x1 = x0 + picSize.width() * scale.x();
  float y0 = (float)(dy - offset.y()) * scale.y();
  float y1 = y0 + picSize.height() * scale.y();

  glBindTexture( GL_TEXTURE_2D, aTextureID);
  float imageWidth, imageHeight;
  glGetTexLevelParameterfv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&imageWidth);
  glGetTexLevelParameterfv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&imageHeight);

  if( imageWidth <= 0 || imageHeight <= 0 )
    return;

  float ox0 = orect.left() / imageWidth;
  float oy0 = orect.top() / imageHeight;
  float ox1 = orect.right() / imageWidth;
  float oy1 = orect.bottom() / imageHeight;
#ifdef USE_GLES
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

  glVertexPointer(3, GL_FLOAT, 0, vtx1 );
  glTexCoordPointer(2, GL_FLOAT, 0, tex1 );
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
  // Bind the texture to which subsequent calls refer to

  glBegin( GL_QUADS );

  //Bottom-left vertex (corner)
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox0, oy0 ); glVertex2f( x0, y0 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox1, oy0 ); glVertex2f( x1, y0 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox1, oy1 ); glVertex2f( x1, y1 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox0, oy1 ); glVertex2f( x0, y1 );

  glEnd();
#endif
}

void GlEngine::draw(const Pictures& pictures, const Point& pos, Rect* clipRect)
{
  foreach( it, pictures )
  {
    draw( *it, pos, clipRect );
  }
}

void GlEngine::drawLine(const NColor& color, const Point& p1, const Point& p2)
{
  int i=0;
}

void GlEngine::draw( const Picture &picture, const Point& pos, Rect* clipRect )
{
  draw( picture, pos.x(), pos.y() );
}

void GlEngine::draw(const Picture& picture, const Rect& src, const Rect& dst, Rect* clipRect)
{
  const GLuint& aTextureID( picture.textureID() );
  if( aTextureID == 0 )
    return;

  _drawCall++;
  const Point& offset = picture.offset();

  float x0 = dst.left() + offset.x();
  float x1 = dst.right() + offset.x();
  float y0 = dst.top() - offset.y();
  float y1 = dst.bottom() - offset.y();

  glBindTexture( GL_TEXTURE_2D, aTextureID);

  float imageWidth, imageHeight;
  glGetTexLevelParameterfv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&imageWidth);
  glGetTexLevelParameterfv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&imageHeight);

  if( imageWidth <= 0 || imageHeight <= 0 )
    return;

  float ox0 = src.left() / imageWidth;
  float oy0 = src.top() / imageHeight;
  float ox1 = src.right() / imageWidth;
  float oy1 = src.bottom() / imageHeight;
#ifdef USE_GLES
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

  glVertexPointer(3, GL_FLOAT, 0, vtx1 );
  glTexCoordPointer(2, GL_FLOAT, 0, tex1 );
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
  // Bind the texture to which subsequent calls refer to

  glBegin( GL_QUADS );

  //Bottom-left vertex (corner)
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox0, oy0 ); glVertex2f( x0, y0 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox1, oy0 ); glVertex2f( x1, y0 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox1, oy1 ); glVertex2f( x1, y1 );
  glColor4f( _rmask, _gmask, _bmask, _amask ); glTexCoord2f( ox0, oy1 ); glVertex2f( x0, y1 );

  glEnd();
#endif
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
#ifdef USE_GLES
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
  Modes ret;

  /* Get available fullscreen/hardware modes */
  int num = SDL_GetNumDisplayModes(0);

  for (int i = 0; i < num; ++i)
  {
    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, i, &mode) == 0 && mode.w > 640 )
    {
      ret.push_back(Size(mode.w, mode.h));
    }
  }

  return ret;
}

}

#endif
