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

#include "label.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "core/event.hpp"
#include "core/variant_map.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color.hpp"

using namespace std;
using namespace gfx;

namespace gui
{

class LabelBackgroundHelper : public EnumsHelper<Label::BackgroundMode>
{
public:
  LabelBackgroundHelper()
    : EnumsHelper<Label::BackgroundMode>(Label::bgNone)
  {
    append( Label::bgWhite, "white" );
    append( Label::bgBlack, "black" );
    append( Label::bgBrown, "brown" );
    append( Label::bgSmBrown, "smallBrown" );
    append( Label::bgNone, "none" );
    append( Label::bgWhiteFrame, "whiteFrame" );
    append( Label::bgBlackFrame, "blackFrame" );
  }
};

class Label::Impl
{
public:
  StringArray brokenText;
  Rect textMargin;
  Font lastBreakFont; // stored because: if skin changes, line break must be recalculated.
  Font font;
  bool isBorderVisible;
  bool OverrideBGColorEnabled;
  bool isWordwrap;
  Point bgOffset;
  Label::BackgroundMode backgroundMode;
  bool RestrainTextInside;
  bool RightToLeft;
  bool lmbPressed;
  string prefix;
  bool needUpdatePicture;
  int lineIntervalOffset;
  Point textOffset, iconOffset;
  Picture bgPicture;
  Picture icon;
  Pictures background;
  PictureRef textPicture;
  unsigned int opaque;

  Impl() : textMargin( Rect( 0, 0, 0, 0) ),
           isBorderVisible( false ),
           OverrideBGColorEnabled(false), isWordwrap(false),
           backgroundMode( Label::bgNone ),
           RestrainTextInside(true), RightToLeft(false),
           needUpdatePicture(false), lineIntervalOffset( 0 )
  {
    font = Font::create( FONT_2 );
    lmbPressed = false;
    opaque = 0xff;
  }

  ~Impl()
  {
    textPicture.reset();
  }

  void breakText( const std::string& text, const Size& size );

public signals:
  Signal0<> onClickedSignal;
};

//! constructor
Label::Label( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
  _d->isBorderVisible = false;
  _d->backgroundMode = bgNone;
  _d->needUpdatePicture = true;

  setTextAlignment( align::automatic, align::automatic );
}

Label::Label(Widget* parent, const Rect& rectangle, const string& text, bool border,
						 BackgroundMode background, int id)
: Widget( parent, id, rectangle),
	_d( new Impl )
{
  _d->isBorderVisible = border;
  _d->backgroundMode = background;
  _d->needUpdatePicture = true;

  #ifdef _DEBUG
    setDebugName( "label");
  #endif

  setTextAlignment( align::automatic, align::automatic );
  setText( text );
}

void Label::_updateTexture(gfx::Engine& painter )
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
  if( !_d->bgPicture.isValid() )
  {
    _updateBackground( painter, useAlpha4Text );
  }

  if( _d->font.isValid() )
  {
    Rect frameRect( Point( 0, 0 ), size() );
    string rText = _d->prefix + text();

    if( rText.size() && _d->font.isValid() )
    {
      //eColor = GetResultColor( eColor );
      if( !_d->isWordwrap )
      {
        Rect textRect = _d->font.getTextRect( rText, frameRect, horizontalTextAlign(), verticalTextAlign() );

        textRect += _d->textOffset;
        _d->font.draw( *_d->textPicture, text(), textRect.lefttop(), useAlpha4Text, false );
      }
      else
      {
        if( _d->font != _d->lastBreakFont )
        {
            _d->breakText( text(), size() );
        }

        Rect r = frameRect;
        int height = _d->font.getTextSize("A").height();

        if( verticalTextAlign() == align::center )
        {
          r -= Point( 0, height * _d->brokenText.size() / 2 );
        }

        foreach( it, _d->brokenText )
        {
          Rect textRect = _d->font.getTextRect( *it, r, horizontalTextAlign(), verticalTextAlign() );
          textRect += _d->textOffset;
          _d->font.draw( *_d->textPicture, *it, textRect.lefttop(), useAlpha4Text, false );
          r += Point( 0, height + _d->lineIntervalOffset );
        }        
      }
    }
  }

  if( _d->textPicture )
  {
    _d->textPicture->setAlpha( _d->opaque );
    _d->textPicture->update();
  }
}

