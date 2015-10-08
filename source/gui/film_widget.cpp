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

#include "film_widget.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "game/video_config.hpp"
#include "smkviewer.hpp"
#include "widget_helper.hpp"
#include "image.hpp"

namespace gui
{

class FilmWidget::Impl
{
public:
  GameAutoPause locker;
  Label* lbTitle;
  TexturedButton* btnExit;
  Label* lbTime;
  Label* lbReceiver;
  Label* lbMessage;
  SmkViewer* smkViewer;
  vfs::Path videoFile;

public signals:
  Signal0<> onCloseSignal;
};

FilmWidget::FilmWidget(Widget* parent, const std::string& movieName )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  _d->locker.activate();
  _d->lbMessage = 0;

  setupUI( ":/gui/filmwidget.gui" );
  setCenter( parent->center() );

  GET_DWIDGET_FROM_UI( _d, lbTitle )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, lbTime )
  GET_DWIDGET_FROM_UI( _d, lbReceiver )
  GET_DWIDGET_FROM_UI( _d, lbMessage )

  _d->videoFile = movie::Config::instance().realPath( movieName ); //"/smk/Emmigrate.smk"
  if( _d->videoFile.exist() )
  {
    _d->smkViewer = new SmkViewer( this, Rect( 12, 12, width() - 12, 12 + 292 ) );
    _d->smkViewer->setFilename( _d->videoFile );
  }
  else
  {
    gfx::Picture pic( movieName, 1 );
    if( !pic.isValid() )
      pic.load( "freska", 1 );

    new Image( this, Rect( 12, 12, width() - 12, 12 + 292 ), pic, Image::fit );
  }

  CONNECT( _d->btnExit, onClicked(), &_d->onCloseSignal, Signal0<>::_emit );
  CONNECT( _d->btnExit, onClicked(), this, FilmWidget::deleteLater );

  setModal();
}

FilmWidget::~FilmWidget( void ) {}

void FilmWidget::setText(const std::string& text)
{
  if( _d->lbMessage ) _d->lbMessage->setText( text );
}

void FilmWidget::setReceiver(std::string text)
{
  if( _d->lbReceiver ) _d->lbReceiver->setText( text );
}

void FilmWidget::setTitle(std::string text)
{
  if( _d->lbTitle ) _d->lbTitle->setText( text );
}

void FilmWidget::setTime(DateTime time)
{
  if( _d->lbTime ) _d->lbTime->setText( utils::format( 0xff, "%s %d %s",
                                                       DateTime::monthName( time.month() ),
                                                       time.year(),
                                                       time.year() < 0 ? "BC" : "AD" ) );
}

Signal0<>& FilmWidget::onClose() {  return _d->onCloseSignal; }


}//end namespace gui
