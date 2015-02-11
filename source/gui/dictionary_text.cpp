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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "dictionary_text.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "scrollbar.hpp"
#include "core/variant_map.hpp"
#include "core/event.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"

using namespace std;
using namespace gfx;

namespace gui
{

static const int DEFAULT_SCROLLBAR_SIZE = 39;

struct TextToken
{
  Font font;
  std::string text;
  std::string alias;
  bool uri;
  int offset;

  const int width() const
  {
    return font.getTextSize( text ).width();
  }

  TextToken()
  {
    uri = false;
    offset = 0;
  }
};

struct Tokens : std::vector<TextToken>
{
  const std::string text() const
  {
    std::string ret;
    foreach( i, *this )  {    ret += i->text;      }
    return ret;
  }
};

typedef std::vector<Tokens> TokensArray;

class DictionaryText::Impl
{
public:
  TokensArray brokenText;
  Rect textMargin;
  Font lastBreakFont; // stored because: if skin changes, line break must be recalculated.
  Font font;
  bool isBorderVisible;
  bool OverrideBGColorEnabled;
  Point bgOffset;
  bool RestrainTextInside;
  bool lmbPressed;
  ScrollBar* scrollbar;
  bool needUpdatePicture;
  int lineIntervalOffset;
  Point textOffset;
  Picture bgPicture;
  PictureRef textPicture;
  unsigned int opaque;
  int yoffset;

  Impl() : textMargin( Rect( 0, 0, 0, 0) ),
           isBorderVisible( false ),
           OverrideBGColorEnabled(false),
           RestrainTextInside(true),
           needUpdatePicture(false), lineIntervalOffset( 0 )
  {
    font = Font::create( FONT_2 );
    lmbPressed = false;
    yoffset = 0;
    opaque = 0xff;
  }

  ~Impl()
  {
    textPicture.reset();
  }

