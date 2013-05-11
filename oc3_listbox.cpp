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

#include "oc3_listbox.hpp"
#include "oc3_listboxprivate.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_time.hpp"
#include "oc3_event.hpp"
#include "oc3_sdl_facade.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_gui_paneling.hpp"

#define DEFAULT_SCROLLBAR_SIZE 15

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
  _d->font = FontCollection::instance().getFont( FONT_2 );
	_d->itemsIconWidth = 0;
	_d->scrollBar = 0;
	_d->selectTime = 0;
	_d->selectedItemIndex = -1;
	_d->lastKeyTime = 0;
	_d->selecting = false;
  _d->background = 0;
	_d->needItemsRepackTextures = true;

#ifdef _DEBUG
  setDebugName( "ListBox");
#endif

	setFlag( LBF_SELECT_ON_MOVE, false );
	setFlag( LBF_MOVEOVER_SELECT, moveOverSelect );
	setFlag( LBF_AUTOSCROLL, true );
	setFlag( LBF_HIGHLIGHTWHEN_NOTFOCUSED, true );
	setFlag( LBF_DRAWBACK, drawBack );

	const int s = DEFAULT_SCROLLBAR_SIZE;

    _d->scrollBar = new ScrollBar( this, Rect( getWidth() - s, 0, getWidth(), getHeight()), false );
    _d->scrollBar->setNotClipped( false );
    _d->scrollBar->setSubElement(true);
    _d->scrollBar->setVisibleFilledArea( false );
    _d->scrollBar->setTabStop(false);
    _d->scrollBar->setAlignment(alignLowerRight, alignLowerRight, alignUpperLeft, alignLowerRight);
    _d->scrollBar->setVisible(false);
    _d->scrollBar->getUpButton()->setVisible( false );
    _d->scrollBar->getDownButton()->setVisible( false );
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
  Size size = getSize();

  if( _d->background && _d->background->getSize() != size )
  {
    SdlFacade::instance().deletePicture( *_d->background );
    SdlFacade::instance().deletePicture( *_d->picture );
    _d->background = 0;
    _d->picture = 0;
  }

  if( !_d->background )
  {    
    _d->background = &SdlFacade::instance().createPicture( size.getWidth(), size.getHeight() );
    _d->picture = &SdlFacade::instance().createPicture( size.getWidth(), size.getHeight() );
    GuiPaneling::instance().draw_black_frame(*_d->background, 0, 0, getWidth(), getHeight() );
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
	_OC3_DEBUG_BREAK_IF( (id >= _d->items.size()) && "Index out of range ListBox::items" );
  if (id>=_d->items.size())
  {
	  return ListBoxItem::getInvalidItem();
  }

  return _d->items[ id ];
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

  _d->recalculateItemHeight( _d->font, getHeight() );
}


int ListBox::getItemAt( const Point& pos ) const
{
  if ( 	pos.getX() < getScreenLeft() || pos.getX() >= getScreenRight()
      ||	pos.getY() < getScreenTop() || pos.getY() >= getScreenBottom() )
  {
	  return -1;
  }

  if ( _d->itemHeight == 0 )
  {
	  return -1;
  }

  int item = ((pos.getY() - getScreenTop() - 1) + _d->scrollBar->getPos()) / _d->itemHeight;
  
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

  _d->recalculateItemHeight( _d->font, getHeight() );
}

