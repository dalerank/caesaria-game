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
#include "core/locale.hpp"
#include "core/variant_map.hpp"
#include "table.hpp"
#include "gfx/loader.hpp"
#include "core/saveadapter.hpp"
#include "gui/widget_helper.hpp"
#include "core/logger.hpp"
#include "core/priorities.hpp"

using namespace gfx;
using namespace vfs;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(DlcFolderViewer)

class DlcFolderViewer::Impl
{
public:
  Picture background;
  Table* table;
  Directory folder;
  StringArray exts;

public:
  void fillTable( const std::vector<Path>& items )
  {
    unsigned int columnCount = table->width() / 150;
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

      Picture pic;
      if( items[ k ].isMyExtension( ".pdf" ) )
      { pic = Picture( "dlcperf", 1 ); }
      else
      { pic = PictureLoader::instance().load( vfs::NFile::open( items[ k ] ) ); }
      Image* image = new Image( table, Rect( 0, 0, 140, 140 ), pic, Image::best );
      table->addElementToCell( rowNumber, columnNumber, image );
      table->setCellData( rowNumber, columnNumber, "path", items[ k ].toString() );
    }
  }

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

DlcFolderViewer::DlcFolderViewer(Widget* parent, Directory folder )
  : Window( parent, Rect( Point(), parent->size() ), "", -1, bgNone ), _d( new Impl )
{
  if( !folder.exist() )
    return;

  Window::setupUI( ":/gui/dlcviewer.gui" );

  _d->init( size() );
  _d->folder = folder;

  Path configFile = folder/".info";
  std::vector<Path> items;

  if( configFile.exist() )
  {
    VariantList list = config::load( configFile.toString() ).get( "items" ).toList();
    foreach( it, list )
    {
      items.push_back( folder/Path(it->toString()) );
    }
  }
  else
  {
    vfs::Entries::Items entries = folder.entries().items();
    foreach( it, entries )
    {
      if( _d->exts.contains( it->name.extension() )  )
        items.push_back( it->fullpath );
    }
  }

  _d->table = new Table( this, -1, Rect( 80, 50, width() - 50, height() - 50 ) );
  _d->table->setDrawFlag( Table::drawColumns, false );
  _d->table->setDrawFlag( Table::drawRows, false );
  _d->table->setDrawFlag( Table::drawActiveCell, true );

  _d->fillTable( items );
  CONNECT( _d->table, onCellClicked(), this, DlcFolderViewer::_resolveCellClick )

  PushButton* btn = new PushButton( this, Rect( Point( width() / 2 - 200, height() - 40 ), Size( 200, 24 ) ), "Open folder" );
  CONNECT( btn, onClicked(), this, DlcFolderViewer::_openFolder )
  btn = new PushButton( this, Rect( Point( width() / 2 + 2, height() - 40 ), Size( 200, 24 ) ), "Close" );
  CONNECT( btn, onClicked(), this, DlcFolderViewer::deleteLater )
}

DlcFolderViewer::~DlcFolderViewer() {}

void DlcFolderViewer::_openFolder()
{
  OSystem::openDir( _d->folder.toString() );
}

void DlcFolderViewer::draw(Engine& painter)
{
  if( !visible() )
    return;

  painter.draw( _d->background, lefttop() );

  Window::draw( painter );
}

void DlcFolderViewer::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );
  _d->exts = ui.get( "etxs" ).toStringArray();
}

void DlcFolderViewer::_loadDesc(Path path)
{
  Window* window = new Window( this, _d->table->relativeRect(), "" );
  window->setupUI( path );

  PushButton* btnClose = findChildA<PushButton*>( "btnClose", true, window );
  CONNECT( btnClose, onClicked(), window, Window::deleteLater )
}

void DlcFolderViewer::_resolveCellClick(int row, int column)
{
  if( _d->table )
  {
    Path path = _d->table->getCellData( row, column, "path" ).toString();
    path = path.changeExtension( Locale::current() );

    if( !path.exist() )
    {
      path.changeExtension( "ru" );
    }

    if( path.exist() )
    {
      _loadDesc( path );
    }
  }
}

}//end namespace gui
