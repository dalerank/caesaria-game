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

#include "font.hpp"
#include "gfx/picture.hpp"
#include "logger.hpp"
#include "exception.hpp"
#include "SDL_ttf.h"
#include "SDL_version.h"
#include "color.hpp"
#include "vfs/directory.hpp"
#include "game/settings.hpp"
#include "core/osystem.hpp"
#include "gfx/engine.hpp"
#include <map>

using namespace gfx;

class Font::Impl
{
public:
  TTF_Font *ttfFont;
  SDL_Color color; 
};


Font::Font() : _d( new Impl )
{
  _d->ttfFont = 0;
  _d->color = SDL_Color();
}

Font::Font( const Font& other ) : _d( new Impl )
{
  *this = other;
}

Font Font::create( const std::string& family, const int size )
{
  return Font();
}

unsigned int Font::getWidthFromCharacter( unsigned int c ) const
{
  int minx, maxx, miny, maxy, advance;
  TTF_GlyphMetrics( _d->ttfFont, c, &minx, &maxx, &miny, &maxy, &advance );
  
  return advance;
}

unsigned int Font::kerningHeight() const {  return 3; }

int Font::getCharacterFromPos(const std::wstring& text, int pixel_x) const
{
  int x = 0;
  int idx = 0;

  while( text[idx] )
  {
    x += getWidthFromCharacter( text[idx] );

    if (x >= pixel_x)
      return idx;

    idx++;
  }

  return -1;
}

int Font::color() const
{
  int ret = 0;
#if SDL_MAJOR_VERSION>1
  ret = (_d->color.a << 24 ) + (_d->color.r << 16) + (_d->color.g << 8) + _d->color.b;
#else
  ret = (_d->color.unused << 24 ) + (_d->color.r << 16) + (_d->color.g << 8) + _d->color.b;
#endif
  return ret;
}

bool Font::isValid() const {  return _d->ttfFont != 0; }

Size Font::getTextSize( const std::string& text ) const
{
  int w=0, h=0;
  if( isValid() )
  {
    TTF_SizeUTF8( _d->ttfFont, text.c_str(), &w, &h );
  }

  return Size( w, h );
}

bool Font::operator!=( const Font& other ) const
{
  return !( _d->ttfFont == other._d->ttfFont );
}

Rect Font::getTextRect(const std::string& text, const Rect& baseRect,
                             align::Type horizontalAlign, align::Type verticalAlign )
{
  Rect resultRect;
  Size d = getTextSize( text );

  // justification
  switch (horizontalAlign)
  {
  case align::center:
    // align to h centre
    resultRect.UpperLeftCorner.setX( (baseRect.width()/2) - (d.width()/2) );
    resultRect.LowerRightCorner.setX( (baseRect.width()/2) + (d.width()/2) );
    break;
  case align::lowerRight:
    // align to right edge
    resultRect.UpperLeftCorner.setX( baseRect.width() - d.width() );
    resultRect.LowerRightCorner.setX( baseRect.width() );
    break;
  default:
    // align to left edge
    resultRect.UpperLeftCorner.setX( 0 );
    resultRect.LowerRightCorner.setX( d.width() );
  }

  switch (verticalAlign)
  {
  case align::center:
    // align to v centre
    resultRect.UpperLeftCorner.setY( (baseRect.height()/2) - (d.height()/2) );
    resultRect.LowerRightCorner.setY( (baseRect.height()/2) + (d.height()/2) );
    break;
  case align::lowerRight:
    // align to bottom edge
    resultRect.UpperLeftCorner.setY( baseRect.height() - d.height() );
    resultRect.LowerRightCorner.setY( baseRect.height() );
    break;
  default:
    // align to top edge
    resultRect.UpperLeftCorner.setY( 0 );
    resultRect.LowerRightCorner.setY( d.height() );
    break;
  }

  resultRect += baseRect.UpperLeftCorner;

  return resultRect;
}

