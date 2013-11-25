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
// You should have received a createCopy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#include "picture.hpp"

#include "core/exception.hpp"
#include "core/position.hpp"
#include "core/rectangle.hpp"
#include "gfx/picture_bank.hpp"
#include "gfx/engine.hpp"
#include "core/requirements.hpp"
#include "core/color.hpp"
#include <SDL.h>

// Picture class functions

static const Picture _invalidPicture = Picture();

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

std::string Picture::getName() const
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
  return PictureBank::instance().getPicture( group, id );
}

Picture& Picture::load( const std::string& filename )
{
  return PictureBank::instance().getPicture( filename );
}

Picture* Picture::createCopy() const
{
  if( !_d->surface )
  {
    _CAESARIA_DEBUG_BREAK_IF( "No surface for duplicate" );
    return GfxEngine::instance().createPicture( Size( 100 ) );
  }

  int width = _d->surface->w;
  int height = _d->surface->h;

  SDL_Surface* img = SDL_ConvertSurface( _d->surface, _d->surface->format, SDL_SWSURFACE);
  if (img == NULL) 
  {
    THROW("Cannot make surface, size=" << width << "x" << height);
  }

  Picture* newpic = GfxEngine::instance().createPicture( Size( width, height ) );
  newpic->init(img, _d->offset );

  return newpic;
}

void Picture::draw( const Picture &srcpic, const Rect& srcrect, const Point& pos, bool useAlpha )
{
  draw( srcpic, srcrect, Rect( pos, srcrect.getSize() ), useAlpha );
}

void Picture::draw( const Picture &srcpic, const Rect& srcrect, const Rect& dstrect, bool useAlpha )
{
  SDL_Surface *srcimg = srcpic.getSurface();

  if( !(srcimg && _d->surface) )
  {
    return;
  }

  SDL_Rect srcRect, dstRect;

  srcRect.x = srcrect.getLeft();
  srcRect.y = srcrect.getTop();
  srcRect.w = srcrect.getWidth();
  srcRect.h = srcrect.getHeight();
  dstRect.x = dstrect.getLeft();
  dstRect.y = dstrect.getTop();
  dstRect.w = dstrect.getWidth();
  dstRect.h = dstrect.getHeight();

  if( useAlpha )
  {
    SDL_BlitSurface(srcimg, &srcRect, _d->surface, &dstRect);
  }
  else
  {
    SDL_Surface* tmpSurface = SDL_ConvertSurface( srcimg, _d->surface->format, SDL_SWSURFACE);
    SDL_SetAlpha( tmpSurface, 0, 0 );

    SDL_BlitSurface(tmpSurface, &srcRect, _d->surface, &dstRect);
    SDL_FreeSurface( tmpSurface );
  }
}

void Picture::draw( const Picture &srcpic, const Point& pos, bool useAlpha )
{
  draw( srcpic, Rect( Point( 0, 0 ), srcpic.getSize() ), 
                Rect( pos + Point( srcpic._d->offset.getX(), -srcpic._d->offset.getY() ), srcpic.getSize() ), useAlpha );

}

void Picture::draw( const Picture &srcpic, int x, int y, bool useAlpha/*=true */ )
{
  draw( srcpic, Point( x, y ), useAlpha );
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

int Picture::getPixel(Point pos )
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

void Picture::setPixel(Point pos, const int color)
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
  SDL_Rect sdlRect = { (short)rect.getLeft(), (short)rect.getTop(), (Uint16)rect.getWidth(), (Uint16)rect.getHeight() };

  SDL_FillRect(source, rect.getWidth() > 0 ? &sdlRect : NULL, SDL_MapRGBA( source->format, color.getRed(), color.getGreen(), 
                                                                                           color.getBlue(), color.getAlpha() )); 
  SDL_UnlockSurface(source);
}

Picture* Picture::create( const Size& size )
{
  Picture* ret = GfxEngine::instance().createPicture( size );
  GfxEngine::instance().loadPicture( *ret );

  return ret;
}

const Picture& Picture::getInvalid()
{
  return _invalidPicture;
}
