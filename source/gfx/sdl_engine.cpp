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

#include "sdl_engine.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

#include "IMG_savepng.h"
#include "core/exception.hpp"
#include "core/requirements.hpp"
#include "core/position.hpp"
#include "pictureconverter.hpp"
#include "core/time.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"
#include "core/font.hpp"
#include "core/eventconverter.hpp"
#include "core/foreach.hpp"
#include "gfx/decorator.hpp"
#include "game/settings.hpp"

#ifdef CAESARIA_PLATFORM_MACOSX
#include <dlfcn.h>
#endif

namespace gfx
{

namespace {
  unsigned int drawTime;
  unsigned int drawTimeBatch;
}

class SdlEngine::Impl
{
public:
  typedef struct
  {
    int red;
    int green;
    int blue;
    int alpha;
    bool enabled;

    void reset() { red = green = blue = alpha = 0; enabled = false; }
  }MaskInfo;

  Picture screen;
  PictureRef fpsText;

  SDL_Window *window;
  SDL_Renderer *renderer;

  std::map< int, SDL_Texture* > renderTargets;

  MaskInfo mask;
  unsigned int fps, lastFps;
  unsigned int lastUpdateFps;
  unsigned int drawCall;
  Font debugFont;
};


Picture& SdlEngine::screen(){  return _d->screen; }

SdlEngine::SdlEngine() : Engine(), _d( new Impl )
{
  resetColorMask();

  _d->lastUpdateFps = DateTime::elapsedTime();
  _d->fps = 0;
}

SdlEngine::~SdlEngine(){}

void SdlEngine::deletePicture( Picture* pic )
{
  if( pic )
    unloadPicture( *pic );
}

unsigned int SdlEngine::format() const
{
  return SDL_GetWindowPixelFormat(_d->window);
}

void SdlEngine::debug(const std::string &text, const Point &pos)
{

}

void SdlEngine::init()
{
  Logger::warning( "SDLGraficEngine: init");
  int rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0)
  {
    Logger::warning( utils::format( 0xff, "CRITICAL!!! Unable to initialize SDL: %s", SDL_GetError() ) );
    THROW("SDLGraficEngine: Unable to initialize SDL: " << SDL_GetError());
  }

  Logger::warning( "SDLGraficEngine: ttf init");
  rc = TTF_Init();
  if (rc != 0)
  {
    Logger::warning( utils::format( 0xff, "CRITICAL!!! Unable to initialize ttf: %s", SDL_GetError() ) );
    THROW("SDLGraficEngine: Unable to initialize SDL: " << SDL_GetError());
  }

#ifdef CAESARIA_PLATFORM_MACOSX
  void* cocoa_lib;
  cocoa_lib = dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY );
  void (*nsappload)(void);
  nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad" );
  nsappload();
#endif

  SDL_Window *window;

#ifdef CAESARIA_PLATFORM_ANDROID
  //_srcSize = Size( mode.w, mode.h );
  Logger::warning( utils::format( 0xff, "SDLGraficEngine: Android set mode %dx%d",  _srcSize.width(), _srcSize.height() ) );

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

  window = SDL_CreateWindow( "CaesarIA:android", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _srcSize.width(), _srcSize.height(),
           SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS );

  Logger::warning("SDLGraficEngine:Android init successfull");

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
  unsigned int flags = SDL_WINDOW_OPENGL;
  Logger::warning( utils::format( 0xff, "SDLGraficEngine: set mode %dx%d",  _srcSize.width(), _srcSize.height() ) );

  if(isFullscreen())
  {
    window = SDL_CreateWindow("CaesariA",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        0, 0,
        flags | SDL_WINDOW_FULLSCREEN_DESKTOP);

  }
  else
  {
    window = SDL_CreateWindow("CaesariA",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _srcSize.width(), _srcSize.height(),
        flags);
  }

  if (window == NULL)
  {
    Logger::warning( utils::format( 0xff, "CRITICAL!!! Unable to create SDL-window: %s", SDL_GetError() ) );
    THROW("Failed to create window");
  }

  Logger::warning("SDLGraficEngine: init successfull");
