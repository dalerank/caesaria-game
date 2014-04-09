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

#include "picture.hpp"

#include "core/exception.hpp"
#include "core/position.hpp"
#include "core/rectangle.hpp"
#include "gfx/picture_bank.hpp"
#include "gfx/engine.hpp"
#include "core/requirements.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include <SDL.h>

// Picture class functions

namespace gfx
{

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
  //unsigned int glTextureID;  // texture ID for openGL

  SDL_Surface* getZoomedSurface(SDL_Surface* src, double zoomx, double zoomy);
  void zoomSurface(SDL_Surface* src, SDL_Surface* dst);
};

Picture::Picture() : _d( new Impl )
{
  _d->surface = NULL;
  _d->offset = Point( 0, 0 );
  //_d->glTextureID = 0;
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
  if( _d->surface != 0 )
  {
    _d->size = Size( _d->surface->w, _d->surface->h );
  }
}

void Picture::setOffset( Point offset ) { _d->offset = offset; }
void Picture::setOffset(int x, int y) { _d->offset = Point( x, y ); }
void Picture::addOffset( Point offset ) { _d->offset += offset; }
void Picture::addOffset( int x, int y ) { _d->offset += Point( x, y ); }

SDL_Surface* Picture::getSurface() const{  return _d->surface;}
Point Picture::getOffset() const{  return _d->offset;}
int Picture::width() const{  return _d->size.width();}
int Picture::height() const{  return _d->size.height();}
void Picture::setName(std::string &name){  _d->name = name;}
std::string Picture::name() const{  return _d->name;}
Size Picture::size() const{  return _d->size; }
bool Picture::isValid() const{  return _d->surface != 0;}
Picture& Picture::load( const std::string& group, const int id ){  return PictureBank::instance().getPicture( group, id );}
Picture& Picture::load( const std::string& filename ){  return PictureBank::instance().getPicture( filename );}

Picture* Picture::clone() const
{
  if( !_d->surface )
  {
    Logger::warning( "No surface for clone" );
    return Engine::instance().createPicture( Size( 100 ) );
  }

  int width = _d->surface->w;
  int height = _d->surface->h;

  SDL_Surface* img = SDL_ConvertSurface( _d->surface, _d->surface->format, SDL_SWSURFACE);
  if (img == NULL) 
  {
    THROW("Cannot make surface, size=" << width << "x" << height);
  }

  Picture* newpic = Engine::instance().createPicture( Size( width, height ) );
  newpic->init(img, _d->offset );

  return newpic;
}

void Picture::setAlpha(unsigned char value)
{
  SDL_SetAlpha( _d->surface, SDL_SRCALPHA | SDL_RLEACCEL, value );
}

void Picture::scale(Size size)
{
 /* Picture scaledPic;
  scaledPic.init*/
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

  srcRect.x = srcrect.left();
  srcRect.y = srcrect.top();
  srcRect.w = srcrect.getWidth();
  srcRect.h = srcrect.getHeight();
  dstRect.x = dstrect.left();
  dstRect.y = dstrect.top();
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
  draw( srcpic, Rect( Point( 0, 0 ), srcpic.size() ), 
                Rect( pos + Point( srcpic._d->offset.x(), -srcpic._d->offset.y() ), srcpic.size() ), useAlpha );

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
  if( _d->surface == NULL || pos.x() < 0 || pos.y() < 0
      || pos.x() >= _d->surface->w || pos.y() >= _d->surface->h)
    return 0;

  Uint32 res = 0;
  switch (_d->surface->format->BytesPerPixel)
  {
  case 1:
    // 8bpp
    Uint8 *bufp8;
    bufp8 = (Uint8 *)_d->surface->pixels + pos.y() *_d->surface->pitch + pos.x();
    res = *bufp8;
    break;

  case 2:
    // 15bpp or 16bpp
    Uint16 *bufp16;
    bufp16 = (Uint16 *)_d->surface->pixels + pos.y() *_d->surface->pitch/2 + pos.x();
    res = *bufp16;
    break;

  case 3:
    // 24bpp, very slow!
    THROW("Unsupported graphic mode 24bpp");
    break;

  case 4:
    // 32bpp
    Uint32 *bufp32;
    bufp32 = (Uint32 *)_d->surface->pixels + pos.y()*_d->surface->pitch/4 + pos.x();
    res = *bufp32;
    break;
  }

  return res;
}

