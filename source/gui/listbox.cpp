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

#include "listbox.hpp"
#include "listboxprivate.hpp"
#include "pushbutton.hpp"
#include "core/time.hpp"
#include "core/stringhelper.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

#define DEFAULT_SCROLLBAR_SIZE 39

namespace gui
{

//! constructor
ListBox::ListBox( Widget* parent,const Rect& rectangle,
			            int id, bool clip,
			            bool drawBack, bool moveOverSelect)
: Widget( parent, id, rectangle),
	_d( new Impl )
{
  _d->dragEventSended = false; 
  _d->hoveredItemIndex = -1;
  _d->itemHeight = 0;
  _d->itemHeightOverride = 0;
  _d->totalItemHeight = 0;
  _d->font = Font();
	_d->itemsIconWidth = 0;
	_d->scrollBar = 0;
  _d->itemDefaultColorText = 0xff000000;
  _d->itemDefaultColorTextHighlight = 0xffe0e0e0;
	_d->selectTime = 0;
	_d->selectedItemIndex = -1;
	_d->lastKeyTime = 0;
	_d->selecting = false;
	_d->needItemsRepackTextures = true;

  _d->recalculateItemHeight( Font::create( FONT_2 ), height() );

#ifdef _DEBUG
  setDebugName( "ListBox");
#endif

	setFlag( LBF_SELECT_ON_MOVE, false );
	setFlag( LBF_MOVEOVER_SELECT, moveOverSelect );
	setFlag( LBF_AUTOSCROLL, true );
	setFlag( LBF_HIGHLIGHTWHEN_NOTFOCUSED, true );
	setFlag( LBF_DRAWBACK, drawBack );

	const int s = DEFAULT_SCROLLBAR_SIZE;

  _d->scrollBar = new ScrollBar( this, Rect( width() - s, 0, width(), height()), false );
  _d->scrollBar->setNotClipped( false );
  _d->scrollBar->setSubElement(true);
  _d->scrollBar->setVisibleFilledArea( false );
  _d->scrollBar->setTabStop(false);
  _d->scrollBar->setAlignment(alignLowerRight, alignLowerRight, alignUpperLeft, alignLowerRight);
  _d->scrollBar->setVisible(false);
  _d->scrollBar->setPos(0);

	setNotClipped(!clip);

	// this element can be tabbed to
  setTabStop(true);
  setTabOrder(-1);

  updateAbsolutePosition();

	setTextAlignment( alignUpperLeft, alignCenter );
}

//! destructor
ListBox::~ListBox()
{
}


void ListBox::_updateTexture()
{
  Size s = size();

  if( _d->background && _d->background->size() != s )
  {
    _d->background.reset();
    _d->picture.reset();
  }

  if( !_d->background )
  {    
    _d->background.reset( Picture::create( s ) );
    _d->picture.reset( Picture::create( s ) );
    PictureDecorator::draw( *_d->background, Rect( 0, 0, width() - _d->scrollBar->width(), height() ), PictureDecorator::blackFrame );
    PictureDecorator::draw( *_d->background,
                            Rect( width() - _d->scrollBar->width(), 0, width(), height() ), 
                            PictureDecorator::whiteArea  );
  }
}

//! returns amount of list items
unsigned int ListBox::getItemCount() const
{
  return _d->items.size();
}


//! returns string of a list item. the may be a value from 0 to itemCount-1
ListBoxItem& ListBox::getItem(unsigned int id)
{
	if( id >= _d->items.size() )
	{
		Logger::warning( "Index out of range ListBox::items [%d]", id );
		return ListBoxItem::getInvalidItem();
	}

	return _d->items[ id ];
}

ListBoxItem& ListBox::getSelectedItem()
{
	return getItem( getSelected() );
}

//! adds a list item, returns id of item
void ListBox::removeItem(unsigned int id)
{
  if (id >= _d->items.size())
  {
	  return;
  }

  if( (unsigned int)_d->selectedItemIndex==id )
	{
        _d->selectedItemIndex = -1;
	}
  else if ((unsigned int)_d->selectedItemIndex > id)
	{
     _d->selectedItemIndex -= 1;
     _d->selectTime = DateTime::getElapsedTime();
	}

  _d->items.erase( _d->items.begin() + id);

  _d->recalculateItemHeight( _d->font, height() );
}


int ListBox::getItemAt(Point pos ) const
{
  if ( 	pos.x() < screenLeft() || pos.x() >= screenRight()
      ||	pos.y() < screenTop() || pos.y() >= screenBottom() )
  {
	  return -1;
  }

  if ( _d->itemHeight == 0 )
  {
	  return -1;
  }

  int item = ((pos.y() - screenTop() - 1) + _d->scrollBar->getPos()) / _d->itemHeight;
  
  if ( item < 0 || item >= (int)_d->items.size())
  {
    return -1;
  }

	return item;
}

//! clears the list
void ListBox::clear()
{
  _d->items.clear();
	_d->itemsIconWidth = 0;
  _d->selectedItemIndex = -1;

  if (_d->scrollBar)
  {
    _d->scrollBar->setPos(0);
  }

  _d->recalculateItemHeight( _d->font, height() );
}

//! sets the selected item. Set this to -1 if no item should be selected
void ListBox::setSelected(int id)
{
    _d->selectedItemIndex = ((unsigned int)id>=_d->items.size() ? -1 : id);

    _d->selectTime = DateTime::getElapsedTime();
    _d->needItemsRepackTextures = true;

    _RecalculateScrollPos();
}

//! sets the selected item. Set this to -1 if no item should be selected
void ListBox::setSelected( const std::string& item )
{
	int index = -1;

  for ( index = 0; index < (int) _d->items.size(); ++index )
  {
    if ( _d->items[index].getText() == item )
		  break;
  }

  setSelected ( index );
}

void ListBox::_IndexChanged( unsigned int eventType )
{
    getParent()->onEvent( NEvent::Gui( this, 0, GuiEventType( eventType ) ) );

    //_CallLuaFunction( eventType );

    switch( eventType )
    {
    case guiListboxChanged:
      _d->indexSelected.emit( _d->selectedItemIndex );
      if( _d->selectedItemIndex >= 0 )
      {
        _d->textSelected.emit( _d->items[ _d->selectedItemIndex ].getText() );
        _d->onItemSelectedSignal.emit( _d->items[ _d->selectedItemIndex ] );
      }
    break;

    case guiListboxSelectedAgain:
      _d->indexSelectedAgain.emit( _d->selectedItemIndex );
      if( _d->selectedItemIndex >= 0 )
      {
          _d->onItemSelectedAgainSignal.emit( _d->items[ _d->selectedItemIndex ].getText() );
      }
    break;

    default:
    break;
    }
}

//! called if an event happened.
bool ListBox::onEvent(const NEvent& event)
{
    if( isEnabled() )
	{
		switch(event.EventType)
        {
        case sEventMax:
        case sEventUser:
        break;

		case sEventKeyboard:
			if (event.keyboard.pressed &&
				(event.keyboard.key == KEY_DOWN ||
				event.keyboard.key == KEY_UP   ||
				event.keyboard.key == KEY_HOME ||
				event.keyboard.key == KEY_END  ||
				event.keyboard.key == KEY_NEXT ||
				event.keyboard.key == KEY_PRIOR ) )
			{
				int oldSelected = _d->selectedItemIndex;
				switch (event.keyboard.key)
				{
					case KEY_DOWN: _d->selectedItemIndex += 1; break;
					case KEY_UP:   _d->selectedItemIndex -= 1; break;
					case KEY_HOME: _d->selectedItemIndex = 0;  break;
					case KEY_END:  _d->selectedItemIndex = (int)_d->items.size()-1; break;
					case KEY_NEXT: _d->selectedItemIndex += height() / _d->itemHeight; break;
					case KEY_PRIOR:_d->selectedItemIndex -= height() / _d->itemHeight; break;
					default: break;
				}
         
        if (_d->selectedItemIndex >= (int)_d->items.size())
        {
          _d->selectedItemIndex = _d->items.size() - 1;
        }
                else if (_d->selectedItemIndex<0)
        {
          _d->selectedItemIndex = 0;
        }
        
        _RecalculateScrollPos();
        _d->needItemsRepackTextures = true;

				// post the news
				if( oldSelected != _d->selectedItemIndex && !_d->selecting && !isFlag( LBF_MOVEOVER_SELECT ) )
				{
					_IndexChanged( guiListboxChanged );
				}

				return true;
			}
			else if (!event.keyboard.pressed && ( event.keyboard.key == KEY_RETURN || event.keyboard.key == KEY_SPACE ) )
			{
				_IndexChanged( guiListboxSelectedAgain );

				return true;
			}
			else if (event.keyboard.pressed && event.keyboard.symbol)
			{
				// change selection based on text as it is typed.
                unsigned int now = DateTime::getElapsedTime();

				if (now - _d->lastKeyTime < 500)
				{
					// add to key buffer if it isn't a key repeat
					if (!(_d->keyBuffer.size() == 1 && _d->keyBuffer[0] == event.keyboard.symbol))
					{
						_d->keyBuffer += " ";
						_d->keyBuffer[_d->keyBuffer.size()-1] = event.keyboard.symbol;
					}
				}
				else
				{
					_d->keyBuffer = " ";
					_d->keyBuffer[0] = event.keyboard.symbol;
				}
				_d->lastKeyTime = now;

				// find the selected item, starting at the current selection
                int start = _d->selectedItemIndex;
                        // dont change selection if the key buffer matches the current item
        if (_d->selectedItemIndex > -1 && _d->keyBuffer.size() > 1)
				{
					if( _d->items[ _d->selectedItemIndex ].getText().size() >= _d->keyBuffer.size()
							&& StringHelper::isEquale( _d->keyBuffer, _d->items[_d->selectedItemIndex].getText().substr( 0,_d->keyBuffer.size() ),
																				StringHelper::equaleIgnoreCase ) )
					{
						return true;
					}
				}

				int current;
				for( current = start+1; current < (int)_d->items.size(); ++current)
				{
					if( _d->items[current].getText().size() >= _d->keyBuffer.size())
					{
						if( StringHelper::isEquale( _d->keyBuffer, _d->items[current].getText().substr(0,_d->keyBuffer.size()),
                                        StringHelper::equaleIgnoreCase ) )
						{
							if ( _d->selectedItemIndex != current && !_d->selecting && !isFlag( LBF_MOVEOVER_SELECT ))
							{
								_IndexChanged( guiListboxChanged );
							}

							setSelected(current);
							return true;
						}
					}
				}

				for( current = 0; current <= start; ++current)
				{
					if( _d->items[current].getText().size() >= _d->keyBuffer.size())
					{
						if( StringHelper::isEquale( _d->keyBuffer, _d->items[current].getText().substr( 0,_d->keyBuffer.size() ),
																				StringHelper::equaleIgnoreCase ) )
						{
							if ( _d->selectedItemIndex != current && !_d->selecting && !isFlag( LBF_MOVEOVER_SELECT ))
							{
								_IndexChanged( guiListboxChanged );
							}

              setSelected(current);
              return true;
            }
          }
        }

        return true;
      }
			break;

		case sEventGui:
			switch(event.gui.type)
			{
			case guiScrollbarChanged:
				{
					if (event.gui.caller == _d->scrollBar)
					{
						_d->needItemsRepackTextures = true;
						return true;
					}
				}
			break;

      case guiElementFocused:
            //          CallScriptFunction( GUI_EVENT + NRP_ELEMENT_FOCUSED, this );
      break;

			case guiElementFocusLost:
				{
          //CallScriptFunction( GUI_EVENT + NRP_ELEMENT_FOCUS_LOST, this );
          if (event.gui.caller == this)
          {
             _d->selecting = false;
          }
        }
      break;

			default:
			break;
			}
			break;

			case sEventMouse:
			{
				Point p = event.mouse.pos();

				switch(event.mouse.type)
				{
				case mouseWheel:
					{
						_d->scrollBar->setPos(_d->scrollBar->getPos() + (event.mouse.wheel < 0 ? -1 : 1) * (-_d->itemHeight/2));
						_d->needItemsRepackTextures = true;
						return true;
					}
				break;

				case mouseLbtnPressed:
				  {
						_d->dragEventSended = false;
						_d->selecting = true;

            if (isPointInside(p) && isFlag( LBF_SELECT_ON_MOUSE_DOWN ) )
            {
              _SelectNew(event.mouse.y);
            }

						return true;
				  }
				break;

				case mouseLbtnRelease:
				  {
						_d->selecting = false;

            if (isPointInside(p) && !isFlag( LBF_SELECT_ON_MOUSE_DOWN ) )
            {
              _SelectNew(event.mouse.y);
            }

						return true;
				  }
				break;

        case mouseMoved:
          if( _d->selecting && isFlag( LBF_SELECT_ON_MOVE )/* || isFlag( LBF_MOVEOVER_SELECT )*/ )
          {
            if (isPointInside(p))
            {
              _SelectNew(event.mouse.y);
              return true;
            }
          }

//        if( _d->selecting && !_d->dragEventSended && !isPointInside(p) )
//           {
//               getParent()->onEvent( NEvent::Drag( this, 0 ));
//               _d->dragEventSended = true;
//               return true;
//           }
        break;

        default:
        break;
        }
          }
          break;
		}
	}

	return Widget::onEvent(event);
}

void ListBox::_SelectNew(int ypos)
{
    unsigned int now = DateTime::getElapsedTime();
    int oldSelected = _d->selectedItemIndex;

    _d->needItemsRepackTextures = true;

    _d->selectedItemIndex = getItemAt( Point( screenLeft(), ypos ) );
    if( _d->selectedItemIndex<0 && !_d->items.empty() )
        _d->selectedItemIndex = 0;

    _RecalculateScrollPos();

		GuiEventType eventType = (_d->selectedItemIndex == oldSelected && now < _d->selectTime + 500)
									? guiListboxSelectedAgain
									: guiListboxChanged;
    _d->selectTime = now;
	// post the news
    _IndexChanged( eventType );
}

//! Update the position and size of the listbox, and update the scrollbar
void ListBox::_resizeEvent()
{
    _d->totalItemHeight = 0;
    _d->recalculateItemHeight( _d->font, height() );
}

ElementState ListBox::_GetCurrentItemState( unsigned int index, bool hl )
{
    if( _d->items[ index ].isEnabled() )
    {
        if( hl && (int)index == _d->selectedItemIndex )
            return stChecked;

        if( (int)index == _d->hoveredItemIndex )
            return stHovered;

        return stNormal;
    }

    return stDisabled;
}

Font ListBox::_GetCurrentItemFont( const ListBoxItem& item, bool selected )
{
  Font itemFont = item.OverrideColors[ selected ? ListBoxItem::LBC_TEXT_HIGHLIGHT : ListBoxItem::LBC_TEXT ].font;

  if( !itemFont.isValid() )
      itemFont = _d->font;

	return itemFont;
}

NColor ListBox::_GetCurrentItemColor( const ListBoxItem& item, bool selected )
{
  NColor ret = 0;
  ListBoxItem::ColorType tmpState = selected ? ListBoxItem::LBC_TEXT_HIGHLIGHT : ListBoxItem::LBC_TEXT;

  if( item.OverrideColors[ tmpState ].Use )
    ret = item.OverrideColors[ tmpState ].color;
  else if( ret == 0 )
    ret = getItemDefaultColor( tmpState );

  return ret;
}

Rect ListBox::getItemTextRect_()
{
  Rect frameRect( Point( 0, 0 ), size() );
  if( _d->scrollBar->isVisible() )
      frameRect.LowerRightCorner.setX( frameRect.LowerRightCorner.x() - DEFAULT_SCROLLBAR_SIZE );

  return frameRect;
}

void ListBox::beforeDraw( GfxEngine& painter)
{
  if ( !isVisible() )
      return;

  if( _d->needItemsRepackTextures )
  {
    _updateTexture();

    _d->picture->draw( *_d->background, 0, 0 );

    bool hl = ( isFlag( LBF_HIGHLIGHTWHEN_NOTFOCUSED ) || isFocused() || _d->scrollBar->isFocused() );
    Rect frameRect = getItemTextRect_();
    frameRect.LowerRightCorner.setY( frameRect.top() + _d->itemHeight );

    Alignment itemTextHorizontalAlign, itemTextVerticalAlign;
    Font currentFont;

    for (int i=0; i<(int)_d->items.size(); ++i)
    {
      ListBoxItem& refItem = _d->items[i];

      if( refItem.getIcon().isValid() )
      {
        Point offset;
        if( refItem.getHorizontalAlign() == alignCenter )
        {
          offset.setX( (width() - refItem.getIcon().width()) / 2 );
        }
        _d->picture->draw( refItem.getIcon(), frameRect.UpperLeftCorner + Point( 0, -_d->scrollBar->getPos() ) + offset );
      }

      int mnY = frameRect.LowerRightCorner.y() - _d->scrollBar->getPos();
      int mxY = frameRect.UpperLeftCorner.y() - _d->scrollBar->getPos();
      if( !refItem.getText().empty() && mnY >= 0 && mxY <= (int)height() )
      {
        refItem.setState( _GetCurrentItemState( i, hl ) );

        itemTextHorizontalAlign = refItem.isAlignEnabled() ? refItem.getHorizontalAlign() : getHorizontalTextAlign();
        itemTextVerticalAlign = refItem.isAlignEnabled() ? refItem.getVerticalAlign() : getVerticalTextAlign();

        currentFont = _GetCurrentItemFont( refItem, i == _d->selectedItemIndex && hl );
        currentFont.setColor( _GetCurrentItemColor( refItem, i==_d->selectedItemIndex && hl ) );

        Rect textRect = currentFont.calculateTextRect( refItem.getText(), frameRect,
                                                       itemTextHorizontalAlign, itemTextVerticalAlign );

        //_DrawItemIcon( refItem, textRect, hl, i == _d->selectedItemIndex, &_d->clientClip, fontColor );

        textRect.UpperLeftCorner += Point( _d->itemsIconWidth+3, 0 );

        currentFont.draw( *_d->picture, refItem.getText(),
                          textRect.UpperLeftCorner + Point( 0, -_d->scrollBar->getPos() ) + refItem.getOffset(),
                          false );
      }

      frameRect += Point( 0, _d->itemHeight );
    }

    _d->needItemsRepackTextures = false;
  }

  Widget::beforeDraw( painter );
}

//! draws the element and its children
void ListBox::draw( GfxEngine& painter )
{
  if ( !isVisible() )
		return;

	if( !_d->picture.isNull() )
	{
		painter.drawPicture( *_d->picture, absoluteRect().UpperLeftCorner );
	}

	Widget::draw( painter );
}

void ListBox::_RecalculateScrollPos()
{
	if (!isFlag( LBF_AUTOSCROLL ))
		return;

  const int selPos = (_d->selectedItemIndex == -1 ? _d->totalItemHeight : _d->selectedItemIndex * _d->itemHeight) - _d->scrollBar->getPos();

	if (selPos < 0)
	{
    _d->scrollBar->setPos( _d->scrollBar->getPos() + selPos );
	}
	else if (selPos > (int)height() - _d->itemHeight)
	{
    _d->scrollBar->setPos( _d->scrollBar->getPos() + selPos - height() + _d->itemHeight );
	}
}

void ListBox::setAutoScrollEnabled(bool scroll) {	setFlag( LBF_AUTOSCROLL, scroll );}
bool ListBox::isAutoScrollEnabled() const{	return isFlag( LBF_AUTOSCROLL );}

void ListBox::save(VariantMap& out) const
{

}

void ListBox::load(const VariantMap& in)
{

}

void ListBox::setItem(unsigned int index, std::string text)
{
  if ( index >= _d->items.size() )
	return;

  _d->items[index].setText( text );
  _d->needItemsRepackTextures = true;
  _d->recalculateItemHeight( _d->font, height() );
}


//! Insert the item at the given index
//! Return the index on success or -1 on failure.
int ListBox::insertItem(unsigned int index, std::string text)
{
  ListBoxItem i;
  i.setText( text );

  _d->items.insert( _d->items.begin() + index, i );

  _d->recalculateItemHeight( _d->font, height() );

	return index;
}

void ListBox::swapItems(unsigned int index1, unsigned int index2)
{
	if ( index1 >= _d->items.size() || index2 >= _d->items.size() )
	return;

  ListBoxItem dummmy = _d->items[index1];
  _d->items[index1] = _d->items[index2];
  _d->items[index2] = dummmy;
}

void ListBox::setItemOverrideColor(unsigned int index, const int color, ListBoxItem::ColorType colorType )
{
  if ( index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::LBC_COUNT )
        return;

  if( colorType == ListBoxItem::LBC_ALL )
  {
    for ( unsigned int c=0; c < ListBoxItem::LBC_COUNT; ++c )
    {
      _d->items[index].OverrideColors[c].Use = true;
      _d->items[index].OverrideColors[c].color = color;
    }
  }
  else
  {
    _d->items[index].OverrideColors[colorType].Use = true;
    _d->items[index].OverrideColors[colorType].color = color;
  }
}

void ListBox::resetItemOverrideColor(unsigned int index)
{
  for (unsigned int c=0; c < (unsigned int)ListBoxItem::LBC_COUNT; ++c )
	{
		_d->items[index].OverrideColors[c].Use = false;
	}
}


void ListBox::resetItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType)
{
  if ( index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::LBC_COUNT )
		return;

    _d->items[index].OverrideColors[colorType].Use = false;
}


