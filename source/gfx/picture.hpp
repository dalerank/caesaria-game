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
#include "core/rectangle.hpp"
#include "core/color.hpp"
#include "core/smartptr.hpp"

struct SDL_Texture;
struct SDL_Surface;

namespace gfx
{

class PictureImpl;
  
// an image with offset, this is the basic rendered object
class Picture
{
public:
  Picture();
  Picture( const Size& size, unsigned char* data=0, bool mayChange=false );
  Picture( const std::string& group, const int id );
  Picture( const std::string& filename );
  ~Picture();

  Picture( const Picture& other );
  Picture& operator=(const Picture& other);
  
  void setOffset( const Point& offset );
  void setOffset( int x, int y );

  void setOriginRect( const Rect& rect );
  const Rect& originRect() const;

  void addOffset(const Point &offset );
  void addOffset(int x, int y);
  const Point& offset() const;

  void setName(const std::string &name);  // for save game
  const std::string& name() const;
  void setAlpha( unsigned char value );

  void init( SDL_Texture* tx, SDL_Surface* srf, unsigned int ogltx );

  SDL_Texture* texture() const;  
  SDL_Surface* surface() const;
  unsigned int& textureID();
  unsigned int textureID() const;

  void load( const std::string& group, const int id );
  void load( const std::string& filename );

  int width() const;
  int height() const;
  int pitch() const;

  void fill(const NColor& color, Rect rect=Rect() );

  unsigned int* lock();
  void unlock();

  Size size() const;
  unsigned int sizeInBytes() const;

  bool isValid() const;  

  static const Picture& getInvalid();

  void update();
private:
  SmartPtr<PictureImpl> _d;

  Point _offset;  // the image is shifted when displayed
  Rect _orect;
  std::string _name; // for game save
};

}//end namespace gfx

#endif //_CAESARIA_PICTURE_HPP_INCLUDE_
