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


#ifndef PICTURE_HPP
#define PICTURE_HPP

#include <vector>
#include <string>
#include "core/size.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "core/position.hpp"

class Rect;
class NColor;
struct SDL_Surface;
  
// an image with offset, this is the basic rendered object
class Picture : public ReferenceCounted
{
public:
  Picture();
  ~Picture();

  Picture( const Picture& other );
  Picture& operator=(const Picture& other);
  
  void init(SDL_Surface* surface, const Point& offset );

  void setOffset(const int xoffset, const int yoffset);
  void setOffset( const Point& xoffset );
  void addOffset(const int dx, const int dy);
  void setName(std::string &name);  // for save game
  std::string getName() const;
  Picture* clone() const;
  void scale( Size size );
  SDL_Surface* getSurface() const;
  Point getOffset() const;
  int getWidth() const;
  int getHeight() const;

  void draw( const Picture &srcpic, int x, int y, bool useAlpha=true );
  void draw( const Picture &srcpic, const Point& pos, bool useAlpha=true );
  void draw( const Picture &srcpic, const Rect& srcrect, const Point& pos, bool useAlpha=true );
  void draw( const Picture &srcpic, const Rect& srcrect, const Rect& dstrect, bool useAlpha=true );

  void fill( const NColor& color, const Rect& rect );

  // lock/unlock the given surface for pixel access
  void lock();
  void unlock();

  // Uint32 is the pixel color in the surface format. The surface must be locked!!!
  int getPixel( Point pos );
  void setPixel( Point pos, const int color);

  Size getSize() const;

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

typedef std::vector<Picture> PicturesArray;

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

#endif