bool ListBox::hasItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const
{
    if ( index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::LBC_COUNT )
		return false;

    return _d->items[index].OverrideColors[colorType].Use;
}

int ListBox::getItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const
{
  if ( (unsigned int)index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::LBC_COUNT )
		return 0;

  return _d->items[index].OverrideColors[colorType].color;
}

NColor ListBox::getItemDefaultColor( ListBoxItem::ColorType colorType) const
{
	switch ( colorType )
	{
		case ListBoxItem::LBC_TEXT:
			return _d->itemDefaultColorText;
		case ListBoxItem::LBC_TEXT_HIGHLIGHT:
			return _d->itemDefaultColorTextHighlight;
		case ListBoxItem::LBC_ICON:
			return 0xffffffff;
		case ListBoxItem::LBC_ICON_HIGHLIGHT:
			return 0xff0f0f0f;
		default:
			return 0;
	}
}

void ListBox::setItemDefaultColor( ListBoxItem::ColorType colorType, NColor color )
{
  switch( colorType )
  {
  case ListBoxItem::LBC_TEXT:
    _d->itemDefaultColorText = color;
  case ListBoxItem::LBC_TEXT_HIGHLIGHT:
    _d->itemDefaultColorTextHighlight = color;
  default: break;
  }
}
//! set global itemHeight
void ListBox::setItemHeight( int height )
{
  _d->itemHeight = height;
  _d->itemHeightOverride = 1;
}

