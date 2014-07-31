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

class FileListBox : public ListBox
{
public:
  FileListBox( Widget* parent, const Rect& rectangle, int id )
    : ListBox( parent, rectangle, id )
  {

  }

  virtual ListBoxItem& addItem(const std::string &text, Font font, const int color)
  {
    DateTime time = vfs::FileSystem::instance().getFileUpdateTime( text );
    std::string timeStr = StringHelper::format( 0xff, "(%02d %s %02d:%02d:%02d)",
                                                time.day(), DateTime::getShortMonthName( time.month()-1 ),
                                                time.hour(), time.minutes(), time.seconds() );
    ListBoxItem& item = ListBox::addItem( vfs::Path( text ).baseName().toString(), font, color );

    item.setData( Variant( timeStr ) );
    return item;
  }

protected:
  virtual void _drawItemText(gfx::Picture& texture, Font font, ListBoxItem& item, const Point& pos)
  {
    font.draw( texture, item.text(), pos, false );
    Font font2 = Font::create( FONT_1 );

    std::string timeStr = item.data().toString();
    Rect textRect = _itemsRect();
    textRect = font2.getTextRect( timeStr, Rect( 0, pos.y(), textRect.width(), pos.y() + itemHeight() ),
                                        align::lowerRight, align::center );

    font2.setColor( font.color() );
    font2.draw( texture, timeStr, textRect.UpperLeftCorner - Point( 10, 0), false );
  }
};

class LoadMapWindow::Impl
{
public:
  Label* lbTitle;
  FileListBox* lbxFiles;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnLoad;
  vfs::Directory directory;
  std::string fileExtension;
  std::string saveItemText;
  bool mayDelete;

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
  _d->mayDelete = false;

  _d->btnExit = findChildA<TexturedButton*>( "btnExit", true, this );
  _d->btnHelp = findChildA<TexturedButton*>( "btnHelp", true, this );
  _d->btnLoad = findChildA<PushButton*>( "btnLoad", true, this );
  if( _d->btnLoad )
  {
    _d->btnLoad->setEnabled( false );
  }
  CONNECT( _d->btnExit, onClicked(), this, LoadMapWindow::deleteLater );
  CONNECT( _d->btnLoad, onClicked(), _d.data(), Impl::emitSelectFile );

  _d->lbxFiles = new FileListBox( this, Rect( 10, 50, width() - 10, height() - 45 ), -1 );
  if( _d->lbxFiles )
  {
    _d->lbxFiles->setItemFont( Font::create( FONT_2_WHITE ) );
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::simple, 0xffffffff );
    _d->lbxFiles->setItemDefaultColor( ListBoxItem::hovered, 0xff000000 );
  }

  CONNECT( _d->lbxFiles, onItemSelected(), _d.data(), Impl::resolveItemSelected )
  CONNECT( _d->lbxFiles, onItemSelectedAgain(), _d.data(), Impl::resolveItemDblClick );
  _d->fillFiles();
}

LoadMapWindow::~LoadMapWindow(){}

void LoadMapWindow::Impl::fillFiles()
{
  if( !lbxFiles )
    return;

  vfs::Entries flist = vfs::Directory( directory ).getEntries();
  flist = flist.filter( vfs::Entries::file | vfs::Entries::extFilter, fileExtension );

  StringArray names;
  for( vfs::Entries::ConstItemIt it=flist.begin(); it != flist.end(); ++it )
  {
    names << (*it).absolutePath().toString();
  }

  lbxFiles->addItems( names );
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

void LoadMapWindow::setMayDelete(bool mayDelete) {  _d->mayDelete = mayDelete;}
bool LoadMapWindow::isMayDelete() const { return _d->mayDelete; }

Signal1<std::string>& LoadMapWindow::onSelectFile() {  return _d->onSelecteFileSignal; }

}//end namespace gui
