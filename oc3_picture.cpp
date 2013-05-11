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

#include "oc3_sdl_facade.hpp"
#include <iostream>
#include <sstream>

#include "oc3_exception.hpp"
#include "oc3_positioni.hpp"
#include "oc3_rectangle.hpp"
#include "oc3_pic_loader.hpp"

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
  int w;
  int h;
  SdlFacade &sdlFacade = SdlFacade::instance();

  std::string currentLine = "";
  while (iss)
  {
    std::string word;
    iss >> word;
    sdlFacade.getTextSize(*this, currentLine + " " + word, w, h);
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
    else if (w <= width)
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

