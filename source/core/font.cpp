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

#include "font.hpp"
#include "gfx/picture.hpp"
#include "logger.hpp"
#include "exception.hpp"
#include <SDL_ttf.h>
#include "color.hpp"
#include <map>

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

unsigned int Font::getWidthFromCharacter( char c ) const
{
  int minx, maxx, miny, maxy, advance;
  TTF_GlyphMetrics( _d->ttfFont, c, &minx, &maxx, &miny, &maxy, &advance );
  
  return advance;
}

unsigned int Font::getKerningHeight() const
{
  return 3;
}

int Font::getCharacterFromPos(const std::string& text, int pixel_x) const
{
  int x = 0;
  int idx = 0;

  while (text[idx])
  {
    x += getWidthFromCharacter(text[idx]);

    if (x >= pixel_x)
      return idx;

    ++idx;
  }

  return -1;
}

int Font::getColor() const
{
  int ret = 0;
  ret = (_d->color.unused << 24 ) + (_d->color.r << 16) + (_d->color.g << 8) + _d->color.b;
  return ret;
}

bool Font::isValid() const
{
  return _d->ttfFont != 0;
}

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
  draw( dstpic, text, pos.getX(), pos.getY(), useAlpha );
}

Font::~Font()
{

}

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
    THROW("Cannot load font file:" << pathFont << ", error:" << TTF_GetError());
  }

  Font font0;
  font0._d->ttfFont = ttf;

  SDL_Color c = { color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() };
  font0._d->color = c;
  setFont( key, name, font0);
}

void FontCollection::initialize(const std::string &resourcePath)
{
  std::string full_font_path = resourcePath + "/FreeSerif.ttf";

  NColor black( 255, 0, 0, 0 );
  NColor red( 255, 160, 0, 0 );  // dim red
  NColor white( 255, 215, 215, 215 );  // dim white
  NColor yellow( 255, 160, 160, 0 );

  addFont( FONT_0,       OC3_STR_EXT(FONT_0),      full_font_path, 12, black );
  addFont( FONT_1,       OC3_STR_EXT(FONT_1),      full_font_path, 16, black );
  addFont( FONT_1_WHITE, OC3_STR_EXT(FONT_1_WHITE),full_font_path, 16, white );
  addFont( FONT_1_RED,   OC3_STR_EXT(FONT_1_RED),  full_font_path, 16, red );
  addFont( FONT_2,       OC3_STR_EXT(FONT_2),      full_font_path, 18, black );
  addFont( FONT_2_RED,   OC3_STR_EXT(FONT_2_RED),  full_font_path, 18, red );
  addFont( FONT_2_WHITE, OC3_STR_EXT(FONT_2_WHITE),full_font_path, 18, white );
  addFont( FONT_2_YELLOW,OC3_STR_EXT(FONT_2_YELLOW), full_font_path, 18, yellow );
  addFont( FONT_3,       OC3_STR_EXT(FONT_3),      full_font_path, 28, black);
}

