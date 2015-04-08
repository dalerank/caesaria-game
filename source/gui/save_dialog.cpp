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
#include "environment.hpp"
#include "widgetescapecloser.hpp"
#include "dialogbox.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

class SaveGame::Impl
{
public:
  GameAutoPause locker;
  PushButton* btnOk;
  PushButton* btnCancel;
  EditBox* edFilename;
  ListBox* lbxSaves;
  vfs::Directory directory;
  std::string extension;
  vfs::Path realFilename;

public:
  void findFiles();

signals public:
  Signal1<std::string> onFileSelectedSignal;
};

void SaveGame::Impl::findFiles()
{
  vfs::Entries flist = directory.getEntries();
  StringArray names;
  names << flist.filter( vfs::Entries::file | vfs::Entries::extFilter, extension );
  std::sort( names.begin(), names.end() );
  if( lbxSaves ) lbxSaves->addItems( names );
}

SaveGame::SaveGame(Ui *ui, vfs::Directory dir, std::string fileExt, int id )
: Window( ui->rootWidget(), Rect( 0, 0, 512, 384 ), "", id ), _d( new Impl )
{
  _d->locker.activate();
  Widget::setupUI( ":/gui/savefile.gui" );

  setCenter( parent()->center() );

  WidgetEscapeCloser::insertTo( this );
  
  GET_DWIDGET_FROM_UI( _d, edFilename )
  GET_DWIDGET_FROM_UI( _d, lbxSaves )
  GET_DWIDGET_FROM_UI( _d, btnOk )
  GET_DWIDGET_FROM_UI( _d, btnCancel )

  _d->directory = dir;
  _d->extension = fileExt;

  CONNECT( _d->lbxSaves,  onItemSelectedAgain(), this, SaveGame::_resolveDblListboxChange )
  CONNECT( _d->lbxSaves,  onItemSelected(),      this, SaveGame::_resolveListboxChange )
  CONNECT( _d->btnOk,     onClicked(),           this, SaveGame::_resolveOkClick )
  CONNECT( _d->btnCancel, onClicked(),           this, SaveGame::deleteLater )

  _d->findFiles();
  setModal();
}

void SaveGame::_resolveOkClick()
{
  _d->realFilename = vfs::Path( _d->edFilename->text() );
  if( _d->realFilename.extension().empty() )
    _d->realFilename = _d->realFilename + _d->extension;

  _d->realFilename = _d->directory/_d->realFilename;

  if( _d->realFilename.exist() )
  {
    Dialog* dialog = Confirmation( ui(),
                                   _("##warning##"),
                                   _("##save_already_exist##") );

    CONNECT( dialog, onOk(), this, SaveGame::_save )
  }
  else
  {
    _save();
  }
}

void SaveGame::_resolveListboxChange( const ListBoxItem& item )
{
  _d->edFilename->setText( vfs::Path( item.text() ).baseName( false ).toString() );
}

void SaveGame::_resolveDblListboxChange( const ListBoxItem& item )
{
  _d->edFilename->setText( vfs::Path( item.text() ).baseName( false ).toString() );
  _resolveOkClick();
}

void SaveGame::_save()
{
  emit _d->onFileSelectedSignal( _d->realFilename.toString() );
  deleteLater();
}

void SaveGame::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Signal1<std::string>& SaveGame::onFileSelected() {  return _d->onFileSelectedSignal; }

}//end namespace dialog

}//end namespace gui
