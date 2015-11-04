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

#include "listbox.hpp"
#include "listboxprivate.hpp"
#include "pushbutton.hpp"
#include "core/time.hpp"
#include "core/utils.hpp"
#include "core/event.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "core/gettext.hpp"
#include "widget_factory.hpp"

#define DEFAULT_SCROLLBAR_SIZE 39

using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(ListBox)

//! constructor
ListBox::ListBox( Widget* parent,const Rect& rectangle,
		  int id, bool clip,
		  bool drawBack, bool mos)
: Widget( parent, id, rectangle),
	_d( new Impl )
{
  _d->dragEventSended = false; 
  _d->index.hovered = -1;
  _d->height.item = 0;
  _d->height.total = 0;
  _d->height.override = 0;
  _d->font = Font();
  _d->itemsIconWidth = 0;
  _d->scrollBar = 0;
  _d->color.text = 0xff000000;
  _d->color.textHighlight = 0xffe0e0e0;
  _d->time.select = 0;
  _d->index.selected = -1;
  _d->time.lastKey = 0;
  _d->selecting = false;
  _d->needItemsRepackTextures = true;

#ifdef _DEBUG
  setDebugName( "ListBox");
#endif

  setFlag( selectOnMove, false );
  setFlag( moveOverSelect, mos );
  setFlag( autoscroll, true );
  setFlag( hightlightNotinfocused, true );
  setFlag( drawBackground, drawBack );

  const int s = DEFAULT_SCROLLBAR_SIZE;

  _d->scrollBar = &add<ScrollBar>( Rect( width() - s, 0, width(), height()), false );
  _d->scrollBar->setNotClipped( false );
  _d->scrollBar->setSubElement(true);
  _d->scrollBar->setVisibleFilledArea( false );
  _d->scrollBar->setTabstop(false);
  _d->scrollBar->setAlignment( align::lowerRight, align::lowerRight, align::upperLeft, align::lowerRight);
  _d->scrollBar->setVisible(false);
  _d->scrollBar->setValue(0);

  setNotClipped(!clip);

  // this element can be tabbed to
  setTabstop(true);
  setTaborder(-1);

  updateAbsolutePosition();

  setTextAlignment( align::upperLeft, align::center );
  _recalculateItemHeight( Font::create( FONT_2 ), height() );
}

void ListBox::_recalculateItemHeight( const Font& defaulFont, int h )
{
  if( !_d->font.isValid() )
  {
    _d->font = defaulFont;

    if ( _d->height.override != 0 )
      _d->height.item = _d->height.override;
    else
      _d->height.item = _d->font.getTextSize("A").height() + 4;
  }

  int newLength = _d->height.item * _d->items.size();
  bool scrollBarVisible = _d->scrollBar->visible();

  if( newLength != _d->height.total )
  {
    _d->height.total = newLength;
    _d->scrollBar->setMaxValue( std::max<int>( 0, _d->height.total - h ) );

    int minItemHeight = _d->height.item > 0 ? _d->height.item : 1;
    _d->scrollBar->setSmallStep ( minItemHeight );
    _d->scrollBar->setLargeStep ( 2*minItemHeight );

    _d->scrollBar->setVisible( !( _d->height.total <= h ) );
  }

  if( scrollBarVisible != _d->scrollBar->visible() )
  {
    _updateBackground( _d->scrollBar->visible() ? _d->scrollBar->width() : 0 );
  }
}

//! destructor
ListBox::~ListBox() {}

//! returns amount of list items
unsigned int ListBox::itemsCount() const {  return _d->items.size(); }

//! returns string of a list item. the may be a value from 0 to itemCount-1
ListBoxItem& ListBox::item(unsigned int id)
{
	if( id >= _d->items.size() )
	{
    Logger::warning( "Index out of range ListBox::items [{0}]", id );
		return ListBoxItem::invalidItem();
	}

	return _d->items[ id ];
}

ListBoxItem& ListBox::selectedItem() {	return item( selected() ); }

//! adds a list item, returns id of item
void ListBox::removeItem(unsigned int id)
{
  if (id >= _d->items.size())
  {
	  return;
  }

  if( (unsigned int)_d->index.selected==id )
	{
    _d->index.selected = -1;
	}
  else if ((unsigned int)_d->index.selected > id)
	{
    _d->index.selected -= 1;
    _d->time.select = DateTime::elapsedTime();
	}

  _d->items.erase( _d->items.begin() + id);

  _recalculateItemHeight( _d->font, height() );
}

