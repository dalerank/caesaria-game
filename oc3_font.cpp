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

#include "oc3_font.hpp"
#include "oc3_picture.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_exception.hpp"
#include <SDL_ttf.h>
#include <map>

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
  TTF_SizeText( _d->ttfFont, text.c_str(), &w, &h);

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

void Font::setColor( const int dc )
{
  _d->color.b = (dc & 0xff);
  _d->color.g = (dc >> 8) & 0xff;
  _d->color.r = (dc >> 16) & 0xff;
  _d->color.unused = ( dc >> 24 ) & 0xff;
}

void Font::draw(Picture& dstpic, const std::string &text, const int dx, const int dy )
{
  if( !_d->ttfFont || !dstpic.isValid() )
    return;

  SDL_Surface* sText = TTF_RenderUTF8_Blended( _d->ttfFont, text.c_str(), _d->color );

  if( sText )
  {
    Picture pic;
    pic.init( sText, Point( 0, 0 ) );
    dstpic.draw( pic, dx, dy);
  }

  SDL_FreeSurface( sText );
}

void Font::draw( Picture &dstpic, const std::string &text, const Point& pos )
{
  draw( dstpic, text, pos.getX(), pos.getY() );
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

class FontCollection::Impl
{
public:
  std::map<int, Font> collection;

  void addFont( const int key, const std::string& pathFont, const int size, const SDL_Color& color )
  {
    TTF_Font* ttf = TTF_OpenFont(pathFont.c_str(), size);
    if( ttf == NULL ) 
    {
      THROW("Cannot load font file:" << pathFont << ", error:" << TTF_GetError());
    }

    Font font0;
    font0._d->ttfFont = ttf;
    font0._d->color = color;
    setFont( key, font0);
  }

  void setFont( const int key, Font font )
  {
    std::pair< std::map<int, Font>::iterator, bool> ret = collection.insert(std::pair<int, Font>(key, font));
    if( ret.second == false )
    {
      // no insert font (already exists)
      StringHelper::debug( 0xff, "Error, font already exists, key=%d", key );
    }
  }
};

FontCollection& FontCollection::instance()
{
  static FontCollection inst;
  return inst;
}

FontCollection::FontCollection() : _d( new Impl )
{ }

Font& FontCollection::getFont_(const int key)
{
  std::map<int, Font>::iterator it = _d->collection.find(key);
  if (it == _d->collection.end())
  {
    StringHelper::debug( 0xff, "Error, font is not initialized, key=%d", key );
    return _d->collection[ FONT_2 ];
  }

  return (*it).second;
}

void FontCollection::setFont(const int key, Font font)
{
  _d->setFont( key, font );
}

void FontCollection::initialize(const std::string &resourcePath)
{
  std::string full_font_path = resourcePath + "/FreeSerif.ttf";

  SDL_Color black = {0, 0, 0, 255};
  SDL_Color red = {160, 0, 0, 255};  // dim red
  SDL_Color white = {215, 215, 215, 255};  // dim white
  SDL_Color yellow = {160, 160, 0, 255}; 

  _d->addFont( FONT_0, full_font_path, 12, black );
  _d->addFont( FONT_1, full_font_path, 16, black );
  _d->addFont( FONT_2, full_font_path, 18, black );
  _d->addFont( FONT_2_RED, full_font_path, 18, red );
  _d->addFont( FONT_2_WHITE, full_font_path, 18, white );
  _d->addFont( FONT_2_YELLOW, full_font_path, 18, yellow );
  _d->addFont( FONT_3, full_font_path, 28, black);
}

