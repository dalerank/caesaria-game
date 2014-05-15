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
#include "listbox.hpp"
#include "editbox.hpp"
#include "gui/label.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "vfs/filesystem.hpp"
#include "vfs/entries.hpp"
#include "core/logger.hpp"
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
  PictureRef background;
  PushButton* btnOk;
  PushButton* btnCancel;
  EditBox* edFilename;
  ListBox* lbxSaves;
  vfs::Directory directory;
  std::string extension;

oc3_signals public:
  Signal1<std::string> onFileSelectedSignal;

public:
  void resolveButtonOkClick()
  {
    vfs::Path filename( edFilename->text() + extension );
    onFileSelectedSignal.emit( (directory/filename).toString() );
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
  lbxSaves->addItems( names );
}

SaveDialog::SaveDialog(Widget* parent, vfs::Directory dir, std::string fileExt, int id )
: Widget( parent, id, Rect( 0, 0, 385, 336 ) ), _d( new Impl )
{
  _d->locker.activate();
  setCenter( parent->center() );

  WidgetEscapeCloser::insertTo( this );
  
  Label* title = new Label( this, Rect( 10, 10, width() - 10, 10 + 30) );
  title->setText( _("##save_city##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( align::center, align::center );

  _d->background.reset( Picture::create( size() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );

  _d->edFilename = new EditBox( this, Rect( 18, 40, 18 + 320, 40 + 30 ), "Savecity" );
  _d->directory = dir;
  _d->extension = fileExt;

  _d->lbxSaves = new ListBox( this, Rect( 18, 70, 18 + 356, 70 + 205 ) );
 
  new Label( this, Rect( 18, 296, width() / 2, 297 + 30 ), "Continue?" );
  _d->btnOk = new TexturedButton( this, Point( 217, 297 ), Size( 39, 26), -1, ResourceMenu::okBtnPicId );

  _d->btnCancel = new TexturedButton( this, Point( 265, 297), Size( 39, 26 ), -1, ResourceMenu::cancelBtnPicId );

  CONNECT( _d->lbxSaves, onItemSelectedAgain(), _d.data(), Impl::resolveListboxChange );
  CONNECT( _d->btnOk, onClicked(), _d.data(), Impl::resolveButtonOkClick );
  CONNECT( _d->btnOk, onClicked(), this, SaveDialog::deleteLater );
  CONNECT( _d->btnCancel, onClicked(), this, SaveDialog::deleteLater );

  _d->findFiles();
}

void SaveDialog::draw(gfx::Engine& painter )
{
  if( !isVisible() )
    return;

  painter.draw( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}

Signal1<std::string>& SaveDialog::onFileSelected() {  return _d->onFileSelectedSignal; }

}//end namespace gui