int ListBox::itemAt(Point pos ) const
{
  if(	pos.x() < screenLeft() || pos.x() >= screenRight()
       ||	pos.y() < screenTop() || pos.y() >= screenBottom() )
  {
	  return -1;
  }

  if ( _d->height.item == 0 )
  {
	  return -1;
  }

  int item = ((pos.y() - screenTop() - 1) + _d->scrollBar->value()) / _d->height.item;
  
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
  _d->index.selected = -1;

  if (_d->scrollBar)
  {
    _d->scrollBar->setValue(0);
  }

  _recalculateItemHeight( _d->font, height() );
}

//! sets the selected item. Set this to -1 if no item should be selected
void ListBox::setSelected(int id)
{
  _d->index.selected = ((unsigned int)id>=_d->items.size() ? -1 : id);

  _d->time.select = DateTime::elapsedTime();
  _d->needItemsRepackTextures = true;

  _recalculateScrollPos();
}

void ListBox::setSelectedTag(const Variant& tag)
{
  int index = -1;
  for( auto it=_d->items.begin(); it != _d->items.end(); ++it )
  {
    if( it->tag() == tag )
    {
      index = std::distance( _d->items.begin(), it );
      break;
    }
  }

  setSelected( index );
}

//! sets the selected item. Set this to -1 if no item should be selected
void ListBox::setSelected( const std::string& item )
{
	int index = -1;

  for ( index = 0; index < (int) _d->items.size(); ++index )
  {
    if ( _d->items[index].text() == item )
		  break;
  }

  setSelected ( index );
}

void ListBox::_indexChanged( unsigned int eventType )
{
  parent()->onEvent( NEvent::Gui( this, 0, GuiEventType( eventType ) ) );

  switch( eventType )
  {
  case guiListboxChanged:
  {
    emit _d->signal.onIndexSelected( _d->index.selected );
    if( _d->index.selected >= 0 )
    {
      emit _d->signal.onTextSelected( _d->items[ _d->index.selected ].text() );
      emit _d->signal.onItemSelected( _d->items[ _d->index.selected ] );
    }
  }
  break;

  case guiListboxSelectedAgain:
  {
    emit _d->signal.onIndexSelectedAgain( _d->index.selected );
    if( _d->index.selected >= 0 )
    {
      emit _d->signal.onItemSelectedAgain( _d->items[ _d->index.selected ] );
    }
  }
  break;

  default:
  break;
  }
}

