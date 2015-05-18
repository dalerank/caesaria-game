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

#include "dlc_folder_viewer.hpp"
#include "core/osystem.hpp"
#include "widget_factory.hpp"
#include "gfx/picture.hpp"
#include "table.hpp"

using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(DlcFolderViewer)

class DlcFolderViewer::Impl
{
public:
  Picture background;

public:
  void init( const Size& size )
  {
    background = Picture( size, 0, true );
    background.fill( DefaultColors::clear.color & 0x88ffffff );
  }
};

DlcFolderViewer::DlcFolderViewer(Widget* parent)
  : Window( parent, Rect( Point(), parent->size() ), "", -1, bgNone ), _d( new Impl )
{
  _d->init( size() );
}

DlcFolderViewer::DlcFolderViewer(Widget* parent, vfs::Directory folder )
  : Window( parent, Rect( Point(), parent->size() ), "", -1, bgNone ), _d( new Impl )
{
  _d->init( size() );

  std::vector<vfs::Path> items;
  if( folder.exist() )
  {
    vfs::Entries::Items entries = folder.entries().items();
    foreach( it, entries )
    {
      if( !it->name.isMyExtension( ".info" ) )
        items.push_back( it->name );
    }
  }

  unsigned int columnCount = width() / 150;
  Table* table = new Table( this, -1, Rect( Point(), size() ) );
  for( int k=0; k < columnCount; k++ )
    table->addColumn( "" );

  for( unsigned int k=0; k < items.size(); k++ )
  {
    unsigned int rowNumber = k / columnCount;
    if( rowNumber >= table->columnCount() )
      table->addRow( rowNumber );

    table->setCellText( rowNumber, k % columnCount, items[ k ].baseName().toString() );
  }
  //OSystem::openDir( path.toString() );
}

}//end namespace gui