#endif

  int render_version = math::clamp( game::Settings::get( "render_mode" ).toInt(), 0, SDL_GetNumRenderDrivers());
  SDL_Renderer *renderer = SDL_CreateRenderer(window, render_version-1, SDL_RENDERER_ACCELERATED );

  if (renderer == NULL)
  {
    Logger::warning( utils::format( 0xff, "CRITICAL!!! Unable to create renderer: %s", SDL_GetError() ) );
    THROW("Failed to create renderer");
  }

  _virtualSize = _srcSize;
  if( isFullscreen() )
  {
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);
    unsigned int fullscreenVirtualWidth = mode.w;
    unsigned int fullscreenVirtualHeight = mode.h;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    SDL_RenderSetLogicalSize(renderer, fullscreenVirtualWidth, fullscreenVirtualHeight );
    _virtualSize = Size( fullscreenVirtualWidth, fullscreenVirtualHeight );
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_RendererInfo info;
  for( int k=0; k < SDL_GetNumRenderDrivers(); k++ )
  {
    SDL_GetRenderDriverInfo( k, &info );
    Logger::warning( "SDLGraficEngine: availabe render %s ", info.name );
  }

  SDL_GetRendererInfo( renderer, &info );
  Logger::warning( "SDLGraficEngine: init render %s ", info.name );


  SDL_Texture *screenTexture = SDL_CreateTexture(renderer,
                                                 SDL_PIXELFORMAT_ARGB8888,
                                                 SDL_TEXTUREACCESS_TARGET,
                                                 _srcSize.width(), _srcSize.height());

  Logger::warning( "SDLGraficEngine: init successfull");
  _d->screen.init( screenTexture, 0, 0 );
  _d->screen.setOriginRect( Rect( 0, 0, _srcSize.width(), _srcSize.height() ) );

  if( !_d->screen.isValid() )
  {
    THROW("Unable to set video mode: " << SDL_GetError());
  }

  Logger::warning( "SDLGraficEngine: set caption");
  std::string versionStr = utils::format( 0xff, "CaesarIA: SDL build %d [%s:%s]",
                                          CAESARIA_BUILD_NUMBER, CAESARIA_PLATFORM_NAME, CAESARIA_COMPILER_NAME );
  SDL_SetWindowTitle( window, versionStr.c_str() );

  _d->window = window;
  _d->renderer = renderer;

  _d->fpsText.reset( Picture::create( Size( 200, 20 ), 0, true ));
}

void SdlEngine::exit()
{
  TTF_Quit();
  SDL_Quit();
}

void SdlEngine::loadPicture(Picture& ioPicture, bool streaming)
{
  if( !ioPicture.surface() )
  {
    Size size = ioPicture.size();
    Logger::warning( utils::format( 0xff, "SdlEngine:: can't make surface, size=%dx%d", size.width(), size.height() ) );
  }

  SDL_Texture* tx = 0;
  if( streaming )
  {
    tx = SDL_CreateTexture(_d->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, ioPicture.width(), ioPicture.height() );
  }
  else
  {
    tx = SDL_CreateTextureFromSurface(_d->renderer, ioPicture.surface());
  }

  if( !tx )
  {
    Logger::warning( "SdlEngine: cannot create texture from surface" + ioPicture.name() );
  }

  ioPicture.init( tx, ioPicture.surface(), 0 );

  if( streaming )
  {
    ioPicture.update();
  }

  SDL_SetTextureBlendMode( ioPicture.texture(), SDL_BLENDMODE_BLEND );
  SDL_SetSurfaceBlendMode( ioPicture.surface(), SDL_BLENDMODE_BLEND );
}

void SdlEngine::unloadPicture( Picture& ioPicture )
{
  try
  {
    if( ioPicture.surface() ) SDL_FreeSurface( ioPicture.surface() );
    if( ioPicture.texture() ) SDL_DestroyTexture( ioPicture.texture() );
  }
  catch(...)
  {

  }

  ioPicture = Picture();
}

void SdlEngine::startRenderFrame()
{
  drawTime =0;
  drawTimeBatch = 0;
  SDL_RenderClear(_d->renderer);  // black background for a complete redraw
}

void SdlEngine::endRenderFrame()
{
  if( getFlag( Engine::debugInfo ) )
  {
    std::string debugText = utils::format( 0xff, "fps:%d call:%d", _d->lastFps, _d->drawCall );
    _d->fpsText->fill( 0, Rect() );
    _d->debugFont.draw( *_d->fpsText, debugText, Point( 0, 0 ) );
    draw( *_d->fpsText, Point( _d->screen.width() / 2, 2 ) );
  }

  //Refresh the screen
  //SDL_SetRenderTarget( _d->renderer, NULL );

  //SDL_RenderCopyEx(_d->renderer, _d->screen.texture(), 0, 0, 0, 0, SDL_FLIP_HORIZONTAL );
  SDL_RenderPresent(_d->renderer);

  _d->fps++;

  if( DateTime::elapsedTime() - _d->lastUpdateFps > 1000 )
  {
    _d->lastUpdateFps = DateTime::elapsedTime();
    _d->lastFps = _d->fps;
    _d->fps = 0;
  }

  _d->drawCall = 0;

  //Logger::warning( "dt=%d  dtb=%d", drawTime, drawTimeBatch );
}

