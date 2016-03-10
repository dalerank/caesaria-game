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

#include "contextmenuitem.hpp"
#include "contextmenuitemprivate.hpp"
#include "core/color_list.hpp"
#include "contextmenu.hpp"

using namespace gfx;

namespace gui
{

ContextMenuItem::ContextMenuItem(Widget* parent, const std::string& text )
  : Label( parent, Rect( 0, 0, 1, 1 ), text, false, Label::bgWhite ),
    __INIT_IMPL(ContextMenuItem)
{
  __D_REF(d,ContextMenuItem)
  d.is.separator = false;
  d.is.checked = false;
  d.is.hovered = false;
  d.is.autoChecking = false;
  d.dim = Size( 0, 0 );
  d.offset = 0;
  d.submenu.widget = nullptr;
  d.submenu.align = alignAuto;
  d.submenu.iconVisible = true;
  d.setFlag( drawSubmenuSprite, false );
  d.commandId = -1;
}

Signal1<bool>& ContextMenuItem::onChecked() {  return _dfunc()->onCheckedSignal; }
Signal1<int>& ContextMenuItem::onAction() { return _dfunc()->onActionSignal; }

void ContextMenuItem::_updateTexture(Engine& painter)
{
  __D_REF(d,ContextMenuItem)
  setTextOffset( Point( isAutoChecking() ? 20 : 4, 0 ) );

  Label::_updateTexture( painter );
  bool updateTexture = false;

  if( isAutoChecking() )
  {
    const int sideOffset = 6;
    const int side = height()-sideOffset;

    Rect checkRect( sideOffset, sideOffset, side, side );
    _textPicture().fill( ColorList::black, checkRect );
    _textPicture().fill( isChecked() ? ColorList::firebrick : ColorList::floralwhite, checkRect.crop( 1 ) );
    updateTexture = true;
  }

  if( isHovered() )
  {
    for( int k=0; k < 2; ++k )
    {
      Rect checkRect( Point( k+1, 2), Size( 1, height()-4 ) );
      _textPicture().fill( ColorList::black, checkRect );
    }

    updateTexture = true;
  }

  if( d.submenu.iconVisible && submenu() != nullptr )
  {
    for( int k=7; k > 0; --k )
    {
      Rect checkRect( Point( width() - (k+6)  , height()/2 - k/2), Size( 1, k ) );
      _textPicture().fill( ColorList::black, checkRect );
    }

    updateTexture = true;
  }

  if( updateTexture )
  {
    _textPicture().update();
  }
}

ContextMenu* ContextMenuItem::addSubmenu(int id)
{
  auto& sub = parent()->add<ContextMenu>(Rect(0,0,100,100), id, false, false);
  setSubmenu(&sub);
  sub.bringToFront();
  return &sub;
}

ContextMenuItem* ContextMenuItem::addSubmenuItem(const std::string& text)
{
  if(!submenu())
    addSubmenu();

  return submenu()->addItem(text,-1);
}

//! Adds a sub menu from an element that already exists.
void ContextMenuItem::setSubmenu( ContextMenu* menu )
{
  __D_REF(d,ContextMenuItem)
  if( menu )
    menu->grab();

  if( d.submenu.widget != nullptr )
    d.submenu.widget->drop();

  d.submenu.widget = menu;
  menu->setVisible(false);

  if( d.submenu.widget )
  {
    menu->setAllowFocus( false );
    if( menu->isFocused() )
      parent()->setFocus();
  }

  if( ContextMenu* parentCntx = safety_cast< ContextMenu* >( parent() ) )
    parentCntx->updateItems();
}

void ContextMenuItem::toggleCheck()
{
  __D_REF(d,ContextMenuItem)
  if( d.is.autoChecking )
  {
    d.is.checked = !d.is.checked;
    emit d.onCheckedSignal( d.is.checked );
  }
}

void ContextMenuItem::draw(Engine& painter)
{
  Label::draw( painter );
}

void ContextMenuItem::setFlag( DrawFlag flagName, bool set/*=true */ )
{
  _dfunc()->setFlag( flagName, set );
}

ContextMenuItem::~ContextMenuItem()
{
  __D_REF(d,ContextMenuItem)
  if( d.submenu.widget )
    d.submenu.widget->drop();
}

void ContextMenuItem::setHovered( bool hover )
{
  _dfunc()->is.hovered = hover;
  setFont( hover ? FONT_2_RED : FONT_2 );
}

void ContextMenuItem::setVisible(bool visible)
{
  Label::setVisible(visible);
  ContextMenu* menu = safety_cast<ContextMenu*>(parent());
  if (menu)
    menu->updateItems();
}

bool ContextMenuItem::isAutoChecking() { return _dfunc()->is.autoChecking; }
bool ContextMenuItem::isSeparator() const {  return _dfunc()->is.separator; }
void ContextMenuItem::setCommandId( int cmdId ){	_dfunc()->commandId = cmdId;}
int ContextMenuItem::commandId() const{  return _dfunc()->commandId;}
bool ContextMenuItem::isHovered() const {  return _dfunc()->is.hovered;}
void ContextMenuItem::setAutoChecking( bool autoChecking ){ _dfunc()->is.autoChecking = autoChecking;}
bool ContextMenuItem::isPointInside( const Point& point ) const{ return false;}
int ContextMenuItem::offset() const{  return _dfunc()->offset;}
void ContextMenuItem::setChecked( bool check ) { _dfunc()->is.checked = check;}
bool ContextMenuItem::isChecked() const{  return _dfunc()->is.checked;}
void ContextMenuItem::setIsSeparator( bool separator ){  _dfunc()->is.separator = separator;}
ContextMenu* ContextMenuItem::submenu() const{  return _dfunc()->submenu.widget;}
void ContextMenuItem::setSubmenuAlignment( SubMenuAlign align ){ _dfunc()->submenu.align = align;}
ContextMenuItem::SubMenuAlign ContextMenuItem::subMenuAlignment() const{ return _dfunc()->submenu.align;}
void ContextMenuItem::setSubmenuIconVisible(bool visible) { _dfunc()->submenu.iconVisible = visible; }
void ContextMenuItem::setIcon(const Picture& icon , const Point& offset){}
void ContextMenuItem::setDimmension( const Size& size ) {  _dfunc()->dim = size;}
const Size& ContextMenuItem::dimmension() const{  return _dfunc()->dim;}
void ContextMenuItem::setOffset( int offset ){  _dfunc()->offset = offset;}

}//end namespace gui