void Picture::setPixel(Point pos, const int color)
{
  // validate arguments
  if (_d->surface == NULL || pos.x() < 0 || pos.y() < 0 || pos.x() >= _d->surface->w || pos.y() >= _d->surface->h)
    return;

  switch (_d->surface->format->BytesPerPixel)
  {
  case 1:
    // 8bpp
    Uint8 *bufp8;
    bufp8 = (Uint8 *)_d->surface->pixels + pos.y() * _d->surface->pitch + pos.x();
    *bufp8 = color;
    break;

  case 2:
    // 15bpp or 16bpp
    Uint16 *bufp16;
    bufp16 = (Uint16 *)_d->surface->pixels + pos.y() * _d->surface->pitch / 2 + pos.x();
    *bufp16 = color;
    break;

  case 3:
    // 24bpp, very slow!
    THROW("Unsupported graphic mode 24bpp");
    break;

  case 4:
    // 32bpp
    Uint32 *bufp32;
    bufp32 = (Uint32 *)_d->surface->pixels + pos.y() * _d->surface->pitch/4 + pos.x();
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
  //_d->glTextureID = other._d->glTextureID;  // texture ID for openGL

  _d->offset = other._d->offset;

  return *this;
}

Picture::~Picture(){}

/*unsigned int& Picture::getGlTextureID() const
{
  return _d->glTextureID;
}*/

void Picture::destroy( Picture* ptr )
{
  Engine::instance().deletePicture( ptr );
}

void Picture::fill( const NColor& color, const Rect& rect )
{
  SDL_Surface* source = _d->surface;

  if( _d->surface )
  {
    SDL_LockSurface( source );
    SDL_Rect sdlRect = { (short)rect.left(), (short)rect.top(), (Uint16)rect.getWidth(), (Uint16)rect.getHeight() };

    SDL_FillRect(source, rect.getWidth() > 0 ? &sdlRect : NULL, SDL_MapRGBA( source->format, color.getRed(), color.getGreen(),
                                                                                             color.getBlue(), color.getAlpha() ));
    SDL_UnlockSurface(source);
  }
  else
  {
    Logger::warning( "Picture: surface not loading " + _d->name );
  }
}

Picture* Picture::create( const Size& size )
{
  Picture* ret = Engine::instance().createPicture( size );
  Engine::instance().loadPicture( *ret );

  return ret;
}

const Picture& Picture::getInvalid() {  return _invalidPicture; }

void Picture::Impl::zoomSurface(SDL_Surface* src, SDL_Surface* dst)
{
	unsigned int* src_pointer = static_cast<unsigned int*>(src->pixels);
	unsigned int* src_help_pointer = src_pointer;
	unsigned int* dst_pointer = static_cast<unsigned int*>(dst->pixels);

	int x, y;
	unsigned int *sx_ca, *sy_ca;
	unsigned int sx = static_cast<unsigned int>(0xffff * src->w / dst->w);
	unsigned int sy = static_cast<unsigned int>(0xffff * src->h / dst->h);
	unsigned int sx_c = 0;
	unsigned int sy_c = 0;

	// Allocates memory and calculates row wide&height
	ScopedArrayPtr< unsigned int > sx_a( new unsigned int[dst->w + 1] );
	sx_ca = sx_a.data();
	for(x = 0; x <= dst->w; x++)
	{
		*sx_ca = sx_c;
		sx_ca++;
		sx_c &= 0xffff;
		sx_c += sx;
	}

	ScopedArrayPtr< unsigned int > sy_a( new unsigned int[dst->h + 1] );
	sy_ca = sy_a.data();
	for (y = 0; y <= dst->h; y++)
	{
		*sy_ca = sy_c;
		sy_ca++;
		sy_c &= 0xffff;
		sy_c += sy;
	}
	sy_ca = sy_a.data();

	// Transfers the image data

	if (SDL_MUSTLOCK(src))
	{
		SDL_LockSurface(src);
	}
	if (SDL_MUSTLOCK(dst))
	{
		SDL_LockSurface(dst);
	}

	for (y = 0; y < dst->h; y++)
	{
		src_pointer = src_help_pointer;
		sx_ca = sx_a.data();
		for (x = 0; x < dst->w; x++)
		{
			*dst_pointer = *src_pointer;
			sx_ca++;
			src_pointer += (*sx_ca >> 16);
			dst_pointer++;
		}
		sy_ca++;
		src_help_pointer = (unsigned int*)((unsigned char*)src_help_pointer + (*sy_ca >> 16) * src->pitch);
	}

	if (SDL_MUSTLOCK(dst))
	{
		SDL_UnlockSurface(dst);
	}
	if (SDL_MUSTLOCK(src))
	{
		SDL_UnlockSurface(src);
	}
}

SDL_Surface* Picture::Impl::getZoomedSurface(SDL_Surface * src, double zoomx, double zoomy)
{
	if (src == NULL)
		return NULL;

	SDL_Surface *zoom_src;
	SDL_Surface *zoom_dst;
	int dst_w = std::max( static_cast<int>(round(src->w * zoomx)), 1 );
	int dst_h = std::max( static_cast<int>(round(src->h * zoomy)), 1 );

	// If source surface has no alpha channel then convert it
	if (src->format->Amask == 0)
	{
		zoom_src = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32,
																		0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff );
		SDL_BlitSurface(src, NULL, zoom_src, NULL);
	}
	else
	{
		zoom_src = src;
	}
	// Create destination surface
	zoom_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dst_w, dst_h, 32,
			zoom_src->format->Rmask, zoom_src->format->Gmask,
			zoom_src->format->Bmask, zoom_src->format->Amask);

	// Zoom surface
	zoomSurface(zoom_src, zoom_dst);

	return zoom_dst;
}

}//end namespace gfx