  void breakText( const std::string& text, const Size& size );
  void setOffset( int value );

public signals:
  Signal1<std::string> onClickedSignal;
};

//! constructor
DictionaryText::DictionaryText( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
  _d->isBorderVisible = false;
  _d->needUpdatePicture = true;

  _init();
  setTextAlignment( align::automatic, align::automatic );
}

DictionaryText::DictionaryText(Widget* parent, const Rect& rectangle, const string& text, bool border, int id)
: Widget( parent, id, rectangle),
	_d( new Impl )
{
  _d->isBorderVisible = border;
  _d->needUpdatePicture = true;

  _init();

  #ifdef _DEBUG
    setDebugName( "label");
  #endif

  setTextAlignment( align::automatic, align::automatic );
  setText( text );
}

void DictionaryText::_updateTexture( gfx::Engine& painter )
{
  if( _d->textPicture && _d->textPicture->size() != size() )
  {
    _d->textPicture.reset( Picture::create( size(), 0, true ) );
  }

  if( !_d->textPicture )
  {
    _d->textPicture.reset( Picture::create( size(), 0, true ) );
  }

  if( _d->textPicture )
  {
    _d->textPicture->fill( 0x00ffffff, Rect( 0, 0, 0, 0) );
  }

  // draw button background
  bool useAlpha4Text = true;

  if( _d->font.isValid() )
  {
    Rect frameRect( Point( 0, 0 ), size() );
    string rText = text();

    if( rText.size() && _d->font.isValid() )
    {
      //eColor = GetResultColor( eColor );
      if( _d->font != _d->lastBreakFont )
      {
        _d->breakText( text(), size() );
      }

      Rect r = frameRect + Point( 0, -_d->yoffset );
      int height = _d->font.getTextSize("A").height();

      if( verticalTextAlign() == align::center )
      {
        r -= Point( 0, height * _d->brokenText.size() / 2 );
      }

      foreach( it, _d->brokenText )
      {
        Tokens& dline = *it;
        std::string tmpString = dline.text();

        Rect textRect = _d->font.getTextRect( tmpString, r, horizontalTextAlign(), verticalTextAlign() );
        textRect += _d->textOffset;

        int offset = 0;
        foreach( chunk, dline )
        {
          chunk->font.draw( *_d->textPicture, chunk->text,
                            textRect.lefttop() + Point( offset + chunk->offset, 0 ),
                            useAlpha4Text, false );
          offset += chunk->offset;
        }

        r += Point( 0, height + _d->lineIntervalOffset );
      }
    }
  }

  if( _d->textPicture )
  {
    _d->textPicture->setAlpha( _d->opaque );
    _d->textPicture->update();
  }
}

void DictionaryText::_handleClick( const Point& p)
{
  Point localPoint = screenToLocal( p );
  int rowHeight = _d->font.getTextSize( "A" ).height() + _d->lineIntervalOffset;
  int rowIndex = (_d->yoffset + localPoint.y()) / rowHeight;

  if( rowIndex >= _d->brokenText.size() )
    return;

  const Tokens& rline = _d->brokenText[ rowIndex ];
  std::string currentText;

  int offset = 0;
  for( Tokens::size_type index=0; index < rline.size(); index++ )
  {
    int leftLimit = offset + rline[index].offset;
    int rightLimit = index+1 < rline.size()
                       ? leftLimit + rline[ index + 1 ].offset
                       : width();

    if( leftLimit < localPoint.x() && localPoint.x() < rightLimit )
    {
      if( rline[ index ].uri )
      {
        currentText = rline[ index ].alias.empty()
                        ? rline[ index ].text
                        : rline[ index ].alias;
      }
      else
      {
        currentText = "";
      }
      break;
    }
    offset += rline[index].offset;
  }

  if( !currentText.empty() )
    emit _d->onClickedSignal( currentText );
}

//! destructor
DictionaryText::~DictionaryText() {}

//! draws the element and its children
void DictionaryText::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  // draw background
  if( _d->bgPicture.isValid() )
  {
    painter.draw( _d->bgPicture, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }

  if( _d->textPicture )
  {
    painter.draw( *_d->textPicture, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }

  Widget::draw( painter );
}

//! Get the font which is used right now for drawing
Font DictionaryText::font() const
{
	/*if( index == activeFont )
	{
		Font overrideFont = getFont( getActiveState().getHash() );
		Label* lb = const_cast< Label* >( this );
		if( !overrideFont.available() )
			overrideFont = Font( lb->getStyle().getState( lb->getActiveState() ).getFont() );

		return overrideFont.available() ? overrideFont : Font( getStyle().getName() );
	}

	return Widget::getFont( index );*/

	return _d->font;
}

//! Sets whether to draw the border
void DictionaryText::setBorderVisible(bool draw) {  _d->isBorderVisible = draw;}
void DictionaryText::setTextRestrainedInside(bool restrainTextInside){  _d->RestrainTextInside = restrainTextInside;}
bool DictionaryText::isTextRestrainedInside() const { return _d->RestrainTextInside;}

//! Breaks the single text line.
void DictionaryText::Impl::breakText( const std::string& text, const Size& wdgSize )
{
  brokenText.clear();

  if( !font.isValid() )
      return;

  lastBreakFont = font;

  Tokens dline;
  TextToken richText;
  std::string line;
  string word;
  string whitespace;
  string rText = text;
	int size = rText.size();
	int length = 0;
	int tabWidth = font.getTextSize( "A" ).width();
	int elWidth = wdgSize.width() - DEFAULT_SCROLLBAR_SIZE;

	char c;

	// We have to deal with right-to-left and left-to-right differently
	// However, most parts of the following code is the same, it's just
	// some order and boundaries which change.
	richText.font = font;
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
		else if( c == '\t' )
		{
			richText.offset += tabWidth;
			continue;
		}

		if( c == '@' )
		{
			richText.text += whitespace;
			dline.push_back( richText );
			const int linewidth = richText.width();
			richText = TextToken();
			richText.offset = linewidth;
			richText.font = font;
			richText.uri = true;
			richText.font.setColor( DefaultColors::blue );
			richText.text = "";
			whitespace = "";
		}

		if( c == '&' )
		{				
			const int whitelgth = font.getTextSize( whitespace ).width();
			const int wordlgth = font.getTextSize( word ).width();

			if(length && (length + wordlgth + whitelgth > elWidth) )
			{
				// break to next line
				std::string alias;
				if( !richText.text.empty() )
				{
					richText.alias = richText.text + whitespace + word;
					alias = richText.alias;
					dline.push_back( richText );
				}

				brokenText.push_back( dline );
				dline.clear();

				richText.text = word;
				richText.offset = 0;
				dline.push_back( richText );

				length = wordlgth;
				line = word;

				richText = TextToken();
				richText.font = font;
				richText.offset = length;
				richText.alias = alias;
				word = "";
			}
			else
			{
				line += whitespace;
				line += word;
				richText.text += whitespace;
				richText.text += word;
				dline.push_back( richText );
				const int linewidth = richText.width();
				richText = TextToken();
				richText.offset = linewidth;
				richText.font = font;
				richText.text = "";

				length += whitelgth + wordlgth;
				word = "";
			}
		}

		bool specSymbol = (c == '@' || c == '&');
		bool isWhitespace = (c == ' ' || c == 0);
		if ( !isWhitespace && !specSymbol )
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
				const int whitelgth = font.getTextSize( whitespace ).width();
				const int wordlgth = font.getTextSize( word ).width();

				if (wordlgth > elWidth)
				{
					// This word is too long to fit in the available space, look for
					// the Unicode Soft HYphen (SHY / 00AD) character for a place to
					// break the word at
					/*int where = word.find_first_of( char(0xAD) );
					if (where != -1)
					{
						string first  = word.substr(0, where);
						string second = word.substr(where, word.size() - where);
						brokenText.push_back(line + first + "-");
						const int secondLength = font.getTextSize( second ).width();

						length = secondLength;
						line = second;
					}
					else*/
					{
						// No soft hyphen found, so there's nothing more we can do
						// break to next line
						if (length)
						{
							dline.push_back( richText );
							brokenText.push_back( dline );
							dline.clear();
						}

						length = wordlgth;
						line = word;
					}
				}
				else if (length && (length + wordlgth + whitelgth > elWidth))
				{
					// break to next line
					dline.push_back( richText );
					brokenText.push_back( dline );
					dline.clear();

					length = wordlgth;
					line = word;
					richText.text = word;
					richText.offset = 0;
				}
				else
				{
					// add word to line
					line += whitespace;
					line += word;
					richText.text += whitespace;
					richText.text += word;

					length += whitelgth + wordlgth;
				}

				word = "";
				whitespace = "";
			}

			if ( isWhitespace )
			{
				whitespace += c;
			}

			// compute line break
			if( lineBreak )
			{
				line += whitespace;
				line += word;
				richText.text += whitespace;
				richText.text += word;

				dline.push_back( richText );
				brokenText.push_back( dline );
				dline.clear();

				richText = TextToken();
				richText.font = font;
				line = "";
				word = "";
				whitespace = "";
				length = 0;
			}
		}
	}

	line += whitespace;
	line += word;

	richText.text += whitespace;
	richText.text += word;

	dline.push_back( richText );
	brokenText.push_back( dline );

  int lineHeight = font.getTextSize("A").height() + lineIntervalOffset;
  int maxValue = std::max<int>( brokenText.size() * lineHeight - wdgSize.height(), 0);
  scrollbar->setMaxValue( maxValue );
  scrollbar->setEnabled( maxValue > 0  );
}

