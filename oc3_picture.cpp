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

#include "oc3_picture.hpp"
#include <SDL_image.h>

#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_rectangle.hpp"
#include "oc3_picture_bank.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_requirements.hpp"
#include "oc3_color.hpp"

// Picture class functions

class Picture::Impl
{
public:
  // the image is shifted when displayed
  Point offset;

  Size size;

  // for game save
  std::string name;

  // for SDL surface
  SDL_Surface* surface;

  // for OPEN_GL surface
  unsigned int glTextureID;  // texture ID for openGL
};

Picture::Picture() : _d( new Impl )
{
  _d->surface = NULL;
  _d->offset = Point( 0, 0 );
  _d->glTextureID = 0;
  _d->size = Size( 0 );
  _d->name = "";
}

Picture::Picture( const Picture& other ) : _d( new Impl )
{
  *this = other;
}

void Picture::init(SDL_Surface *surface, const Point& offset )
{
  _d->surface = surface;
  _d->offset = offset;
  _d->size = Size( _d->surface->w, _d->surface->h );
}

void Picture::setOffset(const int xoffset, const int yoffset)
{
  _d->offset = Point( xoffset, yoffset );
}

void Picture::setOffset( const Point& offset )
{
  _d->offset = offset;
}

void Picture::addOffset(const int dx, const int dy)
{
  _d->offset += Point( dx, dy );
}

SDL_Surface* Picture::getSurface() const
{
  return _d->surface;
}

Point Picture::getOffset() const
{
  return _d->offset;
}

int Picture::getWidth() const
{
  return _d->size.getWidth();
}
int Picture::getHeight() const
{
  return _d->size.getHeight();
}

void Picture::setName(std::string &name)
{
  _d->name = name;
}

std::string Picture::getName()
{
  return _d->name;
}

Size Picture::getSize() const
{
  return _d->size;
}

bool Picture::isValid() const
{
  return _d->surface != 0;
}

Picture& Picture::load( const std::string& group, const int id )
{
  return PicLoader::instance().getPicture( group, id );
}

Picture& Picture::load( const std::string& filename )
{
  return PicLoader::instance().getPicture( filename );
}

Picture* Picture::copy() const
{
  if( !_d->surface )
  {
    _OC3_DEBUG_BREAK_IF( true && "No surface" );
    return GfxEngine::instance().createPicture( Size( 100 ) );
  }

  int width = _d->surface->w;
  int height = _d->surface->h;

  SDL_Surface* img = 0;
  img = SDL_ConvertSurface( _d->surface, _d->surface->format, SDL_SWSURFACE);
  if (img == NULL) 
  {
    THROW("Cannot make surface, size=" << width << "x" << height);
  }

  Picture* newpic = GfxEngine::instance().createPicture( Size( width, height ) );
  newpic->init(img, _d->offset );

  return newpic;
}

void Picture::draw( const Picture &srcpic, const int dx, const int dy )
{
  SDL_Surface *srcimg = srcpic.getSurface();

  if( !srcimg || !_d->surface )
  {
    return;
  }

  SDL_Rect src, dst;

  src.x = 0;
  src.y = 0;
  src.w = srcimg->w;
  src.h = srcimg->h;
  dst.x = dx + srcpic._d->offset.getX();
  dst.y = dy - srcpic._d->offset.getY();
  dst.w = src.w;
  dst.h = src.h;

  SDL_BlitSurface(srcimg, &src, _d->surface, &dst);
}

void Picture::draw( const Picture &srcpic, const Rect& srcrect, const Point& pos )
{
  SDL_Surface *srcimg = srcpic.getSurface();

  if( !srcimg || !_d->surface )
  {
    return;
  }

  SDL_Rect src, dst;

  src.x = srcrect.UpperLeftCorner.getX();
  src.y = srcrect.UpperLeftCorner.getY();
  src.w = srcrect.getWidth();
  src.h = srcrect.getHeight();
  dst.x = pos.getX();
  dst.y = pos.getY();
  dst.w = src.w;
  dst.h = src.h;

  SDL_BlitSurface(srcimg, &src, _d->surface, &dst);
}

