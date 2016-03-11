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
#include "core/color_list.hpp"
#include "widget_factory.hpp"
#include "gfx/drawstate.hpp"

using namespace std;
using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(Label)

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
    append( Label::bgWhiteBorderA, "whiteBorder" );
  }
};

class Label::Impl
{
public:
  StringArray brokenText;
  Font lastBreakFont; // stored because: if skin changes, line break must be recalculated.
  Font font;
  bool OverrideBGColorEnabled;
  Label::BackgroundMode backgroundMode;
  bool RestrainTextInside;
  int lineIntervalOffset;

  struct {
    bool borderVisible = false;
    bool wordwrap = false;
    bool lmbPressed = false;
    bool needUpdate = false;
  } is;

  struct {
    Picture picture;
    Point offset;
  } icon;

  struct {
    Picture picture;
    Point offset;
    string prefix;
    bool rightToLeft = false;
    Rect margin;
  } text;

  struct {
    Picture picture;
    Batch batch;
    Pictures fallback;
    Point offset;
  } background;

  unsigned int opaque;

  Impl() : OverrideBGColorEnabled(false),
           backgroundMode( Label::bgNone ),
           RestrainTextInside(true),
           lineIntervalOffset( 0 )
  {
    font = Font::create( FONT_2 );
    opaque = 0xff;
  }

  ~Impl()
  {
    text.picture = Picture();
  }

  void breakText(const std::string& text, Size size );

public signals:
  Signal0<> onClickedSignal;
  Signal1<Widget*> onClickedSignalA;
};

//! constructor
Label::Label( Widget* parent ) : Widget( parent, -1, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
  _d->is.borderVisible = false;
  _d->backgroundMode = bgNone;
  _d->is.needUpdate = true;

  setTextAlignment( align::automatic, align::automatic );
}

Label::Label(gui::Widget* parent, const Rect& rectangle, const std::string& text, Font font)
  : Widget( parent, -1, rectangle ), _d( new Impl )
{
  _d->is.borderVisible = false;
  _d->backgroundMode = bgNone;
  _d->is.needUpdate = true;

  setTextAlignment( align::automatic, align::automatic );
  setFont( font );
  setText( text );
}

Label::Label(Widget* parent, const Rect& rectangle, const string& text, bool border,
             BackgroundMode background, int id)
: Widget( parent, id, rectangle),
  _d( new Impl )
{
  _d->is.borderVisible = border;
  _d->backgroundMode = background;
  _d->is.needUpdate = true;

  #ifdef _DEBUG
    setDebugName( "label");
  #endif

  setTextAlignment( align::automatic, align::automatic );
  setText( text );
}

void Label::_updateTexture(gfx::Engine& painter)
{
  if( _d->text.picture.isValid() && _d->text.picture.size() != size() )
  {
    _d->text.picture = Picture( size(), 0, true );
  }

  if( !_d->text.picture.isValid() )
  {
    _d->text.picture = Picture( size(), 0, true );
  }

  if( _d->text.picture.isValid() )
  {
    _d->text.picture.fill( ColorList::clear, Rect( 0, 0, 0, 0) );
  }

  // draw button background
  bool useAlpha4Text = true;
  if( !_d->background.picture.isValid() )
  {
    _updateBackground( painter, useAlpha4Text );
  }

  if( _d->font.isValid() )
  {
    Rect frameRect( _d->text.margin.left(), _d->text.margin.top(),
                    width()-_d->text.margin.right(), height()-_d->text.margin.bottom() );

    string rText = _d->text.prefix + text();

    if( rText.size() )
    {
      //eColor = GetResultColor( eColor );
      if( !_d->is.wordwrap )
      {
        Rect textRect = _d->font.getTextRect( rText, frameRect, horizontalTextAlign(), verticalTextAlign() );

        textRect += _d->text.offset;
        _d->font.draw( _d->text.picture, text(), textRect.lefttop(), useAlpha4Text, false );
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

        for( const auto& btext : _d->brokenText )
        {
          Rect textRect = _d->font.getTextRect( btext, r, horizontalTextAlign(), verticalTextAlign() );
          textRect += _d->text.offset;
          _d->font.draw( _d->text.picture, btext, textRect.lefttop(), useAlpha4Text, false );
          r += Point( 0, height + _d->lineIntervalOffset );
        }
      }
    }
  }

  if( _d->text.picture.isValid() )
  {
    _d->text.picture.setAlpha( _d->opaque );
    _d->text.picture.update();
  }
}

