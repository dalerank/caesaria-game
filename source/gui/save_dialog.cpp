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
#include "widget_factory.hpp"

using namespace gfx;

namespace gui
{

namespace dialog
{

REGISTER_CLASS_IN_WIDGETFACTORY(SaveGame)

class SaveGame::Impl
{
public:
  EditBox* edFilename;
  ListBox* lbxSaves;
  vfs::Directory directory;
  std::string extension;
  vfs::Path realFilename;

public:
  void findFiles();

signals public:
  Signal1<std::string> onFileSelectedSignal;
  Signal2<Widget*,std::string> onFileSelectedSignalEx;
};

void SaveGame::Impl::findFiles()
{
  vfs::Entries flist = directory.entries();
  StringArray names;
  names << flist.filter( vfs::Entries::file | vfs::Entries::extFilter, extension );
  std::sort( names.begin(), names.end() );

  if( lbxSaves )
  {
    lbxSaves->clear();
    lbxSaves->addLines(names);
  }
}

SaveGame::SaveGame(Widget* parent)
  : SaveGame(parent->ui(), vfs::Directory(), ".oc3save", -1)
{

}

SaveGame::SaveGame(Ui *ui, vfs::Directory dir, std::string fileExt, int id )
  : Window( ui->rootWidget(), Rect( 0, 0, 512, 384 ), "", id ), _d( new Impl )
{
  Widget::setupUI( ":/gui/savefile.gui" );

  GET_DWIDGET_FROM_UI( _d, edFilename )
  GET_DWIDGET_FROM_UI( _d, lbxSaves )

  _d->directory = dir;
  _d->extension = fileExt;

  CONNECT_LOCAL( _d->lbxSaves,  onItemSelectedAgain(), SaveGame::_resolveDblListboxChange )
  CONNECT_LOCAL( _d->lbxSaves,  onItemSelected(),      SaveGame::_resolveListboxChange )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnOk,  onClicked(), SaveGame::_resolveOkClick )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnCancel, onClicked(), SaveGame::deleteLater )

  _d->findFiles();

  moveToCenter();
  WidgetClosers::insertTo( this, KEY_RBUTTON );
  GameAutoPauseWidget::insertTo( this );
  setModal();
}

void SaveGame::setDirectory(const std::string& dir)
{
  _d->directory = dir;
  _d->findFiles();
}

void SaveGame::setFilter(const std::string& filter)
{
  _d->extension = filter;
  _d->findFiles();
}

void SaveGame::_resolveOkClick()
{
  _d->realFilename = _d->edFilename->text();

  if( !_d->realFilename.haveExtension() )
    _d->realFilename = _d->realFilename + _d->extension;

  _d->realFilename = _d->directory/_d->realFilename;

  if( _d->realFilename.exist() )
  {
    Confirmation( ui(),
                  _("##warning##"),
                  _("##save_already_exist##"),
                  makeDelegate( this, &SaveGame::_save ) );
  }
  else
  {
    _save();
  }
}

void SaveGame::_resolveListboxChange( const ListBoxItem& item )
{
  vfs::Path path = item.text();
  _d->edFilename->setText( path.baseName().removeExtension() );
}

void SaveGame::_resolveDblListboxChange( const ListBoxItem& item )
{
  vfs::Path path = item.text();
  _d->edFilename->setText(path.baseName().removeExtension());
  _resolveOkClick();
}

void SaveGame::_save()
{
  emit _d->onFileSelectedSignal(_d->realFilename.toString());
  emit _d->onFileSelectedSignalEx( this, _d->realFilename.toString());
  deleteLater();
}

void SaveGame::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Signal1<std::string>& SaveGame::onFileSelected() {  return _d->onFileSelectedSignal; }
Signal2<Widget*, std::string>&SaveGame::onFileSelectedEx() { return _d->onFileSelectedSignalEx; }

}//end namespace dialog

}//end namespace gui
