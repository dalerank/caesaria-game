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

#ifndef __OPENCAESAR3_FONT_H_INCLUDED__
#define __OPENCAESAR3_FONT_H_INCLUDED__

#include <string>
#include "core/size.hpp"
#include "core/rectangle.hpp"
#include "core/alignment.hpp"
#include "core/scopedptr.hpp"
#include "core/color.hpp"

class Picture;

enum FontType { FONT_0, FONT_1, FONT_1_WHITE, FONT_1_RED, 
                FONT_2, FONT_2_RED, FONT_2_WHITE, FONT_2_YELLOW, 
                FONT_3, 
                FONT_4, 
                FONT_5, 
                FONT_6, 
                FONT_7 };
class Font
{
  friend class FontCollection;

public:
  Font();
  static Font create( const std::string& family, const int size );
  static Font create( FontType type );
  static Font create( const std::string& type );
  
  ~Font();

  Font( const Font& other );

  Font& operator=(const Font& other);

  int getColor() const;
  void setColor( const NColor& color );

  bool isValid() const;

  Size getSize( const std::string& text ) const;

  bool operator!=(const Font& other) const;

  Rect calculateTextRect( const std::string& text, const Rect& baseRect, 
                          TypeAlign horizontalAlign, TypeAlign verticalAlign );

  void draw(Picture &dstpic, const std::string &text, const int dx, const int dy, bool useAlpha=true);
  void draw(Picture &dstpic, const std::string &text, const Point& pos, bool useAlpha=true );

  unsigned int getWidthFromCharacter( char c ) const;
  int getCharacterFromPos(const std::string& text, int pixel_x) const;
  unsigned int getKerningHeight() const;
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

class FontCollection
{
public:
  static FontCollection& instance();

  void initialize(const std::string &resourcePath);

  Font& getFont_(const int key);  // get a saved font
  Font& getFont_(const std::string& name );  // get a saved font

  void setFont(const int key, const std::string& name, Font font);  // save a font
  void addFont(const int key, const std::string& name, const std::string& pathFont, const int size, const NColor& color);

private:
  FontCollection();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_FONT_H_INCLUDED__
