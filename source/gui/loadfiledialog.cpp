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

#include "loadfiledialog.hpp"
#include "gui/label.hpp"
#include "texturedbutton.hpp"
#include "gfx/decorator.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/engine.hpp"
#include "listbox.hpp"
#include "core/utils.hpp"
#include "vfs/filesystem.hpp"
#include "vfs/entries.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "widget_helper.hpp"
#include "filelistbox.hpp"
#include "vfs/file.hpp"
#include "gameautopause.hpp"
#include "widgetescapecloser.hpp"

namespace gui
{

namespace dialog
{

class LoadFile::Impl
{
public:
  FileListBox* lbxFiles;
  vfs::Directory directory;
  std::string fileExtensions;
  std::string saveItemText;
  bool mayDelete;

  void deleteFile();
  void emitSelectFile();

signals public:
  Signal1<std::string> onSelecteFileSignal;
};

LoadFile::LoadFile( Widget* parent, const Rect& rect,
                    const vfs::Directory& dir, const std::string& ext,
                    int id )
  : Window( parent, rect, "", id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/loadfile.gui" );

  _d->directory = dir;
  _d->fileExtensions = ext;

  INIT_WIDGET_FROM_UI( PushButton*, btnHelp )
  INIT_WIDGET_FROM_UI( PushButton*, btnLoad )
  INIT_WIDGET_FROM_UI( PushButton*, btnDelete )
  GET_DWIDGET_FROM_UI( _d, lbxFiles )

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnExit,   onClicked(), LoadFile::deleteLater )
  CONNECT( btnLoad,   onClicked(), _d.data(), Impl::emitSelectFile )
  CONNECT( btnDelete, onClicked(), _d.data(), Impl::deleteFile )
  CONNECT( btnDelete, onClicked(), this,      LoadFile::_fillFiles )

  if( _d->lbxFiles )
  {
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::simple, 0xffffffff );
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::hovered, 0xff000000 );
    _d->lbxFiles->setFocus();
  }

  CONNECT_LOCAL( _d->lbxFiles, onItemSelected(),      LoadFile::_resolveItemSelected )
  CONNECT_LOCAL( _d->lbxFiles, onItemSelectedAgain(), LoadFile::_resolveItemDblClick );

  _fillFiles();

  setMayDelete( false );
  setWindowFlag( fdraggable, false );
  WidgetClosers::insertTo( this, KEY_RBUTTON );
  GameAutoPauseWidget::insertTo( this );
  moveToCenter();
  setModal();
}

LoadFile::~LoadFile(){}

void LoadFile::_fillFiles()
{
  if( !_d->lbxFiles )
    return;
  _d->lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( _d->directory ).entries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, _d->fileExtensions );

  StringArray names = flist.items().fullnames();

  std::sort( names.begin(), names.end() );

  _d->lbxFiles->addItems( names );
}

void LoadFile::_resolveItemSelected(const ListBoxItem& item)
{
  _d->saveItemText = item.text();

  INIT_WIDGET_FROM_UI( PushButton*, btnLoad )
  INIT_WIDGET_FROM_UI( PushButton*, btnDelete )

  if( btnLoad ) btnLoad->setEnabled( !_d->saveItemText.empty() );
  if( btnDelete ) btnDelete->setEnabled( !_d->saveItemText.empty() );
}

void LoadFile::_resolveItemDblClick(const ListBoxItem& item)
{
  _d->saveItemText = item.text();
  _d->emitSelectFile();
}

void LoadFile::Impl::deleteFile()
{
  vfs::Path path4delete( saveItemText );
  vfs::NFile::remove( directory/path4delete );
}

void LoadFile::Impl::emitSelectFile()
{
  if( saveItemText.empty() )
    return;

  vfs::Path fn(saveItemText);
  emit onSelecteFileSignal( (directory/fn).toString() );
}

void LoadFile::draw(gfx::Engine& engine )
{
  Window::draw( engine );
}

bool LoadFile::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return parent()->absoluteRect().isPointInside( point );
}

void LoadFile::setMayDelete(bool mayDelete)
{
  INIT_WIDGET_FROM_UI( PushButton*, btnLoad )
  INIT_WIDGET_FROM_UI( PushButton*, btnDelete )

  _d->mayDelete = mayDelete;
  if( btnDelete ) btnDelete->setVisible( mayDelete );
  if( btnLoad )
  {
    btnLoad->bringToFront();
    btnLoad->setRight( !mayDelete
                          ? btnDelete->right()
                          : btnDelete->left() );
  }
}

void LoadFile::setShowExtension(bool showExtension)
{
  if( _d->lbxFiles )
  {
    _d->lbxFiles->setShowExtension( showExtension );
    _d->lbxFiles->refresh();
  }
}

void LoadFile::setTitle( const std::string& title )
{
  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  if( lbTitle )
    lbTitle->setText( title );
}

void LoadFile::setText(const std::string &text)
{
  INIT_WIDGET_FROM_UI( PushButton*, btnLoad )
  if( btnLoad )
    btnLoad->setText( text );
}

bool LoadFile::isMayDelete() const { return _d->mayDelete; }
Signal1<std::string>& LoadFile::onSelectFile() {  return _d->onSelecteFileSignal; }
FileListBox* LoadFile::_fileslbx() const { return _d->lbxFiles; }
const vfs::Directory& LoadFile::_directory() const { return _d->directory; }
const std::string&LoadFile::_extensions() const { return _d->fileExtensions; }

}//end namespace dialog

}//end namespace gui
