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

#include "save_dialog.hpp"
#include "gfx/picture.hpp"
#include "pushbutton.hpp"
#include "filelistbox.hpp"
#include "editbox.hpp"
#include "gui/label.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "vfs/filesystem.hpp"
#include "vfs/entries.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"
#include "vfs/directory.hpp"
#include "gameautopause.hpp"
#include "core/gettext.hpp"
#include "widgetescapecloser.hpp"

using namespace gfx;

namespace gui
{

class SaveDialog::Impl
{
public:
  GameAutoPause locker;
  PushButton* btnOk;
  PushButton* btnCancel;
  EditBox* edFilename;
  ListBox* lbxSaves;
  vfs::Directory directory;
  std::string extension;

signals public:
  Signal1<std::string> onFileSelectedSignal;

public:
  void resolveButtonOkClick()
  {
    vfs::Path filename( edFilename->text() );
    if( filename.extension().empty() )
      filename = filename + extension;

    emit onFileSelectedSignal( (directory/filename).toString() );
  }

  void resolveListboxChange( std::string text )
  {
    edFilename->setText( text );
  }

  void findFiles();
};

void SaveDialog::Impl::findFiles()
{
  vfs::Entries flist = directory.getEntries();
  StringArray names;
  names << flist.filter( vfs::Entries::file | vfs::Entries::extFilter, extension );
  std::sort( names.begin(), names.end() );
  if( lbxSaves ) lbxSaves->addItems( names );
}

SaveDialog::SaveDialog(Widget* parent, vfs::Directory dir, std::string fileExt, int id )
: Window( parent, Rect( 0, 0, 512, 384 ), "", id ), _d( new Impl )
{
  _d->locker.activate();
  Widget::setupUI( ":/gui/savefile.gui" );

  setCenter( parent->center() );

  WidgetEscapeCloser::insertTo( this );
  
  GET_DWIDGET_FROM_UI( _d, edFilename )
  GET_DWIDGET_FROM_UI( _d, lbxSaves )
  GET_DWIDGET_FROM_UI( _d, btnOk )
  GET_DWIDGET_FROM_UI( _d, btnCancel )

  _d->directory = dir;
  _d->extension = fileExt;

  CONNECT( _d->lbxSaves, onItemSelectedAgain(), _d.data(), Impl::resolveListboxChange );
  CONNECT( _d->btnOk, onClicked(), _d.data(), Impl::resolveButtonOkClick );
  CONNECT( _d->btnOk, onClicked(), this, SaveDialog::deleteLater );
  CONNECT( _d->btnCancel, onClicked(), this, SaveDialog::deleteLater );

  _d->findFiles();
}

void SaveDialog::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Signal1<std::string>& SaveDialog::onFileSelected() {  return _d->onFileSelectedSignal; }

}//end namespace gui
