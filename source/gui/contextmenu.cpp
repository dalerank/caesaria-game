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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "contextmenu.hpp"
#include "contextmenuprivate.hpp"
#include "contextmenuitem.hpp"
#include "core/event.hpp"
#include "core/utils.hpp"
#include "core/time.hpp"
#include "core/position.hpp"
#include "environment.hpp"

namespace gui
{

//! constructor
ContextMenu::ContextMenu( Widget* parent, const Rect& rectangle,
                          int id, bool getFocus, bool allowFocus)
    : Widget( parent, id, rectangle),
    _d( new Impl )
{
  #ifdef _DEBUG
    setDebugName( "ContextMenu");
  #endif

  _d->eventParent = 0;
  _d->closeHandling = cmRemove;
  _d->pos = rectangle.lefttop();
  _d->flags.invalidate = true;

  if( getFocus )
    setFocus();

  setNotClipped(true);

  setTextAlignment( align::center, align::upperLeft );

  _d->flags.allowFocus = allowFocus;
  _d->changeTime = 0;
  _d->highlihted.index = -1;
}

//! destructor
ContextMenu::~ContextMenu() {}

//! set behaviour when menus are closed
void ContextMenu::setCloseHandling(CloseMode onClose) {  _d->closeHandling = onClose;}

//! get current behaviour when the menue will be closed
ContextMenu::CloseMode ContextMenu::getCloseHandling() const {  return _d->closeHandling;}

//! Returns amount of menu items
unsigned int ContextMenu::itemCount() const {  return _d->items.size();}

ContextMenuItem* ContextMenu::addItem( const std::string& path, const std::string& text, int commandId,
                                       bool enabled, bool hasSubMenu,
                                       bool checked, bool autoChecking)
{
  StringArray items = utils::split(path, "/");

  if (items.empty())
  {
    return addItem( text,  commandId, enabled, hasSubMenu, checked, autoChecking );
  }

  ContextMenuItem* lastItem = findItem(items.front());
  if (lastItem == NULL)
  {
    lastItem = addItem( items.front(), -1, true, true );
  }

  items.erase(items.begin());
  for (auto& item : items)
  {
    if (lastItem->submenu() == NULL)
    {
      lastItem = lastItem->addSubmenu()->addItem(item, -1, true, true);
    }
    else
    {
      ContextMenuItem* saveItem = lastItem;
      lastItem = lastItem->submenu()->findItem(item);
      if (!lastItem)
        lastItem = saveItem->submenu()->addItem(item, -1, true, true);
    }
  }

  if( lastItem->submenu() )
  {
    lastItem = lastItem->submenu()->addItem( text, commandId );
  }

  return lastItem;
}

//! Adds a menu item.
ContextMenuItem* ContextMenu::addItem( const std::string& text, int commandId,
                          bool enabled, bool hasSubMenu,
                          bool checked, bool autoChecking)
{
  return insertItem( _d->items.size(), text, commandId, enabled, hasSubMenu, checked, autoChecking);
}

ContextMenuItem* ContextMenu::addItem(const std::string& path, const std::string& text)
{
  updateItems();
  if(path.empty())
    return addItem(text,-1);
  else
    return addItem(path,text,-1);
}

//! Insert a menu item at specified position.
ContextMenuItem* ContextMenu::insertItem(unsigned int idx, const std::string& text, int commandId, bool enabled,
                                         bool hasSubMenu, bool checked, bool autoChecking)
{
  ContextMenuItem& newItem = add<ContextMenuItem>( text );
  newItem.setEnabled( enabled );
  newItem.setSubElement( true );
  newItem.setChecked( checked );
  newItem.setAutoChecking( autoChecking );
  newItem.setText( text );
  newItem.setFlag( ContextMenuItem::drawSubmenuSprite );
  newItem.setIsSeparator( text.empty() );
  newItem.setCommandId( commandId );
  newItem.sendToBack();

  if (hasSubMenu)
  {
    ContextMenu* subMenu = newItem.addSubmenu( commandId );
    subMenu->setVisible( false );
  }

  if ( idx < _d->items.size() )
  {
    _d->items.insert( _d->items.begin() + idx, &newItem );
  }
  else
  {
    _d->items.push_back( &newItem );
  }

  return &newItem;
}

ContextMenuItem* ContextMenu::findItem(const std::string& name) const
{
  for( auto it : _d->items )
  {
    if ( it->text() == name )
      return it;
  }

  return nullptr;
}

bool ContextMenu::itemExist(const std::string& name) const
{
  if (name.empty())
    return false;

  return findItem(name) != nullptr;
}

//! Adds a separator item to the menu
void ContextMenu::addSeparator() {  addItem(0, -1, true, false, false, false); }

//! Returns text of the menu item.
ContextMenuItem* ContextMenu::item( unsigned int idx ) const
{
  if( idx >= _d->items.size() )
    return NULL;

  return _d->items[ idx ];
}

//! Sets text of the menu item.
void ContextMenu::updateItems() {  _d->flags.invalidate = true; }

//! Removes a menu item
void ContextMenu::removeItem(unsigned int idx)
{
  if (idx >= _d->items.size())
    return;

  _d->items[idx]->deleteLater();
  _d->items.erase( _d->items.begin() + idx );
  updateItems();
}

//! Removes all menu items
void ContextMenu::removeAllItems()
{
  for (unsigned int i = 0; i < _d->items.size(); ++i)
    _d->items[ i ]->deleteLater();

  _d->items.clear();
  updateItems();
}

//! called if an event happened.
bool ContextMenu::onEvent(const NEvent& event)
{
  if( enabled() )
  {
    switch(event.EventType)
    {
    case sEventGui:
      switch(event.gui.type)
      {
      case guiElementFocusLost:
        if (event.gui.caller == this && !isMyChild(event.gui.element) && _d->flags.allowFocus)
        {
          // set event parent of submenus
          Widget* p = _d->eventParent ? _d->eventParent : parent();
          setEventParent(p);

          NEvent event;
          event.EventType = sEventGui;
          event.gui.caller = this;
          event.gui.element = 0;
          event.gui.type = guiElementClosed;
          if ( !p->onEvent(event) )
          {
            if( (_d->closeHandling & cmHide) > 0 )
            {
              hide();
            }
            else if( (_d->closeHandling & cmRemove) > 0 )
            {
              deleteLater();
            }
          }

          return false;
        }
        break;
      case guiElementFocused:
        if (event.gui.caller == this && !_d->flags.allowFocus)
        {
          return true;
        }
        break;
      default:
        break;
      }
    break;

    case sEventMouse:
      switch(event.mouse.type)
      {
      case NEvent::Mouse::mouseLbtnRelease:
        {
          // menu might be removed if it loses focus in sendClick, so grab a reference
          grab();
          const unsigned int t = _sendClick( event.mouse.pos() );
          if( (t==0 || t==1) && isFocused() )
            removeFocus();
          drop();
        }
        return true;
      case NEvent::Mouse::btnLeftPressed:
        return true;
      case NEvent::Mouse::moved:
        if( isHovered() )
          _isHighlighted( event.mouse.pos(), true);
        return true;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }

  return Widget::onEvent(event);
}

//! Sets the visible state of this element.
void ContextMenu::setVisible(bool visible)
{
  _setHovered( -1 );
  _closeAllSubMenus();

  if( visible )
    updateItems();

  Widget::setVisible( visible );
}

void ContextMenu::_setHovered( int index )
{
  _d->highlihted.index = index;
  _d->changeTime = DateTime::elapsedTime();
}

//! sends a click Returns:
//! 0 if click went outside of the element,
//! 1 if a valid button was clicked,
//! 2 if a nonclickable element was clicked
unsigned int ContextMenu::_sendClick(const Point& p)
{
  unsigned int t = 0;

  // get number of open submenu
  int openmenu = -1;
  int j;
  for( j=0; j<(int)_d->items.size(); ++j )
    if (_d->items[j]->submenu() && _d->items[j]->submenu()->visible())
    {
      openmenu = j;
      break;
    }

  // delegate click operation to submenu
  if (openmenu != -1)
  {
    t = _d->items[j]->submenu()->_sendClick( p );
    if (t != 0)
      return t; // clicked something
  }

  // check click on myself
  if( isPointInside(p) &&
    (unsigned int)_d->highlihted.index < _d->items.size())
  {
    if (!_d->items[_d->highlihted.index]->enabled() ||
      _d->items[_d->highlihted.index ]->isSeparator() ||
      _d->items[_d->highlihted.index ]->submenu() )
      return 2;

    selectedItem()->toggleCheck();

    NEvent event;
    event.EventType = sEventGui;
    event.gui.caller = this;
    event.gui.element = 0;
    event.gui.type = guiMenuItemSelected;
    if( _d->eventParent )
      _d->eventParent->onEvent(event);
    else
      parent()->onEvent(event);

    ContextMenuItem* tItem = selectedItem();
    if( tItem )
    {
      emit tItem->onClicked()();
      emit tItem->onAction()(tItem->commandId());
      emit tItem->onClickedA()(tItem);
      emit _d->onItemActionSignal(tItem->commandId());
    }
    return 1;
  }

  return 0;
}

void ContextMenu::setItemVisible( unsigned int index, bool visible )
{
  if( index >= _d->items.size() )
    return;

  ContextMenu* menuPtr = item( index )->submenu();
  if( menuPtr )
  {
    menuPtr->setVisible( visible );
  }
}

//! returns true, if an element was highligted
bool ContextMenu::_isHighlighted( const Point& p, bool canOpenSubMenu )
{
  if (!enabled())
  {
    return false;
  }

  // get number of open submenu
  int openmenu = -1;
  foreach( it, _d->items )
  {
    if( (*it)->enabled() && (*it)->submenu() && (*it)->submenu()->visible() )
    {
      openmenu = std::distance( _d->items.begin(), it );
      break;
    }
  }

  // delegate highlight operation to submenu
  if (openmenu != -1)
  {
    if (_d->items[openmenu]->enabled() && _d->items[openmenu]->submenu()->_isHighlighted(p, canOpenSubMenu))
    {
      _d->highlihted.index = openmenu;
      _d->changeTime = DateTime::elapsedTime();
      return true;
    }
  }

  for( auto it : _d->items )
  {
    it->setHovered( false );
  }

  // highlight myself
  _d->highlihted.last = -1;
  foreach( it, _d->items )
  {
    if ( (*it)->enabled() && (*it)->absoluteRect().isPointInside( p ))
    {
      _d->highlihted.index = std::distance( _d->items.begin(), it );
      _d->changeTime = DateTime::elapsedTime();

      // make submenus visible/invisible
      if( _d->highlihted.index != _d->highlihted.last )
      {
        _closeAllSubMenus();

        setItemVisible( _d->highlihted.last, false );

        ContextMenuItem* rItem = _d->items[ _d->highlihted.index ];
        if( rItem->submenu() && canOpenSubMenu && rItem->enabled() )
        {
          rItem->submenu()->setVisible( true );
          setItemVisible( _d->highlihted.index, true );
        }

        _d->highlihted.last = _d->highlihted.index;
        rItem->setHovered( true );
      }
      return true;
    }
  }

  _d->highlihted.index = openmenu;
  return false;
}

void ContextMenu::beforeDraw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Font font = Font::create( FONT_2_WHITE );

  if( font != _d->lastFont )
  {
    _d->lastFont = font;
    updateItems();
  }

  if( _d->flags.invalidate )
  {
    _d->flags.invalidate = false;
    _recalculateSize();
  }

  Widget::beforeDraw( painter );
}

//! draws the element and its children
void ContextMenu::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  Widget::draw( painter );
}

void ContextMenu::_recalculateSize()
{
  Rect rect;
  rect._lefttop = relativeRect().lefttop();
  Size maxSize( 100, 3 );

  unsigned int i;
  unsigned int itemSize = _d->items.size();
  for (i=0; i< itemSize; ++i)
  {
    ContextMenuItem* refItem = _d->items[i];
    if (refItem->isSeparator() )
    {
      refItem->setDimmension( Size( 150, 10 ) );
    }
    else
    {
      Font font = refItem->font();
      if( font.isValid() )
        refItem->setDimmension( font.getTextSize( refItem->text() ) + Size( 40, 0 ) );

      maxSize.setWidth( std::max<unsigned int>( refItem->dimmension().width(), maxSize.width() ) );
    }

    refItem->setOffset( maxSize.height() );
    maxSize += Size( 0, std::max<int>( refItem->dimmension().height(), 10 ) );
  }

  maxSize.setHeight( std::max<unsigned int>( maxSize.height()+5, 10 ) );

  rect._bottomright = relativeRect().lefttop() + Point( maxSize.width(), maxSize.height() );

  setGeometry(rect);

  // recalculate submenus
  for (i=0; i<_d->items.size(); ++i)
  {
    ContextMenuItem* refItem = _d->items[i];
    Rect rectangle( 0, refItem->offset(), width(), refItem->offset() + refItem->dimmension().height() );
    refItem->setGeometry( rectangle );

    if( refItem->submenu() )
    {
      // move submenu
      ContextMenu* subMenu = refItem->submenu();
      const Size subMenuSize = subMenu->absoluteRect().size();

      Rect subRect( maxSize.width()-5, refItem->offset(),
                    maxSize.width()+subMenuSize.width()-5, refItem->offset() +subMenuSize.height() );

      // if it would be drawn beyond the right border, then add it to the left side
      Widget * root = ui()->rootWidget();
      if( root && ContextMenuItem::alignAuto == refItem->subMenuAlignment() )
      {
        Rect rectRoot( root->absoluteRect() );
        if ( absoluteRect().left() + subRect.right() > rectRoot.right() )
        {
          subRect.setLeft( -subMenuSize.width() );
          subRect.setRight( 0 );
        }
      }
      else
      {
        switch( refItem->subMenuAlignment() & 0x0f )
        {
        case ContextMenuItem::alignLeft:
          subRect.setLeft( -subMenuSize.width() );
          subRect.setRight( 0 );
        break;

        case ContextMenuItem::alignRigth:
        break;

        case ContextMenuItem::alignHorizCenter:
          subRect.setLeft( ( absoluteRect().width() - subMenuSize.width() ) / 2 );
          subRect.setRight( subRect.left() + subMenuSize.width() );
        break;
        }

        switch( refItem->subMenuAlignment() & 0xf0 )
        {
        case ContextMenuItem::alignTop:
          subRect -= Point( 0, subMenuSize.height() / 2 + refItem->dimmension().height() );
        break;

        case ContextMenuItem::alignBottom:
        break;

        case ContextMenuItem::alignVertCenter:
          subRect -= Point( 0, subMenuSize.height() / 4 );
        break;
        }
            }

      subMenu->setGeometry(subRect);
    }
  }
}

//! Returns the selected item in the menu
int ContextMenu::selected() const
{
  return _d->highlihted.index;
}

ContextMenuItem *ContextMenu::selectedItem() const
{
  return item( _d->highlihted.index );
}

// because sometimes the element has no parent at click time
void ContextMenu::setEventParent( Widget *parent )
{
  _d->eventParent = parent;

  for( auto item : _d->items )
    if( item->submenu() )
      item->submenu()->setEventParent(parent);
}


bool ContextMenu::_hasOpenSubMenu() const
{
  for( auto i : _d->items )
    if( i->submenu() && i->submenu()->visible() )
      return true;

  return false;
}


void ContextMenu::_closeAllSubMenus()
{
  for(unsigned int i=0; i<_d->items.size(); ++i)
  {
    if( _d->items[i]->submenu() && _d->items[i]->visible())
    {
      setItemVisible( i, false );
    }
  }

  //HighLighted = -1;
}

void ContextMenu::setAllowFocus( bool enabled ) {	_d->flags.allowFocus = enabled;}
int ContextMenu::hovered() const {	return _d->highlihted.index; }
Signal1<int>& ContextMenu::onItemAction() { return _d->onItemActionSignal; }

}//end namespace gui
