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

#include "loadgamedialog.hpp"
#include "vfs/directory.hpp"
#include "filelistbox.hpp"
#include "gfx/loader.hpp"
#include "widget_helper.hpp"
#include "image.hpp"
#include "core/logger.hpp"

namespace gui
{

namespace dialog
{

LoadGame::LoadGame(Widget* parent, const vfs::Directory& dir )
  : LoadFile( parent, Rect(), dir, ".oc3save", -1 )
{
  Widget::setupUI( ":/gui/loadgame.gui" );

  CONNECT( _fileslbx(), onItemSelected(), this, LoadGame::_showPreview )
  setCenter( parent->center() );
}

void LoadGame::_fillFiles()
{
  FileListBox* lbxFiles = _fileslbx();
  if( !lbxFiles )
    return;

  lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( _directory() ).entries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, _extensions() );

  StringArray names = flist.items().files( "" );

  std::sort( names.begin(), names.end() );

  for( auto& path : names )
  {
    ListBoxItem& item = lbxFiles->addItem( path, Font(), DefaultColors::black.color );
    vfs::Path imgpath = vfs::Path( path ).changeExtension( "png" );
    item.setData( "image", imgpath.toString() );
  }
}

void LoadGame::_showPreview(const ListBoxItem &item)
{
  vfs::Path imgpath = item.data( "image" ).toString();
  gfx::Picture pic = PictureLoader::instance().load( vfs::NFile::open( imgpath ) );

  INIT_WIDGET_FROM_UI( Image*, imgPreview )
  if( imgPreview )
  {
    imgPreview->setPicture( pic );
  }
}

LoadGame* LoadGame::create(Widget *parent, const vfs::Directory &dir)
{
  LoadGame* ret = new LoadGame( parent, dir );
  ret->_fillFiles();

  return ret;
}

LoadGame::~LoadGame()
{
  _previewImg = gfx::Picture();
}

}//end namespace dialog

}//end namespace gui
