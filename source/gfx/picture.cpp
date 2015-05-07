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

#include "picture.hpp"

#include "core/exception.hpp"
#include "core/position.hpp"
#include "core/rectangle.hpp"
#include "gfx/picture_bank.hpp"
#include "gfx/engine.hpp"
#include "core/requirements.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include "core/time.hpp"
#include "core/timer.hpp"
#include <SDL.h>

// Picture class functions

namespace gfx
{

class Impl : public ReferenceCounted
{
public:
  SDL_Surface* surface;
  SDL_Texture* texture;  // for SDL surface
  std::string name; // for game save
  unsigned int opengltx;
  Rect orect;

  Impl& operator=(const Impl& other)
  {
    surface = other.surface;
    texture = other.texture;
    name = other.name;
    opengltx = other.opengltx;
    orect = other.orect;
  }

  ~Impl()
  {
    if( surface ) SDL_FreeSurface( surface );
    if( texture ) SDL_DestroyTexture( texture );
    surface = 0;
    texture = 0;
  }
};

static const Picture _invalidPicture = Picture();

Picture::Picture() : _d( new Impl )
{  
  _d->texture = NULL;
  _d->name = "";
  _d->surface = 0;
  _d->opengltx = 0;
  _offset = Point( 0, 0 );
}

Picture::Picture( const Picture& other ) : _d( new Impl )
{
  *this = other;
}

void Picture::init(SDL_Texture *texture, SDL_Surface* srf, unsigned int ogltx)
{
  _d->texture = texture;
  _d->surface = srf;
  _d->opengltx = ogltx;
}

void Picture::setOffset(const Point &offset ) { _offset = offset; }
void Picture::setOffset(int x, int y)         { _offset = Point( x, y ); }
void Picture::setOriginRect(const Rect& rect) { _d->orect = rect; }
void Picture::addOffset( const Point& offset ){ _offset += offset; }
void Picture::addOffset( int x, int y )       { _offset += Point( x, y ); }
const Rect& Picture::originRect() const       { return _d->orect; }
SDL_Texture* Picture::texture() const         { return _d->texture;}
SDL_Surface* Picture::surface() const         { return _d->surface;  }
unsigned int Picture::textureID() const       { return _d->opengltx; }
unsigned int& Picture::textureID()            { return _d->opengltx; }
const Point& Picture::offset() const          { return _offset;}
int Picture::width() const                    { return _d->orect.width();}
int Picture::height() const                   { return _d->orect.height();}
int Picture::pitch() const                    { return width() * 4; }
void Picture::setName(const std::string &name){ _d->name = name;}
std::string Picture::name() const             { return _d->name;}
Size Picture::size() const                    { return _d->orect.size(); }
unsigned int Picture::sizeInBytes() const     { return size().area() * 4; }
Picture& Picture::load( const std::string& group, const int id ){  return PictureBank::instance().getPicture( group, id );}
Picture& Picture::load(const std::string& filename ) { return PictureBank::instance().getPicture( filename ); }
bool Picture::isValid() const                 { return (_d->texture || _d->opengltx); }

void Picture::setAlpha(unsigned char value)
{
  if( _d->texture )
  {
    SDL_SetTextureAlphaMod( _d->texture, value );
  }

  if( _d->surface )
  {
    SDL_SetSurfaceAlphaMod( _d->surface, value );
  }
}

unsigned int* Picture::lock()
{
  /*if( _d->texture )
  {
    int a;
    SDL_QueryTexture( _d->texture, 0, &a, 0, 0 );
    if( a == SDL_TEXTUREACCESS_STREAMING )
    {
      unsigned int* pixels;
      int pitch;
      int rc = SDL_LockTexture(_d->texture, 0, (void**)&pixels, &pitch );
      if (rc < 0)
      {
        Logger::warning( "Picture: cannot lock texture: %s", SDL_GetError() );
        return 0;
      }

      return pixels;
    }
  }*/

  if( _d->surface )
  {
    if( SDL_MUSTLOCK(_d->surface) )
    {
      SDL_LockSurface(_d->surface);      
    }
    return (unsigned int*)_d->surface->pixels;
  }  

  return 0;
}

void Picture::unlock()
{
  /*if( _d->texture )
  {
    int a;
    SDL_QueryTexture( _d->texture, 0, &a, 0, 0 );
    if( a == SDL_TEXTUREACCESS_STREAMING )
    {
      SDL_UnlockTexture(_d->texture);
    }
  }
  else if( _d->surface )*/
  {
    if( SDL_MUSTLOCK(_d->surface) )
    {
      SDL_UnlockSurface(_d->surface);
    }
  }
}

Picture& Picture::operator=( const Picture& other )
{
  _d = other._d;
  _offset = other._offset;

  return *this;
}

Picture::~Picture(){}

void Picture::update()
{
  if( _d->texture && _d->surface )
  {
    SDL_UpdateTexture(_d->texture, 0, _d->surface->pixels, _d->surface->pitch );
    return;
  }

  if( _d->surface && _d->opengltx > 0 )
  {    
    Engine::instance().loadPicture( *this, false );
  }
}

void Picture::fill( const NColor& color, Rect rect )
{
  if( _d->surface )
  {
    SDL_Rect r = { rect.left(), rect.top(), rect.width(), rect.height() };
    SDL_FillRect( _d->surface, rect.width() == 0 ? 0 : &r, color.color );
  }
  else
  {
    Logger::warning( "Picture: surface not loading " + _d->name );
  }
}

Picture Picture::create(const Size& size, unsigned char* data, bool mayChange)
{
  Picture ret;
  pic._d->orect = Rect( 0, 0, size.width(), size.height() );

  if( data )
  {
    pic._d->surface = SDL_CreateRGBSurfaceFrom( data, size.width(), size.height(), 32, size.width() * 4,
                                                 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );
  }
  else
  {
    pic._d->surface = SDL_CreateRGBSurface( 0, size.width(), size.height(), 32,
                                             0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );
    SDL_FillRect( pic._d->surface, 0, 0 );
  }

  Engine::instance().loadPicture( ret, mayChange );
  if( !mayChange )
  {
    SDL_FreeSurface( ret._d->surface );
    ret._d->surface = 0;
  }

  return ret;
}

const Picture& Picture::getInvalid() {  return _invalidPicture; }

}//end namespace gfx
