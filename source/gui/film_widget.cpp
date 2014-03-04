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

#include "film_widget.hpp"
#include "game/settings.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "core/logger.hpp"

namespace gui
{

class FilmWidget::Impl
{
public:
  Label* lbTitle;
  TexturedButton* btnExit;
  Label* lbTime;
  Label* lbReceiver;
  Label* lbMessage;

public oc3_signals:
  Signal0<> onCloseSignal;
};

FilmWidget::FilmWidget(Widget* parent, vfs::Path film )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->lbMessage = 0;

  setupUI( GameSettings::rcpath( "/gui/filmwidget.gui" ) );

  setPosition( Point( parent->width() - width(), parent->getHeight() - getHeight() ) / 2 );

  _d->lbTitle = findChildA<Label*>( "lbTitle", true, this );
  _d->btnExit = findChildA<TexturedButton*>( "btnExit", true, this );
  _d->lbTime = findChildA<Label*>( "lbTime", true, this );
  _d->lbReceiver = findChildA<Label*>( "lbReceiver", true, this );
  _d->lbMessage = findChildA<Label*>( "lbMessage", true, this );

  CONNECT( _d->btnExit, onClicked(), &_d->onCloseSignal, Signal0<>::emit );
  CONNECT( _d->btnExit, onClicked(), this, FilmWidget::deleteLater );
}

FilmWidget::~FilmWidget( void )
{
}

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
  if( _d->lbTime ) _d->lbTime->setText( StringHelper::format( 0xff, "%s %d %s",
                                                              DateTime::getMonthName( time.month() ),
                                                              time.year(),
                                                              time.year() < 0 ? "BC" : "AD" ) );
}

Signal0<>& FilmWidget::onClose()
{
  return _d->onCloseSignal;
}


}//end namespace gui