int ListBox::getItemHeight() const { return _d->itemHeight; }

void ListBox::setItemAlignment(int index, Alignment horizontal, Alignment vertical)
{
  ListBoxItem& item = getItem( index );
  item.setItemTextAlignment( horizontal, vertical );
  _d->needItemsRepackTextures = true;
}

ListBoxItem& ListBox::addItem( const std::string& text, Font font, const int color )
{
  ListBoxItem i;
  i.setText( text );
  i.setState( stNormal );
  i.setOffset( _d->itemTextOffset );
  //i.currentHovered = 255;
  i.OverrideColors[ ListBoxItem::LBC_TEXT ].font = font.isValid() ? font : _d->font;
  i.OverrideColors[ ListBoxItem::LBC_TEXT ].color = color;
  i.setItemTextAlignment( getHorizontalTextAlign(), getVerticalTextAlign() );

  _d->needItemsRepackTextures = true;

  _d->items.push_back(i);

  _d->recalculateItemHeight( _d->font, height() );

  return _d->items.back();
}

ListBoxItem&ListBox::addItem(Picture pic)
{
  ListBoxItem& item = addItem( "", Font() );
  item.setIcon( pic  );

  return item;
}

void ListBox::fitText(const std::string& text)
{
  StringArray items = _d->font.breakText( text, width() - _d->scrollBar->width() );
  addItems( items );
}

void ListBox::addItems(const StringArray& strings)
{
  for( StringArray::const_iterator it=strings.begin(); it != strings.end(); ++it )
  { addItem( *it ); }
}

Font ListBox::getFont() const{  return _d->font;}
void ListBox::setDrawBackground(bool draw){    setFlag( LBF_DRAWBACK, draw );} //! Sets whether to draw the background
int ListBox::getSelected() {    return _d->selectedItemIndex; }
Signal1<std::string>& ListBox::onItemSelectedAgain(){  return _d->onItemSelectedAgainSignal;}
Signal1<const ListBoxItem&>& ListBox::onItemSelected(){  return _d->onItemSelectedSignal;}
void ListBox::setItemFont( Font font ){ _d->font = font; }
void ListBox::setItemTextOffset( Point p ) { _d->itemTextOffset = p; }

}//end namespace gui
