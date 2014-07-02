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

#ifndef _CAESARIA_PICTURE_HPP_INCLUDE_
#define _CAESARIA_PICTURE_HPP_INCLUDE_

#include <vector>
#include <string>
#include "core/size.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "core/position.hpp"
#include "core/rectangle.hpp"
#include "core/color.hpp"

struct SDL_Surface;

namespace gfx
{
  
// an image with offset, this is the basic rendered object
class Picture : public ReferenceCounted
{
public:
  Picture();
  ~Picture();

  Picture( const Picture& other );
  Picture& operator=(const Picture& other);
  
  void init(SDL_Surface* surface, const Point& offset );

  void setOffset( Point offset );
  void setOffset( int x, int y );

  void addOffset( Point offset );
  void addOffset(int x, int y);
  Point offset() const;

  void setName(std::string &name);  // for save game
  std::string name() const;
  Picture* clone() const;
  void setAlpha( unsigned char value );
  void scale( Size size );
  SDL_Surface* surface() const;
  unsigned int& textureID();
  unsigned int& textureID() const;

  int width() const;
  int height() const;

  void draw( const Picture &srcpic, int x, int y, bool useAlpha=true );
  void draw( const Picture &srcpic, const Point& pos, bool useAlpha=true );
  void draw( const Picture &srcpic, const Rect& srcrect, const Point& pos, bool useAlpha=true );
  void draw( const Picture &srcpic, const Rect& srcrect, const Rect& dstrect, bool useAlpha=true );

  void fill( const NColor& color, const Rect& rect );

  // lock/unlock the given surface for pixel access
  void lock();
  void unlock();

  // Uint32 is the pixel color in the surface format. The surface must be locked!!!
  int pixel( Point pos );
  void setPixel( Point pos, const int color);

  Size size() const;

  bool isValid() const;

  static Picture& load( const std::string& group, const int id );
  static Picture& load( const std::string& filename ); 

  static Picture* create( const Size& size );
  static const Picture& getInvalid();
  static void destroy( Picture* ptr );
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

struct PictureRefDeleter
{
  static inline void cleanup( Picture* pic )
  {
    // Enforce a complete type.
    // If you get a compile error here, you need add destructor in class-container
    typedef char IsIncompleteType[ sizeof(Picture) ? 1 : -1 ];
    (void) sizeof(IsIncompleteType);

    Picture::destroy( pic );
  }
};

class PictureRef : public ScopedPtr< Picture, PictureRefDeleter >
{
public:
  void init( const Size& size )
  {
    reset( Picture::create( size ) );
  }
};

}//end namespace gfx

#endif //_CAESARIA_PICTURE_HPP_INCLUDE_
