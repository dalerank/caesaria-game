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
  d.isSeparator = false;
  d.subMenuAlignment = alignAuto;
  d.checked = false;
  d.isHovered = false;
  d.isAutoChecking = false;
  d.dim = Size( 0, 0 );
  d.offset = 0;
  d.subMenu = NULL;
  d.setFlag( drawSubmenuSprite, false );
  d.commandId = -1;
  d.luaFunction = -1;
}

Signal1<bool>& ContextMenuItem::onChecked() {  return _dfunc()->onCheckedSignal; }
Signal1<int>&ContextMenuItem::onAction() { return _dfunc()->onActionSignal; }

void ContextMenuItem::_updateTexture(Engine& painter)
{
  setTextOffset( Point( isAutoChecking() ? 20 : 4, 0 ) );

  Label::_updateTexture( painter );

  if( isAutoChecking() )
  {
    const int sideOffset = 6;
    const int side = height()-sideOffset;

    Rect checkRect( sideOffset, sideOffset, side, side );
    _textPicture().fill( ColorList::black, checkRect );
    _textPicture().fill( isChecked() ? ColorList::firebrick : ColorList::floralwhite, checkRect.crop( 1 ) );
    _textPicture().update();
  }
}

ContextMenu* ContextMenuItem::addSubMenu( int id )
{
  auto& sub = parent()->add<ContextMenu>( Rect(0,0,100,100), id, false, false );
  setSubMenu( &sub );
  sub.bringToFront();
  return &sub;
}

//! Adds a sub menu from an element that already exists.
void ContextMenuItem::setSubMenu( ContextMenu* menu )
{
  __D_REF(d,ContextMenuItem)
  if( menu )
    menu->grab();

  if( d.subMenu )
    d.subMenu->drop();

  d.subMenu = menu;
  menu->setVisible(false);

  if( d.subMenu )
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
  if( d.isAutoChecking )
  {
    d.checked = !d.checked;
    emit d.onCheckedSignal( d.checked );
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
  if( d.subMenu )
    d.subMenu->drop();
}

void ContextMenuItem::setHovered( bool hover )
{
  _dfunc()->isHovered = hover;
  setFont( Font::create( hover ? FONT_2_RED : FONT_2 ));
}

bool ContextMenuItem::isAutoChecking() { return _dfunc()->isAutoChecking; }
bool ContextMenuItem::isSeparator() const {  return _dfunc()->isSeparator; }
void ContextMenuItem::setCommandId( int cmdId ){	_dfunc()->commandId = cmdId;}
int ContextMenuItem::commandId() const{  return _dfunc()->commandId;}
bool ContextMenuItem::isHovered() const {  return _dfunc()->isHovered;}
void ContextMenuItem::setAutoChecking( bool autoChecking ){ _dfunc()->isAutoChecking = autoChecking;}
bool ContextMenuItem::isPointInside( const Point& point ) const{ return false;}
int ContextMenuItem::offset() const{  return _dfunc()->offset;}
void ContextMenuItem::setChecked( bool check ) { _dfunc()->checked = check;}
bool ContextMenuItem::isChecked() const{  return _dfunc()->checked;}
void ContextMenuItem::setIsSeparator( bool isSepar ){  _dfunc()->isSeparator = isSepar;}
ContextMenu* ContextMenuItem::subMenu() const{  return _dfunc()->subMenu;}
void ContextMenuItem::setSubMenuAlignment( SubMenuAlign align ){ _dfunc()->subMenuAlignment = align;}
ContextMenuItem::SubMenuAlign ContextMenuItem::subMenuAlignment() const{ return _dfunc()->subMenuAlignment;}
void ContextMenuItem::setIcon(const Picture& icon , Point offset){}
void ContextMenuItem::setDimmension( const Size& size ) {  _dfunc()->dim = size;}
const Size& ContextMenuItem::dimmension() const{  return _dfunc()->dim;}
void ContextMenuItem::setOffset( int offset ){  _dfunc()->offset = offset;}

}//end namespace gui
