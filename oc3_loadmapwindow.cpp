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

#include "oc3_loadmapwindow.hpp"
#include "gui/label.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_listbox.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_filesystem.hpp"
#include "oc3_filesystem_filelist.hpp"
#include "oc3_color.hpp"

class LoadMapWindow::Impl
{
public:
  PictureRef bgPicture;
  Label* lbTitle;
  ListBox* files;
  PushButton* btnExit;
  PushButton* btnHelp;
  io::FilePath directory;
  std::string fileExtension;

  void fillFiles();

  void resolveFileSelected( std::string fileName )
  {
    onSelecteFileSignal.emit( directory.toString() + fileName );
  }

oc3_signals public:
  Signal1<std::string> onSelecteFileSignal;
};

LoadMapWindow::LoadMapWindow( Widget* parent, const Rect& rect,
                              const io::FilePath& dir, const std::string& ext,
                              int id )
: Widget( parent, id, rect ), _d( new Impl )
{
  // create the title
  _d->lbTitle = new Label( this, Rect( 16, 10, getWidth()-16, 10 + 30 ), "", true );
  _d->lbTitle->setFont( Font::create( FONT_3 ) );
  _d->lbTitle->setTextAlignment( alignCenter, alignCenter );
  _d->directory = dir;
  _d->fileExtension = ext;

  _d->btnExit = new TexturedButton( this, Point( 472, getHeight() - 39), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnHelp = new TexturedButton( this, Point( 14, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  CONNECT( _d->btnExit, onClicked(), this, LoadMapWindow::deleteLater );

  _d->files = new ListBox( this, Rect( 10, _d->lbTitle->getBottom(), getWidth() - 10, _d->btnHelp->getTop() - 5 ), -1, true, true, false ); 
  _d->files->setItemFont( Font::create( FONT_2_WHITE ) );
  _d->files->setItemDefaultColor( ListBoxItem::LBC_TEXT, 0xffffffff );
  _d->files->setItemDefaultColor( ListBoxItem::LBC_TEXT_HIGHLIGHT, 0xff000000 );

  CONNECT( _d->files, onItemSelectedAgain(), _d.data(), Impl::resolveFileSelected );
  _d->fillFiles();

  _d->bgPicture.reset( Picture::create( getSize() ) );

  // draws the box and the inner black box
  PictureDecorator::draw( *_d->bgPicture, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );
}

LoadMapWindow::~LoadMapWindow()
{

}

void LoadMapWindow::Impl::fillFiles()
{
  io::FileList flist = io::FileDir( directory ).getEntries();
  StringArray names;
  names << flist.filter( io::FileList::file | io::FileList::extFilter, fileExtension );
  files->addItems( names );
}

void LoadMapWindow::draw( GfxEngine& engine )
{
  engine.drawPicture( getBgPicture(), getScreenLeft(), getScreenTop() );
  Widget::draw( engine );
}

Picture& LoadMapWindow::getBgPicture()
{
  return *_d->bgPicture;
}

bool LoadMapWindow::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return getParent()->getAbsoluteRect().isPointInside( point );
}

bool LoadMapWindow::onEvent( const NEvent& event)
{
  return Widget::onEvent( event );
}

void LoadMapWindow::setTitle( const std::string& title )
{
  _d->lbTitle->setText( title );
}

Signal1<std::string>& LoadMapWindow::onSelectFile()
{
  return _d->onSelecteFileSignal;
}
