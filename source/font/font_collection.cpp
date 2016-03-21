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

#include "font_collection.hpp"
#include "font.hpp"
#include <GameLogger>
#include <GameCore>
#include "SDL_ttf.h"

using namespace gfx;

class FontCollection::Impl
{
public:
  struct {
    vfs::Path path;
    std::string famility;
  } dfont;

  std::map<std::string, Font> collection;
  std::map<std::string, vfs::Path> familyPaths;
};

FontCollection& FontCollection::instance()
{
  static FontCollection inst;
  return inst;
}

FontCollection::FontCollection() : _d(new Impl) {}

Font FontCollection::getFont(const std::string& name)
{
  auto it = _d->collection.find(name);
  if (it != _d->collection.end())
    return it->second;

  return Font();
}

Font FontCollection::getFont(const std::string& family, int size, bool italic, bool bold)
{
  std::string name = _getName(family,size,italic,bold);
  Font ret = getFont(name);
  if (ret.isValid())
    return ret;

  auto it = _d->familyPaths.find(family);
  if (it != _d->familyPaths.end())
  {
    Font font = _addFont(family,it->second,size,italic,bold,ColorList::black);
    return font;
  }

  return Font();
}

Font FontCollection::getDefault(int size, bool italic, bool bold)
{
  Font font = getFont(_d->dfont.famility,size,italic,bold);
  if (!font.isValid())
    font = _addFont(Font::defname,_d->dfont.path,size,italic,bold,ColorList::black);

  return font;
}

void FontCollection::_setFont(const std::string& name, Font font)
{
  auto ret = _d->collection.insert(std::pair<std::string, Font>(name, font));
  if( ret.second == false )
  {
    // no insert font (already exists)
    Logger::warning( "WARNING!!! font already exists, name={}", name);
    return;
  }
}

std::string FontCollection::_getName(const std::string& family, int size, bool italic, bool bold)
{
  std::string name = family;
  name += "_" + utils::i2str(size);
  name += italic ? "_italic" : "";
  name += bold ? "_bold" : "";
  return name;
}

Font FontCollection::_addFont(const std::string& name, vfs::Path pathFont, const int size, bool italic, bool bold, const NColor& color )
{
  TTF_Font* ttf = TTF_OpenFont(pathFont.toCString(), size);
  if  (ttf == nullptr)
  {
    std::string errorStr(TTF_GetError());
    if (OSystem::isWindows())
      errorStr += "\n Is it only latin symbols in path to game?";
  }

  Font font0;
  font0._setHdc( ttf );
  font0.setColor( color );

  int style = 0;
  if (italic)
    style |= TTF_STYLE_ITALIC;

  if (bold)
    style |= TTF_STYLE_BOLD;
  font0._setStyle( style );

  TTF_SetFontStyle(ttf, style);

  std::string rname = name;
  if (rname.empty())
  {
    std::string familyName = TTF_FontFaceFamilyName(ttf);
    rname = _getName(familyName,size,italic,bold);
  }

  _setFont(rname, font0);

  return font0;
}

void FontCollection::initialize(const vfs::Directory& resourcePath, const std::string& family)
{
  _d->collection.clear();

  vfs::Path absolutFontfilename = resourcePath/family;

  vfs::Entries::Items items = resourcePath.entries().items();
  for( const auto& item : items )
  {
    TTF_Font* tmp_font = TTF_OpenFont(item.fullpath.toCString(),10);
    std::string familyName = TTF_FontFaceFamilyName(tmp_font);
    _d->familyPaths[familyName] = item.fullpath;
    TTF_CloseFont(tmp_font);
  }

  if (absolutFontfilename.exist())
  {
    TTF_Font* tmp_font = TTF_OpenFont(absolutFontfilename.toCString(),10);
    _d->dfont.famility = TTF_FontFaceFamilyName(tmp_font);
    _d->dfont.path = absolutFontfilename;
    TTF_CloseFont(tmp_font);
  }

  _addFont( "FONT_0",      absolutFontfilename, 12, false, false, ColorList::black );
  _addFont( "FONT_1",      absolutFontfilename, 16, false, false, ColorList::black );
  _addFont( "FONT_1_WHITE",absolutFontfilename, 16, false, false, ColorList::white );
  _addFont( "FONT_1_RED",  absolutFontfilename, 16, false, false, ColorList::caesarRed );
  _addFont( "FONT_2",      absolutFontfilename, 18, false, false, ColorList::black );
  _addFont( "FONT_2_RED",  absolutFontfilename, 18, false, false, ColorList::caesarRed );
  _addFont( "FONT_2_WHITE",absolutFontfilename, 18, false, false, ColorList::white );
  _addFont( "FONT_2_YELLOW",absolutFontfilename,18, false, false, ColorList::yellow );
  _addFont( "FONT_3",      absolutFontfilename, 20, false, false, ColorList::black );
  _addFont( "FONT_4",      absolutFontfilename, 24, false, false, ColorList::black );
  _addFont( "FONT_5",      absolutFontfilename, 28, false, false, ColorList::black);
  _addFont( "FONT_6",      absolutFontfilename, 32, false, false, ColorList::black);
  _addFont( "FONT_7",      absolutFontfilename, 36, false, false, ColorList::black);
  _addFont( "FONT_8",      absolutFontfilename, 42, false, false, ColorList::black);
}
