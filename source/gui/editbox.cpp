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

#include "editbox.hpp"
#include "core/stringarray.hpp"
#include "core/event.hpp"
#include "environment.hpp"
#include "gfx/engine.hpp"
#include "core/variant_map.hpp"
#include "gfx/picture.hpp"
#include "core/time.hpp"
#include "core/foreach.hpp"
#include "gfx/decorator.hpp"

using namespace gfx;

namespace gui
{

class EditBox::Impl
{
public:
  std::wstring text;
  std::vector<std::wstring> brokenText;
  std::vector<int> brokenTextPositions;
  Rect currentTextRect;
  Rect cursorRect;
  Rect markAreaRect;
  Point textOffset;
  Font overrideFont;
  Font lastBreakFont;
	bool mouseMarking;
	bool border;
	bool drawBackground;
	bool overrideColorEnabled;
	int markBegin;
	int markEnd;
  bool needUpdateTexture;
	NColor overrideColor;

	int cursorPos, oldCursorPos;
	int horizScrollPos, vertScrollPos; // scroll position in characters
	unsigned int max;
	std::string holderText;
  Picture bgPicture;
  Pictures background;
  PictureRef textPicture;

	bool wordWrapEnabled, multiLine, autoScrollEnabled, isPasswordBox;
	char passwordChar;

  Impl() : currentTextRect(0,0,1,1)
	{
		oldCursorPos = 0;
		mouseMarking = false;
		overrideColorEnabled = false;
		markBegin = 0;
		markEnd = 0;
		border = false;
		drawBackground = true;
		overrideColor = NColor(101,255,255,255);
		cursorPos = 0;
		horizScrollPos = 0;
		vertScrollPos = 0;
		max = 0;
		wordWrapEnabled = false;
		multiLine = false;
		autoScrollEnabled = true;
		isPasswordBox = false;
		passwordChar = '*';
		needUpdateTexture = true;
	}

	int getCursorPos( EditBox* who, int x, int y );