void Font::setColor( NColor color )
{
#ifdef CAESARIA_PLATFORM_ANDROID
  color = color.abgr();
#endif
  _d->color.b = color.blue();
  _d->color.g = color.green();
  _d->color.r = color.red();
#if SDL_MAJOR_VERSION>1
  _d->color.a = color.alpha();
#else
  _d->color.unused = color.alpha();
#endif
}

void Font::draw( Picture& dstpic, const std::string &text, const int dx, const int dy, bool useAlpha, bool updatextTx )
{
  if( !_d->ttfFont || !dstpic.isValid() )
    return;

#if defined(CAESARIA_PLATFORM_EMSCRIPTEN)
  SDL_Surface* sText = TTF_RenderText_Solid( _d->ttfFont, text.c_str(), _d->color );
#else
  SDL_Surface* sText = TTF_RenderUTF8_Blended( _d->ttfFont, text.c_str(), _d->color );
#endif

  if( sText )
  {

    if( useAlpha )
    {
#if SDL_MAJOR_VERSION>1
      SDL_SetSurfaceBlendMode( sText, SDL_BLENDMODE_NONE );
#else
      SDL_SetAlpha( sText, 0, 0 );
#endif
    }

    if( !dstpic.surface() )
    {
      Logger::warning("Font::draw dstpic surface is null");
      return;
    }

    SDL_Rect srcRect, dstRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = sText->w;
    srcRect.h = sText->h;
    dstRect.x = dx;
    dstRect.y = dy;
    dstRect.w = sText->w;
    dstRect.h = sText->h;

    SDL_BlitSurface( sText, &srcRect, dstpic.surface(), &dstRect );
    SDL_FreeSurface( sText );
  }

  if( updatextTx )
    dstpic.update();
}       

void Font::draw(Picture &dstpic, const std::string &text, const Point& pos, bool useAlpha , bool updateTx)
{
  draw( dstpic, text, pos.x(), pos.y(), useAlpha, updateTx );
}

Picture* Font::once(const std::string &text, bool mayChange)
{
  SDL_Surface* textSurface = TTF_RenderUTF8_Blended( _d->ttfFont, text.c_str(), _d->color );
  Picture* ret = Picture::create( Size( textSurface->w, textSurface->h ), (unsigned char*)textSurface->pixels, mayChange );
  SDL_FreeSurface( textSurface );
  ret->update();

  return ret;
}

Font::~Font() {}

Font& Font::operator=( const Font& other )
{
  _d->ttfFont = other._d->ttfFont;
  _d->color = other._d->color;
  return *this;
}

Font Font::create(FontType type)
{
  return FontCollection::instance().getFont_( type );
}


Font Font::create(FontType type, NColor color)
{
  Font ret = FontCollection::instance().getFont_( type );
  ret.setColor( color );
  return ret;
}

Font Font::create(const std::string& type)
{
  return FontCollection::instance().getFont_( type );
}

class FontTypeHelper : public EnumsHelper<int>
{
public:
  FontTypeHelper() : EnumsHelper<int>(0) {}
};

class FontCollection::Impl
{
public:
  FontTypeHelper fhelper;
  std::map< int, Font> collection;
};

FontCollection& FontCollection::instance()
{
  static FontCollection inst;
  return inst;
}

FontCollection::FontCollection() : _d( new Impl )
{ }

Font& FontCollection::getFont_(const std::string& name)
{
  int type = _d->fhelper.findType( name );

  return getFont_( type );
}

Font& FontCollection::getFont_(const int key)
{
  std::map<int, Font>::iterator it = _d->collection.find(key);
  if (it == _d->collection.end())
  {
    Logger::warning( "Error, font is not initialized, key=%d", key );
    return _d->collection[ FONT_2 ];
  }

  return (*it).second;
}

