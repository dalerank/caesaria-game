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


#ifndef PICTURE_HPP
#define PICTURE_HPP


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <vector>
#include <map>
#include <list>
#include <string>

#include <SDL_ttf.h>

class Point;

// an image with offset, this is the basic rendered object
class Picture
{
   friend class GfxSdlEngine;
   friend class GfxSdlDumbEngine;
   friend class GfxGlEngine;

public:
   Picture();

   void init(SDL_Surface* surface, const int xoffset, const int yoffset);
   void set_offset(const int xoffset, const int yoffset);
   void set_offset( const Point& xoffset );
   void add_offset(const int dx, const int dy);
   void set_name(std::string &name);  // for save game
   std::string get_name();

   SDL_Surface *get_surface() const;
   int get_xoffset() const;
   int get_yoffset() const;
   int get_width() const;
   int get_height() const;

private:
   // the image is shifted when displayed
   int _xoffset;
   int _yoffset;
   int _width;
   int _height;

   // for game save
   std::string _name;

   // for SDL surface
   SDL_Surface *_surface;

   // for OPEN_GL surface
   GLuint _glTextureID;  // texture ID for openGL
};


// several frames for a basic visual animation
class Animation
{
public:
   Animation() {}

   void init(const std::vector<Picture*> &pictures);

   std::vector<Picture*>& get_pictures();
   const std::vector<Picture*>& get_pictures() const;

   void nextFrame();
   Picture* get_current_picture();

private:
   std::vector<Picture*> _pictures;
   unsigned int _animIndex;  // index of the current frame
};


class Font
{
public:
   Font(TTF_Font &ttfFont, SDL_Color &color);

   TTF_Font &getTTF();

   SDL_Color &getColor();
   std::list<std::string> split_text(const std::string &text, const int width);

private:
   TTF_Font *_ttfFont;
   SDL_Color _color;
};


enum FontType { FONT_0, FONT_1, FONT_2, FONT_2_RED, FONT_2_WHITE, FONT_2_YELLOW, FONT_3, FONT_4, FONT_5, FONT_6, FONT_7 };
class FontCollection
{
public:
   static FontCollection& instance();

   Font& getFont(const int key);  // get a saved font
   void setFont(const int key, Font &font);  // save a font

private:
   FontCollection();

   static FontCollection *_instance;
   std::map<int, Font> _collection;
};


#endif
