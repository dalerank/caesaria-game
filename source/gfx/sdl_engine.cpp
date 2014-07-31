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
#include <vector>
#include <SDL.h>
#include <ttf/SDL_ttf.h>

#include "IMG_savepng.h"
#include "core/exception.hpp"
#include "core/requirements.hpp"
#include "core/position.hpp"
#include "pictureconverter.hpp"
#include "core/time.hpp"
#include "core/logger.hpp"
#include "core/stringhelper.hpp"
#include "core/font.hpp"
#include "core/eventconverter.hpp"
#include "core/foreach.hpp"
#include "gfx/decorator.hpp"

#ifdef CAESARIA_PLATFORM_MACOSX
#include <dlfcn.h>
#endif

namespace gfx
{

class SdlEngine::Impl
{
public:
  typedef struct
  {
    int red;
    int green;
    int blue;
    int alpha;
  }MaskInfo;

  Picture screen;
  Picture maskedPic;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  MaskInfo mask;
  unsigned int fps, lastFps;  
  unsigned int lastUpdateFps;
  unsigned int drawCall;
  Font debugFont;
  bool showDebugInfo;
};


Picture& SdlEngine::screen(){  return _d->screen; }

SdlEngine::SdlEngine() : Engine(), _d( new Impl )
{
  resetColorMask();

  _d->lastUpdateFps = DateTime::elapsedTime();
  _d->fps = 0;
  _d->showDebugInfo = false;
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

void SdlEngine::init()
{
  Logger::warning( "GrafixEngine: init");
  int rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0)
  {
    Logger::warning( StringHelper::format( 0xff, "Unable to initialize SDL: %d", SDL_GetError() ) );
    THROW("Unable to initialize SDL: " << SDL_GetError());
  }
  
  Logger::warning( "GrafixEngine: ttf init");
  rc = TTF_Init();
  if (rc != 0)
  {
    THROW("Unable to initialize SDL: " << SDL_GetError());
  }
    
#ifdef CAESARIA_PLATFORM_MACOSX
  void* cocoa_lib;
  cocoa_lib = dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY );
  void (*nsappload)(void);
  nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad" );
  nsappload();
#endif
 
  unsigned int flags = SDL_WINDOW_OPENGL;

  Logger::warning( StringHelper::format( 0xff, "GrafixEngine: set mode %dx%d",  _srcSize.width(), _srcSize.height() ) );
    
  SDL_Window *window;

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
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _srcSize.width(), _srcSize.height(),
        flags);
  }

  if (window == NULL)
  {
    THROW("Failed to create window");
  }

  Logger::warning("SDLGraficEngine: init successfull");
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

  if (renderer == NULL)
  {
    THROW("Failed to create renderer");
  }

  if (isFullscreen())
  {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    SDL_RenderSetLogicalSize(renderer, _srcSize.width(), _srcSize.height());
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_Texture *screenTexture = SDL_CreateTexture(renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
    _srcSize.width(), _srcSize.height());

  Logger::warning( "GrafixEngine: init successfull");
  _d->screen.init( screenTexture, Point(0, 0));
  
  if( !_d->screen.isValid() ) 
  {
    THROW("Unable to set video mode: " << SDL_GetError());
  }

  Logger::warning( "GrafixEngine: set caption");
  SDL_SetWindowTitle( window, "CaesarIA: "CAESARIA_VERSION );

  //SDL_EnableKeyRepeat(1, 100);

  _d->window = window;
  _d->renderer = renderer;
  _d->texture = screenTexture;
}

void SdlEngine::exit()
{
  TTF_Quit();
  SDL_Quit();
}

/* Convert picture to SDL surface and then put surface into Picture class
 */
void SdlEngine::loadPicture( Picture& ioPicture )
{
  // convert pixel format
  if( !ioPicture.texture() )
  {
    Logger::warning( "SdlEngine: cannot load NULL surface " + ioPicture.name() );
    return;
  }

  /*SDL_Surface* newImage = SDL_ConvertSurfaceFormat( ioPicture.texture(), SDL_GetWindowPixelFormat(_d->window), 0);
  if( newImage == NULL ) 
  {
    THROW("Cannot convert surface, maybe out of memory");
  }
  SDL_FreeSurface(ioPicture.texture());

  ioPicture.init( newImage, ioPicture.offset() );
  */
}

void SdlEngine::unloadPicture( Picture& ioPicture )
{
  if( ioPicture.surface() ) SDL_FreeSurface( ioPicture.surface() );
  if( ioPicture.texture() ) SDL_DestroyTexture( ioPicture.texture() );

  ioPicture = Picture();
}

void SdlEngine::startRenderFrame()
{
  SDL_RenderClear(_d->renderer);  // black background for a complete redraw
}

