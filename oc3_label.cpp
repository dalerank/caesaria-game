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


#include "oc3_label.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_pictureconverter.hpp"

using namespace std;

typedef vector< string > StringArray;

class Label::Impl
{
public:
  StringArray brokenText;
  Rect textMargin;
  Font lastBreakFont; // stored because: if skin changes, line break must be recalculated.
  Font font;
  bool isBorderVisible;
  bool OverrideBGColorEnabled;
  bool WordWrap;
  Point bgOffset;
  bool isBackgroundVisible;
  bool RestrainTextInside;
  bool RightToLeft;
  string prefix;
  bool needUpdatePicture;
  int lineIntervalOffset;
  Picture* bgPicture;
  Picture* picture;

  Impl() : textMargin( Rect( 0, 0, 0, 0) ),
		    	 OverrideBGColorEnabled(false), WordWrap(false),
			     RestrainTextInside(true), RightToLeft(false), 
           needUpdatePicture(false), bgPicture( 0 ), picture( 0 ),
           lineIntervalOffset( 0 )
	{
    font = Font( FONT_2 );
	}

    ~Impl()
    {
        releaseTexture();
    }

    void breakText( const std::string& text, const Size& size );

    void releaseTexture()
    {
        if( picture )
        {
          GfxEngine::instance().deletePicture( *picture );
          picture = 0;
        }
    }

public oc3_signals:
	Signal0<> onClickedSignal;
};

//! constructor
Label::Label(Widget* parent, const Rect& rectangle, const string& text, bool border,
						bool background, int id)
: Widget( parent, id, rectangle),
	_d( new Impl )
{
  _d->bgPicture = 0;
  _d->isBorderVisible = border;
  _d->isBackgroundVisible = background;
 

  #ifdef _DEBUG
    setDebugName( "OC3_label");
  #endif

  setTextAlignment( alignAuto, alignAuto );
  setText( text );
}

void Label::_updateTexture( GfxEngine& painter )
{
    Size btnSize = getSize();

    if( _d->picture && _d->picture->getSize() != btnSize )
        _d->releaseTexture();

    if( !_d->picture )
    {
        _d->picture = &painter.createPicture( btnSize.getWidth(), btnSize.getHeight() );
        painter.loadPicture( *_d->picture );
    }

    // draw button background
    if( _d->bgPicture )
    {
        _d->picture->draw( *_d->bgPicture, _d->bgOffset.getX(), _d->bgOffset.getY() );
    }    
    else
    {
        if( !_d->isBackgroundVisible )
        {
            GuiPaneling::instance().draw_white_area( *_d->picture, 0, 0, getSize().getWidth(), getSize().getHeight() );
        }
        else
        {
            GuiPaneling::instance().draw_black_area( *_d->picture, 0, 0, getSize().getWidth(), getSize().getHeight() );
        }
    }

    if( _d->font.isValid() )
    {
        Rect frameRect( Point( 0, 0 ), getSize() );
        string rText = _d->prefix + _text;

        if( rText.size() && _d->font.isValid() )
        {
            //eColor = GetResultColor( eColor );
            if( !_d->WordWrap )
            {
                Rect textRect = _d->font.calculateTextRect( rText, frameRect, 
                                                            getHorizontalTextAlign(), getVerticalTextAlign() );

                _d->font.draw( *_d->picture, getText(), textRect.getLeft(), textRect.getTop() );
            }
            else
            {
                if( _d->font != _d->lastBreakFont )
                {  
                    _d->breakText( getText(), getSize() );
                }

                Rect r = frameRect;
                int height = _d->font.getSize("A").getHeight();// + font.GetKerningHeight();

                for (unsigned int i=0; i<_d->brokenText.size(); ++i)
                {
                    Rect textRect = _d->font.calculateTextRect( rText, r, 
                                                                getHorizontalTextAlign(), getVerticalTextAlign() );

                    _d->font.draw( *_d->picture, _d->brokenText[i], textRect.getLeft(), textRect.getTop() );

                    r += Point( 0, height + _d->lineIntervalOffset );
                }
            }
        }
    }
}

//! destructor
Label::~Label()
{
}

//! draws the element and its children
void Label::draw( GfxEngine& painter )
{
  if ( !isVisible() )
    return;

  Rect frameRect( getAbsoluteRect() );

  // draw background
  if( _d->picture )
    painter.drawPicture( *_d->picture, frameRect.getLeft(), frameRect.getTop() );

  Widget::draw( painter );
}

//! Get the font which is used right now for drawing
Font Label::getFont() const
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

//! Sets another color for the text.
// void Label::setBackgroundColor( Color color)
// {
// 	setColor( color, bgColor );
// 	_d->OverrideBGColorEnabled = true;
// 	_d->isBackgroundVisible = true;
// }