void DictionaryText::Impl::setOffset( int value)
{
  yoffset = value;
  needUpdatePicture = true;
}

//! Sets the new caption of this element.
void DictionaryText::setText(const string& newText)
{
  Widget::setText( newText );

  _d->yoffset = 0;
  _d->scrollbar->setValue( 0 );
  _d->breakText( text(), size() );
  _d->needUpdatePicture = true;
}

Signal1<std::string>& DictionaryText::onWordClick() {  return _d->onClickedSignal; }

//! Returns the height of the text in pixels when it is drawn.
int DictionaryText::textHeight() const
{
    Font font = _d->font;
    if( !font.isValid() )
        return 0;

    int height = font.getTextSize("A").height();// + font.GetKerningHeight();

    height *= _d->brokenText.size();

    return height;
}


int DictionaryText::textWidth() const
{
  Font font = _d->font;
  if( !font.isValid() )
      return 0;

  int widest = 0;

  for(unsigned int line = 0; line < _d->brokenText.size(); ++line)
  {
    int width = font.getTextSize( _d->brokenText[line].text() ).width();

    if(width > widest)
      widest = width;
  }

  return widest;
}

void DictionaryText::setPadding( const Rect& margin ) {  _d->textMargin = margin; }

void DictionaryText::beforeDraw(gfx::Engine& painter )
{
  if( _d->needUpdatePicture )
  {
    _updateTexture( painter );

    _d->needUpdatePicture = false;
  }

  Widget::beforeDraw( painter );
}