//! called if an event happened.
bool ListBox::onEvent(const NEvent& event)
{
	if( enabled() )
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
        int oldSelected = _d->index.selected;
				switch (event.keyboard.key)
				{
          case KEY_DOWN: _d->index.selected += 1; break;
          case KEY_UP:   _d->index.selected -= 1; break;
          case KEY_HOME: _d->index.selected = 0;  break;
          case KEY_END:  _d->index.selected = (int)_d->items.size()-1; break;
          case KEY_NEXT: _d->index.selected += height() / _d->height.item; break;
          case KEY_PRIOR:_d->index.selected -= height() / _d->height.item; break;
					default: break;
				}
         
        math::clamp_to<int>( _d->index.selected, 0, _d->items.size() - 1 );
        
        _recalculateScrollPos();
        _d->needItemsRepackTextures = true;

				// post the news
        if( oldSelected != _d->index.selected && !_d->selecting && !isFlag( moveOverSelect ) )
				{
					_indexChanged( guiListboxChanged );
				}

				return true;
			}
			else if (!event.keyboard.pressed && ( event.keyboard.key == KEY_RETURN || event.keyboard.key == KEY_SPACE ) )
			{
				_indexChanged( guiListboxSelectedAgain );

				return true;
			}
			else if (event.keyboard.pressed && event.keyboard.symbol)
			{
				// change selection based on text as it is typed.
				unsigned int now = DateTime::elapsedTime();

        if (now - _d->time.lastKey < 500)
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
        _d->time.lastKey = now;

				// find the selected item, starting at the current selection
        int start = _d->index.selected;

        // dont change selection if the key buffer matches the current item
        if (_d->index.selected > -1 && _d->keyBuffer.size() > 1)
				{
          if( _d->items[ _d->index.selected ].text().size() >= _d->keyBuffer.size()
              && utils::isEquale( _d->keyBuffer, _d->items[_d->index.selected].text().substr( 0,_d->keyBuffer.size() ),
																				utils::equaleIgnoreCase ) )
					{
						return true;
					}
				}

				int current;
				for( current = start+1; current < (int)_d->items.size(); ++current)
				{
					if( _d->items[current].text().size() >= _d->keyBuffer.size())
					{
						if( utils::isEquale( _d->keyBuffer, _d->items[current].text().substr(0,_d->keyBuffer.size()),
                                        utils::equaleIgnoreCase ) )
						{
              if ( _d->index.selected != current && !_d->selecting && !isFlag( moveOverSelect ))
							{
								_indexChanged( guiListboxChanged );
							}

							setSelected(current);
							return true;
						}
					}
				}

				for( current = 0; current <= start; ++current)
				{
					if( _d->items[current].text().size() >= _d->keyBuffer.size())
					{
						if( utils::isEquale( _d->keyBuffer, _d->items[current].text().substr( 0,_d->keyBuffer.size() ),
																				utils::equaleIgnoreCase ) )
						{
              if ( _d->index.selected != current && !_d->selecting && !isFlag( moveOverSelect ))
							{
								_indexChanged( guiListboxChanged );
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
      break;

			case guiElementFocusLost:
			{
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
          _d->scrollBar->setValue(_d->scrollBar->value() + (event.mouse.wheel < 0 ? -1 : 1) * (-_d->height.item/2));
					_d->needItemsRepackTextures = true;
					return true;
				}
				break;

				case mouseLbtnPressed:
				{
					_d->dragEventSended = false;
					_d->selecting = true;

          if (isPointInside(p) && isFlag( selectOnMDown ) )
          {
            _selectNew(event.mouse.y);
          }

					return true;
				}
				break;

				case mouseLbtnRelease:
				{
					_d->selecting = false;

          if (isPointInside(p) && !isFlag( selectOnMDown ) )
          {
            _selectNew(event.mouse.y);
          }

					return true;
				}
				break;

        case mouseMoved:
        {
          if( _d->selecting && isFlag( selectOnMove ) )
          {
            if (isPointInside(p))
            {
              _selectNew(event.mouse.y);
              return true;
            }
          }
        }
        break;

        default:
        break;
        }
      }
      break;

			default: break;
		}
	}

	return Widget::onEvent(event);
}

void ListBox::_selectNew(int ypos)
{
  unsigned int now = DateTime::elapsedTime();
  int oldSelected = _d->index.selected;

  _d->needItemsRepackTextures = true;

  int newIndex = itemAt( Point( screenLeft(), ypos ) );
  ListBoxItem& ritem = item( newIndex );

  if( ritem.isEnabled() )
  {
    _d->index.selected = newIndex;
    if( _d->index.selected<0 && !_d->items.empty() )
        _d->index.selected = 0;

    _recalculateScrollPos();

    GuiEventType eventType = ( _d->index.selected == oldSelected && now < _d->time.select + 500)
                                   ? guiListboxSelectedAgain
                                   : guiListboxChanged;
    _d->time.select = now;
    // post the news
    _indexChanged( eventType );
  }
}

//! Update the position and size of the listbox, and update the scrollbar
void ListBox::_finalizeResize()
{
  _d->height.total = 0;
  _recalculateItemHeight( _d->font, height() );
  _updateBackground( _d->scrollBar->visible() ? _d->scrollBar->width() : 0 );
}

ElementState ListBox::_getCurrentItemState( unsigned int index, bool hl )
{
  if( _d->items[ index ].isEnabled() )
  {
    if( hl && (int)index == _d->index.selected )
      return stChecked;

    if( (int)index == _d->index.selected )
      return stHovered;

    return stNormal;
  }

  return stDisabled;
}

Font ListBox::_getCurrentItemFont( const ListBoxItem& item, bool selected )
{
  Font itemFont = item.overrideColors[ selected ? ListBoxItem::hovered : ListBoxItem::simple ].font;

  if( !itemFont.isValid() )
      itemFont = _d->font;

	return itemFont;
}

NColor ListBox::_getCurrentItemColor( const ListBoxItem& item, bool selected )
{
  NColor ret = 0;
  ListBoxItem::ColorType tmpState = selected ? ListBoxItem::hovered : ListBoxItem::simple;

  if( item.overrideColors[ tmpState ].Use )
    ret = item.overrideColors[ tmpState ].color;
  else if( ret == 0 )
    ret = itemDefaultColor( tmpState );

  return ret;
}

