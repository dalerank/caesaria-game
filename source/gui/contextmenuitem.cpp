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
#include "contextmenu.hpp"

using namespace gfx;

namespace gui
{

ContextMenuItem::ContextMenuItem( ContextMenu* parent, const std::string& text ) 
  : Label( parent, Rect( 0, 0, 1, 1 ), text, false, Label::bgWhite ), _d( new Impl )
{
  _d->isSeparator = false;
  _d->subMenuAlignment = alignAuto;
  _d->checked = false;
  _d->isHovered = false;
  _d->isAutoChecking = false;
  _d->dim = Size( 0, 0 );
  _d->offset = 0;
  _d->subMenu = NULL;
  _d->setFlag( drawSubmenuSprite, false );
  _d->commandId = -1;
  _d->luaFunction = -1;
}

Signal1<bool>& ContextMenuItem::onChecked() {  return _d->onCheckedSignal; }
Signal1<int>&ContextMenuItem::onAction() { return _d->onActionSignal; }

ContextMenu* ContextMenuItem::addSubMenu( int id )
{
  ContextMenu* sub = new ContextMenu( parent(), Rect(0,0,100,100), id, false, false);
  setSubMenu( sub );
  parent()->bringChildToFront( sub );
  return sub;
}

//! Adds a sub menu from an element that already exists.
void ContextMenuItem::setSubMenu( ContextMenu* menu )
{
  if( menu )
    menu->grab();

  if( _d->subMenu )
    _d->subMenu->drop();

  _d->subMenu = menu;
  menu->setVisible(false);

  if( _d->subMenu )
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
  if( _d->isAutoChecking )
  {
    _d->checked = !_d->checked;
    emit _d->onCheckedSignal( _d->checked );
  }
}

void ContextMenuItem::setFlag( DrawFlag flagName, bool set/*=true */ )
{
  _d->setFlag( flagName, set );
}

ContextMenuItem::~ContextMenuItem()
{
  if( _d->subMenu )
    _d->subMenu->drop();
}

void ContextMenuItem::setHovered( bool hover )
{
  _d->isHovered = hover;
  setFont( Font::create( hover ? FONT_2_RED : FONT_2 ));
}

bool ContextMenuItem::isSeparator() const {  return _d->isSeparator; }
void ContextMenuItem::setCommandId( int cmdId ){	_d->commandId = cmdId;}
int ContextMenuItem::commandId() const{  return _d->commandId;}
bool ContextMenuItem::isHovered() const {  return _d->isHovered;}
void ContextMenuItem::setAutoChecking( bool autoChecking ){  _d->isAutoChecking = autoChecking;}
bool ContextMenuItem::isPointInside( const Point& point ) const{  return false;}
int ContextMenuItem::offset() const{  return _d->offset;}
void ContextMenuItem::setChecked( bool check ){  _d->checked = check;}
bool ContextMenuItem::isChecked() const{  return _d->checked;}
void ContextMenuItem::setIsSeparator( bool isSepar ){  _d->isSeparator = isSepar;}
ContextMenu* ContextMenuItem::subMenu() const{  return _d->subMenu;}
void ContextMenuItem::setSubMenuAlignment( SubMenuAlign align ){  _d->subMenuAlignment = align;}
ContextMenuItem::SubMenuAlign ContextMenuItem::subMenuAlignment() const{  return _d->subMenuAlignment;}
void ContextMenuItem::setIcon(const Picture& icon , Point offset){}
void ContextMenuItem::setDimmension( const Size& size ) {  _d->dim = size;}
const Size& ContextMenuItem::dimmension() const{  return _d->dim;}
void ContextMenuItem::setOffset( int offset ){  _d->offset = offset;}

}//end namespace gui
