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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_FONT_H_INCLUDED__
#define __CAESARIA_FONT_H_INCLUDED__

#include <string>
#include "core/size.hpp"
#include "core/rectangle.hpp"
#include "core/alignment.hpp"
#include "core/scopedptr.hpp"
#include "core/stringarray.hpp"
#include "core/color.hpp"
#include "vfs/path.hpp"

namespace gfx
{
 class Picture;
 class PictureRef;
}

enum FontType { FONT_0, FONT_1, FONT_1_WHITE, FONT_1_RED, 
                FONT_2, FONT_2_RED, FONT_2_WHITE, FONT_2_YELLOW, 
                FONT_3, FONT_4,
                FONT_5,
                FONT_6,
                FONT_7,
                FONT_8,
                FONT_9 };
class Font
{
  friend class FontCollection;

public:
  Font();
  static Font create( const std::string& family, const int size );
  static Font create( FontType type );
  static Font create( FontType type, NColor color );
  static Font create( const std::string& type );  
  
  ~Font();

  Font( const Font& other );

  Font& operator=(const Font& other);

  int color() const;
  void setColor(NColor color );

  bool isValid() const;

  bool operator!=(const Font& other) const;

  Size getTextSize( const std::string& text ) const;
  Rect getTextRect( const std::string& text, const Rect& baseRect,
                    align::Type horizontalAlign, align::Type verticalAlign );

  void draw(gfx::Picture& dstpic, const std::string &text, const int dx, const int dy, bool useAlpha=true, bool updatextTx=true);
  void draw(gfx::Picture& dstpic, const std::string &text, const Point& pos, bool useAlpha=true, bool updateTx=true );

  gfx::Picture* once(const std::string &text, bool mayChange=false);

  unsigned int getWidthFromCharacter( unsigned int c ) const;
  int getCharacterFromPos(const std::wstring& text, int pixel_x) const;
  unsigned int kerningHeight() const;
  StringArray breakText( const std::string& text, int pixelLength );
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
  void addFont(const int key, const std::string& name, vfs::Path filename, const int size, const NColor& color);

private:
  FontCollection();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_FONT_H_INCLUDED__
