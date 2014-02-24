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

#include "window_gamespeed_options.hpp"
#include "gfx/engine.hpp"
#include "game/settings.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "label.hpp"
#include "listbox.hpp"
#include "core/stringhelper.hpp"
#include "gameautopause.hpp"

namespace gui
{

class GameSpeedOptionsWindow::Impl
{
public:
  GameAutoPause locker;
  Signal1<int> onGameSpeedChangeSignal;
  Signal1<int> onScrollSpeedChangeSignal;
  int speedValue, scrollValue;
};

GameSpeedOptionsWindow::GameSpeedOptionsWindow(Widget* parent, int gameSpeed, int scrollSpeed  )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->speedValue = gameSpeed;
  _d->scrollValue = scrollSpeed;
  _d->locker.activate();

  setupUI( GameSettings::rcpath( "/gui/speedoptions.gui" ) );

  setPosition( Point( parent->getWidth() - getWidth(), parent->getHeight() - getHeight() ) / 2 );

  _update();
}

GameSpeedOptionsWindow::~GameSpeedOptionsWindow( void )
{
}

bool GameSpeedOptionsWindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && guiButtonClicked == event.gui.type )
  {
    int id = event.gui.caller->getID();
    switch( id )
    {
    case 1: case 2: _d->speedValue += (id == 1 ? -10 : +10 ); _update(); break;
    case 11: case 12: _d->scrollValue+= (id == 11 ? -10 : +10 ); _update(); break;

    case 1001:
    {
      _d->onGameSpeedChangeSignal.emit( _d->speedValue );
      _d->onScrollSpeedChangeSignal.emit( _d->scrollValue );
      deleteLater();
    }
    break;

    case 1002:
      deleteLater();
    break;
    }
  }

  return Widget::onEvent( event );
}

Signal1<int>& GameSpeedOptionsWindow::onGameSpeedChange()
{
  return _d->onGameSpeedChangeSignal;
}

Signal1<int>& GameSpeedOptionsWindow::onScrollSpeedChange()
{
  return _d->onScrollSpeedChangeSignal;
}

void GameSpeedOptionsWindow::_update()
{
  Label* lbSpeed = findChildA<Label*>( "lbGameSpeedPercent", true, this );
  Label* lbScroll = findChildA<Label*>( "lbScrollSpeedPercent", true, this );

  if( lbSpeed ) { lbSpeed->setText( StringHelper::format( 0xff, "%d%%", _d->speedValue ) ); }
  if( lbScroll ) { lbScroll->setText( StringHelper::format( 0xff, "%d%%", _d->scrollValue ) ); }
}

}//end namespace gui
