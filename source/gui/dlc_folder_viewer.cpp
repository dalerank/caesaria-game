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
#include "gfx/engine.hpp"
#include "image.hpp"
#include "core/variant_map.hpp"
#include "table.hpp"
#include "gfx/loader.hpp"
#include "core/saveadapter.hpp"

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
    background.fill( DefaultColors::black.color & 0xaaffffff );
    background.update();
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
  if( !folder.exist() )
    return;

  _d->init( size() );

  vfs::Path configFile = folder/".info";
  std::vector<vfs::Path> items;

  if( configFile.exist() )
  {
    VariantList list = config::load( configFile.toString() ).get( "items" ).toList();
    foreach( it, list )
    {
      items.push_back( folder/vfs::Path(it->toString()) );
    }
  }
  else
  {
    vfs::Entries::Items entries = folder.entries().items();
    foreach( it, entries )
    {
      if( !it->name.isMyExtension( ".desc" ) )
        items.push_back( it->fullpath );
    }
  }

  Table* table = new Table( this, -1, Rect( 50, 50, width() - 50, height() - 50 ) );
  table->setDrawFlag( Table::drawColumns, false );
  table->setDrawFlag( Table::drawRows, false );
  table->setDrawFlag( Table::drawActiveCell, true );
  unsigned int columnCount = width() / 150;
  table->setRowHeight( 150 );
  for( unsigned int k=0; k < columnCount; k++ )
  {
    table->addColumn( "" );
    table->setColumnWidth( table->columnCount()-1, 150 );
  }

  for( unsigned int k=0; k < items.size(); k++ )
  {
    int rowNumber = k / columnCount;
    int columnNumber = k % columnCount;
    if( rowNumber >= table->rowCount() )
    {
      table->addRow( rowNumber );
    }

    Picture pic = PictureLoader::instance().load( vfs::NFile::open( items[ k ] ) );
    Image* image = new Image( this, Rect( 0, 0, 140, 140 ), pic, Image::best );
    table->setCellElement( rowNumber, columnNumber, image );
    table->setCellData( rowNumber, columnNumber, "path", items[ k ].toString() );
  }
  //OSystem::openDir( path.toString() );
}

void DlcFolderViewer::draw(Engine& painter)
{
  if( !visible() )
    return;

  painter.draw( _d->background, lefttop() );

  Window::draw( painter );
}

}//end namespace gui
