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


#include "oc3_gfx_sdl_engine.hpp"

#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

#include "oc3_exception.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_requirements.hpp"
#include "oc3_positioni.hpp"
#include "oc3_pictureconverter.hpp"

class GfxSdlEngine::Impl
{
public:
  Picture screen;
  int rmask, gmask, bmask, amask;
  Picture maskedPic;
  std::list<Picture*> createdPics;  // list of all pictures created by the sdl_facade
};


GfxSdlEngine::GfxSdlEngine() : GfxEngine(), _d( new Impl )
{
  resetTileDrawMask();
}

GfxSdlEngine::~GfxSdlEngine()
{
}

void GfxSdlEngine::deletePicture( Picture &pic )
{
  unload_picture( pic );
  delete &pic;
  _d->createdPics.remove(&pic);
}

void GfxSdlEngine::init()
{
  int rc;
  rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());
  rc = TTF_Init();
  if (rc != 0) THROW("Unable to initialize SDL: " << SDL_GetError());

  Uint32 aFlags = 0;
  aFlags |= SDL_DOUBLEBUF;
  aFlags |= SDL_SWSURFACE;

  SDL_Surface* scr = SDL_SetVideoMode(_screen_width, _screen_height, 32, aFlags);  // 32bpp
  _d->screen.init( scr, 0, 0 );
  if( !_d->screen.isValid() ) 
  {
    THROW("Unable to set video mode: " << SDL_GetError());
  }

  SDL_WM_SetCaption( "OpenCaesar 3:"OC3_VERSION, 0 );    
}


void GfxSdlEngine::exit()
{
  TTF_Quit();
  SDL_Quit();
}

/* Convert picture to SDL surface and then put surface into Picture class
 */

void GfxSdlEngine::load_picture(Picture& ioPicture)
{
  // convert pixel format
  SDL_Surface *newImage;
  newImage = SDL_DisplayFormatAlpha(ioPicture._surface);
  SDL_FreeSurface(ioPicture._surface);
  if (newImage == NULL) THROW("Cannot convert surface, maybe out of memory");
  ioPicture._surface = newImage;
}

void GfxSdlEngine::unload_picture(Picture& ioPicture)
{
  SDL_FreeSurface( ioPicture._surface );
  ioPicture._surface = NULL;
}


void GfxSdlEngine::startRenderFrame()
{
  SDL_FillRect( _d->screen.get_surface(), NULL, 0);  // black background for a complete redraw
}


void GfxSdlEngine::endRenderFrame()
{
  SDL_Flip( _d->screen.get_surface() ); //Refresh the screen
}

void GfxSdlEngine::drawPicture(const Picture &picture, const int dx, const int dy)
{
  if( _d->rmask || _d->gmask || _d->bmask )
  {
    PictureConverter::maskColor( _d->maskedPic, picture, _d->rmask, _d->gmask, _d->bmask, _d->amask );
    _d->screen.draw( _d->maskedPic, dx, dy );
  }
  else
  {
    _d->screen.draw( picture, dx, dy );
  }
}

void GfxSdlEngine::drawPicture( const Picture &picture, const Point& pos )
{
  drawPicture( picture, pos.getX(), pos.getY() );
}

void GfxSdlEngine::setTileDrawMask( int rmask, int gmask, int bmask, int amask )
{
  _d->rmask = rmask;
  _d->gmask = gmask;
  _d->bmask = bmask;
  _d->amask = amask;
}

void GfxSdlEngine::resetTileDrawMask()
{
  _d->rmask = _d->gmask = _d->bmask = _d->amask = 0;
}

Picture& GfxSdlEngine::createPicture(const int width, const int height)
{
  SDL_Surface* img;
  const Uint32 flags = 0;
  img = SDL_CreateRGBSurface(flags, width, height, 32, 0,0,0,0);  // opaque picture with default mask
  if (img == NULL)
  {
    THROW("Cannot make surface, size=" << width << "x" << height);
  }

  Picture *pic = new Picture();
  pic->init(img, 0, 0);  // no offset

  _d->createdPics.push_back(pic);
  return *_d->createdPics.back();
}