  //! sets the area of the given line
  void setTextRect( EditBox* who, int line, const std::string& r="");

signals public:
  Signal1<std::string> onTextChangedSignal;
  Signal0<> onEnterPressedSignal;
};

std::string __ucs2utf8( const std::wstring& text )
{
	std::string ret;
	foreach( i, text )
	{
		if( (unsigned short)*i < 0x80 )
		{
			ret.push_back( (char)(*i & 0xff ) );
		}
		else
		{
			ret.push_back( (char)( (*i >> 8) & 0xff) );
			ret.push_back( (char)( *i & 0xff ) );
		}
	}

	return ret;
}

void EditBox::_init()
{
  _d->lastBreakFont = activeFont();

  #ifdef _DEBUG
      setDebugName( "EditBox");
  #endif

  // this element can be tabbed to
  setTabStop(true);
  setTabOrder(-1);

  _breakText();

  _calculateScrollPos();

  setTextAlignment( align::upperLeft, align::center );
}

void EditBox::_setText(const std::wstring& r)
{
  _d->text = r;
  emit _d->onTextChangedSignal( __ucs2utf8( r ) );

  if( (unsigned int)_d->cursorPos > _d->text.size())
  {
    _d->cursorPos = _d->text.size();
  }

  _d->horizScrollPos = 0;
  _breakText();

  _d->needUpdateTexture = true;
}

EditBox::EditBox( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _init();
}

//! constructor
EditBox::EditBox( Widget* parent, const Rect& rectangle, const std::string& text, int id, bool border )
	: Widget( parent, id, rectangle), _d( new Impl )
{
	_init();

	setText( text );
}

Signal1<std::string>& EditBox::onTextChanged() {	return _d->onTextChangedSignal;}
Signal0<>&EditBox::onEnterPressed(){ return _d->onEnterPressedSignal;	}
EditBox::~EditBox() {}

//! Sets another skin independent font.
void EditBox::setFont( const Font& font )
{
  _d->overrideFont = font;
  _breakText();
	_d->needUpdateTexture = true;
}

//! Gets the override font (if any)
Font EditBox::font() const {	return _d->overrideFont; }

//! Get the font which is used right now for drawing
Font EditBox::activeFont()
{
  if ( _d->overrideFont.isValid() )
    return _d->overrideFont;
  
  return Font::create( FONT_2 );
}

//! Sets another color for the text.
void EditBox::setOverrideColor( NColor color )
{
	_d->overrideColor = color;
	_d->overrideColorEnabled = true;
}

//! Turns the border on or off
void EditBox::setDrawBorder(bool border) {	_d->border = border; }

//! Sets if the text should use the overide color or the color in the gui skin.
void EditBox::setEnabledColor(bool enable) {	_d->overrideColorEnabled = enable; }
bool EditBox::isOverrideColorEnabled() const{	return _d->overrideColorEnabled; }

//! Enables or disables word wrap
void EditBox::setWordWrap(bool enable)
{
	_d->wordWrapEnabled = enable;
	_breakText();
}

void EditBox::_resizeEvent()
{
  _breakText();
  _calculateScrollPos();
  _d->needUpdateTexture = true;
}

bool EditBox::isWordwrapEnabled() const {	return _d->wordWrapEnabled; }   //! Checks if word wrap is enabled
void EditBox::setMultiline(bool enable) {	_d->multiLine = enable; }       //! Enables or disables newlines.
bool EditBox::isMultilineEnabled() const {	return _d->multiLine; }       //! Checks if multi line editing is enabled

void EditBox::moveCursor(int index)
{  
  _d->cursorPos = math::clamp<int>( index, 0, _d->text.size() );
}

void EditBox::setPasswordBox(bool passwordBox, char passwordChar)
{
	_d->isPasswordBox = passwordBox;
	if (_d->isPasswordBox)
	{
		_d->passwordChar = passwordChar;
		setMultiline(false);
		setWordWrap(false);
		_d->brokenText.clear();
	}
}

bool EditBox::isPasswordBox() const {	return _d->isPasswordBox;}
void EditBox::setupUI(const VariantMap& ui)
{
	Widget::setupUI( ui );

	setFont( Font::create( ui.get( "font", "FONT_2" ).toString() ) );

	_d->textOffset = ui.get( "textOffset" ).toPoint();

	Variant vOffset = ui.get( "text.offset" );
	if( vOffset.isValid() )
		_d->textOffset = vOffset.toPoint();

	_d->needUpdateTexture = true;
}

//! Sets text justification
void EditBox::setTextAlignment(Alignment horizontal, Alignment vertical)
{
	Widget::setTextAlignment( horizontal, vertical );
}

//! called if an event happened.
bool EditBox::onEvent(const NEvent& event)
{
	if (enabled())
	{
		switch(event.EventType)
		{
		case sEventGui:
			if (event.gui.type == guiElementFocusLost)
			{
				if (event.gui.caller == this)
				{
					_d->mouseMarking = false;
					_setTextMarkers(0,0);
				}
			}
			break;
		case sTextInput:
			_inputChar(*(unsigned short*)event.text.text);
		break;

		case sEventKeyboard:
			if (_processKey(event))
				return true;
			break;
		case sEventMouse:
			if (_processMouse(event))
				return true;
			break;
		default:
			break;
		}
	}

	return Widget::onEvent(event);
}


bool EditBox::_processKey(const NEvent& event)
{
	if (!event.keyboard.pressed)
		return false;

	bool textChanged = false;
	int newMarkBegin = _d->markBegin;
	int newMarkEnd = _d->markEnd;

  // control shortcut handling
  if (event.keyboard.control)
	{
		// german backlash '\' entered with control + '?'
		if ( event.keyboard.symbol == L'\\' )
		{
			_inputChar(event.keyboard.symbol);
			return true;
		}

		switch(event.keyboard.key)
		{
		case KEY_KEY_A:
			// select all
			newMarkBegin = 0;
			newMarkEnd = _d->text.size();
		break;

		case KEY_KEY_C:
			// copy to clipboard
			/*if (!_d->isPasswordBox && _d->osOperator && _d->markBegin != _d->markEnd)
			{
				const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
				const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;

				String s;
				s = _text.subString(realmbgn, realmend - realmbgn).c_str();
				_d->osOperator->copyToClipboard( s );
			}*/
	  break;
		
    case KEY_KEY_X:
			// cut to the clipboard
			/*if (!_d->isPasswordBox && _d->osOperator && _d->markBegin != _d->markEnd)
			{
				const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
				const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;

				// copy
				String sc;
				sc = _text.subString(realmbgn, realmend - realmbgn).c_str();
				_d->osOperator->copyToClipboard( sc );

				if (isEnabled())
				{
					// delete
					String s;
					s = _text.subString(0, realmbgn);
					s.append( _text.subString(realmend, _text.size()-realmend) );
					_text = s;

					_d->cursorPos = realmbgn;
					newMarkBegin = 0;
					newMarkEnd = 0;
					textChanged = true;
				}
			}*/
	  break;
		case KEY_KEY_V:
			if ( !enabled() )
				break;

			// paste from the clipboard
			/*if (_d->osOperator)
			{
				const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
				const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;

				// add new character
                String p( _d->osOperator->getTextFromClipboard() );
				if( p.size() )
				{
					if (_d->markBegin == _d->markEnd)
					{
						// insert text
						String s = _text.subString(0, _d->cursorPos);
                        s.append( p );
						s.append( _text.subString(_d->cursorPos, _text.size()-_d->cursorPos) );

						if (!_d->max || s.size()<=_d->max) // thx to Fish FH for fix
						{
							_text = s;
							s = p;
							_d->cursorPos += s.size();
						}
					}
					else
					{
						// replace text

						String s = _text.subString(0, realmbgn);
						s.append(p);
						s.append( _text.subString(realmend, _text.size()-realmend) );

						if (!_d->max || s.size()<=_d->max)  // thx to Fish FH for fix
						{
							_text = s;
							s = p;
							_d->cursorPos = realmbgn + s.size();
						}
					}
				}

				newMarkBegin = 0;
				newMarkEnd = 0;
				textChanged = true;
			}*/
	  break;
		
    case KEY_HOME:
			// move/highlight to start of text
			if (event.keyboard.shift)
			{
				newMarkEnd = _d->cursorPos;
				newMarkBegin = 0;
				_d->cursorPos = 0;
			}
			else
			{
				_d->cursorPos = 0;
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			break;
		case KEY_END:
			// move/highlight to end of text
			if (event.keyboard.shift)
			{
				newMarkBegin = _d->cursorPos;
				newMarkEnd = _d->text.size();
				_d->cursorPos = 0;
			}
			else
			{
				_d->cursorPos = _d->text.size();
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			break;
		default:
			return false;
		}
	} 
	// default keyboard handling
	else
	switch(event.keyboard.key)
	{
	case KEY_END:
		{
			int p = _d->text.size();
			if (_d->wordWrapEnabled || _d->multiLine)
			{
				p = _getLineFromPos(_d->cursorPos);
				p = _d->brokenTextPositions[p] + (int)_d->brokenText[p].size();
				if (p > 0 && (_d->text[ p-1 ] == L'\r' || _d->text[ p-1 ] == L'\n' ))
					p-=1;
			}

			if (event.keyboard.shift)
			{
				if (_d->markBegin == _d->markEnd)
					newMarkBegin = _d->cursorPos;

				newMarkEnd = p;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			_d->cursorPos = p;
		}
		break;
	case KEY_HOME:
		{

			int p = 0;
			if (_d->wordWrapEnabled || _d->multiLine)
			{
				p = _getLineFromPos(_d->cursorPos);
				p = _d->brokenTextPositions[p];
			}

			if (event.keyboard.shift)
			{
				if (_d->markBegin == _d->markEnd)
					newMarkBegin = _d->cursorPos;
				newMarkEnd = p;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}
			_d->cursorPos = p;
		}
		break;
	case KEY_RETURN:
		if (_d->multiLine)
		{
			_inputChar(L'\n');
			return true;
		}
		else
		{
			emit _d->onEnterPressedSignal();
			_sendGuiEvent( guiEditboxEnter );
		}
		break;
	case KEY_LEFT:
		if (event.keyboard.shift)
		{
			if (_d->cursorPos > 0)
			{
				if (_d->markBegin == _d->markEnd)
					newMarkBegin = _d->cursorPos;

				newMarkEnd = _d->cursorPos-1;
			}
		}
		else
		{
			newMarkBegin = 0;
			newMarkEnd = 0;
		}

		if (_d->cursorPos > 0)
			_d->cursorPos -= 1;
		break;

	case KEY_RIGHT:
		if (event.keyboard.shift)
		{
			if( _d->text.size() > (unsigned int)_d->cursorPos)
			{
				if (_d->markBegin == _d->markEnd)
					newMarkBegin = _d->cursorPos;

				newMarkEnd = _d->cursorPos+1;
			}
		}
		else
		{
			newMarkBegin = 0;
			newMarkEnd = 0;
		}

		if( _d->text.size() > (unsigned int)_d->cursorPos)
		{
			_d->cursorPos+=1;
		}
		break;
	case KEY_UP:
		if (_d->multiLine || (_d->wordWrapEnabled && _d->brokenText.size() > 1) )
		{
			int lineNo = _getLineFromPos(_d->cursorPos);
			int mb = (_d->markBegin == _d->markEnd) ? _d->cursorPos : (_d->markBegin > _d->markEnd ? _d->markBegin : _d->markEnd);
			if (lineNo > 0)
			{
				int cp = _d->cursorPos - _d->brokenTextPositions[lineNo];
				if ((int)_d->brokenText[lineNo-1].size() < cp)
					_d->cursorPos = _d->brokenTextPositions[lineNo-1] + (int)_d->brokenText[lineNo-1].size()-1;
				else
					_d->cursorPos = _d->brokenTextPositions[lineNo-1] + cp;
			}

			if (event.keyboard.shift)
			{
				newMarkBegin = mb;
				newMarkEnd = _d->cursorPos;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}

		}
		else
		{
			return false;
		}
		break;
	case KEY_DOWN:
		if (_d->multiLine || (_d->wordWrapEnabled && _d->brokenText.size() > 1) )
		{
			int lineNo = _getLineFromPos(_d->cursorPos);
			int mb = (_d->markBegin == _d->markEnd) ? _d->cursorPos : (_d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd);
			if (lineNo < (int)_d->brokenText.size()-1)
			{
				int cp = _d->cursorPos - _d->brokenTextPositions[lineNo];
				if ((int)_d->brokenText[lineNo+1].size() < cp)
					_d->cursorPos = _d->brokenTextPositions[lineNo+1] + _d->brokenText[lineNo+1].size()-1;
				else
					_d->cursorPos = _d->brokenTextPositions[lineNo+1] + cp;
			}

			if (event.keyboard.shift)
			{
				newMarkBegin = mb;
				newMarkEnd = _d->cursorPos;
			}
			else
			{
				newMarkBegin = 0;
				newMarkEnd = 0;
			}

		}
		else
		{
			return false;
		}
		break;

	case KEY_BACK:
		if ( !enabled() )
			break;

		if( _d->text.size() )
		{
			std::wstring s;

			if (_d->markBegin != _d->markEnd)
			{
				// delete marked text
				const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
				const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;

				s = _d->text.substr(0, realmbgn);
				s.append( _d->text.substr( realmend, _d->text.size() - realmend ) );
				_setText( s );

				_d->cursorPos = realmbgn;
			}
			else
			{
				// delete text behind cursor
				s = _d->cursorPos>0 ? _d->text.substr( 0, _d->cursorPos-1) : L"";

				s.append( _d->text.substr(_d->cursorPos, _d->text.size()-_d->cursorPos) );
				_d->cursorPos -= 1;
				_setText( s );
			}

			if (_d->cursorPos < 0)
				_d->cursorPos = 0;

			newMarkBegin = 0;
			newMarkEnd = 0;
			textChanged = true;
		}
		break;
	case KEY_DELETE:
		if ( !enabled() )
			break;

		if( _d->text.size() != 0)
		{
			std::wstring s;

			if (_d->markBegin != _d->markEnd)
			{
				// delete marked text
				const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
				const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;

				s = _d->text.substr(0, realmbgn);
				s.append( _d->text.substr(realmend, _d->text.size()-realmend) );
				_setText( s );

				_d->cursorPos = realmbgn;
			}
			else
			{
				// delete text before cursor
				s = _d->text.substr(0, _d->cursorPos);
        if( _d->cursorPos+1 < (int)_d->text.size() )
				{
					s.append( _d->text.substr(_d->cursorPos+1, _d->text.size()-_d->cursorPos-1) );
				}
				_setText( s );
			}

			if (_d->cursorPos > (int)_d->text.size())
				_d->cursorPos = (int)_d->text.size();

			newMarkBegin = 0;
			newMarkEnd = 0;
			textChanged = true;
		}
		break;

	case KEY_ESCAPE:
	case KEY_TAB:
	case KEY_SHIFT:
	case KEY_LSHIFT:
	case KEY_RSHIFT:
	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
	case KEY_F5:
	case KEY_F6:
	case KEY_F7:
	case KEY_F8:
	case KEY_F9:
	case KEY_F10:
	case KEY_F11:
	case KEY_F12:
	case KEY_F13:
	case KEY_F14:
	case KEY_F15:
	case KEY_F16:
	case KEY_F17:
	case KEY_F18:
	case KEY_F19:
	case KEY_F20:
	case KEY_F21:
	case KEY_F22:
	case KEY_F23:
	case KEY_F24:
	case KEY_LALT:
	case KEY_RALT:
		// ignore these keys
	return false;

	default:		
	return false;
	}

    // Set new text markers
    _setTextMarkers( newMarkBegin, newMarkEnd );

	// break the text if it has changed
	if (textChanged)
	{
		_breakText();
    _d->needUpdateTexture = true;
    _sendGuiEvent( guiEditboxChanged );
	}

	_calculateScrollPos();

	return true;
}

void EditBox::_drawHolderText( Font font, Rect* clip )
{
  if( isFocused() )
  {
    _d->setTextRect( this, 0, _d->holderText );
    Font holderFont = font;

    if( holderFont.isValid() )
    {
        holderFont.draw( *_d->textPicture, _d->holderText, 0, 0 );
    }
  }
}

void EditBox::beforeDraw(Engine& painter)
{
  int startPos = 0;

  bool needUpdateCursor = _d->needUpdateTexture;
  if( _d->needUpdateTexture )
  {
    _d->needUpdateTexture = false;

    if( !_d->textPicture || ( _d->textPicture && size() != _d->textPicture->size()) )
    {
      _d->textPicture.reset( Picture::create( size(), 0, true ) );
      _d->textPicture->fill( 0x00000000, Rect( 0, 0, 0, 0) );
    }

    if( !_d->bgPicture.isValid() )
    {
      Decorator::draw( _d->background, Rect( 0, 0, width(), height() ), Decorator::blackFrame );
    }

    Rect localClipRect = absoluteRect();
    _d->markAreaRect = Rect( 0, 0, -1, -1 );
    localClipRect.clipAgainst( absoluteClippingRect() );

    NColor simpleTextColor, markTextColor;

    simpleTextColor = 0xff000000;

    markTextColor = 0xffffffff;

    if( _d->lastBreakFont.isValid() )
    {
      // calculate cursor pos
      std::wstring myText = _d->text;
      std::wstring* txtLine = &myText;

      std::string s2;

      // get mark position
      const bool ml = (!_d->isPasswordBox && (_d->wordWrapEnabled || _d->multiLine));
      const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
      const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;
      const int hlineStart = ml ? _getLineFromPos(realmbgn) : 0;
      const int hlineCount = ml ? _getLineFromPos(realmend) - hlineStart + 1 : 1;
      const int lineCount = ml ? _d->brokenText.size() : 1;

      _d->textPicture->fill( 0x00000000, Rect(0, 0, 0, 0) );
      if( !_d->text.empty() )
      {
        for (int i=0; i < lineCount; ++i)
        {
           _d->setTextRect( this, i);

           // clipping test - don't draw anything outside the visible area
           Rect c = localClipRect;
           c.clipAgainst( _d->currentTextRect );
           if (!c.isValid())
               continue;

           // get current line%
           if (_d->isPasswordBox)
           {
             if (_d->brokenText.size() != 1)
             {
               _d->brokenText.clear();
               _d->brokenText.push_back(std::wstring());
             }

             if( _d->brokenText[0].size() != _d->text.size())
             {
               _d->brokenText[0] = _d->text;
               for (unsigned int q = 0; q < myText.size(); ++q)
               {
                 _d->brokenText[0][q] = _d->passwordChar;
               }
             }
             txtLine = &_d->brokenText[0];
             startPos = 0;
           }
           else
           {
             txtLine = ml ? &_d->brokenText[i] : &myText;
             startPos = ml ? _d->brokenTextPositions[i] : 0;
           }

           std::string rText = __ucs2utf8( *txtLine );
           //font->Draw(txtLine->c_str(), _d->currentTextRect_ + marginOffset, simpleTextColor,	false, true, &localClipRect);
           Rect curTextureRect( Point( 0, 0), _d->currentTextRect.size() );
           curTextureRect = _d->lastBreakFont.getTextRect( rText, curTextureRect, horizontalTextAlign(), verticalTextAlign() );
           curTextureRect += (_d->currentTextRect.UpperLeftCorner - absoluteRect().UpperLeftCorner );

           _d->lastBreakFont.draw( *_d->textPicture, rText, curTextureRect.UpperLeftCorner );

           // draw mark and marked text
           if( isFocused() && _d->markBegin != _d->markEnd && i >= hlineStart && i < hlineStart + hlineCount)
           {
             int mbegin = 0, mend = 0;
             int lineStartPos = 0, lineEndPos = txtLine->size();

             std::string s;

             if (i == hlineStart)
             {
                 // highlight start is on this line
                 s = rText.substr(0, realmbgn - startPos);
                 mbegin = _d->lastBreakFont.getTextSize( s ).width();

                 // deal with kerning
                 //mbegin += _d->lastBreakFont.getKerningSize( &((*txtLine)[realmbgn - startPos]),
                 //                                      realmbgn - startPos > 0 ? &((*txtLine)[realmbgn - startPos - 1]) : 0);
                 mbegin += 3;

                 lineStartPos = realmbgn - startPos;
             }

             if( i == hlineStart + hlineCount - 1 )
             {
                 // highlight end is on this line
                 s2 = rText.substr( 0, realmend - startPos );
                 mend = _d->lastBreakFont.getTextSize( s2 ).width();
                 lineEndPos = (int)s2.size();
             }
             else
                 mend = _d->lastBreakFont.getTextSize( (char*)txtLine->c_str() ).width();

             _d->markAreaRect = _d->currentTextRect - _d->currentTextRect.UpperLeftCorner;
             _d->markAreaRect.UpperLeftCorner += Point( mbegin, 0 );
             _d->markAreaRect.LowerRightCorner += Point( _d->markAreaRect.UpperLeftCorner.x() + mend - mbegin, 0 );

             //draw mark
             _d->markAreaRect = _d->markAreaRect /*+ marginOffset */;
             //_d->markAreaRect.UpperLeftCorner += _d->markStyle->GetMargin().getRect().UpperLeftCorner;
             //_d->markAreaRect.LowerRightCorner -= _d->markStyle->GetMargin().getRect().LowerRightCorner;

             // draw marked text
             s = rText.substr(lineStartPos, lineEndPos - lineStartPos);

             if( s.size() )
             {
                 //_d->txs4Text.NeedUpdateTextureWithText();
                 //_d->txs4Text.CreateTextureWithText( painter, textureRect, *txtLine, font, simpleTextColor, _textHorzAlign, _textVertAlign, false );
             }
           }
         }
      }
      else
          _drawHolderText( _d->lastBreakFont, &localClipRect );
    }
  }

  if( _d->cursorPos != _d->oldCursorPos || needUpdateCursor )
  {
    int cursorLine = 0;
    int charcursorpos = 0;
    _d->oldCursorPos = _d->cursorPos;

    std::wstring myText = _d->text;
    std::wstring* txtLine = &myText;

    if( _d->wordWrapEnabled || _d->multiLine )
    {
      cursorLine = _getLineFromPos(_d->cursorPos);
      txtLine = &_d->brokenText[ cursorLine ];
      startPos = _d->brokenTextPositions[ cursorLine ];
    }

    std::wstring stringBeforeCursor = txtLine->substr(0,_d->cursorPos-startPos);
    charcursorpos = _d->lastBreakFont.getTextSize( __ucs2utf8( stringBeforeCursor ) ).width() + 1/*font.GetKerningWidth(L"_", lastChar ? &lastChar : NULL )*/ ;

    _d->setTextRect( this, cursorLine);
    _d->cursorRect = _d->currentTextRect - absoluteClippingRect().lefttop();

    _d->cursorRect.UpperLeftCorner += Point( charcursorpos-1, 6 );
    _d->cursorRect.LowerRightCorner = _d->cursorRect.UpperLeftCorner + Point( 1, height() - 4 );
    //_d->cursorRect.UpperLeftCorner += style.GetMargin().getRect().UpperLeftCorner;
    //_d->cursorRect.LowerRightCorner -= style.GetMargin().getRect().LowerRightCorner;
  }

  Widget::beforeDraw( painter );
}

//! draws the element and its children
void EditBox::draw( Engine& painter )
{
  if( !visible() )
		return;

	const bool focus = _environment->hasFocus(this);

  //const ElementStyle& style = getStyle().GetState( getActiveState() );
	//const ElementStyle& markStyle = getStyle().GetState( L"Marked" );
  //core::Point marginOffset = style.GetMargin().getRect().UpperLeftCorner;

  if( _d->markAreaRect.isValid() )
  {
    NColor markAreaColor( 0xff0000ff );
    //markAreaColor.setAlpha( _d->resultTransparent );
    //painter.drawRectangle( markAreaColor, convertLocalToScreen( _d->markAreaRect ), &getAbsoluteClippingRectRef() );
  }

	// draw the text
  if( _d->bgPicture.isValid() )
  {
    painter.draw( _d->bgPicture, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }
  else
  {
    painter.draw( _d->background, absoluteRect().UpperLeftCorner, &absoluteClippingRectRef() );
  }

  if( _d->textPicture )
  {
    painter.draw( *_d->textPicture, _d->textOffset + absoluteRect().UpperLeftCorner );
  }

  if( focus )
	{
		unsigned int t = DateTime::elapsedTime() % 1000;
    if( t < 500 )
    {
      Point p = _d->textOffset + absoluteRect().lefttop();
      painter.drawLine( 0xff000000, p + _d->cursorRect.lefttop() + Point( 0, 3),
                                    p + _d->cursorRect.leftbottom() - Point( 0, 6 ) );
    }
	}

	// draw children
	Widget::draw( painter );
}

//! Sets the new caption of this element.
void EditBox::setText(const std::string& text)
{
	_d->text.clear();
	foreach( i, text )
	{
		wchar_t t;
		if( (unsigned char)*i < 0x80 )
		{
			t = (wchar_t)*i;
		}
		else
		{
			unsigned char t1 = (unsigned char)*i; i++;
			unsigned char t2 = (unsigned char)*i;
			t = (wchar_t)( (t1 << 8) + t2 );
		}

		_d->text += t;
	}

	_setText( _d->text );
}

std::string EditBox::text() const
{
	return __ucs2utf8( _d->text );
}


//! Enables or disables automatic scrolling with cursor position
//! \param enable: If set to true, the text will move around with the cursor position
void EditBox::setAutoscroll(bool enable){	_d->autoScrollEnabled = enable;}

//! Checks to see if automatic scrolling is enabled
//! \return true if automatic scrolling is enabled, false if not
bool EditBox::isAutoscrollEnabled() const{	return _d->autoScrollEnabled;}

//! Gets the area of the text in the edit box
//! \return Returns the size in pixels of the text
Size EditBox::textDimension()
{
	Rect ret;

  _d->setTextRect( this, 0 );
  ret = _d->currentTextRect;

	for (unsigned int i=1; i < _d->brokenText.size(); ++i)
	{
		_d->setTextRect( this, i);
    ret.addInternalPoint(_d->currentTextRect.UpperLeftCorner);
    ret.addInternalPoint(_d->currentTextRect.LowerRightCorner);
	}

	return ret.size();
}


//! Sets the maximum amount of characters which may be entered in the box.
//! \param max: Maximum amount of characters. If 0, the character amount is
//! infinity.
void EditBox::setMaxCharactersNumber(unsigned int max)
{
	_d->max = max;

	if( _d->text.size() > _d->max && _d->max != 0)
		_setText( _d->text.substr(0, _d->max ) );
}


//! Returns maximum amount of characters, previously set by setMax();
unsigned int EditBox::maxCharactersNumber() const{	return _d->max; }

bool EditBox::_processMouse(const NEvent& event)
{
	switch(event.mouse.type)
	{
	case mouseLbtnRelease:
		if (_environment->hasFocus(this))
		{
			Point rpos = event.mouse.pos() - _d->textOffset;
			_d->cursorPos = _d->getCursorPos( this, rpos.x(), rpos.y());
			if (_d->mouseMarking)
			{
				_setTextMarkers( _d->markBegin, _d->cursorPos );
			}
			_d->mouseMarking = false;
			_calculateScrollPos();
			return true;
		}
		break;
	case mouseMoved:
		{
			if (_d->mouseMarking)
			{
				_d->cursorPos = _d->getCursorPos( this, event.mouse.x, event.mouse.y);
				_setTextMarkers( _d->markBegin, _d->cursorPos );
				_calculateScrollPos();
				return true;
			}
		}
		break;
	case mouseLbtnPressed:
		if (!_environment->hasFocus(this))
		{
			_d->mouseMarking = true;
			Point rpos = event.mouse.pos() - _d->textOffset;
			_d->cursorPos = _d->getCursorPos( this, rpos.x(), rpos.y() );
			_setTextMarkers(_d->cursorPos, _d->cursorPos );
			_calculateScrollPos();
			return true;
		}
		else
		{
			if( !absoluteClippingRect().isPointInside( event.mouse.pos() ) )
			{
				return false;
			}
			else
			{
				// move cursor
				_d->cursorPos = _d->getCursorPos( this, event.mouse.x, event.mouse.y);

				int newMarkBegin = _d->markBegin;
				if (!_d->mouseMarking)
					newMarkBegin = _d->cursorPos;

				_d->mouseMarking = true;
				_setTextMarkers( newMarkBegin, _d->cursorPos);
				_calculateScrollPos();
				return true;
			}
		}
	default:
		break;
	}

	return false;
}


int EditBox::Impl::getCursorPos( EditBox* who, int x, int y)
{
	Font font = who->activeFont();

	const unsigned int lineCount = (wordWrapEnabled || multiLine) ? brokenText.size() : 1;

  std::wstring myText = text;
  std::wstring *txtLine=0;
	int startPos=0;
	x+=3;

	for( unsigned int i=0; i < lineCount; ++i)
	{
		setTextRect( who, i);
		if( i == 0 && y < currentTextRect.top() )
    {
      y = currentTextRect.top();
    }
 
    if( i == lineCount - 1 && y > currentTextRect.bottom() )
    {
      y = currentTextRect.bottom();
    }

		// is it inside this region?
		if( y >= currentTextRect.top() && y <= currentTextRect.bottom() )
		{
			// we've found the clicked line
			txtLine = (wordWrapEnabled || multiLine) ? &brokenText[i] : &myText;
			startPos = (wordWrapEnabled || multiLine) ? brokenTextPositions[i] : 0;
			break;
		}
	}

  if( x < currentTextRect.left() )
  {
    x = currentTextRect.left();
  }

	if ( !txtLine )
  {
		return 0;
  }

  int idx = font.getCharacterFromPos( *txtLine, x - currentTextRect.left() );

	// click was on or left of the line
	if (idx != -1)
		return idx + startPos;

	// click was off the right edge of the line, go to end.
	return txtLine->size() + startPos;
}

//! Breaks the single text line.
void EditBox::_breakText()
{
  Font font = activeFont();
	if( !font.isValid() )
		return;

	_d->lastBreakFont = font;

	if ((!_d->wordWrapEnabled && !_d->multiLine))
		return;

	_d->brokenText.clear(); // need to reallocate :/
	_d->brokenTextPositions.clear();

	std::wstring line;
	std::wstring word;
	std::wstring whitespace;
	int lastLineStart = 0;
	int size = _d->text.size();
	int length = 0;
	int elWidth = width() - 6;
	wchar_t c;

	for (int i=0; i<size; ++i)
	{
		c = _d->text[i];
		bool lineBreak = false;

		if( c == L'\r' ) // Mac or Windows breaks
		{
			lineBreak = true;
			c = 0;
			if( _d->text[ i+1 ] == L'\n') // Windows breaks
			{
				_d->text.erase(i+1);
				--size;
			}
		}
		else if( c == L'\n' ) // Unix breaks
		{
			lineBreak = true;
			c = 0;
		}

		// don't break if we're not a multi-line edit box
		if (!_d->multiLine)
			lineBreak = false;

		if (c == L' ' || c == 0 || i == (size-1))
		{
			// here comes the next whitespace, look if
			// we can break the last word to the next line
			// We also break whitespace, otherwise cursor would vanish beside the right border.
			int whitelgth = font.getTextSize( (char*)whitespace.c_str() ).width();
			int worldlgth = font.getTextSize( (char*)word.c_str() ).width();

			if (_d->wordWrapEnabled && length + worldlgth + whitelgth > elWidth)
			{
				// break to next line
				length = worldlgth;
				_d->brokenText.push_back(line);
				_d->brokenTextPositions.push_back(lastLineStart);
				lastLineStart = i - (int)word.size();
				line = word;
			}
			else
			{
				// add word to line
				line += whitespace;
				line += word;
				length += whitelgth + worldlgth;
			}

			word = L"";
			whitespace = L"";

			if ( c )
				whitespace += c;

			// compute line break
			if (lineBreak)
			{
				line += whitespace;
				line += word;
				_d->brokenText.push_back(line);
				_d->brokenTextPositions.push_back(lastLineStart);
				lastLineStart = i+1;
				line = L"";
				word = L"";
				whitespace = L"";
				length = 0;
			}
		}
		else
		{
			// yippee this is a word..
			word += c;
		}
	}

	line += whitespace;
	line += word;
	_d->brokenText.push_back(line);
	_d->brokenTextPositions.push_back(lastLineStart);
}

void EditBox::Impl::setTextRect( EditBox* who, int line, const std::string& tempText )
{
	if ( line < 0 )
		return;

	Font font = who->activeFont();
	if( !font.isValid() )
		return;

	Size d;

	// get text dimension
        //const unsigned int lineCount = (WordWrap || MultiLine) ? BrokenText.size() : 1;
  if (wordWrapEnabled || multiLine)
	{
		d = font.getTextSize( tempText.size() > 0 ? tempText : (char*)brokenText[line].c_str() );
	}
	else
	{
		d = font.getTextSize( tempText.size() > 0 ? tempText : (char*)text.c_str() );
		d.setHeight( who->height() );
	}
	
  d.setHeight( d.height() + font.kerningHeight() );

  currentTextRect = who->absoluteRect();

  currentTextRect.UpperLeftCorner += Point( -horizScrollPos, d.height() * line - vertScrollPos );
  currentTextRect.LowerRightCorner = Point( currentTextRect.right() + horizScrollPos, currentTextRect.UpperLeftCorner.y() + d.height() );
}

int EditBox::_getLineFromPos(int pos)
{
	if (!_d->wordWrapEnabled && !_d->multiLine)
		return 0;

	int i=0;
	while (i < (int)_d->brokenTextPositions.size())
	{
		if (_d->brokenTextPositions[i] > pos)
			return i-1;
		++i;
	}
	return (int)_d->brokenTextPositions.size() - 1;
}

std::wstring __unic2utf8(unsigned short wc)
{
 std::wstring ret;

 if( wc < 0x80 )
 {
   ret += (wchar_t)wc;
 }
 else if (wc < 0x800)
 {
   ret += (wchar_t)( ((0xC0 | wc>>6) << 8 ) + (0x80 | (wc & 0x3F)) );
 }
 /*else if (wc < 0x10000)
 {
   unsigned char a = (0xe0 | ((wc >> 12)& 0x0f)) ;
   unsigned char b = ( (0x80| ((wc >> 6) & 0x3f)) + (0x80| (wc & 0x3f)) );
   ret += (a << 8) + b;
 }*/
 else
   ret += '?';

 return ret;
}

void EditBox::_inputChar(unsigned short c)
{
	if (!enabled())
		return;

	if (c != 0)
	{
		if( _d->text.size() < _d->max || _d->max == 0)
		{
			std::wstring s;

			if (_d->markBegin != _d->markEnd)
			{
				// replace marked text
				const int realmbgn = _d->markBegin < _d->markEnd ? _d->markBegin : _d->markEnd;
				const int realmend = _d->markBegin < _d->markEnd ? _d->markEnd : _d->markBegin;

        s = _d->text.substr(0, realmbgn);
        s += __unic2utf8( c );
        s += _d->text.substr(realmend, _d->text.size()-realmend);
        _setText( s );
				_d->cursorPos = realmbgn+1;
			}
			else
			{
				// add new character
				s = _d->text.substr(0, _d->cursorPos);
				s += __unic2utf8( c );
				s += _d->text.substr( _d->cursorPos, _d->text.size()-_d->cursorPos);
				_setText( s );
				++_d->cursorPos;
			}

			_setTextMarkers(0, 0);
		}
	}

	_breakText();
	_sendGuiEvent( guiEditboxChanged );
	_calculateScrollPos();
  _d->needUpdateTexture = true;
}


void EditBox::_calculateScrollPos()
{
	if (!_d->autoScrollEnabled)
		return;

	// calculate horizontal scroll position
	int cursLine = _getLineFromPos(_d->cursorPos);
	if ( cursLine < 0 )
		return;
	_d->setTextRect( this, cursLine);

	// don't do horizontal scrolling when wordwrap is enabled.
	if (!_d->wordWrapEnabled)
	{
		// get cursor position
    Font font = activeFont();
		if( !font.isValid() )
			return;

		std::wstring myText = _d->text;
		std::wstring *txtLine = _d->multiLine ? &_d->brokenText[cursLine] : &myText;
		int cPos = _d->multiLine ? _d->cursorPos - _d->brokenTextPositions[cursLine] : _d->cursorPos;

    int cStart = _d->currentTextRect.UpperLeftCorner.x() + _d->horizScrollPos +
                                font.getTextSize( (char*)txtLine->substr(0, cPos).c_str() ).width();

		int cEnd = cStart + font.getTextSize( "_ " ).width();

		if ( screenRight() < cEnd)
			_d->horizScrollPos = cEnd - screenRight();
		else if ( screenLeft() > cStart)
			_d->horizScrollPos = cStart - screenLeft();
		else
			_d->horizScrollPos = 0;

		// todo: adjust scrollbar
	}

	// vertical scroll position
  if( screenBottom() < _d->currentTextRect.LowerRightCorner.y() + _d->vertScrollPos)
  {
    _d->vertScrollPos = _d->currentTextRect.LowerRightCorner.y() - screenBottom() + _d->vertScrollPos;
  }
  else if ( screenTop() > _d->currentTextRect.UpperLeftCorner.y() + _d->vertScrollPos)
  {
    _d->vertScrollPos = _d->currentTextRect.UpperLeftCorner.y() - screenTop() + _d->vertScrollPos;
  }
	else
		_d->vertScrollPos = 0;

	// todo: adjust scrollbar
}

//! set text markers
void EditBox::_setTextMarkers(int begin, int end)
{
    if ( begin != _d->markBegin || end != _d->markEnd )
    {
        _d->markBegin = begin;
        _d->markEnd = end;
        _sendGuiEvent( guiEditboxMarkingChanged );
        _d->needUpdateTexture;
    }
}

//! send some gui event to parent
void EditBox::_sendGuiEvent( unsigned int type)
{
    parent()->onEvent( NEvent::Gui( this, 0, (GuiEventType)type ));
}

//! Writes attributes of the element.
//void EditBox::save( core::VariantArray* out ) const
//{
	/*out->addBool  ("Border", 			  Border);
	out->addBool  ("Background", 		  Background);
	out->addBool  ("_d->overrideColorEnabled",_d->overrideColorEnabled );
	//out->addColor ("OverrideColor",       OverrideColor);
	// out->addFont("OverrideFont",OverrideFont);
	out->addInt   ("MaxChars",            Max);
	out->addBool  ("WordWrap",            WordWrap);
	out->addBool  ("MultiLine",           MultiLine);
	out->addBool  ("AutoScroll",          AutoScroll);
	out->addBool  ("PasswordBox",         PasswordBox);
	String ch = L" ";
	ch[0] = PasswordChar;
	out->addString("PasswordChar",        ch.c_str());
	out->addEnum  ("HTextAlign",          _textHorzAlign, NrpAlignmentNames);
	out->addEnum  ("VTextAlign",          _textVertAlign, NrpAlignmentNames);

	INrpElement::serializeAttributes(out,options);
    */
//}


//! Reads attributes of the element
//void EditBox::load( core::VariantArray* in )
//{
//       Widget::load(in);
/*
	setDrawBorder( in->getAttributeAsBool("Border") );
	setDrawBackground( in->getAttributeAsBool("Background") );
	setOverrideColor(in->getAttributeAsColor("OverrideColor"));
	enableOverrideColor(in->getAttributeAsBool("_d->overrideColorEnabled"));
	setMax(in->getAttributeAsInt("MaxChars"));
	setWordWrap(in->getAttributeAsBool("WordWrap"));
	setMultiLine(in->getAttributeAsBool("MultiLine"));
	setAutoScroll(in->getAttributeAsBool("AutoScroll"));
	String ch = in->getAttributeAsStringW("PasswordChar");

	if (!ch.size())
		setPasswordBox(in->getAttributeAsBool("PasswordBox"));
	else
		setPasswordBox(in->getAttributeAsBool("PasswordBox"), ch[0]);

	setTextAlignment( (CAESARIA_ALIGNMENT) in->getAttributeAsEnumeration("HTextAlign", GUIAlignmentNames),
			(CAESARIA_ALIGNMENT) in->getAttributeAsEnumeration("VTextAlign", GUIAlignmentNames));

	// setOverrideFont(in->getAttributeAsFont("OverrideFont"));
    */
//}

NColor EditBox::overrideColor() const
{
    return _d->overrideColor;
}

void EditBox::setDrawBackground( bool enabled )
{
    _d->drawBackground = enabled;
}

}//end namespace gui