void SdlEngine::draw(const Picture &picture, const int dx, const int dy, Rect* clipRect )
{
  if( !picture.isValid() )
      return;

  int t = DateTime::elapsedTime();
  _d->drawCall++;

  if( clipRect != 0 )
  {
    SDL_Rect r = { clipRect->left(), clipRect->top(), clipRect->width(), clipRect->height() };
    SDL_RenderSetClipRect( _d->renderer, &r );
  }

  const Impl::MaskInfo& mask = _d->mask;
  SDL_Texture* ptx = picture.texture();
  const Rect& orect = picture.originRect();
  Size picSize = orect.size();
  const Point& offset = picture.offset();

  if( mask.enabled )
  {
    SDL_SetTextureColorMod( ptx, mask.red >> 16, mask.green >> 8, mask.blue );
    SDL_SetTextureAlphaMod( ptx, mask.alpha >> 24 );
  }

  SDL_Rect srcRect = { orect.left(), orect.top(), picSize.width(), picSize.height() };
  SDL_Rect dstRect = { dx+offset.x(), dy-offset.y(), picSize.width(), picSize.height() };

  SDL_RenderCopy( _d->renderer, ptx, &srcRect, &dstRect );

  if( mask.enabled )
  {
    SDL_SetTextureColorMod( ptx, 0xff, 0xff, 0xff );
    SDL_SetTextureAlphaMod( ptx, 0xff );
  }

  if( clipRect != 0 )
  {
    SDL_RenderSetClipRect( _d->renderer, 0 );
  }
  drawTime += DateTime::elapsedTime() - t;
}

void SdlEngine::draw( const Picture& picture, const Point& pos, Rect* clipRect )
{
  draw( picture, pos.x(), pos.y(), clipRect );
}

void SdlEngine::draw( const Pictures& pictures, const Point& pos, Rect* clipRect)
{
  if( pictures.empty() )
      return;

  int t = DateTime::elapsedTime();
  _d->drawCall++;

  if( clipRect != 0 )
  {
    SDL_Rect r = { clipRect->left(), clipRect->top(), clipRect->width(), clipRect->height() };
    SDL_RenderSetClipRect( _d->renderer, &r );
  }

  const Impl::MaskInfo& mask = _d->mask;
  foreach( it, pictures )
  {
    const Picture& picture = *it;
    SDL_Texture* ptx = picture.texture();
    const Rect& orect = picture.originRect();
    Size size = orect.size();
    const Point& offset = picture.offset();

    if( mask.enabled )
    {
      SDL_SetTextureColorMod( ptx, mask.red >> 16, mask.green >> 8, mask.blue );
      SDL_SetTextureAlphaMod( ptx, mask.alpha >> 24 );
    }

    SDL_Rect srcRect = { orect.left(), orect.top(), size.width(), size.height() };
    SDL_Rect dstRect = { pos.x() + offset.x(), pos.y() - offset.y(), size.width(), size.height() };

    SDL_RenderCopy( _d->renderer, ptx, &srcRect, &dstRect );

    if( mask.enabled )
    {
      SDL_SetTextureColorMod( ptx, 0xff, 0xff, 0xff );
      SDL_SetTextureAlphaMod( ptx, 0xff );
    }
  }

  if( clipRect != 0 )
  {
    SDL_RenderSetClipRect( _d->renderer, 0 );
  }
  drawTimeBatch += DateTime::elapsedTime() - t;
}