//! sets the selected item. Set this to -1 if no item should be selected
void ListBox::setSelected(int id)
{
    _d->selectedItemIndex = ((unsigned int)id>=_d->items.size() ? -1 : id);

    _d->selectTime = DateTime::getElapsedTime();

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
    getParent()->onEvent( NEvent::Gui( this, 0, OC3_GUI_EVENT_TYPE( eventType ) ) );

    //_CallLuaFunction( eventType );

    switch( eventType )
    {
    case OC3_LISTBOX_CHANGED:
        _d->indexSelected.emit( _d->selectedItemIndex );
        if( _d->selectedItemIndex >= 0 )
            _d->textSelected.emit( _d->items[ _d->selectedItemIndex ].getText() );
    break;

    case OC3_LISTBOX_SELECTED_AGAIN:
        _d->indexSelectedAgain.emit( _d->selectedItemIndex );
        if( _d->selectedItemIndex >= 0 )
            _d->textSelectedAgain.emit( _d->items[ _d->selectedItemIndex ].getText() );
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
		case OC3_KEYBOARD_EVENT:
			if (event.KeyboardEvent.PressedDown &&
				(event.KeyboardEvent.Key == KEY_DOWN ||
				event.KeyboardEvent.Key == KEY_UP   ||
				event.KeyboardEvent.Key == KEY_HOME ||
				event.KeyboardEvent.Key == KEY_END  ||
				event.KeyboardEvent.Key == KEY_NEXT ||
				event.KeyboardEvent.Key == KEY_PRIOR ) )
			{
        int oldSelected = _d->selectedItemIndex;
				switch (event.KeyboardEvent.Key)
				{
					case KEY_DOWN:
                        _d->selectedItemIndex += 1;
						break;
					case KEY_UP:
                        _d->selectedItemIndex -= 1;
						break;
					case KEY_HOME:
                        _d->selectedItemIndex = 0;
						break;
					case KEY_END:
                        _d->selectedItemIndex = (int)_d->items.size()-1;
						break;
					case KEY_NEXT:
                        _d->selectedItemIndex += getHeight() / _d->itemHeight;
						break;
					case KEY_PRIOR:
                        _d->selectedItemIndex -= getHeight() / _d->itemHeight;
						break;
					default:
						break;
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

				// post the news
        if( oldSelected != _d->selectedItemIndex && !_d->selecting && !isFlag( LBF_MOVEOVER_SELECT ) )
				    _IndexChanged( OC3_LISTBOX_CHANGED );

				return true;
			}
			else if (!event.KeyboardEvent.PressedDown && ( event.KeyboardEvent.Key == KEY_RETURN || event.KeyboardEvent.Key == KEY_SPACE ) )
			{
        _IndexChanged( OC3_LISTBOX_SELECTED_AGAIN );

				return true;
			}
			else if (event.KeyboardEvent.PressedDown && event.KeyboardEvent.Char)
			{
				// change selection based on text as it is typed.
        unsigned int now = DateTime::getElapsedTime();

				if (now - _d->lastKeyTime < 500)
				{
					// add to key buffer if it isn't a key repeat
					if (!(_d->keyBuffer.size() == 1 && _d->keyBuffer[0] == event.KeyboardEvent.Char))
					{
						_d->keyBuffer += " ";
						_d->keyBuffer[_d->keyBuffer.size()-1] = event.KeyboardEvent.Char;
					}
				}
				else
				{
					_d->keyBuffer = " ";
					_d->keyBuffer[0] = event.KeyboardEvent.Char;
				}
				_d->lastKeyTime = now;

				// find the selected item, starting at the current selection
        int start = _d->selectedItemIndex;
				// dont change selection if the key buffer matches the current item
        if (_d->selectedItemIndex > -1 && _d->keyBuffer.size() > 1)
				{
            if( _d->items[ _d->selectedItemIndex ].getText().size() >= _d->keyBuffer.size() 
                && !stricmp( _d->keyBuffer.c_str(), _d->items[_d->selectedItemIndex].getText().substr( 0,_d->keyBuffer.size() ).c_str() ) )
						return true;
				}

				int current;
        for( current = start+1; current < (int)_d->items.size(); ++current)
				{
          if( _d->items[current].getText().size() >= _d->keyBuffer.size())
					{
            if( !stricmp( _d->keyBuffer.c_str(), _d->items[current].getText().substr(0,_d->keyBuffer.size()).c_str() ) )
						{
              if ( _d->selectedItemIndex != current && !_d->selecting && !isFlag( LBF_MOVEOVER_SELECT ))
              {
                _IndexChanged( OC3_LISTBOX_CHANGED );
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
            if( !stricmp( _d->keyBuffer.c_str(), _d->items[current].getText().substr( 0,_d->keyBuffer.size()).c_str() ) )
						{
              if ( _d->selectedItemIndex != current && !_d->selecting && !isFlag( LBF_MOVEOVER_SELECT ))
              {
                _IndexChanged( OC3_LISTBOX_CHANGED );
              }

              setSelected(current);
							return true;
						}
					}
				}

				return true;
			}
			break;

		case OC3_GUI_EVENT:
			switch(event.GuiEvent.EventType)
			{
			case OC3_SCROLL_BAR_CHANGED:
        {
          if (event.GuiEvent.Caller == _d->scrollBar)
			      return true;
        }
		  break;

      case OC3_ELEMENT_FOCUSED:
      //          CallScriptFunction( GUI_EVENT + NRP_ELEMENT_FOCUSED, this );
      break;

			case OC3_ELEMENT_FOCUS_LOST:
				{
          //CallScriptFunction( GUI_EVENT + NRP_ELEMENT_FOCUS_LOST, this );
					if (event.GuiEvent.Caller == this)
          {
              _d->selecting = false;
          }
				}
      break;

			default:
			break;
			}
			break;

		case OC3_MOUSE_EVENT:
			{
				Point p = event.MouseEvent.getPosition();

				switch(event.MouseEvent.Event)
				{
				case OC3_MOUSE_WHEEL:
          {
            _d->scrollBar->setPos(_d->scrollBar->getPos() + (event.MouseEvent.Wheel < 0 ? -1 : 1) * (-_d->itemHeight/2));
			      return true;
          }
        break;

				case OC3_LMOUSE_PRESSED_DOWN:
				  {
            _d->dragEventSended = false;
            _d->selecting = true;

            if (isPointInside(p) && isFlag( LBF_SELECT_ON_MOUSE_DOWN ) )
            {
              _SelectNew(event.MouseEvent.Y);
            }

					  return true;
				  }
        break;

        case OC3_LMOUSE_LEFT_UP:
				  {
            _d->selecting = false;

					  if (isPointInside(p) && !isFlag( LBF_SELECT_ON_MOUSE_DOWN ) )
            {
                _SelectNew(event.MouseEvent.Y);
            }

					  return true;
				  }
        break;

				case OC3_MOUSE_MOVED:
          if( _d->selecting && isFlag( LBF_SELECT_ON_MOVE )/* || isFlag( LBF_MOVEOVER_SELECT )*/ )
					{
						if (isPointInside(p))
						{
              _SelectNew(event.MouseEvent.Y);
							return true;
						}
					}

//           if( _d->selecting && !_d->dragEventSended && !isPointInside(p) )
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

    needUpdateTexture4Text_( oldSelected );

    _d->selectedItemIndex = getItemAt( Point( getScreenLeft(), ypos ) );
    if (_d->selectedItemIndex<0 && !_d->items.empty())
        _d->selectedItemIndex = 0;

    needUpdateTexture4Text_( _d->selectedItemIndex );

    _RecalculateScrollPos();

    OC3_GUI_EVENT_TYPE eventType = (_d->selectedItemIndex == oldSelected && now < _d->selectTime + 500) 
									? OC3_LISTBOX_SELECTED_AGAIN 
									: OC3_LISTBOX_CHANGED;
    _d->selectTime = now;
	// post the news
    _IndexChanged( eventType );
}

//! Update the position and size of the listbox, and update the scrollbar
void ListBox::resizeEvent_()
{
    _d->totalItemHeight = 0;
    _d->recalculateItemHeight( _d->font, getHeight() );
}

ElementState ListBox::_GetCurrentItemState( unsigned int index, bool hl )
{
    if( _d->items[ index ].isEnabled() )
    {
        if( hl && index == _d->selectedItemIndex )
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

int ListBox::_GetCurrentItemColor( const ListBoxItem& item, bool selected )
{
  int ret = 0;
  ListBoxItem::ColorType tmpState = selected ? ListBoxItem::LBC_TEXT_HIGHLIGHT : ListBoxItem::LBC_TEXT;

  if( item.OverrideColors[ tmpState ].Use )
    ret = item.OverrideColors[ tmpState ].color;
  else if( ret == 0 )
    ret = getItemDefaultColor( tmpState );

  return ret;
}

Rect ListBox::getItemTextRect_()
{
  Rect frameRect( Point( 0, 0 ), getSize() );
  if( _d->scrollBar->isVisible() )
      frameRect.LowerRightCorner.setX( frameRect.LowerRightCorner.getX() - DEFAULT_SCROLLBAR_SIZE );

  frameRect.LowerRightCorner.setY( frameRect.UpperLeftCorner.getY() + _d->itemHeight );
  frameRect.LowerRightCorner -= Point( 4 + _d->itemsIconWidth + 3, 0 );

  return frameRect;
}

void ListBox::needUpdateTexture4Text_( unsigned int index )
{
    if ( (unsigned int)index >= _d->items.size() )
        return;

    _d->needItemsRepackTextures = true;
}

void ListBox::beforeDraw( GfxEngine& painter)
{
    if ( !isVisible() )
        return;

    _d->recalculateItemHeight( _d->font, getHeight() );

    if( _d->needItemsRepackTextures )
    {
      _updateTexture();

      SdlFacade::instance().drawPicture( *_d->background, *_d->picture, 0, 0 );
    
      bool hl = ( isFlag( LBF_HIGHLIGHTWHEN_NOTFOCUSED ) || isFocused() || _d->scrollBar->isFocused() );
      Rect frameRect = getItemTextRect_();

      TypeAlign itemTextHorizontalAlign, itemTextVerticalAlign;
      Font currentFont;
      int fontColor;

      for (int i=0; i<(int)_d->items.size(); ++i)
      {
         ListBoxItem& refItem = _d->items[i];

         if( frameRect.LowerRightCorner.getY() >= getScreenTop() &&
             frameRect.UpperLeftCorner.getY() <= getScreenBottom() )
         {
           refItem.setState( _GetCurrentItemState( i, hl ) );
           
           itemTextHorizontalAlign = refItem.isAlignEnabled() ? refItem.getHorizontalAlign() : getHorizontalTextAlign();
           itemTextVerticalAlign = refItem.isAlignEnabled() ? refItem.getVerticalAlign() : getVerticalTextAlign();

           Rect textRect = frameRect;
           textRect.UpperLeftCorner += Point( 3, 0 );

           currentFont = _GetCurrentItemFont( refItem, i == _d->selectedItemIndex && hl );
           fontColor = _GetCurrentItemColor( refItem, i==_d->selectedItemIndex && hl );

           _DrawItemIcon( refItem, textRect, hl, i == _d->selectedItemIndex, &_d->clientClip, fontColor );

           textRect.UpperLeftCorner += Point( _d->itemsIconWidth+3, 0 );

           SdlFacade::instance().drawText( *_d->picture, refItem.getText(), textRect.getLeft(), textRect.getTop(), currentFont );
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

  GfxEngine::instance().drawPicture( *_d->picture, getAbsoluteRect().UpperLeftCorner );

	Widget::draw( painter );
}

void ListBox::_DrawItemIcon( const ListBoxItem& item, const Rect& rectangle, bool highlighted,
                             bool selected, Rect* clip, const int color )
{
//     if (_d->iconBank && ( item.getIcon() > -1) )
//     {
//         Point iconPos = rectangle.UpperLeftCorner;
//         iconPos.Y += rectangle.getHeight() / 2;
//         iconPos.X += _d->itemsIconWidth/2;
// 
//         _d->iconBank->draw2DSprite( (unsigned int)item.getIcon(), iconPos, clip,
//                                  color, _d->selectTime, selected ? DateTime::getElapsedTime() : 0, false, true);
//     }
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
	else if (selPos > getHeight() - _d->itemHeight)
	{
        _d->scrollBar->setPos( _d->scrollBar->getPos() + selPos - getHeight() + _d->itemHeight );
	}
}

void ListBox::setAutoScrollEnabled(bool scroll)
{
	setFlag( LBF_AUTOSCROLL, scroll );
}

bool ListBox::isAutoScrollEnabled() const
{
	return isFlag( LBF_AUTOSCROLL );
}

//! Writes attributes of the element.
void ListBox::save( VariantMap& out ) const
{
   // Widget::save( out );
/*
	// todo: out->addString	("_d->iconBank",		_d->iconBank->getName?);
	out->addBool    ( core::SerializeHelper::drawBackProp,        IsFlag( LBF_DRAWBACK ));
	out->addBool    ("MoveOverSelect",  IsFlag( LBF_MOVEOVER_SELECT ) );
	out->addBool    ("AutoScroll",      IsFlag( LBF_AUTOSCROLL ) );

	out->addInt( core::SerializeHelper::itemCountProp, Items.size());
	for (unsigned int i=0;i<Items.size(); ++i)
	{
        const ListItem& refItem = Items[i];
        core::stringc itemName = core::SerializeHelper::itemProp; itemName += i;
        out->addString( core::SerializeHelper::subSectionStartProp, itemName.c_str() );

		core::stringc label("text");
		label += i;
		out->addString(label.c_str(), Items[i].text.c_str() );

        label = core::SerializeHelper::hTextAlignProp; label += i;
        out->addEnum( label.c_str(), refItem.horizontal, GUIAlignmentNames );

        label = core::SerializeHelper::vTextAlignProp; label += i;
        out->addEnum( label.c_str(), refItem.vertical, GUIAlignmentNames );

        label = core::SerializeHelper::textAlignEnabledProp; label += i;
        out->addBool( label.c_str(), refItem.alignEnabled );

		for ( int c=0; c < (int)EGUI_LBC_COUNT; ++c )
		{
			core::String useColorLabel, colorLabel, fontLabel;
			if ( !getSerializationLabels((NRP_LISTBOX_COLOR)c, useColorLabel, colorLabel, fontLabel ) )
				return;
			label = useColorLabel; label += i;
			if( refItem.OverrideColors[c].Use )
			{
				out->addBool(label.c_str(), true );
				label = colorLabel; label += i;
				out->addColor(label.c_str(), refItem.OverrideColors[c].Color);
                label = fontLabel; label += i;
                String fontName;
                Environment->GetFontManager()->GetFontName( refItem.OverrideColors[c].font, fontName );
                out->addString( label.c_str(), fontName.c_str() );
			}
			else
			{
				out->addBool(label.c_str(), false );
			}
		}

        out->addString( core::SerializeHelper::subSectionEndProp, itemName.c_str() );
	}
    */
}


//! Reads attributes of the element
void ListBox::load( const VariantMap& in )
{
/*
    clear();

	SetFlag( LBF_DRAWBACK, in->getAttributeAsBool( core::SerializeHelper::drawBackProp ) );
	SetFlag( LBF_MOVEOVER_SELECT, in->getAttributeAsBool( core::SerializeHelper::moveOverSelectProp ) );
	SetFlag( LBF_AUTOSCROLL, in->getAttributeAsBool("AutoScroll") );

	INrpListBox::deserializeAttributes(in,options);

	const int count = in->getAttributeAsInt( nrp::SerializeHelper::itemCountProp );
	for (int i=0; i<count; ++i)
	{
		core::stringc label("text");
		ListItem item;

		label += i;
		item.text = in->getAttributeAsStringW(label.c_str());

		addItem(item.text.c_str(), item.icon);

        ListItem& refItem = Items[i];
        label = nrp::SerializeHelper::hTextAlignProp; label += i;
        refItem.horizontal = (NRP_ALIGNMENT)in->getAttributeAsEnumeration( label.c_str(), GUIAlignmentNames );

        label = nrp::SerializeHelper::vTextAlignProp; label += i;
        refItem.vertical = (NRP_ALIGNMENT)in->getAttributeAsEnumeration( label.c_str(), GUIAlignmentNames );

        label = nrp::SerializeHelper::textAlignEnabledProp; label += i;
        refItem.alignEnabled = in->getAttributeAsBool( label.c_str() );

		for ( unsigned int c=0; c < EGUI_LBC_COUNT; ++c )
		{
			core::stringc useColorLabel, colorLabel, fontLabel;
			if ( !getSerializationLabels((EGUI_LISTBOX_COLOR)c, useColorLabel, colorLabel, fontLabel) )
				return;
			label = useColorLabel; label += i;
			refItem.OverrideColors[c].Use = in->getAttributeAsBool(label.c_str());
			if ( Items[i].OverrideColors[c].Use )
			{
				label = colorLabel; label += i;
				refItem.OverrideColors[c].Color = in->getAttributeAsColor(label.c_str());
                label = fontLabel; label += i;
                refItem.OverrideColors[c].font = GetEnvironment()->GetFontManager()->GetFont( in->getAttributeAsStringW( label.c_str() ) );
			}
		}
	}
    */
}

void ListBox::setItem(unsigned int index, const std::string& text, int icon)
{
  if ( index >= _d->items.size() )
	return;

  _d->items[index].setText( text );
  _d->items[index].setIcon( icon );

  _d->needItemsRepackTextures = true;

  _d->recalculateItemHeight( _d->font, getHeight() );
}


//! Insert the item at the given index
//! Return the index on success or -1 on failure.
int ListBox::insertItem(unsigned int index, const std::string& text, int icon)
{
  ListBoxItem i;
  i.setText( text );
  i.setIcon( icon );

  _d->items.insert( _d->items.begin() + index, i );

  _d->recalculateItemHeight( _d->font, getHeight() );

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

int ListBox::getItemDefaultColor( ListBoxItem::ColorType colorType) const
{
	switch ( colorType )
	{
		case ListBoxItem::LBC_TEXT:
			return 0xff000000;
		case ListBoxItem::LBC_TEXT_HIGHLIGHT:
			return 0xffe0e0e0;
		case ListBoxItem::LBC_ICON:
			return 0xffffffff;
		case ListBoxItem::LBC_ICON_HIGHLIGHT:
			return 0xff0f0f0f;
		default:
			return 0;
	}
}

//! set global itemHeight
void ListBox::setItemHeight( int height )
{
    _d->itemHeight = height;
    _d->itemHeightOverride = 1;
}


//! Sets whether to draw the background
void ListBox::setDrawBackground(bool draw)
{
    setFlag( LBF_DRAWBACK, draw );
}

void ListBox::SetItemAction( unsigned int index, int funcRef )
{
    if ( index >= _d->items.size() )
        return;

    //_d->items[index].luaFunction = funcRef;
}

ListBoxItem& ListBox::addItem( const std::string& text, const Font& font, const int color )
{
  ListBoxItem i;
  i.setText( text );
  i.setIcon( -1 );
  i.setState( stNormal );
  //i.currentHovered = 255;
	i.OverrideColors[ ListBoxItem::LBC_TEXT ].font = font;
  i.OverrideColors[ ListBoxItem::LBC_TEXT ].color = color;

  _d->needItemsRepackTextures = true;

  _d->items.push_back(i);

  _d->recalculateItemHeight( _d->font, getHeight() );

  return _d->items.back();
}

int ListBox::getSelected()
{
    return _d->selectedItemIndex;
}
