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
#include <SDL_ttf.h>

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

#ifdef CAESARIA_PLATFORM_MACOSX
#include <dlfcn.h>
#endif

namespace gfx
{

namespace {
  enum { screenBpp24 = 24, screenBpp32 = 32 };
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
  }MaskInfo;

  Picture screen;
  Picture maskedPic;
  
  MaskInfo mask;
  unsigned int fps, lastFps;  
  unsigned int lastUpdateFps;
  unsigned int drawCall;
  Font debugFont;
  bool showDebugInfo;
};


Picture& SdlEngine::getScreen(){  return _d->screen; }

SdlEngine::SdlEngine() : Engine(), _d( new Impl )
{
  resetTileDrawMask();

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

  unsigned int flags = SDL_DOUBLEBUF | SDL_SWSURFACE;
  flags |= (getFlag( Engine::fullscreen ) > 0 ? SDL_FULLSCREEN : 0);
  int systemBpp = screenBpp32;
    
#ifdef CAESARIA_PLATFORM_MACOSX
  void* cocoa_lib;
  cocoa_lib = dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY );
  void (*nsappload)(void);
  nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad" );
  nsappload();
  systemBpp = screenBpp24;
#endif
 
  Logger::warning( StringHelper::format( 0xff, "GrafixEngine: set mode %dx%d",  _srcSize.width(), _srcSize.height() ) );
  SDL_Surface* scr = SDL_SetVideoMode(_srcSize.width(), _srcSize.height(), systemBpp, flags );  // 32bpp
    
  Logger::warning( "GrafixEngine: init successfull");
  _d->screen.init( scr, Point( 0, 0 ) );
  
  if( !_d->screen.isValid() ) 
  {
    THROW("Unable to set video mode: " << SDL_GetError());
  }

  Logger::warning( "GrafixEngine: set caption");
  SDL_WM_SetCaption( "CaesarIA: "CAESARIA_VERSION, 0 );

  SDL_EnableKeyRepeat(1, 100);
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
  if( !ioPicture.surface() )
  {
    Logger::warning( "GfxSdlEngine: cannot load NULL surface " + ioPicture.name() );
    return;
  }
  SDL_Surface* newImage = SDL_DisplayFormatAlpha( ioPicture.surface() );
  
  if( newImage == NULL ) 
  {
    THROW("Cannot convert surface, maybe out of memory");
  }
  SDL_FreeSurface(ioPicture.surface());

  ioPicture.init( newImage, ioPicture.offset() );
}

void SdlEngine::unloadPicture( Picture& ioPicture )
{
  SDL_FreeSurface( ioPicture.surface() );
  ioPicture = Picture();
}

void SdlEngine::startRenderFrame()
{
  SDL_FillRect( _d->screen.surface(), NULL, 0 );  // black background for a complete redraw
}

void SdlEngine::endRenderFrame()
{
  if( _d->showDebugInfo )
  {
    std::string debugText = StringHelper::format( 0xff, "fps:%d call:%d", _d->lastFps, _d->drawCall );
    _d->debugFont.draw( _d->screen, debugText, _d->screen.width() / 2, 2, false );
  }

  SDL_Flip( _d->screen.surface() ); //Refresh the screen
  _d->fps++;

  if( DateTime::elapsedTime() - _d->lastUpdateFps > 1000 )
  {
    _d->lastUpdateFps = DateTime::elapsedTime();
    _d->lastFps = _d->fps;
    _d->fps = 0;
  }

  _d->drawCall = 0;
}

void SdlEngine::draw(const Picture& picture, const int dx, const int dy, Rect* clipRect )
{
  if( !picture.isValid() )
      return;

  _d->drawCall++;

  Picture& screen = _d->screen;
  if( clipRect != 0 )
  {
    SDL_Rect r = { (short)clipRect->left(), (short)clipRect->top(), (Uint16)clipRect->width(), (Uint16)clipRect->height() };
    SDL_SetClipRect( screen.surface(), &r );
  }

  const Impl::MaskInfo& mask = _d->mask;
  if( mask.red || mask.green|| mask.blue || mask.alpha )
  {
    PictureConverter::maskColor( _d->maskedPic, picture, mask.red, mask.green, mask.blue, mask.alpha );

    screen.draw( _d->maskedPic, dx, dy );
  }
  else
  {
    screen.draw( picture, dx, dy );
  }

  if( clipRect != 0 )
  {
    SDL_SetClipRect( screen.surface(), 0 );
  }
}

void SdlEngine::draw( const Picture &picture, const Point& pos, Rect* clipRect )
{
  draw( picture, pos.x(), pos.y(), clipRect );
}

void SdlEngine::draw(const Pictures& pictures, const Point& pos, Rect* clipRect)
{
  for( Pictures::const_iterator it=pictures.begin(); it != pictures.end(); ++it )
  {
    draw( *it, pos, clipRect );
  }
}

void SdlEngine::setTileDrawMask( int rmask, int gmask, int bmask, int amask )
{
  Impl::MaskInfo& mask = _d->mask;
  mask.red = rmask;
  mask.green = gmask;
  mask.blue = bmask;
  mask.alpha = amask;
}

void SdlEngine::resetTileDrawMask() {  memset( &_d->mask, 0, sizeof( Impl::MaskInfo ) ); }

Picture* SdlEngine::createPicture(const Size& size )
{
  SDL_Surface* img = SDL_CreateRGBSurface( 0, size.width(), size.height(), 32,
                                           0, 0, 0, 0 );

  Logger::warningIf( NULL == img, StringHelper::format( 0xff, "SdlEngine:: can't make surface, size=%dx%d", size.width(), size.height() ) );

  Picture *pic = new Picture();
  pic->init(img, Point( 0, 0 ));  // no offset
  
  return pic;
}

void SdlEngine::createScreenshot( const std::string& filename )
{
  IMG_SavePNG( filename.c_str(), _d->screen.surface(), -1 );
}

Engine::Modes SdlEngine::modes() const
{
  Modes ret;

  /* Get available fullscreen/hardware modes */
  SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

  for(int i=0; modes[i]; ++i)
  {
    ret.push_back( Size( modes[i]->w, modes[i]->h) );
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
