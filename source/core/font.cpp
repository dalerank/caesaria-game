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
#include <SDL_ttf.h>
#include "color.hpp"
#include "vfs/directory.hpp"
#include "core/osystem.hpp"
#include <map>

using namespace gfx;

class Font::Impl
{
public:
  TTF_Font *ttfFont;
  SDL_Color color;

  void setSurfaceAlpha (SDL_Surface *surface, Uint8 alpha);    
};

void Font::Impl::setSurfaceAlpha( SDL_Surface *surface, Uint8 alpha )
{
  SDL_PixelFormat* fmt = surface->format;

  // If surface has no alpha channel, just set the surface alpha.
  if( fmt->Amask == 0 )
  {
    SDL_SetAlpha( surface, SDL_SRCALPHA, alpha );
  }
  // Else change the alpha of each pixel.
  else 
  {
    unsigned bpp = fmt->BytesPerPixel;
    // Scaling factor to clamp alpha to [0, alpha].
    float scale = alpha / 255.0f;

    SDL_LockSurface(surface);

    for (int y = 0; y < surface->h; ++y) 
    {
      for (int x = 0; x < surface->w; ++x) 
      {
        // Get a pointer to the current pixel.
        Uint32* pixel_ptr = (Uint32 *)( (Uint8 *)surface->pixels + y * surface->pitch + x * bpp );

        // Get the old pixel components.
        Uint8 r, g, b, a;
        SDL_GetRGBA( *pixel_ptr, fmt, &r, &g, &b, &a );

        // Set the pixel with the new alpha.
        *pixel_ptr = SDL_MapRGBA( fmt, r, g, b, (Uint8)(scale * a) );
      }   
    }
    
    SDL_UnlockSurface(surface);
  }
}
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

// unsigned int Font::getKerningSize( )
// {
//   if( _d->ttfFont )
//   {
//     return TTF_GetFontKerningSize( );
//   }
// }

unsigned int Font::getWidthFromCharacter( unsigned int c ) const
{
  int minx, maxx, miny, maxy, advance;
  TTF_GlyphMetrics( _d->ttfFont, c, &minx, &maxx, &miny, &maxy, &advance );
  
  return advance;
}

unsigned int Font::getKerningHeight() const {  return 3; }

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
  ret = (_d->color.unused << 24 ) + (_d->color.r << 16) + (_d->color.g << 8) + _d->color.b;
  return ret;
}

bool Font::isValid() const {  return _d->ttfFont != 0; }

Size Font::getSize( const std::string& text ) const
{
  int w, h;
  TTF_SizeUTF8( _d->ttfFont, text.c_str(), &w, &h );

  return Size( w, h );
}

bool Font::operator!=( const Font& other ) const
{
  return !( _d->ttfFont == other._d->ttfFont );
}

