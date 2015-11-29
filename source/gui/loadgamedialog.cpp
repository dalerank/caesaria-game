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
#include "core/color_list.hpp"
#include "widget_helper.hpp"
#include "image.hpp"
#include "core/gettext.hpp"
#include "vfs/filesystem.hpp"
#include "core/logger.hpp"

namespace gui
{

namespace dialog
{

LoadGame::LoadGame(Widget* parent, const vfs::Directory& dir )
  : LoadFile( parent, Rect(), dir, ".oc3save", -1 )
{
  _sortMode = sortCount;
  Widget::setupUI( ":/gui/loadgame.gui" );

  INIT_WIDGET_FROM_UI(PushButton*, btnSort)

  CONNECT( _fileslbx(), onItemSelected(), this, LoadGame::_showPreview )
  CONNECT( btnSort, onClicked(), this, LoadGame::_changeSort )

  moveTo( Widget::parentCenter );

  _changeSort();
}

namespace internal{

StringArray sortByDate( const vfs::Entries& array )
{
  std::map<DateTime,std::string> sortedByTime;
  for( const auto& item : array.items() )
  {
    DateTime time = vfs::FileSystem::instance().getFileUpdateTime( item.fullpath );
    sortedByTime[ time ] = item.fullpath.toString();
  }

  StringArray ret;
  for( const auto& item : sortedByTime )
    ret.push_front( item.second );

  return ret;
}

StringArray sortWithName( const vfs::Entries& array, const StringArray& parts, bool negative=false )
{
  StringArray sortedByName;
  for( const auto& item : array.items() )
  {
    bool containPart = false;

    for( const auto& part : parts )
      containPart |= (item.name.toString().find( part ) != std::string::npos);

    if( (negative && !containPart) || (containPart && !negative))
      sortedByName.push_back( item.fullpath.toString() );
  }

  return sortedByName;
}

}//end namespace internal

void LoadGame::_fillFiles()
{
  FileListBox* lbxFiles = _fileslbx();
  if( !lbxFiles )
    return;

  lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( _directory() ).entries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, _extensions() );

  StringArray names;

  switch( _sortMode )
  {
  case sortDate:
    names = internal::sortByDate( flist );
  break;

  case sortFast:
  {
    StringArray exclude;
    exclude << "fastsave";
    names = internal::sortWithName( flist, exclude );
  }
  break;

  case sortAuto:
  {
    StringArray exclude;
    exclude << "autosave";
    names = internal::sortWithName( flist, exclude );
  }
  break;

  case sortUser:
  {
    StringArray exclude;
    exclude << "fastsave" << "autosave";
    names = internal::sortWithName( flist, exclude, true );
  }
  break;

  default:
  case sortName:
    names = flist.items().files( "" );
    std::sort( names.begin(), names.end() );
  break;
  }

  for( const auto& path : names )
  {
    ListBoxItem& item = lbxFiles->addItem( path, Font(), ColorList::black.color );
    vfs::Path imgpath = vfs::Path( path ).changeExtension( "png" );
    item.setData( "image", imgpath.toString() );
  }

  moveTo( Widget::parentCenter );
}

void LoadGame::_showPreview(const ListBoxItem &item)
{
  vfs::Path imgpath = item.data( "image" ).toString();
  gfx::Picture pic = PictureLoader::instance().load( vfs::NFile::open( imgpath ) );

  INIT_WIDGET_FROM_UI( Image*, imgPreview )
  if( imgPreview )
    imgPreview->setPicture( pic );
}

void LoadGame::_changeSort()
{
  _sortMode = SortMode((_sortMode+1)%sortCount);

  StringArray sortText;
  sortText << "##srt_by_name##" << "##srt_by_date##"
           << "##srt_by_fast##" << "##srt_by_auto##"
           << "##srt_by_user##";

  INIT_WIDGET_FROM_UI(PushButton*, btnSort)
  if( btnSort )
    btnSort->setText( _( sortText.valueOrEmpty(_sortMode) ) );

  _fillFiles();
}

LoadGame::~LoadGame()
{
  _previewImg = gfx::Picture();
}

}//end namespace dialog

}//end namespace gui