Rect ListBox::_itemsRect()
{
  Rect frameRect( Point( 0, 0 ), size() );

  frameRect.rright() = frameRect.right() - DEFAULT_SCROLLBAR_SIZE;

  return frameRect;
}

void ListBox::_drawItemIcon( Engine& painter, ListBoxItem& item, const Point& pos, Rect* clipRect)
{
  painter.draw( item.icon(), pos + item.iconOffset(), clipRect );
}

void ListBox::_drawItemText( Engine& painter, ListBoxItem& item, const Point& pos, Rect* clipRect )
{
  painter.draw( item.picture(), pos + item.textOffset(), clipRect );
}

void ListBox::_updateItemText(Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect)
{
  item.updateText( textRect.lefttop(), font, frameRect.size() );
}

void ListBox::beforeDraw(gfx::Engine& painter)
{
  if ( !visible() )
      return;

  if( _d->needItemsRepackTextures )
  {
    bool hl = ( isFlag( hightlightNotinfocused ) || isFocused() || _d->scrollBar->isFocused() );
    Rect frameRect = _itemsRect();
    frameRect.rbottom() = frameRect.top() + _d->height.item;

    Alignment itemTextHorizontalAlign, itemTextVerticalAlign;
    Font currentFont;

    for( int i = 0; i < (int)_d->items.size();  i++ )
    {
      ListBoxItem& refItem = _d->items[ i ];

      if( refItem.icon().isValid() )
      {
        Point offset;
        if( refItem.horizontalAlign() == align::center )
        {
          offset.setX( (width() - refItem.icon().width()) / 2 );
        }

        refItem.setIconOffset( offset );
      }

      int mnY = frameRect.bottom() - _d->scrollBar->value();
      int mxY = frameRect.top() - _d->scrollBar->value();
      if( !refItem.text().empty() && mnY >= 0 && mxY <= (int)height() )
      {
        refItem.setState( _getCurrentItemState( i, hl ) );

        itemTextHorizontalAlign = refItem.isAlignEnabled() ? refItem.horizontalAlign() : horizontalTextAlign();
        itemTextVerticalAlign = refItem.isAlignEnabled() ? refItem.verticalAlign() : verticalTextAlign();

        currentFont = _getCurrentItemFont( refItem, i == _d->index.selected && hl );
        currentFont.setColor( _getCurrentItemColor( refItem, i==_d->index.selected && hl ) );

        Rect textRect = currentFont.getTextRect( refItem.text(), Rect( Point(0, 0), frameRect.size() ),
                                                 itemTextHorizontalAlign, itemTextVerticalAlign );

        textRect._lefttop += Point( _d->itemsIconWidth+3, 0 );

        _updateItemText( painter, refItem, textRect, currentFont, frameRect);
      }

      frameRect += Point( 0, _d->height.item );
    }

    _d->needItemsRepackTextures = false;
  }

  Widget::beforeDraw( painter );
}

void ListBox::refresh() {  _d->needItemsRepackTextures = true; }

//! draws the element and its children
void ListBox::draw( gfx::Engine& painter )
{
  if ( !visible() )
		return;

  if( isFlag( drawBackground ) )
  {
    if( _d->background.valid() )
      painter.draw( _d->background, &absoluteClippingRectRef() );
    else
      painter.draw( _d->backgroundNb, absoluteRect().lefttop(), &absoluteClippingRectRef() );
  }

  Point scrollBarOffset( 0, -_d->scrollBar->value() );
  Rect frameRect = _itemsRect();
  frameRect += _d->margin.lefttop();
  frameRect.rbottom() = frameRect.top() + _d->height.item;
  const Point& widgetLeftup = absoluteRect().lefttop();

  Rect clipRect = absoluteClippingRectRef();
  clipRect._lefttop += Point( 3, 3 );
  clipRect._bottomright -= Point( 3, 3 );

  for( unsigned int i = 0; i < _d->items.size();  i++ )
  {
    ListBoxItem& refItem = _d->items[ i ];

    int mnY = frameRect.bottom() - _d->scrollBar->value();
    int mxY = frameRect.top() - _d->scrollBar->value();

    mnY += std::max( 0, refItem.icon().height() - frameRect.height() );

    bool overBorder = (mnY < 0 && mxY < 0 ) || (mnY > (int)height() && mxY > (int)height() );
    if( !overBorder )
    {
      if( refItem.icon().isValid() )
      {
        _drawItemIcon( painter, refItem, widgetLeftup + frameRect.lefttop() + scrollBarOffset, &clipRect );
      }

      if( refItem.picture().isValid() )
      {
        _drawItemText( painter, refItem, widgetLeftup + frameRect.lefttop() + scrollBarOffset, &clipRect  );
      }

     /* if( !refItem.url().empty() )
      {
        Point r = frameRect.rightbottom();
        r += Point( 0, -_d->scrollBar->value() );
        //_d->background->fill( currentFont.color(), textRect + Point( 0, -_d->scrollBar->position() ) + refItem.offset() );
        Point offset = localToScreen( lefttop() );
        painter.drawLine( 0xff00ff00, r - Point( frameRect.width(), 0 ) + offset, r + offset );
      } */
    }

    frameRect += Point( 0, _d->height.item );
  }

	Widget::draw( painter );
}

