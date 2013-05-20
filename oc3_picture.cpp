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

#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_rectangle.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_requirements.hpp"

// Picture class functions

Picture::Picture()
{
  _surface = NULL;
  _xoffset = 0;
  _yoffset = 0;
  _glTextureID = 0;
  _width = 0;
  _height = 0;

  _name = "";
}

void Picture::init(SDL_Surface *surface, const int xoffset, const int yoffset)
{
  _surface = surface;
  _xoffset = xoffset;
  _yoffset = yoffset;
  _width = _surface->w;
  _height = _surface->h;
}

void Picture::set_offset(const int xoffset, const int yoffset)
{
  _xoffset = xoffset;
  _yoffset = yoffset;
}

void Picture::set_offset( const Point& offset )
{
  _xoffset = offset.getX();
  _yoffset = offset.getY();
}

void Picture::add_offset(const int dx, const int dy)
{
  _xoffset += dx;
  _yoffset += dy;
}

SDL_Surface* Picture::get_surface() const
{
  return _surface;
}

int Picture::get_xoffset() const
{
  return _xoffset;
}

int Picture::get_yoffset() const
{
  return _yoffset;
}

int Picture::get_width() const
{
  return _width;
}
int Picture::get_height() const
{
  return _height;
}

void Picture::set_name(std::string &name)
{
  _name = name;
}

std::string Picture::get_name()
{
  return _name;
}

Size Picture::getSize() const
{
  return Size( _width, _height );
}

bool Picture::isValid() const
{
  return _surface != 0;
}

Picture& Picture::load( const char* group, const int id )
{
  return PicLoader::instance().get_picture( group, id );
}

Picture& Picture::load( const std::string& filename )
{
  return PicLoader::instance().get_picture( filename );
}

Picture& Picture::copy() const
{
  if( !_surface )
  {
    _OC3_DEBUG_BREAK_IF( true && "No surface" );
    return GfxEngine::instance().createPicture( _surface->w, _surface->h );
  }

  int width = _surface->w;
  int height = _surface->h;

  SDL_Surface* img = 0;
  img = SDL_ConvertSurface( _surface, _surface->format, SDL_SWSURFACE);
  if (img == NULL) 
  {
    THROW("Cannot make surface, size=" << width << "x" << height);
  }

  Picture& newpic = GfxEngine::instance().createPicture( width, height );
  newpic.init(img, width, height);

  return newpic;
}

void Picture::draw( const Picture &srcpic, const int dx, const int dy )
{
  SDL_Surface *srcimg = srcpic.get_surface();

  if( !srcimg || !_surface )
  {
    return;
  }

  SDL_Rect src, dst;

  src.x = 0;
  src.y = 0;
  src.w = srcimg->w;
  src.h = srcimg->h;
  dst.x = dx + srcpic.get_xoffset();
  dst.y = dy - srcpic.get_yoffset();
  dst.w = src.w;
  dst.h = src.h;

  SDL_BlitSurface(srcimg, &src, _surface, &dst);
}

void Picture::draw( const Picture &srcpic, const Rect& srcrect, const Point& pos )
{
  SDL_Surface *srcimg = srcpic.get_surface();

  if( !srcimg || !_surface )
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

  SDL_BlitSurface(srcimg, &src, _surface, &dst);
}
// Font class functions

Font::Font(TTF_Font &ttfFont, SDL_Color &color)
{
  _ttfFont = &ttfFont;
  _color = color;
}

Font::Font()
{
  _ttfFont = 0;
  _color = SDL_Color();
}

TTF_Font& Font::getTTF()
{
  return *_ttfFont;
}

SDL_Color& Font::getColor()
{
  return _color;
}

std::list<std::string> Font::split_text(const std::string &text, const int width)
{
  std::list<std::string> res;
  std::istringstream iss(text);
  Size size;

  std::string currentLine = "";
  while (iss)
  {
    std::string word;
    iss >> word;
    size = getSize( currentLine + " " + word );
    if (word == "")
    {
      // end of text
      break;
    }
    else if (currentLine == "")
    {
      // first word
      currentLine = word;
    }
    else if (size.getWidth() <= width)
    {
      // write on current line
      currentLine += " " + word;
    }
    else
    {
      // too long, need another line
      res.push_back(currentLine);
      currentLine = word;
    }
  }

  if (currentLine != "")
  {
    res.push_back(currentLine);
  }

  return res;
}

bool Font::isValid() const
{
    return _ttfFont != 0;
}

Size Font::getSize( const std::string& text ) const
{
    int w, h;
    TTF_SizeText( _ttfFont, text.c_str(), &w, &h);

    return Size( w, h );
}

bool Font::operator!=( const Font& other ) const
{
    return !( _ttfFont == other._ttfFont );
}