void SdlEngine::draw(const Picture& pic, const Rect& srcRect, const Rect& dstRect, Rect* clipRect)
{
  if( !pic.isValid() )
      return;

  int t = DateTime::elapsedTime();

  _d->drawCall++;
  SDL_Texture* ptx = pic.texture();

  if( clipRect != 0 )
  {
    SDL_Rect r = { clipRect->left(), clipRect->top(), clipRect->width(), clipRect->height() };
    SDL_RenderSetClipRect( _d->renderer, &r );
  }

  const Impl::MaskInfo& mask = _d->mask;
  if( mask.enabled )
  {
    SDL_SetTextureColorMod( ptx, mask.red >> 16, mask.green >> 8, mask.blue );
    SDL_SetTextureAlphaMod( ptx, mask.alpha >> 24 );
  }

  const Point& offset = pic.offset();

  SDL_Rect srcr = { srcRect.left(), srcRect.top(), srcRect.width(), srcRect.height() };
  SDL_Rect dstr = { dstRect.left()+offset.x(), dstRect.top()-offset.y(), dstRect.width(), dstRect.height() };

  SDL_RenderCopy( _d->renderer, ptx, &srcr, &dstr );

  if( mask.enabled )
  {
    SDL_SetTextureColorMod( ptx, 0xff, 0xff, 0xff );
    SDL_SetTextureAlphaMod( ptx, 0xff );
  }

  if( clipRect != 0 )
  {
    SDL_RenderSetClipRect( _d->renderer, 0 );
  }

  drawTime += DateTime::elapsedTime() - t;
}

void SdlEngine::drawLine(const NColor &color, const Point &p1, const Point &p2)
{
  SDL_SetRenderDrawColor( _d->renderer, color.red(), color.green(), color.blue(), color.alpha() );
  SDL_RenderDrawLine( _d->renderer, p1.x(), p1.y(), p2.x(), p2.y() );

  SDL_SetRenderDrawColor( _d->renderer, 0, 0, 0, 0 );
}

void SdlEngine::setColorMask( int rmask, int gmask, int bmask, int amask )
{
  Impl::MaskInfo& mask = _d->mask;
  mask.red = rmask;
  mask.green = gmask;
  mask.blue = bmask;
  mask.alpha = amask;
  mask.enabled = true;
}

void SdlEngine::resetColorMask() { _d->mask.reset(); }

void SdlEngine::initViewport(int index, Size s)
{
  SDL_Texture*& target = _d->renderTargets[ index ];
  if( target != 0 )
  {
    SDL_DestroyTexture( target );
    target = 0;
  }

  if( s.area() > 0 )
  {
    target = SDL_CreateTexture( _d->renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, s.width(), s.height() );
  }
}

void SdlEngine::setViewport(int index, bool render)
{
  SDL_Texture* target = _d->renderTargets.at( index );
  if( target )
  {
    SDL_SetRenderTarget( _d->renderer, render ? target : 0 );
    if( render )
    {
      SDL_RenderClear(_d->renderer);  // black background for a complete redraw
    }
  }
}

void SdlEngine::drawViewport(int index, Rect r)
{
  SDL_Texture* target = _d->renderTargets[ index ];
  if( target )
  {
    SDL_RenderCopyEx(_d->renderer, target, 0, 0, 0, 0, SDL_FLIP_NONE );
  }
}

void SdlEngine::createScreenshot( const std::string& filename )
{
  SDL_Surface* surface = SDL_CreateRGBSurface( 0, _srcSize.width(), _srcSize.height(), 24, 0, 0, 0, 0 );
  if( surface )
  {
    SDL_RenderReadPixels( _d->renderer, 0, SDL_PIXELFORMAT_BGR24, surface->pixels, surface->pitch );

    IMG_SavePNG( filename.c_str(), surface, -1 );
    SDL_FreeSurface( surface );
  }
}

Engine::Modes SdlEngine::modes() const
{
  Modes ret;

  /* Get available fullscreen/hardware modes */
  int num = SDL_GetNumDisplayModes(0);

  std::set<unsigned int> uniqueModes;

  for (int i = 0; i < num; ++i)
  {
    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, i, &mode) == 0 && mode.w > 640 )
    {
      unsigned int modeHash = (mode.w << 16) + mode.h;
      if( uniqueModes.count( modeHash ) == 0)
      {
        ret.push_back(Size(mode.w, mode.h));
        uniqueModes.insert( modeHash );
      }
    }
  }

  return ret;
}

Point SdlEngine::cursorPos() const
{
  int x,y;
  SDL_GetMouseState(&x,&y);

  return Point( x, y );
}

unsigned int SdlEngine::fps() const {  return _d->fps; }

void SdlEngine::setFlag( int flag, int value )
{
  Engine::setFlag( flag, value );

  if( flag == debugInfo )
  {
    _d->debugFont = Font::create( FONT_2 );
  }
}

void SdlEngine::delay( const unsigned int msec ) { SDL_Delay( std::max<unsigned int>( msec, 0 ) ); }

bool SdlEngine::haveEvent( NEvent& event )
{
  SDL_Event sdlEvent;

  if( SDL_PollEvent(&sdlEvent) )
  {
    event = EventConverter::instance().get( sdlEvent );
    return true;
  }

  return false;
}

}//end namespace gfx
