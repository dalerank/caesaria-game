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

namespace gui
{

namespace dialog
{

LoadGame::LoadGame( Widget* parent, const Rect& rect,
                    const vfs::Directory& dir )
  : LoadFileDialog( parent, rect, dir, "oc3save", -1 )
{
  Widget::setupUI( ":/gui/loadgame.gui" );
}

void LoadGame::_fillFiles()
{
  FileListBox* lbxFiles = _fileslbx();
  if( !lbxFiles )
    return;

  lbxFiles->clear();

  vfs::Entries flist = vfs::Directory( _directory() ).entries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, _extensions() );

  StringArray names;
  foreach( it, flist )
    names << (*it).fullpath.toString();

  std::sort( names.begin(), names.end() );

  foreach( it, names )
  {
    vfs::Path imgpath = vfs::Path( *it ).changeExtension( "png" );
    lbxFiles->addItem( *it, Font(), DefaultColors::black );
  }
}

LoadGame::~LoadGame(){}

}//end namespace dialog

}//end namespace gui