Rect Font::calculateTextRect( const std::string& text, const Rect& baseRect, 
                              TypeAlign horizontalAlign, TypeAlign verticalAlign )
{
    Rect resultRect;
    Size d = getSize( text );

    // justification
    switch (horizontalAlign)
    {
    case alignCenter:
        // align to h centre
        resultRect.UpperLeftCorner.setX( (baseRect.getWidth()/2) - (d.getWidth()/2) );
        resultRect.LowerRightCorner.setX( (baseRect.getWidth()/2) + (d.getWidth()/2) );
        break;
    case alignLowerRight:
        // align to right edge
        resultRect.UpperLeftCorner.setX( baseRect.getWidth() - d.getWidth() );
        resultRect.LowerRightCorner.setX( baseRect.getWidth() );
        break;
    default:
        // align to left edge
        resultRect.UpperLeftCorner.setX( 0 );
        resultRect.LowerRightCorner.setX( d.getWidth() );
    }

    switch (verticalAlign)
    {
    case alignCenter:
        // align to v centre
        resultRect.UpperLeftCorner.setY( (baseRect.getHeight()/2) - (d.getHeight()/2) );
        resultRect.LowerRightCorner.setY( (baseRect.getHeight()/2) + (d.getHeight()/2) );
        break;
    case alignLowerRight:
        // align to bottom edge
        resultRect.UpperLeftCorner.setY( baseRect.getHeight() - d.getHeight() );
        resultRect.LowerRightCorner.setY( baseRect.getHeight() );
        break;
    default:
        // align to top edge
        resultRect.UpperLeftCorner.setY( 0 );
        resultRect.LowerRightCorner.setY( d.getHeight() );
        break;
    }

    resultRect += baseRect.UpperLeftCorner;

    return resultRect;
}

void Font::setColor( const int dc )
{
  _color.b = (dc & 0xff);
  _color.g = (dc >> 8) & 0xff;
  _color.r = (dc >> 16) & 0xff;
  _color.unused = ( dc >> 24 ) & 0xff;
}

void Font::draw(Picture& dstpic, const std::string &text, const int dx, const int dy )
{
  if( !_ttfFont || !dstpic.isValid() )
    return;

  SDL_Surface* sText = TTF_RenderUTF8_Blended( _ttfFont, text.c_str(), _color );

  if( sText )
  {
    Picture pic;
    pic.init( sText, 0, 0 );
    dstpic.draw( pic, dx, dy);
  }
 
  SDL_FreeSurface( sText );
}

FontCollection* FontCollection::_instance = NULL;

FontCollection& FontCollection::instance()
{
   if (_instance == NULL)
   {
      _instance = new FontCollection();
   }

   return *_instance;
}

FontCollection::FontCollection()
{ }

Font& FontCollection::getFont(const int key)
{
   std::map<int, Font>::iterator it = _collection.find(key);
   if (it == _collection.end())
   {
      THROW("Error, font is not initialized, key=" << key);
   }
   return (*it).second;
}

void FontCollection::setFont(const int key, Font& font)
{
   std::pair<std::map<int, Font>::iterator, bool> ret = _collection.insert(std::pair<int, Font>(key, font));
   if (ret.second == false)
   {
      // no insert font (already exists)
      THROW("Error, font already exists, key=" << key);
   }

   // // insert font
   // std::cout << "Registered font with key=" << key << std::endl;
}

void Picture::lock()
{
  if (SDL_MUSTLOCK(_surface))
  {
    int rc = SDL_LockSurface(_surface);
    if (rc < 0) THROW("Cannot lock surface: " << SDL_GetError());
  }
}

void Picture::unlock()
{
  if (SDL_MUSTLOCK(_surface))
  {
    SDL_UnlockSurface(_surface);
  }
}

Uint32 Picture::get_pixel(const int x, const int y)
{
  // validate arguments
  if (_surface == NULL || x < 0 || y < 0 || x >= _surface->w || y >= _surface->h)
    return 0;

  Uint32 res = 0;
  switch (_surface->format->BytesPerPixel)
  {
  case 1:
    // 8bpp
    Uint8 *bufp8;
    bufp8 = (Uint8 *)_surface->pixels + y*_surface->pitch + x;
    res = *bufp8;
    break;

  case 2:
    // 15bpp or 16bpp
    Uint16 *bufp16;
    bufp16 = (Uint16 *)_surface->pixels + y*_surface->pitch/2 + x;
    res = *bufp16;
    break;

  case 3:
    // 24bpp, very slow!
    THROW("Unsupported graphic mode 24bpp");
    break;

  case 4:
    // 32bpp
    Uint32 *bufp32;
    bufp32 = (Uint32 *)_surface->pixels + y*_surface->pitch/4 + x;
    res = *bufp32;
    break;
  }

  return res;
}

void Picture::set_pixel(const int x, const int y, const Uint32 color)
{
  // validate arguments
  if (_surface == NULL || x < 0 || y < 0 || x >= _surface->w || y >= _surface->h)
    return;

  switch (_surface->format->BytesPerPixel)
  {
  case 1:
    // 8bpp
    Uint8 *bufp8;
    bufp8 = (Uint8 *)_surface->pixels + y * _surface->pitch + x;
    *bufp8 = color;
    break;

  case 2:
    // 15bpp or 16bpp
    Uint16 *bufp16;
    bufp16 = (Uint16 *)_surface->pixels + y * _surface->pitch / 2 + x;
    *bufp16 = color;
    break;

  case 3:
    // 24bpp, very slow!
    THROW("Unsupported graphic mode 24bpp");
    break;

  case 4:
    // 32bpp
    Uint32 *bufp32;
    bufp32 = (Uint32 *)_surface->pixels + y * _surface->pitch/4 + x;
    *bufp32 = color;
    break;
  }
}


