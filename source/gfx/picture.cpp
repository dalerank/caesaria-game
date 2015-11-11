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
#include "gfx/IMG_savepng.h"
#include "gfx/engine.hpp"
#include "core/requirements.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include "core/time.hpp"
#include "core/timer.hpp"
#include "pictureimpl.hpp"

// Picture class functions
namespace gfx
{

static const Picture _invalidPicture = Picture();

Picture::Picture() : _d( new PictureImpl )
{  
  _d->drop();
  _d->texture = NULL;
  _d->surface = 0;
  _d->opengltx = 0;
  _name = "";
  _offset = Point( 0, 0 );
}

Picture::Picture( const Picture& other ) : _d( new PictureImpl )
{
  _d->drop();
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
void Picture::setOriginRect(const Rect& rect) { _orect = rect; }
void Picture::addOffset( const Point& offset ){ _offset += offset; }
void Picture::addOffset( int x, int y )       { _offset += Point( x, y ); }
const Rect& Picture::originRect() const       { return _orect; }
SDL_Texture* Picture::texture() const         { return _d->texture;}
SDL_Surface* Picture::surface() const         { return _d->surface;  }
unsigned int Picture::textureID() const       { return _d->opengltx; }
unsigned int& Picture::textureID()            { return _d->opengltx; }
const Point& Picture::offset() const          { return _offset;}
int Picture::width() const                    { return _orect.width();}
int Picture::height() const                   { return _orect.height();}
int Picture::pitch() const                    { return width() * 4; }
void Picture::setName(const std::string &name){ _name = name;}
const std::string& Picture::name() const      { return _name;}
Size Picture::size() const                    { return _orect.size(); }
unsigned int Picture::sizeInBytes() const     { return size().area() * 4; }
bool Picture::isValid() const                 { return (_d->texture || _d->opengltx); }

void Picture::save(const std::string& filename)
{
  if( _d->surface )
    IMG_SavePNG( filename.c_str(), _d->surface, -1 );
}

#ifndef CAESARIA_DISABLE_PICTUREBANK
void Picture::load( const std::string& group, const int id )
{
  *this = PictureBank::instance().getPicture( group, id );
}

void Picture::load( const std::string& filename )
{
  *this = PictureBank::instance().getPicture( filename );
}
#endif

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
  _name = other._name;
  _orect = other._orect;
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

#ifndef CAESARIA_DISABLE_PICTUREBANK
  if( _d->surface && _d->opengltx > 0 )
  {    
    Engine::instance().loadPicture( *this, false );
  }
#endif
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
    Logger::warning( "Picture: surface not loading " + _name );
  }
}

Picture::Picture(const Size& size, unsigned char* data, bool mayChange) : _d( new PictureImpl )
{
  _d->drop();
  _orect = Rect( 0, 0, size.width(), size.height() );

  _d->surface = SDL_CreateRGBSurface( 0, size.width(), size.height(), 32,
                                      0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 );

  if( data )
  {
    memcpy( _d->surface->pixels, data, size.area() * 4 );
  }
  else
  {
    SDL_FillRect( _d->surface, 0, 0 );
  }

  Engine::instance().loadPicture( *this, mayChange );
  if( !mayChange )
  {
    SDL_FreeSurface( _d->surface );
    _d->surface = 0;
  }
}

#ifndef CAESARIA_DISABLE_PICTUREBANK
Picture::Picture(const std::string& group, const int id) : _d( new PictureImpl )
{
  _d->drop();
  load( group, id );
}

Picture::Picture(const std::string& filename )  : _d( new PictureImpl )
{
  _d->drop();
  load( filename );
}
#endif

const Picture& Picture::getInvalid() {  return _invalidPicture; }

Picture& Picture::draw(Picture pic, const Point& point, const Size& size)
{
  if( pic.surface() && _d->surface && _d->texture )
  {
    SDL_Rect rect = { (short)point.x(), (short)point.y(),
                      (unsigned short)size.width(), (unsigned short)size.height() };
    SDL_BlitSurface( pic.surface(), nullptr, _d->surface, &rect );

    update();
  }

  return *this;
}

Picture& Picture::draw(Picture pic, const Rect& src, const Rect& dst)
{
  if( pic.surface() && _d->surface && _d->texture )
  {
    SDL_Rect srcRect = { (short)src.left(), (short)src.top(),
                         (unsigned short)src.width(), (unsigned short)src.height() };
    SDL_Rect dstRect = { (short)dst.left(), (short)dst.top(),
                         (unsigned short)dst.width(), (unsigned short)dst.height() };

    SDL_BlitSurface( pic.surface(), &srcRect, _d->surface, dst.width() > 0 ? &dstRect : nullptr );

    update();
  }

  return *this;
}

}//end namespace gfx