void FontCollection::setFont(const int key, const std::string& name, Font font)
{
  std::pair< std::map< int, Font>::iterator, bool> ret = _d->collection.insert(std::pair<int, Font>(key, font) );

  if( ret.second == false )
  {
    // no insert font (already exists)
    Logger::warning( "WARNING!!! font already exists, key=%d", key );
    return;
  }

  _d->fhelper.append( key, name );
}

void FontCollection::addFont(const int key, const std::string& name, vfs::Path pathFont, const int size, const NColor& color )
{
  TTF_Font* ttf = TTF_OpenFont(pathFont.toString().c_str(), size);
  if( ttf == NULL )
  {
    std::string errorStr( TTF_GetError() );
#ifdef CAESARIA_PLATFORM_WIN
    errorStr += "\n Is it only latin symbols in path to game?";
#endif
    OSystem::error( "CRITICAL!!! ", errorStr );
    THROW( errorStr );
  }

  Font font0;
  font0._d->ttfFont = ttf;

  SDL_Color c = { color.red(), color.green(), color.blue(), color.alpha() };
  font0._d->color = c;
  setFont( key, name, font0);
}

void FontCollection::initialize(const std::string &resourcePath)
{
  vfs::Directory resDir( resourcePath );
  vfs::Path fontFilename = SETTINGS_VALUE( font ).toString();
  vfs::Path absolutFontfilename = resDir/fontFilename;

  addFont( FONT_0,       CAESARIA_STR_EXT(FONT_0),      absolutFontfilename, 12, DefaultColors::black );
  addFont( FONT_1,       CAESARIA_STR_EXT(FONT_1),      absolutFontfilename, 16, DefaultColors::black );
  addFont( FONT_1_WHITE, CAESARIA_STR_EXT(FONT_1_WHITE),absolutFontfilename, 16, DefaultColors::white );
  addFont( FONT_1_RED,   CAESARIA_STR_EXT(FONT_1_RED),  absolutFontfilename, 16, DefaultColors::caesarRed );
  addFont( FONT_2,       CAESARIA_STR_EXT(FONT_2),      absolutFontfilename, 18, DefaultColors::black );
  addFont( FONT_2_RED,   CAESARIA_STR_EXT(FONT_2_RED),  absolutFontfilename, 18, DefaultColors::caesarRed );
  addFont( FONT_2_WHITE, CAESARIA_STR_EXT(FONT_2_WHITE),absolutFontfilename, 18, DefaultColors::white );
  addFont( FONT_2_YELLOW,CAESARIA_STR_EXT(FONT_2_YELLOW),absolutFontfilename, 18, DefaultColors::yellow );
  addFont( FONT_3,       CAESARIA_STR_EXT(FONT_3),      absolutFontfilename, 20, DefaultColors::black );
  addFont( FONT_4,       CAESARIA_STR_EXT(FONT_4),      absolutFontfilename, 24, DefaultColors::black );
  addFont( FONT_5,       CAESARIA_STR_EXT(FONT_5),      absolutFontfilename, 28, DefaultColors::black);
  addFont( FONT_6,       CAESARIA_STR_EXT(FONT_6),      absolutFontfilename, 32, DefaultColors::black);
  addFont( FONT_7,       CAESARIA_STR_EXT(FONT_7),      absolutFontfilename, 36, DefaultColors::black);
  addFont( FONT_8,       CAESARIA_STR_EXT(FONT_8),      absolutFontfilename, 42, DefaultColors::black);
}

