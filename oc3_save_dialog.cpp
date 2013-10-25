// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_save_dialog.hpp"
#include "oc3_picture.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_listbox.hpp"
#include "oc3_gui_editbox.hpp"
#include "gui/label.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_filesystem.hpp"
#include "oc3_filesystem_filelist.hpp"

class SaveDialog::Impl
{
public:
  PictureRef background;
  PushButton* btnOk;
  PushButton* btnCancel;
  EditBox* edFilename;
  ListBox* lbxSaves;
  io::FilePath directory;
  std::string extension;

oc3_signals public:
  Signal1<std::string> onFileSelectedSignal;

public:
  void resolveButtonOkClick()
  {
    onFileSelectedSignal.emit( directory.toString() + edFilename->getText() + extension );
  }

  void resolveListboxChange( std::string text )
  {
    edFilename->setText( text );
  }

  void findFiles();
};

void SaveDialog::Impl::findFiles()
{
  io::FileList flist = io::FileDir( directory ).getEntries();
  StringArray names;
  names << flist.filter( io::FileList::file | io::FileList::extFilter, extension );
  lbxSaves->addItems( names );
}

SaveDialog::SaveDialog( Widget* parent, const io::FilePath& dir, const std::string& fileExt, int id )
: Widget( parent, id, Rect( 0, 0, 385, 336 ) ), _d( new Impl )
{
  setPosition( Point( (parent->getWidth() - getWidth())/2, (parent->getHeight() - getHeight()) / 2 ) );
  
  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 30) );
  title->setText( "Save city" );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  _d->edFilename = new EditBox( this, Rect( 18, 40, 18 + 320, 40 + 30 ), "Savecity" );
  _d->directory = io::FileDir::getApplicationDir().addEndSlash().toString() + dir.toString();
  _d->directory = _d->directory.addEndSlash();
  _d->extension = fileExt;

  _d->lbxSaves = new ListBox( this, Rect( 18, 70, 18 + 356, 70 + 205 ) );
  CONNECT( _d->lbxSaves, onItemSelectedAgain(), _d.data(), Impl::resolveListboxChange );
 
  new Label( this, Rect( 18, 296, getWidth() / 2, 297 + 30 ), "Continue?" );
  _d->btnOk = new TexturedButton( this, Point( 217, 297 ), Size( 39, 26), -1, ResourceMenu::okBtnPicId );
  CONNECT( _d->btnOk, onClicked(), _d.data(), Impl::resolveButtonOkClick );
  CONNECT( _d->btnOk, onClicked(), this, SaveDialog::deleteLater );

  _d->btnCancel = new TexturedButton( this, Point( 265, 297), Size( 39, 26 ), -1, ResourceMenu::cancelBtnPicId );
  CONNECT( _d->btnCancel, onClicked(), this, SaveDialog::deleteLater );

  _d->findFiles();
}

void SaveDialog::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}

Signal1<std::string>& SaveDialog::onFileSelected()
{
  return _d->onFileSelectedSignal;
}
