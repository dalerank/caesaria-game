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

#include "loadmapwindow.hpp"
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
#include "game/settings.hpp"

namespace gui
{

class LoadMapWindow::Impl
{
public:
  Label* lbTitle;
  ListBox* files;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnLoad;
  vfs::Directory directory;
  std::string fileExtension;
  std::string saveItemText;

  void fillFiles();

  void resolveItemSelected( const ListBoxItem& item )
  {
    saveItemText = item.text();
    if( btnLoad )
      btnLoad->setEnabled( !saveItemText.empty() );
  }

  void resolveItemDblClick( std::string fileName )
  {
    saveItemText = fileName;
    emitSelectFile();
  }

  void emitSelectFile()
  {
    if( saveItemText.empty() )
      return;

    vfs::Path fn(saveItemText);
    onSelecteFileSignal.emit( (directory/fn).toString() );
  }

oc3_signals public:
  Signal1<std::string> onSelecteFileSignal;
};

LoadMapWindow::LoadMapWindow( Widget* parent, const Rect& rect,
                              const vfs::Directory& dir, const std::string& ext,
                              int id )
: Widget( parent, id, rect ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/loadfile.gui" ) );
  setCenter( parent->center() );

  // create the title
  _d->lbTitle = findChildA<Label*>( "lbTitle", true, this );
  _d->directory = dir;
  _d->fileExtension = ext;

  _d->btnExit = findChildA<TexturedButton*>( "btnExit", true, this );
  _d->btnHelp = findChildA<TexturedButton*>( "btnHelp", true, this );
  _d->btnLoad = findChildA<PushButton*>( "btnLoad", true, this );
  if( _d->btnLoad )
  {
    _d->btnLoad->setEnabled( false );
  }
  CONNECT( _d->btnExit, onClicked(), this, LoadMapWindow::deleteLater );
  CONNECT( _d->btnLoad, onClicked(), _d.data(), Impl::emitSelectFile );

  _d->files = findChildA<ListBox*>( "lbxFiles", true, this );
  if( _d->files )
  {
    _d->files->setItemFont( Font::create( FONT_2_WHITE ) );
    _d->files->setItemDefaultColor( ListBoxItem::LBC_TEXT, 0xffffffff );
    _d->files->setItemDefaultColor( ListBoxItem::LBC_TEXT_HIGHLIGHT, 0xff000000 );
  }

  CONNECT( _d->files, onItemSelected(), _d.data(), Impl::resolveItemSelected )
  CONNECT( _d->files, onItemSelectedAgain(), _d.data(), Impl::resolveItemDblClick );
  _d->fillFiles();
}

LoadMapWindow::~LoadMapWindow(){}

void LoadMapWindow::Impl::fillFiles()
{
  vfs::Entries flist = vfs::Directory( directory ).getEntries();
  StringArray names;
  names << flist.filter( vfs::Entries::file | vfs::Entries::extFilter, fileExtension );
  if( files )
  {
    files->addItems( names );
  }
}

void LoadMapWindow::draw(gfx::Engine& engine )
{
  Widget::draw( engine );
}

bool LoadMapWindow::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return parent()->absoluteRect().isPointInside( point );
}

bool LoadMapWindow::onEvent( const NEvent& event)
{
  return Widget::onEvent( event );
}

void LoadMapWindow::setTitle( const std::string& title )
{
  _d->lbTitle->setText( title );
}

Signal1<std::string>& LoadMapWindow::onSelectFile() {  return _d->onSelecteFileSignal; }

}//end namespace gui