//! Sets whether to draw the background
void Label::setBackgroundVisible(bool draw)
{
  _d->isBackgroundVisible = draw;
}

//! Sets whether to draw the border
void Label::setBorderVisible(bool draw)
{
  _d->isBorderVisible = draw;
}


void Label::setTextRestrainedInside(bool restrainTextInside)
{
  _d->RestrainTextInside = restrainTextInside;
}


bool Label::isTextRestrainedInside() const
{
  return _d->RestrainTextInside;
}


//! Enables or disables word wrap for using the static text as
//! multiline text control.
void Label::setWordWrap(bool enable)
{
  _d->WordWrap = enable;
  _d->breakText( getText(), getSize() );
  _d->needUpdatePicture = true;
}

bool Label::isWordWrapEnabled() const
{
  return _d->WordWrap;
}

void Label::setRightToLeft(bool rtl)
{
  if( _d->RightToLeft != rtl )
  {
    _d->RightToLeft = rtl;
    _d->breakText( getText(), getSize() );
    _d->needUpdatePicture = true;
  }
}


bool Label::isRightToLeft() const
{
	return _d->RightToLeft;
}


//! Breaks the single text line.
void Label::Impl::breakText( const std::string& text, const Size& wdgSize )
{
    if (!WordWrap)
            return;

    brokenText.clear();

    _OC3_DEBUG_BREAK_IF( !font.isValid() && "Font must be exists" );

    if( !font.isValid() )
        return;

    lastBreakFont = font;

    string line;
    string word;
    string whitespace;
    string rText = prefix + text;
	int size = rText.size();
	int length = 0;
	int elWidth = wdgSize.getWidth();

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

			if (c == '\r') // Mac or Windows breaks
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
					const int whitelgth = font.getSize( whitespace ).getWidth();
					const int wordlgth = font.getSize( word ).getWidth();

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
							const int secondLength = font.getSize( second ).getWidth();

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
					const int whitelgth = font.getSize( whitespace ).getWidth();
					const int wordlgth = font.getSize( word ).getWidth();

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
void Label::setText(const string& text)
{
    Widget::setText( text );

    _d->breakText( getText(), getSize() );
    _d->needUpdatePicture = true;
}

Signal0<>& Label::onClicked()
{
    return _d->onClickedSignal;
}

//! Returns the height of the text in pixels when it is drawn.
int Label::getTextHeight() const
{
    Font font = _d->font;
    if( !font.isValid() )
        return 0;

    int height = font.getSize("A").getHeight();// + font.GetKerningHeight();

    if( _d->WordWrap)
            height *= _d->brokenText.size();

    return height;
}


int Label::getTextWidth() const
{
    Font font = _d->font;
    if( !font.isValid() )
        return 0;

    if( _d->WordWrap )
    {
        int widest = 0;

        for(unsigned int line = 0; line < _d->brokenText.size(); ++line)
        {
            int width = font.getSize( _d->brokenText[line] ).getWidth();

            if(width > widest)
               widest = width;
        }

        return widest;
    }
    else
    {
        return font.getSize( _text ).getWidth();
    }
}

void Label::setPadding( const Rect& margin )
{
  _d->textMargin = margin;
}

int Label::getBackgroundColor() const
{
  return 0;//_d->bgColor; //getColor( bgColor );
}

void Label::beforeDraw( GfxEngine& painter )
{
  if( _d->needUpdatePicture )
  {
    _updateTexture( painter );

    _d->needUpdatePicture = false;		
  }

  Widget::beforeDraw( painter );
}

bool Label::isBackgroundVisible() const
{
  return _d->isBackgroundVisible;
}

bool Label::isBorderVisible() const
{
  return _d->isBorderVisible;
}

void Label::setPrefixText( const string& prefix )
{
  _d->prefix = prefix;
  _d->needUpdatePicture = true;
}

void Label::setBackgroundPicture( const Picture& picture, const Point& offset )
{
    _d->bgPicture = const_cast< Picture* >( &picture );
    _d->bgOffset = offset;
    _d->needUpdatePicture = true;
}

void Label::setFont( const Font& font )
{
  _d->font = font;
  _d->needUpdatePicture = true;
}

void Label::setTextAlignment( TypeAlign horizontal, TypeAlign vertical )
{
  Widget::setTextAlignment( horizontal, vertical );
  _d->needUpdatePicture = true;
}

void Label::resizeEvent_()
{
    _d->needUpdatePicture = true;
}

void Label::setLineIntervalOffset( const int offset )
{
    _d->lineIntervalOffset = offset;
    _d->needUpdatePicture = true;
}