void Picture::draw( const Picture &srcpic, const Rect& srcrect, const Rect& dstrect )
{
  SDL_Surface *srcimg = srcpic.getSurface();

  if( !srcimg || !_d->surface )
  {
    return;
  }

  SDL_Rect src, dst;

  src.x = srcrect.getLeft();
  src.y = srcrect.getTop();
  src.w = srcrect.getWidth();
  src.h = srcrect.getHeight();
  dst.x = dstrect.getLeft();
  dst.y = dstrect.getTop();
  dst.w = dstrect.getWidth();
  dst.h = dstrect.getHeight();

  SDL_BlitSurface(srcimg, &src, _d->surface, &dst);
}

void Picture::draw( const Picture &srcpic, const Point& pos )
{
  draw( srcpic, pos.getX(), pos.getY() );
}

void Picture::lock()
{
  if (SDL_MUSTLOCK(_d->surface))
  {
    int rc = SDL_LockSurface(_d->surface);
    if (rc < 0) THROW("Cannot lock surface: " << SDL_GetError());
  }
}

void Picture::unlock()
{
  if (SDL_MUSTLOCK(_d->surface))
  {
    SDL_UnlockSurface(_d->surface);
  }
}

int Picture::getPixel(const Point& pos )
{
  // validate arguments
  if( _d->surface == NULL || pos.getX() < 0 || pos.getY() < 0 
      || pos.getX() >= _d->surface->w || pos.getY() >= _d->surface->h)
    return 0;

  Uint32 res = 0;
  switch (_d->surface->format->BytesPerPixel)
  {
  case 1:
    // 8bpp
    Uint8 *bufp8;
    bufp8 = (Uint8 *)_d->surface->pixels + pos.getY() *_d->surface->pitch + pos.getX();
    res = *bufp8;
    break;

  case 2:
    // 15bpp or 16bpp
    Uint16 *bufp16;
    bufp16 = (Uint16 *)_d->surface->pixels + pos.getY() *_d->surface->pitch/2 + pos.getX();
    res = *bufp16;
    break;

  case 3:
    // 24bpp, very slow!
    THROW("Unsupported graphic mode 24bpp");
    break;

  case 4:
    // 32bpp
    Uint32 *bufp32;
    bufp32 = (Uint32 *)_d->surface->pixels + pos.getY()*_d->surface->pitch/4 + pos.getX();
    res = *bufp32;
    break;
  }

  return res;
}

void Picture::setPixel(const Point& pos, const int color)
{
  // validate arguments
  if (_d->surface == NULL || pos.getX() < 0 || pos.getY() < 0 || pos.getX() >= _d->surface->w || pos.getY() >= _d->surface->h)
    return;

  switch (_d->surface->format->BytesPerPixel)
  {
  case 1:
    // 8bpp
    Uint8 *bufp8;
    bufp8 = (Uint8 *)_d->surface->pixels + pos.getY() * _d->surface->pitch + pos.getX();
    *bufp8 = color;
    break;

  case 2:
    // 15bpp or 16bpp
    Uint16 *bufp16;
    bufp16 = (Uint16 *)_d->surface->pixels + pos.getY() * _d->surface->pitch / 2 + pos.getX();
    *bufp16 = color;
    break;

  case 3:
    // 24bpp, very slow!
    THROW("Unsupported graphic mode 24bpp");
    break;

  case 4:
    // 32bpp
    Uint32 *bufp32;
    bufp32 = (Uint32 *)_d->surface->pixels + pos.getY() * _d->surface->pitch/4 + pos.getX();
    *bufp32 = color;
    break;
  }
}

Picture& Picture::operator=( const Picture& other )
{
  _d->size = other._d->size;
  // for game save
  _d->name = other._d->name;

  // for SDL surface
  _d->surface = other._d->surface;

  // for OPEN_GL surface
  _d->glTextureID = other._d->glTextureID;  // texture ID for openGL

  _d->offset = other._d->offset;

  return *this;
}

Picture::~Picture()
{

}

unsigned int& Picture::getGlTextureID() const
{
  return _d->glTextureID;
}

void Picture::destroy( Picture* ptr )
{
  GfxEngine::instance().deletePicture( ptr );
}

void Picture::fill( const NColor& color, const Rect& rect )
{
  SDL_Surface* source = _d->surface;

  SDL_LockSurface( source );
  SDL_Rect sdlRect = { rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight() };

  SDL_FillRect(source, rect.getWidth() > 0 ? &sdlRect : NULL, SDL_MapRGBA(source->format, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() )); 
  SDL_UnlockSurface(source);
}

Picture* Picture::create( const Size& size )
{
  Picture* ret = GfxEngine::instance().createPicture( size );
  GfxEngine::instance().loadPicture( *ret );

  return ret;
}