void ListBox::_recalculateScrollPos()
{
	if (!isFlag( autoscroll ))
		return;

  const int selPos = (_d->index.selected == -1 ? _d->height.total : _d->index.selected * _d->height.item) - _d->scrollBar->value();

	if (selPos < 0)
	{
    _d->scrollBar->setValue( _d->scrollBar->value() + selPos );
	}
  else if (selPos > (int)height() - _d->height.item)
	{
    _d->scrollBar->setValue( _d->scrollBar->value() + selPos - height() + _d->height.item );
  }
}

void ListBox::_updateBackground( int scrollbarWidth)
{
  _d->background.destroy();

  Pictures pics;

  Decorator::draw( pics, Rect( 0, 0, width() - scrollbarWidth, height() ), Decorator::blackFrame );
  Decorator::draw( pics, Rect( width() - scrollbarWidth, 0, width(), height() ), Decorator::whiteArea, nullptr, Decorator::normalY  );

  bool batchOk = _d->background.load( pics, absoluteRect().lefttop() );
  if( !batchOk )
  {
    _d->background.destroy();
    Decorator::reverseYoffset( pics );
    _d->backgroundNb = pics;
  }
}

void ListBox::setAutoScrollEnabled(bool scroll) {	setFlag( autoscroll, scroll );}
bool ListBox::isAutoScrollEnabled() const{	return isFlag( autoscroll );}

void ListBox::setItem(unsigned int index, std::string text)
{
  if ( index >= _d->items.size() )
	return;

  _d->items[index].setText( text );
  _d->needItemsRepackTextures = true;
  _recalculateItemHeight( _d->font, height() );
}

//! Insert the item at the given index
//! Return the index on success or -1 on failure.
int ListBox::insertItem(unsigned int index, std::string text)
{
  ListBoxItem i;
  i.setText( text );

  _d->items.insert( _d->items.begin() + index, i );

  _recalculateItemHeight( _d->font, height() );

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
  if ( index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::count )
        return;

  if( colorType == ListBoxItem::all )
  {
    for ( unsigned int c=0; c < ListBoxItem::count; ++c )
    {
      _d->items[index].overrideColors[c].Use = true;
      _d->items[index].overrideColors[c].color = color;
    }
  }
  else
  {
    _d->items[index].overrideColors[colorType].Use = true;
    _d->items[index].overrideColors[colorType].color = color;
  }
}

void ListBox::resetItemOverrideColor(unsigned int index)
{
  for (unsigned int c=0; c < (unsigned int)ListBoxItem::count; ++c )
	{
    _d->items[index].overrideColors[c].Use = false;
	}
}


void ListBox::resetItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType)
{
  if ( index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::count )
		return;

    _d->items[index].overrideColors[colorType].Use = false;
}


bool ListBox::hasItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const
{
    if ( index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::count )
		return false;

    return _d->items[index].overrideColors[colorType].Use;
}

NColor ListBox::getItemOverrideColor(unsigned int index, ListBoxItem::ColorType colorType) const
{
  if ( (unsigned int)index >= _d->items.size() || colorType < 0 || colorType >= ListBoxItem::count )
		return 0;

  return _d->items[index].overrideColors[colorType].color;
}