void Label::_updateBackground(Engine& painter, bool& useAlpha4Text )
{
  Rect r( Point( 0, 0 ), size() );
  _d->background.clear();

  switch( _d->backgroundMode )
  {
  case bgSimpleWhite:
    _d->textPicture->fill( 0xffffffff, Rect( 0, 0, 0, 0) );
    useAlpha4Text = false;
    Decorator::draw( *_d->textPicture, r, Decorator::lineBlackBorder );
  break;

  case bgSimpleBlack:
    _d->textPicture->fill( 0xff000000, Rect( 0, 0, 0, 0) );
    useAlpha4Text = false;
    Decorator::draw( *_d->textPicture, r, Decorator::lineWhiteBorder );
  break;

  case bgWhite: Decorator::draw( _d->background, r, Decorator::whiteArea); break;
  case bgBlack: Decorator::draw( _d->background, r, Decorator::blackArea ); break;
  case bgBrown: Decorator::draw( _d->background, r, Decorator::brownBorder );  break;
  case bgSmBrown: Decorator::draw( _d->background, r, Decorator::brownPanelSmall ); break;
  case bgWhiteFrame: Decorator::draw( _d->background, r, Decorator::whiteFrame ); break;
  case bgBlackFrame: Decorator::draw( _d->background, r, Decorator::blackFrame ); break;
  case bgNone:  break;
  case bgWhiteBorderA: Decorator::draw( _d->background, r, Decorator::whiteBorderA ); break;
  }
}

void Label::_handleClick()
{
  emit _d->onClickedSignal();
}

//! destructor
Label::~Label() {}

//! draws the element and its children
void Label::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  // draw background
  if( _d->bgPicture.isValid() )
  {
    painter.draw( _d->bgPicture, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }
  else
  {
    painter.draw( _d->background, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }

  if( _d->icon.isValid() )
  {
    painter.draw( _d->icon, absoluteRect().UpperLeftCorner + _d->iconOffset, &absoluteClippingRectRef() );
  }

  if( _d->textPicture )
  {
    painter.draw( *_d->textPicture, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }

  Widget::draw( painter );
}

//! Get the font which is used right now for drawing
Font Label::font() const
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

//! Sets whether to draw the background
void Label::setBackgroundMode( BackgroundMode mode )
{
  _d->backgroundMode = mode;
  _d->needUpdatePicture = true;
}

//! Sets whether to draw the border
void Label::setBorderVisible(bool draw) {  _d->isBorderVisible = draw;}
void Label::setTextRestrainedInside(bool restrainTextInside){  _d->RestrainTextInside = restrainTextInside;}
bool Label::isTextRestrainedInside() const{  return _d->RestrainTextInside;}

//! Enables or disables word wrap for using the static text as
//! multiline text control.
void Label::setWordwrap(bool enable)
{
  _d->isWordwrap = enable;
  _d->breakText( text(), size() );
  _d->needUpdatePicture = true;
}

bool Label::isWordWrapEnabled() const {  return _d->isWordwrap; }

void Label::setRightToLeft(bool rtl)
{
  if( _d->RightToLeft != rtl )
  {
    _d->RightToLeft = rtl;
    _d->breakText( text(), size() );
    _d->needUpdatePicture = true;
  }
}

bool Label::isRightToLeft() const{	return _d->RightToLeft;}

//! Breaks the single text line.
void Label::Impl::breakText( const std::string& text, const Size& wdgSize )
{
  if (!isWordwrap)
          return;

  brokenText.clear();

  if( !font.isValid() )
      return;

  lastBreakFont = font;

  string line;
  string word;
  string whitespace;
  string rText = prefix + text;
	int size = rText.size();
	int length = 0;
	int elWidth = wdgSize.width();

	char c;

	// We have to deal with right-to-left and left-to-right differently
	// However, most parts of the following code is the same, it's just
	// some order and boundaries which change.
	if (!RightToLeft)
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
					const int whitelgth = font.getTextSize( whitespace ).width();
					const int wordlgth = font.getTextSize( word ).width();

					if (wordlgth > elWidth)
					{
						// This word is too long to fit in the available space, look for
						// the Unicode Soft HYphen (SHY / 00AD) character for a place to
						// break the word at
						int where = word.find_first_of( char(0xAD) );
						if (where != -1)
						{
							string first  = word.substr(0, where);
							string second = word.substr(where, word.size() - where);
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
						line += whitespace;
						line += word;
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
				if (lineBreak)
				{
					line += whitespace;
					line += word;
					brokenText.push_back(line);
					line = "";
					word = "";
					whitespace = "";
					length = 0;
				}
			}
		}

		line += whitespace;
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

			if (c == L'\r') // Mac or Windows breaks
			{
				lineBreak = true;
				if ((i>0) && rText[i-1] == L'\n') // Windows breaks
				{
					rText.erase(i-1);
					--size;
				}
				c = '\0';
			}
			else if (c == L'\n') // Unix breaks
			{
				lineBreak = true;
				c = '\0';
			}

			if (c==L' ' || c==0 || i==0)
			{
				if (word.size())
				{
					// here comes the next whitespace, look if
					// we must break the last word to the next line.
					const int whitelgth = font.getTextSize( whitespace ).width();
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
						line = whitespace + line;
						line = word + line;
						length += whitelgth + wordlgth;
					}

					word = "";
					whitespace = "";
				}

				if (c != 0)
					whitespace = string(&c, 1) + whitespace;

				// compute line break
				if (lineBreak)
				{
					line = whitespace + line;
					line = word + line;
					brokenText.push_back(line);
					line = "";
					word = "";
					whitespace = "";
					length = 0;
				}
			}
			else
			{
				// yippee this is a word..
				word = string(&c, 1) + word;
			}
		}

		line = whitespace + line;
		line = word + line;
		brokenText.push_back(line);
	}
}