Rect Font::calculateTextRect(const std::string& text, const Rect& baseRect,
                             align::Type horizontalAlign, align::Type verticalAlign )
{
  Rect resultRect;
  Size d = getSize( text );

  // justification
  switch (horizontalAlign)
  {
  case align::center:
    // align to h centre
    resultRect.UpperLeftCorner.setX( (baseRect.getWidth()/2) - (d.width()/2) );
    resultRect.LowerRightCorner.setX( (baseRect.getWidth()/2) + (d.width()/2) );
    break;
  case align::lowerRight:
    // align to right edge
    resultRect.UpperLeftCorner.setX( baseRect.getWidth() - d.width() );
    resultRect.LowerRightCorner.setX( baseRect.getWidth() );
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
    resultRect.UpperLeftCorner.setY( (baseRect.getHeight()/2) - (d.height()/2) );
    resultRect.LowerRightCorner.setY( (baseRect.getHeight()/2) + (d.height()/2) );
    break;
  case align::lowerRight:
    // align to bottom edge
    resultRect.UpperLeftCorner.setY( baseRect.getHeight() - d.height() );
    resultRect.LowerRightCorner.setY( baseRect.getHeight() );
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

void Font::setColor( const NColor& color )
{
  _d->color.b = color.getBlue();
  _d->color.g = color.getGreen();
  _d->color.r = color.getRed();
  _d->color.unused = color.getAlpha();
}

void Font::draw(Picture& dstpic, const std::string &text, const int dx, const int dy, bool useAlpha )
{
  if( !_d->ttfFont || !dstpic.isValid() )
    return;

  SDL_Surface* sText = TTF_RenderUTF8_Blended( _d->ttfFont, text.c_str(), _d->color );
  if( sText && useAlpha )
  {
    SDL_SetAlpha( sText, 0, 0 );
  }

  if( sText )
  {
    Picture pic;
    pic.init( sText, Point( 0, 0 ) );
    dstpic.draw( pic, dx, dy);
  }

  SDL_FreeSurface( sText );
}       

void Font::draw( Picture &dstpic, const std::string &text, const Point& pos, bool useAlpha )
{
  draw( dstpic, text, pos.x(), pos.y(), useAlpha );
}

Font::~Font() {}

Font& Font::operator=( const Font& other )
{
  _d->ttfFont = other._d->ttfFont;
  _d->color = other._d->color;
  return *this;
}

Font Font::create( FontType type )
{
  return FontCollection::instance().getFont_( type );
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
    Logger::warning( "Error, font already exists, key=%d", key );
    return;
  }

  _d->fhelper.append( key, name );
}

void FontCollection::addFont(const int key, const std::string& name, const std::string& pathFont, const int size, const NColor& color )
{
  TTF_Font* ttf = TTF_OpenFont(pathFont.c_str(), size);
  if( ttf == NULL )
  {
    std::string errorStr = "Cannot load font file:" + pathFont + "\n, error:" + TTF_GetError();
    OSystem::error( "Critical error", errorStr );
    THROW( errorStr );
  }

  Font font0;
  font0._d->ttfFont = ttf;

  SDL_Color c = { color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
  font0._d->color = c;
  setFont( key, name, font0);
}

void FontCollection::initialize(const std::string &resourcePath)
{
  vfs::Directory resDir( resourcePath );
  vfs::Path fontDir( "FreeSerif.ttf" );
  vfs::Path full_font_path = resDir/fontDir;

  NColor black( 255, 0, 0, 0 );
  NColor red( 255, 160, 0, 0 );  // dim red
  NColor white( 255, 215, 215, 215 );  // dim white
  NColor yellow( 255, 160, 160, 0 );

  addFont( FONT_0,       CAESARIA_STR_EXT(FONT_0),      full_font_path.toString(), 12, black );
  addFont( FONT_1,       CAESARIA_STR_EXT(FONT_1),      full_font_path.toString(), 16, black );
  addFont( FONT_1_WHITE, CAESARIA_STR_EXT(FONT_1_WHITE),full_font_path.toString(), 16, white );
  addFont( FONT_1_RED,   CAESARIA_STR_EXT(FONT_1_RED),  full_font_path.toString(), 16, red );
  addFont( FONT_2,       CAESARIA_STR_EXT(FONT_2),      full_font_path.toString(), 18, black );
  addFont( FONT_2_RED,   CAESARIA_STR_EXT(FONT_2_RED),  full_font_path.toString(), 18, red );
  addFont( FONT_2_WHITE, CAESARIA_STR_EXT(FONT_2_WHITE),full_font_path.toString(), 18, white );
  addFont( FONT_2_YELLOW,CAESARIA_STR_EXT(FONT_2_YELLOW), full_font_path.toString(), 18, yellow );
  addFont( FONT_3,       CAESARIA_STR_EXT(FONT_3),      full_font_path.toString(), 28, black);
}

static StringArray _font_breakText(const std::string& text, const Font& f, int elWidth, bool RightToLeft )
{
  StringArray brokenText;

  Font font = f;

  if( !font.isValid() )
  {
    Logger::warning( "StringHelper::breakText font must be exists" );
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
					const int whitelgth = font.getSize( rwhitespace ).width();
					const int wordlgth = font.getSize( word ).width();

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
							const int secondLength = font.getSize( second ).width();

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
					const int whitelgth = font.getSize( rwhitespace ).width();
					const int wordlgth = font.getSize( word ).width();

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