void SdlEngine::endRenderFrame()
{
  if( _d->showDebugInfo )
  {
    std::string debugText = StringHelper::format( 0xff, "fps:%d call:%d", _d->lastFps, _d->drawCall );
    _d->debugFont.draw( _d->screen, debugText, _d->screen.width() / 2, 2, false );
  }

  //Refresh the screen
  //SDL_RenderClear(_d->renderer);
//  SDL_RenderCopy(_d->renderer, _d->texture, NULL, NULL);
  SDL_RenderPresent(_d->renderer);

  _d->fps++;

  if( DateTime::elapsedTime() - _d->lastUpdateFps > 1000 )
  {
    _d->lastUpdateFps = DateTime::elapsedTime();
    _d->lastFps = _d->fps;
    _d->fps = 0;
  }

  _d->drawCall = 0;
}

void SdlEngine::draw(const Picture &picture, const int dx, const int dy, Rect* clipRect )
{
  if( !picture.isValid() )
      return;

  _d->drawCall++;

  if( clipRect != 0 )
  {
    SDL_Rect r = { (short)clipRect->left(), (short)clipRect->top(), (Uint16)clipRect->width(), (Uint16)clipRect->height() };
    SDL_RenderSetClipRect( _d->renderer, &r );
  }

  const Impl::MaskInfo& mask = _d->mask;
  bool masked = mask.red || mask.green|| mask.blue || mask.alpha;
  if( masked )
  {
    SDL_SetTextureColorMod( _d->maskedPic.texture(), mask.red >> 16, mask.green >> 8, mask.blue );
  }

  const Size& picSize = picture.size();
  const Point& offset = picture.offset();

  SDL_Rect srcRect = { 0, 0, picSize.width(), picSize.height() };
  SDL_Rect dstRect = { dx+ offset.x(), dy-offset.y(),
                       picSize.width(), picSize.height() };

  SDL_RenderCopy( _d->renderer, picture.texture(), &srcRect, &dstRect );

  if( masked )
    SDL_SetTextureColorMod( picture.texture(), 0, 0, 0 );

  if( clipRect != 0 )
  {
    SDL_RenderSetClipRect( _d->renderer, 0 );
  }
}

void SdlEngine::draw( const Picture& picture, const Point& pos, Rect* clipRect )
{
  draw( picture, pos.x(), pos.y(), clipRect );
}

void SdlEngine::draw( const Pictures& pictures, const Point& pos, Rect* clipRect)
{
  foreach( it, pictures )
  {
    draw( *it, pos, clipRect );
  }
}

void SdlEngine::setColorMask( int rmask, int gmask, int bmask, int amask )
{
  Impl::MaskInfo& mask = _d->mask;
  mask.red = rmask;
  mask.green = gmask;
  mask.blue = bmask;
  mask.alpha = amask;
}

void SdlEngine::resetColorMask()
{
  _d->mask = Impl::MaskInfo();
}

Picture* SdlEngine::createPicture(const Size& size, unsigned char *data, bool mayChange)
{
  SDL_Texture* img = 0;
  if( !data )
  {
    img = SDL_CreateTexture( _d->renderer,
                             SDL_PIXELFORMAT_ARGB8888,
                             mayChange ? SDL_TEXTUREACCESS_STREAMING : SDL_TEXTUREACCESS_STATIC,
                             size.width(), size.height() );
  }
  else
  {
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom( data, size.width(), size.height(), 32, size.width() * 4,
                                                     0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );

    img = SDL_CreateTextureFromSurface(_d->renderer, surface);
    SDL_FreeSurface( surface );
  }

  Logger::warningIf( NULL == img, StringHelper::format( 0xff, "SdlEngine:: can't make surface, size=%dx%d", size.width(), size.height() ) );

  SDL_SetTextureBlendMode( img, SDL_BLENDMODE_BLEND );

  Picture *pic = new Picture();
  pic->init(img, Point( 0, 0 ));  // no offset
  
  return pic;
}

void SdlEngine::createScreenshot( const std::string& filename )
{
  unsigned int* pixes = _d->screen.lock();
  if( pixes )
  {
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom( pixes, _d->screen.width(), _d->screen.height(), 32, _d->screen.width() * 4,
                                                     0, 0, 0, 0 );
    IMG_SavePNG( filename.c_str(), surface, -1 );
    SDL_FreeSurface( surface );
  }
  _d->screen.unlock();
}

Engine::Modes SdlEngine::modes() const
{
  Modes ret;

  /* Get available fullscreen/hardware modes */
  int num = SDL_GetNumDisplayModes(0);

  for (int i = 0; i < num; ++i)
  {
    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, i, &mode) == 0)
    {
      ret.push_back(Size(mode.w, mode.h));
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
    _d->showDebugInfo = value > 0;
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
