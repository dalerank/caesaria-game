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
#include "widgetescapecloser.hpp"

namespace gui
{

namespace dialog
{

class LoadFile::Impl
{
public:
  Label* lbTitle;
  FileListBox* lbxFiles;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnLoad;
  PushButton* btnDelete;
  vfs::Directory directory;
  std::string fileExtensions;
  std::string saveItemText;
  bool mayDelete;

  void resolveItemSelected( const ListBoxItem& item );
  void resolveItemDblClick( const ListBoxItem& item );

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
  _d->btnLoad = 0;

  Widget::setupUI( ":/gui/loadfile.gui" );

  WidgetEscapeCloser::insertTo( this );

  // create the title
  GET_DWIDGET_FROM_UI( _d, lbTitle )

  _d->directory = dir;
  _d->fileExtensions = ext;

  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnLoad )
  GET_DWIDGET_FROM_UI( _d, btnDelete )
  GET_DWIDGET_FROM_UI( _d, lbxFiles )

  CONNECT( _d->btnExit,   onClicked(), this,      LoadFile::deleteLater )
  CONNECT( _d->btnLoad,   onClicked(), _d.data(), Impl::emitSelectFile )
  CONNECT( _d->btnDelete, onClicked(), _d.data(), Impl::deleteFile )
  CONNECT( _d->btnDelete, onClicked(), this,      LoadFile::_fillFiles )

  if( _d->lbxFiles )
  {
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::simple, 0xffffffff );
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::hovered, 0xff000000 );
    _d->lbxFiles->setFocus();
  }

  CONNECT( _d->lbxFiles, onItemSelected(), _d.data(), Impl::resolveItemSelected )
  CONNECT( _d->lbxFiles, onItemSelectedAgain(), _d.data(), Impl::resolveItemDblClick );

  setMayDelete( false );
  setCenter( parent->center() );
}

LoadFile* LoadFile::create(Widget *parent, const Rect &rect, const vfs::Directory &dir, const std::string &ext, int id)
{
  LoadFile* ret = new LoadFile( parent, rect, dir, ext, id );
  ret->_fillFiles();

  return ret;
}

LoadFile::~LoadFile(){}

void LoadFile::_fillFiles()
{
  if( !_d->lbxFiles )
    return;
  _d->lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( _d->directory ).entries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, _d->fileExtensions );

  StringArray names;
  foreach( it, flist )
  {
    names <<  (*it).fullpath.toString();
  }
  std::sort( names.begin(), names.end() );

  _d->lbxFiles->addItems( names );
}

void LoadFile::Impl::resolveItemSelected(const ListBoxItem& item)
{
  saveItemText = item.text();
  if( btnLoad ) btnLoad->setEnabled( !saveItemText.empty() );
  if( btnDelete ) btnDelete->setEnabled( !saveItemText.empty() );
}

void LoadFile::Impl::resolveItemDblClick(const ListBoxItem& item)
{
  saveItemText = item.text();
  emitSelectFile();
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

bool LoadFile::onEvent( const NEvent& event)
{
  return Widget::onEvent( event );
}

void LoadFile::setMayDelete(bool mayDelete)
{
  _d->mayDelete = mayDelete;
  _d->btnDelete->setVisible( mayDelete );
  _d->btnLoad->bringToFront();
  _d->btnLoad->setRight( !mayDelete
                          ? _d->btnDelete->right()
                          : _d->btnDelete->left() );
}

void LoadFile::setShowExtension(bool showExtension)
{
  if( _d->lbxFiles )
  {
    _d->lbxFiles->setShowExtension( showExtension );
    _d->lbxFiles->refresh();
  }
}

void LoadFile::setTitle( const std::string& title ) { if( _d->lbTitle ) _d->lbTitle->setText( title ); }
void LoadFile::setText(const std::string &text) { if( _d->btnLoad ) _d->btnLoad->setText( text ); }
bool LoadFile::isMayDelete() const { return _d->mayDelete; }
Signal1<std::string>& LoadFile::onSelectFile() {  return _d->onSelecteFileSignal; }
FileListBox* LoadFile::_fileslbx() const { return _d->lbxFiles; }
const vfs::Directory& LoadFile::_directory() const { return _d->directory; }
const std::string&LoadFile::_extensions() const { return _d->fileExtensions; }

}//end namespace dialog

}//end namespace gui
