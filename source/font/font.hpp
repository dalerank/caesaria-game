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
#include "vfs/predefenitions.hpp"

namespace gfx
{
 class Picture;
}

class Font
{
  friend class FontCollection;

public:
  enum { alphaDraw=1, solidDraw=0, updateTx=1, ignoreTx=0 };
  static const char* defname;

  Font();
  static Font create(const std::string& family, int size, bool italic=false, bool bold=false, const NColor& color=ColorList::black);
  static Font create(const std::string& alias);

  ~Font();

  Font( const Font& other );
  Font& fallback(int size, bool italic, bool bold, const NColor& color);

  Font& operator=(const Font& other);

  int color() const;
  void setColor(NColor color );

  Font withColor( NColor color );
  Font withBoldItalic(bool bold, bool italic);
  Font withBold();
  Font withItalic();

  bool isValid() const;

  bool operator!=(const Font& other) const;

  Size getTextSize(const std::string& text) const;
  Rect getTextRect(const std::string& text, const Rect& baseRect,
                   align::Type horizontalAlign, align::Type verticalAlign);

  void draw(gfx::Picture& dstpic, const std::string &text, const int dx, const int dy, bool useAlpha=true, bool updatextTx=true);
  void draw(gfx::Picture& dstpic, const std::string &text, const Point& pos, bool useAlpha=true, bool updateTx=true );

  gfx::Picture once(const std::string& text, bool mayChange=false);

  unsigned int getWidthFromCharacter( unsigned int c ) const;
  int getCharacterFromPos(const std::wstring& text, int pixel_x) const;
  unsigned int kerningHeight() const;
  StringArray breakText(const std::string& text, int pixelLength);
private:
  void _setHdc(void* ptr);
  void _setStyle(int style);

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_FONT_H_INCLUDED__