void Label::_updateBackground(Engine& painter, bool& useAlpha4Text )
{
  Rect r( Point( 0, 0 ), size() );
  _d->background.batch.destroy();

  Pictures pics;
  switch( _d->backgroundMode )
  {
  case bgSimpleWhite:
    _d->text.picture.fill( 0xffffffff, Rect( 0, 0, 0, 0) );
    useAlpha4Text = false;
    Decorator::draw( _d->text.picture, r, Decorator::lineBlackBorder );
  break;

  case bgSimpleBlack:
    _d->text.picture.fill( 0xff000000, Rect( 0, 0, 0, 0) );
    useAlpha4Text = false;
    Decorator::draw( _d->text.picture, r, Decorator::lineWhiteBorder );
  break;

  case bgWhite: Decorator::draw( pics, r, Decorator::whiteArea, nullptr, Decorator::normalY ); break;
  case bgBlack: Decorator::draw( pics, r, Decorator::blackArea, nullptr, Decorator::normalY  ); break;
  case bgBrown: Decorator::draw( pics, r, Decorator::brownBorder, nullptr, Decorator::normalY  );  break;
  case bgSmBrown: Decorator::draw( pics, r, Decorator::brownPanelSmall, nullptr, Decorator::normalY  ); break;
  case bgWhiteFrame: Decorator::draw( pics, r, Decorator::whiteFrame, nullptr, Decorator::normalY  ); break;
  case bgBlackFrame: Decorator::draw( pics, r, Decorator::blackFrame, nullptr, Decorator::normalY  ); break;
  case bgNone:  break;
  case bgWhiteBorderA: Decorator::draw( pics, r, Decorator::whiteBorderA, nullptr, Decorator::normalY  ); break;
  }

  bool batchOk = _d->background.batch.load( pics, absoluteRect().lefttop() );
  if( !batchOk )
  {
    _d->background.batch.destroy();
    Decorator::reverseYoffset( pics );
    _d->background.fallback = pics;
  }
}

void Label::_handleClick()
{
  emit _d->onClickedSignal();
  emit _d->onClickedSignalA( this );
}

//! destructor
Label::~Label() {}

//! draws the element and its children
void Label::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  // draw background
  DrawState pipe( painter, absoluteRect().lefttop(), &absoluteClippingRectRef() );

  pipe.draw( _d->background.picture )
        .fallback( _d->background.batch )
        .fallback( _d->background.fallback )
      .draw( _d->icon.picture, _d->icon.offset )
      .draw( _d->text.picture );

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
void Label::setBackgroundStyle(BackgroundMode style )
{
  _d->backgroundMode = style;
  _d->is.needUpdate = true;
}

void Label::setBackgroundStyle(const string& style)
{
  LabelBackgroundHelper helper;
  Label::BackgroundMode mode = helper.findType(style);
  if (mode != bgNone)
    setBackgroundStyle( mode );
}

//! Sets whether to draw the border
void Label::setBorderVisible(bool draw) {  _d->is.borderVisible = draw;}
void Label::setTextRestrainedInside(bool restrainTextInside){  _d->RestrainTextInside = restrainTextInside;}
bool Label::isTextRestrainedInside() const{  return _d->RestrainTextInside;}

//! Enables or disables word wrap for using the static text as
//! multiline text control.
void Label::setWordwrap(bool enable)
{
  _d->is.wordwrap = enable;
  _d->breakText( text(), size() );
  _d->is.needUpdate = true;
}

bool Label::isWordwrapEnabled() const {  return _d->is.wordwrap; }

