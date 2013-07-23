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
#include "oc3_editbox.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_filesystem.hpp"
#include "oc3_filelist.hpp"

class SaveDialog::Impl
{
public:
  PictureRef background;
  PushButton* btnOk;
  PushButton* btnCancel;
  EditBox* edFilename;
  ListBox* lbxSaves;
  std::string directory;
  std::string extension;

oc3_signals public:
  Signal1<std::string> onFileSelectedSignal;

public:
  void resolveButtonOkClick()
  {
    onFileSelectedSignal.emit( directory + edFilename->getText() + extension );
  }

  void resolveListboxChange( std::string text )
  {
    edFilename->setText( text );
  }

  void findFiles();
};

void SaveDialog::Impl::findFiles()
{
  io::FileList::Items files = io::FileDir( directory ).getEntries().getItems();

  for( io::FileList::ItemIterator it=files.begin(); it !=files.end(); ++it)
  {
    if( !(*it).isDirectory && (*it).fullName.getExtension() == extension )
    {
      lbxSaves->addItem( (*it).fullName.getBasename().toString(), Font(), 0 );
    }
  }
}

SaveDialog::SaveDialog( Widget* parent, const std::string& dir, const std::string& fileExt, int id ) 
: Widget( parent, id, Rect( 0, 0, 385, 336 ) ), _d( new Impl )
{
  setPosition( Point( (parent->getWidth() - getWidth())/2, (parent->getHeight() - getHeight()) / 2 ) );
  
  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 30) );
  title->setText( "Save city" );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  //main _d->_d->background
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );

  _d->edFilename = new EditBox( this, Rect( 18, 40, 18 + 320, 40 + 30 ), "Savecity" );
  _d->directory = dir;
  _d->extension = fileExt;

  _d->lbxSaves = new ListBox( this, Rect( 18, 70, 18 + 356, 70 + 205 ) );
  CONNECT( _d->lbxSaves, onItemSelectedAgain(), _d.data(), Impl::resolveListboxChange );
 
  new Label( this, Rect( 18, 296, getWidth() / 2, 297 + 30 ), "Continue?" );
  _d->btnOk = new TexturedButton( this, Point( 217, 297 ), Size( 39, 26), -1, 239 );
  CONNECT( _d->btnOk, onClicked(), _d.data(), Impl::resolveButtonOkClick );
  CONNECT( _d->btnOk, onClicked(), this, SaveDialog::deleteLater );

  _d->btnCancel = new TexturedButton( this, Point( 265, 297), Size( 39, 26 ), -1, 243 );
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