NColor ListBox::itemDefaultColor( ListBoxItem::ColorType colorType) const
{
	switch ( colorType )
	{
		case ListBoxItem::simple:
      return _d->color.text;
		case ListBoxItem::hovered:
      return _d->color.textHighlight;
		case ListBoxItem::iconSimple:
			return 0xffffffff;
		case ListBoxItem::iconHovered:
			return 0xff0f0f0f;
		default:
			return 0;
	}
}

void ListBox::setItemDefaultColor( ListBoxItem::ColorType colorType, NColor color )
{
  switch( colorType )
  {
  case ListBoxItem::simple:
    _d->color.text = color;
  case ListBoxItem::hovered:
    _d->color.textHighlight = color;
  default: break;
  }
}
//! set global itemHeight
void ListBox::setItemHeight( int height )
{
  _d->height.item = height;
  _d->height.override = 1;
}

int ListBox::itemHeight() const { return _d->height.item; }

void ListBox::setItemAlignment(int index, Alignment horizontal, Alignment vertical)
{
  item( index ).setTextAlignment( horizontal, vertical );
  _d->needItemsRepackTextures = true;
}

ListBoxItem& ListBox::addItem( const std::string& text, Font font, const int color )
{
  ListBoxItem i;
  i.setText( text );
  i.setState( stNormal );
  i.setTextOffset( _d->itemTextOffset );
  i.overrideColors[ ListBoxItem::simple ].font = font.isValid() ? font : _d->font;
  i.overrideColors[ ListBoxItem::simple ].color = color;
  i.setTextAlignment( horizontalTextAlign(), verticalTextAlign() );

  _d->needItemsRepackTextures = true;

  _d->items.push_back(i);

  _recalculateItemHeight( _d->font, height() );

  return _d->items.back();
}

ListBoxItem& ListBox::addItem(Picture pic)
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
  for( auto& line : strings )
  {
    if( line.find( "\tc" ) != std::string::npos )
    {
      std::string nLine = utils::replace( line, "\tc", "" );
      ListBoxItem& item = addItem( nLine );
      item.setTextAlignment( align::center, align::center );
    }
    else
    {
      addItem( line );
    }
  }
}

Font ListBox::font() const{  return _d->font;}
void ListBox::setDrawBackground(bool draw){    setFlag( drawBackground, draw );} //! Sets whether to draw the background
int ListBox::selected() {    return _d->index.selected; }
Signal1<const ListBoxItem&>& ListBox::onItemSelectedAgain(){  return _d->signal.onItemSelectedAgain;}
Signal1<const ListBoxItem&>& ListBox::onItemSelected(){  return _d->signal.onItemSelected;}
void ListBox::setItemFont( Font font ){ _d->font = font; }
void ListBox::setItemTextOffset( Point p ) { _d->itemTextOffset = p; }

void ListBox::setupUI(const VariantMap& ui)
{
  Widget::setupUI( ui );

  int itemheight = ui.get( "itemheight" );
  if( itemheight != 0 ) setItemHeight( itemheight );
  bool drawborder = ui.get( "border.visible", true );

  setDrawBackground( drawborder );
  std::string fontname = ui.get( "itemfont" ).toString();
  if( !fontname.empty() ) setItemFont( Font::create( fontname ) );


  fontname = ui.get( "items.font" ).toString();
  if( !fontname.empty() ) setItemFont( Font::create( fontname ) );

  Variant itemtextoffset = ui.get( "items.offset" );
  if( itemtextoffset.isValid() )
    setItemTextOffset( itemtextoffset.toPoint() );

  _d->margin.rleft() = ui.get( "margin.left", _d->margin.left() );
  _d->margin.rtop() = ui.get( "margin.top", _d->margin.top() );

  bool scrollBarVisible = ui.get( "scrollbar.visible", true );
  _d->scrollBar->setVisible( scrollBarVisible );

  VariantList items = ui.get( "items" ).toList();
  for( auto& item : items )
  {
    VariantMap vm = item.toMap();
    if( vm.empty() )
    {
      addItem( item.toString() );
    }
    else
    {
      std::string fontName = vm.get( "font" ).toString();
      std::string text = vm.get( "text" ).toString();
      int tag = vm.get( "tag" );
      Font f = fontName.empty() ? font() : Font::create( fontName );
      ListBoxItem& item = addItem( _(text), f );
      item.setTag( tag );
      item.setUrl( vm.get( "url").toString() );
      align::Helper alignHelper;
      item.setTextAlignment( alignHelper.findType( vm.get( "align").toString() ),
                             align::center );
    }
  }
}

}//end namespace gui