void Label::setRightToLeft(bool rtl)
{
  if( _d->text.rightToLeft != rtl )
  {
    _d->text.rightToLeft = rtl;
    _d->breakText( text(), size() );
    _d->is.needUpdate = true;
  }
}

bool Label::isRightToLeft() const{	return _d->text.rightToLeft;}

//! Breaks the single text line.
void Label::Impl::breakText( const std::string& ntext, Size wdgSize )
{
  if( !is.wordwrap )
    return;

  brokenText.clear();

  if( !font.isValid() )
      return;

  lastBreakFont = font;

  string line;
  string word;
  string whitespace;
  string rText = text.prefix + ntext;
  int size = rText.size();
  int length = 0;

  struct {
    int space;
    int word;
    inline int length() { return space+word; }
  } textChunck;

  const int widgetWidth = wdgSize.width() - (text.margin.left() + text.margin.right());

  char c;

  // We have to deal with right-to-left and left-to-right differently
  // However, most parts of the following code is the same, it's just
  // some order and boundaries which change.
  if (!text.rightToLeft)
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
        if (word.size()>0)
        {
          // here comes the next whitespace, look if
          // we must break the last word to the next line.
          textChunck.space = font.getTextSize( whitespace ).width();
          textChunck.word = font.getTextSize( word ).width();

          if( textChunck.word > widgetWidth )
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
              length = textChunck.word;
              line = word;
            }
          }
          else if (length && (length + textChunck.length() > widgetWidth))
          {
            // break to next line
            brokenText.push_back(line);
            length = textChunck.word;
            line = word;
          }
          else
          {
            // add word to line
            line += whitespace;
            line += word;
            length += textChunck.length();
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

      if (c == '\r') // Mac or Windows breaks
      {
        lineBreak = true;
        if ((i>0) && rText[i-1] == '\n') // Windows breaks
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
          textChunck.space = font.getTextSize( whitespace ).width();
          textChunck.word = font.getTextSize( word ).width();

          if (length && (length + textChunck.length() > widgetWidth))
          {
            // break to next line
            brokenText.push_back(line);
            length = textChunck.word;
            line = word;
          }
          else
          {
            // add word to line
            line = whitespace + line;
            line = word + line;
            length += textChunck.length();
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
  _d->is.needUpdate = true;
}

Signal0<>& Label::onClicked() { return _d->onClickedSignal; }
Signal1<Widget*>& Label::onClickedA() { return _d->onClickedSignalA; }

//! Returns the height of the text in pixels when it is drawn.
int Label::textHeight() const
{
  Font font = _d->font;
  if( !font.isValid() )
    return 0;

  int height = font.getTextSize("A").height();// + font.GetKerningHeight();

  if( _d->is.wordwrap)
    height *= _d->brokenText.size();

  return height;
}


int Label::textWidth() const
{
  Font font = _d->font;
  if( !font.isValid() )
      return 0;

  if( _d->is.wordwrap )
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

void Label::setPadding( const Rect& margin ) {  _d->text.margin = margin; }

void Label::beforeDraw(gfx::Engine& painter )
{
  if( _d->is.needUpdate )
  {
    _updateTexture( painter );

    _d->is.needUpdate = false;
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
    case NEvent::Mouse::btnLeftPressed: _d->is.lmbPressed = true;
    break;

    case NEvent::Mouse::mouseLbtDblClick:
    {
      _handleClick();
    }
    break;

    case NEvent::Mouse::mouseLbtnRelease:
    {
      if( _d->is.lmbPressed )
      {
        _d->is.lmbPressed = false;
        _handleClick();
      }
    }
    break;

    default: break;
    }
  }

  return Widget::onEvent( event );
}

bool Label::isBorderVisible() const {  return _d->is.borderVisible; }

void Label::canvasDraw(const string& text, const Point& point, Font dfont, NColor color)
{
  Picture& texture = _textPicture();
  Font rfont = dfont.isValid() ? dfont : font();
  if( color != 0 )
    rfont.setColor( color );

  rfont.draw( texture, text, point.x(), point.y(), true );
}

void Label::canvasDraw(const string& text, const Rect& rect, Font dfont, NColor color, Alignment halign, Alignment valign)
{
  if( halign == align::automatic )
    halign = horizontalTextAlign();

  if( valign == align::automatic )
    valign = verticalTextAlign();

  Font rfont = dfont.isValid() ? dfont : font();
  if( !text.empty() )
  {
    Rect textRect = rfont.getTextRect( text, rect, halign, valign );
    canvasDraw( text, textRect.lefttop(), rfont, color );
  }
}

void Label::canvasDraw(const Picture& picture, const Point& point)
{

}

void Label::setPrefixText( const string& prefix )
{
  _d->text.prefix = prefix;
  _d->is.needUpdate = true;
}

void Label::setBackgroundPicture(const Picture& picture, Point offset )
{
  _d->background.picture = picture;
  _d->background.offset = offset;
  _d->is.needUpdate = true;
}

void Label::setIcon(const Picture& icon, Point offset )
{
  _d->icon.picture = icon;
  _d->icon.offset = offset;
  _d->is.needUpdate = true;
}

void Label::setIcon(const string& rc, int index)
{
  _d->icon.picture.load( rc, index );
  _d->is.needUpdate = true;
}

void Label::setIconOffset(const Point& offset)
{
  _d->icon.offset = offset;
  _d->is.needUpdate = true;
}

void Label::setFont( const Font& font )
{
  _d->font = font;
  _d->is.needUpdate = true;
}

void Label::setFont(const string& fontname)
{
  Widget::setFont( fontname );
}

void Label::setFont(FontType type, NColor color)
{
  Widget::setFont(type, color);
}

void Label::setAlpha(unsigned int value)
{
  _d->opaque = value;
  _d->is.needUpdate = true;
}

void Label::setColor(NColor color)
{
  _d->font.setColor( color );
  _d->is.needUpdate = true;
}

void Label::setColor(const string& color)
{
  NColor c = ColorList::find(color);
  if (c.color != 0)
    setColor(c);
}

void Label::setTextAlignment( Alignment horizontal, Alignment vertical )
{
  Widget::setTextAlignment( horizontal, vertical );
  _d->is.needUpdate = true;
}

void Label::setTextAlignment(const string& horizontal, const string& vertical)
{
  Widget::setTextAlignment(horizontal,vertical);
}

void Label::_finalizeResize() {  _d->is.needUpdate = true; }

void Label::setLineIntervalOffset( const int offset )
{
  _d->lineIntervalOffset = offset;
  _d->is.needUpdate = true;
}

void Label::setupUI(const VariantMap& ui)
{
  Widget::setupUI(ui);

  setFont(Font::create( ui.get( "font", std::string("FONT_2")).toString()));
  setBackgroundPicture( Picture(ui.get("image").toString()));
  setWordwrap((bool)ui.get("multiline", false));

  Variant vTextOffset = ui.get("text.offset");
  if (vTextOffset.isValid()) { setTextOffset(vTextOffset.toPoint()); }

  Variant vIcon = ui.get("icon");
  if (vIcon.isValid())
  {
    Point iconOffset = ui.get("icon.offset").toPoint();
    setIcon( Picture(vIcon.toString()), iconOffset);
  }

  Variant marginLefttop = ui.get("margin.lefttop");
  if (marginLefttop.isValid())
  {
    Point value = marginLefttop.toPoint();
    _d->text.margin.setLeft(value.x());
    _d->text.margin.setTop(value.y());
  }

  Variant margin = ui.get("margin");
  if (margin.isValid())
  {
    _d->text.margin = margin.toRect();
  }

  setBackgroundStyle(ui.get("bgtype").toString());
}

void Label::setTextOffset(Point offset) { _d->text.offset = offset; _d->is.needUpdate = true;}
void Label::setTextOffset(int x, int y) { _d->text.offset = Point(x,y); _d->is.needUpdate = true; }
Picture& Label::_textPicture() { return _d->text.picture; }
Batch& Label::_background() { return _d->background.batch; }
Pictures& Label::_backgroundNb() { return _d->background.fallback; }

}//end namespace gui