static StringArray _font_breakText(const std::string& text, const Font& f, int elWidth, bool RightToLeft )
{
  StringArray brokenText;

  Font font = f;

  if( !font.isValid() )
  {
    Logger::warning( "utils::breakText font must be exists" );
    brokenText.push_back( text );
    return brokenText;
  }

  std::string line;
  std::string word;
  std::string rwhitespace;
  std::string rText = text;
  int size = rText.size();
  int length = 0;

  char c;

	// We have to deal with right-to-left and left-to-right differently
	// However, most parts of the following code is the same, it's just
	// some order and boundaries which change.
	if( !RightToLeft)
	{
		// regular (left-to-right)
		for (int i=0; i<size; ++i)
		{
			c = rText[i];
			bool lineBreak = false;

			if( c == '\r' ) // Mac or Windows breaks
			{
				lineBreak = true;
				if (rText[i+1] == '\n') // Windows breaks
				{
					rText.erase(i+1);
					--size;
				}
				c = '\0';
			}
			else if (c == '\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			bool isWhitespace = (c == ' ' || c == 0);
			if ( !isWhitespace )
			{
				// part of a word
				word += c;
			}

			if ( isWhitespace || i == (size-1))
			{
				if (word.size())
				{
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const int whitelgth = font.getTextSize( rwhitespace ).width();
					const int wordlgth = font.getTextSize( word ).width();

					if (wordlgth > elWidth)
					{
						// This word is too long to fit in the available space, look for
						// the Unicode Soft HYphen (SHY / 00AD) character for a place to
						// break the word at
						int where = word.find_first_of( char(0xAD) );
						if (where != -1)
						{
							std::string first  = word.substr(0, where);
							std::string second = word.substr(where, word.size() - where);
							brokenText.push_back(line + first + "-");
							const int secondLength = font.getTextSize( second ).width();

							length = secondLength;
							line = second;
						}
						else
						{
							// No soft hyphen found, so there's nothing more we can do
							// break to next line
							if (length)
								brokenText.push_back(line);
							length = wordlgth;
							line = word;
						}
					}
					else if (length && (length + wordlgth + whitelgth > elWidth))
					{
						// break to next line
						brokenText.push_back(line);
						length = wordlgth;
						line = word;
					}
					else
					{
						// add word to line
						line += rwhitespace;
						line += word;
						length += whitelgth + wordlgth;
					}

					word = "";
					rwhitespace = "";
				}

				if ( isWhitespace )
				{
					rwhitespace += c;
				}

				// compute line break
				if (lineBreak)
				{
					line += rwhitespace;
					line += word;
					brokenText.push_back(line);
					line = "";
					word = "";
					rwhitespace = "";
					length = 0;
				}
			}
		}

		line += rwhitespace;
		line += word;
		brokenText.push_back(line);
	}
	else
	{
		// right-to-left
		for (int i=size; i>=0; --i)
		{
			c = rText[i];
			bool lineBreak = false;

			if(c == '\r') // Mac or Windows breaks
			{
				lineBreak = true;
				if( (i>0) && rText[i-1] == '\n' ) // Windows breaks
				{
					rText.erase(i-1);
					--size;
				}
				c = '\0';
			}
			else if (c == '\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			if (c==' ' || c==0 || i==0)
			{
				if (word.size())
				{
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const int whitelgth = font.getTextSize( rwhitespace ).width();
					const int wordlgth = font.getTextSize( word ).width();

					if (length && (length + wordlgth + whitelgth > elWidth))
					{
						// break to next line
						brokenText.push_back(line);
						length = wordlgth;
						line = word;
					}
					else
					{
						// add word to line
						line = rwhitespace + line;
						line = word + line;
						length += whitelgth + wordlgth;
					}

					word = "";
					rwhitespace = "";
				}

				if (c != 0)
					rwhitespace = std::string(&c, 1) + rwhitespace;

				// compute line break
				if (lineBreak)
				{
					line = rwhitespace + line;
					line = word + line;
					brokenText.push_back(line);
					line = "";
					word = "";
					rwhitespace = "";
					length = 0;
				}
			}
			else
			{
				// yippee this is a word..
				word = std::string(&c, 1) + word;
			}
		}

		line = rwhitespace + line;
		line = word + line;
		brokenText.push_back(line);
	}

	return brokenText;
}

StringArray Font::breakText(const std::string& text, int pixelLength)
{
	return _font_breakText( text, *this, pixelLength, false );
}
