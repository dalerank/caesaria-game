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
#include "core/stringhelper.hpp"
#include "vfs/filesystem.hpp"
#include "vfs/entries.hpp"
#include "core/color.hpp"
#include "core/logger.hpp"
#include "vfs/directory.hpp"
#include "filelistbox.hpp"
#include "vfs/file.hpp"

namespace gui
{

class LoadFileDialog::Impl
{
public:
  Label* lbTitle;
  FileListBox* lbxFiles;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnLoad;
  PushButton* btnDelete;
  vfs::Directory directory;
  std::string fileExtension;
  std::string saveItemText;
  bool mayDelete;

  void fillFiles();

  void resolveItemSelected( const ListBoxItem& item );

  void resolveItemDblClick( std::string fileName );

  void deleteFile();

  void emitSelectFile();

oc3_signals public:
  Signal1<std::string> onSelecteFileSignal;
};

LoadFileDialog::LoadFileDialog( Widget* parent, const Rect& rect,
                              const vfs::Directory& dir, const std::string& ext,
                              int id )
  : Window( parent, rect, "", id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/loadfile.gui" );
  setCenter( parent->center() );

  // create the title
  _d->lbTitle = findChildA<Label*>( "lbTitle", true, this );
  _d->directory = dir;
  _d->fileExtension = ext;

  _d->btnExit = findChildA<TexturedButton*>( "btnExit", true, this );
  _d->btnHelp = findChildA<TexturedButton*>( "btnHelp", true, this );
  _d->btnLoad = findChildA<PushButton*>( "btnLoad", true, this );
  _d->btnDelete = findChildA<PushButton*>( "btnDelete", true, this );

  CONNECT( _d->btnExit, onClicked(), this, LoadFileDialog::deleteLater );
  CONNECT( _d->btnLoad, onClicked(), _d.data(), Impl::emitSelectFile );
  CONNECT( _d->btnDelete, onClicked(), _d.data(), Impl::deleteFile );

  _d->lbxFiles = findChildA<FileListBox*>( "lbxFiles", true, this );
  if( _d->lbxFiles )
  {
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::simple, 0xffffffff );
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::hovered, 0xff000000 );
  }

  CONNECT( _d->lbxFiles, onItemSelected(), _d.data(), Impl::resolveItemSelected )
  CONNECT( _d->lbxFiles, onItemSelectedAgain(), _d.data(), Impl::resolveItemDblClick );
  _d->fillFiles();

  setMayDelete( false );
}

LoadFileDialog::~LoadFileDialog(){}

void LoadFileDialog::Impl::fillFiles()
{
  if( !lbxFiles )
    return;
  lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( directory ).getEntries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, fileExtension );

  StringArray names;
  for( vfs::Entries::ConstItemIt it=flist.begin(); it != flist.end(); ++it )
  {
    names << (*it).absolutePath().toString();
  }

  lbxFiles->addItems( names );
}

void LoadFileDialog::Impl::resolveItemSelected(const ListBoxItem& item)
{
  saveItemText = item.text();
  if( btnLoad ) btnLoad->setEnabled( !saveItemText.empty() );
  if( btnDelete ) btnDelete->setEnabled( !saveItemText.empty() );
}

void LoadFileDialog::Impl::resolveItemDblClick(std::string fileName)
{
  saveItemText = fileName;
  emitSelectFile();
}

void LoadFileDialog::Impl::deleteFile()
{
  vfs::Path path4delete( saveItemText );
  vfs::NFile::remove( directory/path4delete );
  fillFiles();
}

void LoadFileDialog::Impl::emitSelectFile()
{
  if( saveItemText.empty() )
    return;

  vfs::Path fn(saveItemText);
  oc3_emit onSelecteFileSignal( (directory/fn).toString() );
}

void LoadFileDialog::draw(gfx::Engine& engine )
{
  Window::draw( engine );
}

bool LoadFileDialog::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return parent()->absoluteRect().isPointInside( point );
}

bool LoadFileDialog::onEvent( const NEvent& event)
{
  return Widget::onEvent( event );
}

void LoadFileDialog::setMayDelete(bool mayDelete)
{
  _d->mayDelete = mayDelete;
  _d->btnDelete->setVisible( mayDelete );
  _d->btnLoad->bringToFront();
  _d->btnLoad->setRight( !mayDelete
                          ? _d->btnDelete->right()
                          : _d->btnDelete->left() );
}

void LoadFileDialog::setTitle( const std::string& title ) { if( _d->lbTitle ) _d->lbTitle->setText( title ); }
bool LoadFileDialog::isMayDelete() const { return _d->mayDelete; }
Signal1<std::string>& LoadFileDialog::onSelectFile() {  return _d->onSelecteFileSignal; }

}//end namespace gui