bool DictionaryText::onEvent(const NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type )
    {
    case mouseLbtnPressed: _d->lmbPressed = true;
    break;

    case mouseLbtnRelease:
    {
      _d->lmbPressed = false;
      _handleClick( event.mouse.pos() );
    }
    break;

    case mouseWheel:
    {

    }
    break;

    default: break;
    }
  }

  return Widget::onEvent( event );
}

bool DictionaryText::isBorderVisible() const {  return _d->isBorderVisible; }

void DictionaryText::setBackgroundPicture(const Picture& picture, Point offset )
{
  _d->bgPicture = picture;
  _d->bgOffset = offset;
  _d->needUpdatePicture = true;
}

void DictionaryText::setFont( const Font& font )
{
  _d->font = font;
  _d->needUpdatePicture = true;
}

void DictionaryText::setAlpha(unsigned int value)
{
  _d->opaque = value;
  _d->needUpdatePicture = true;
}

void DictionaryText::setTextAlignment( Alignment horizontal, Alignment vertical )
{
  Widget::setTextAlignment( horizontal, vertical );
  _d->needUpdatePicture = true;
}

void DictionaryText::_resizeEvent() {  _d->needUpdatePicture = true; }

void DictionaryText::setLineIntervalOffset( const int offset )
{
  _d->lineIntervalOffset = offset;
  _d->needUpdatePicture = true;
}

void DictionaryText::setupUI(const VariantMap& ui)
{
  Widget::setupUI( ui );

  setFont( Font::create( ui.get( "font", "FONT_2" ).toString() ) );
  setBackgroundPicture( Picture::load( ui.get( "image" ).toString() ) );

  Variant vTextOffset = ui.get( "text.offset" );
  if( vTextOffset.isValid() ){ setTextOffset( vTextOffset.toPoint() ); }
}

void DictionaryText::setTextOffset(Point offset) {  _d->textOffset = offset;}
PictureRef& DictionaryText::_textPictureRef(){  return _d->textPicture; }

void DictionaryText::_init()
{
  _d->scrollbar = new ScrollBar( this, Rect( width() - DEFAULT_SCROLLBAR_SIZE, 0, width(), height()), false );
  _d->scrollbar->setNotClipped( false );
  _d->scrollbar->setSubElement(true);
  _d->scrollbar->setVisibleFilledArea( false );
  _d->scrollbar->setTabStop(false);
  _d->scrollbar->setAlignment( align::lowerRight, align::lowerRight, align::upperLeft, align::lowerRight);
  _d->scrollbar->setVisible(true);
  _d->scrollbar->setValue(0);

  CONNECT( _d->scrollbar, onPositionChanged(), _d.data(), Impl::setOffset )
}

}//end namespace gui