//! Sets the new caption of this element.
void Label::setText(const string& newText)
{
  Widget::setText( newText );

  _d->breakText( text(), size() );
  _d->needUpdatePicture = true;
}

Signal0<>& Label::onClicked() {  return _d->onClickedSignal; }

//! Returns the height of the text in pixels when it is drawn.
int Label::textHeight() const
{
    Font font = _d->font;
    if( !font.isValid() )
        return 0;

    int height = font.getTextSize("A").height();// + font.GetKerningHeight();

    if( _d->isWordwrap)
            height *= _d->brokenText.size();

    return height;
}


int Label::textWidth() const
{
    Font font = _d->font;
    if( !font.isValid() )
        return 0;

    if( _d->isWordwrap )
    {
      int widest = 0;

      for(unsigned int line = 0; line < _d->brokenText.size(); ++line)
      {
        int width = font.getTextSize( _d->brokenText[line] ).width();

        if(width > widest)
          widest = width;
      }

      return widest;
    }
    else
    {
      return font.getTextSize( text() ).width();
    }
}

void Label::setPadding( const Rect& margin ) {  _d->textMargin = margin; }

void Label::beforeDraw(gfx::Engine& painter )
{
  if( _d->needUpdatePicture )
  {
    _updateTexture( painter );

    _d->needUpdatePicture = false;
  }

  Widget::beforeDraw( painter );
}

Label::BackgroundMode Label::backgroundMode() const {  return _d->backgroundMode; }

bool Label::onEvent(const NEvent& event)
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
      _handleClick();
    }
    break;

    default: break;
    }
  }

  return Widget::onEvent( event );
}

bool Label::isBorderVisible() const {  return _d->isBorderVisible; }

void Label::setPrefixText( const string& prefix )
{
  _d->prefix = prefix;
  _d->needUpdatePicture = true;
}

void Label::setBackgroundPicture(const Picture& picture, Point offset )
{
  _d->bgPicture = picture;
  _d->bgOffset = offset;
  _d->needUpdatePicture = true;
}

void Label::setIcon(const Picture& icon, Point offset )
{
  _d->icon = icon;
  _d->iconOffset = offset;
  _d->needUpdatePicture = true;
}

void Label::setFont( const Font& font )
{
  _d->font = font;
  _d->needUpdatePicture = true;
}

void Label::setAlpha(unsigned int value)
{
  _d->opaque = value;
  _d->needUpdatePicture = true;
}

void Label::setColor(NColor color)
{
  _d->font.setColor( color );
  _d->needUpdatePicture = true;
}

void Label::setTextAlignment( Alignment horizontal, Alignment vertical )
{
  Widget::setTextAlignment( horizontal, vertical );
  _d->needUpdatePicture = true;
}

void Label::_resizeEvent() {  _d->needUpdatePicture = true; }

void Label::setLineIntervalOffset( const int offset )
{
  _d->lineIntervalOffset = offset;
  _d->needUpdatePicture = true;
}

void Label::setupUI(const VariantMap& ui)
{
  Widget::setupUI( ui );

  setFont( Font::create( ui.get( "font", "FONT_2" ).toString() ) );
  setBackgroundPicture( Picture::load( ui.get( "image" ).toString() ) );
  setWordwrap( (bool)ui.get( "multiline", false ) );

  Variant vTextOffset = ui.get( "text.offset" );
  if( vTextOffset.isValid() ){ setTextOffset( vTextOffset.toPoint() ); }

  Variant vIcon = ui.get( "icon" );
  if( vIcon.isValid() )
  {
    Point iconOffset = ui.get( "icon.offset" ).toPoint();
    setIcon( Picture::load( vIcon.toString() ), iconOffset );
  }

  LabelBackgroundHelper helper;
  Label::BackgroundMode mode = helper.findType( ui.get( "bgtype" ).toString() );
  if( mode != bgNone )
    setBackgroundMode( mode );
}

void Label::setTextOffset(Point offset) {  _d->textOffset = offset;}
PictureRef& Label::_textPictureRef(){  return _d->textPicture; }
gfx::Pictures&Label::_backgroundRef(){ return _d->background; }

}//end namespace gui
