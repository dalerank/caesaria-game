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

#include "oc3_window_video_options.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_game_settings.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_event.hpp"

class VideoOptionsWindow::Impl
{
public:
  Signal1<Size> onScreenSizeChangeSignal;
};

VideoOptionsWindow::VideoOptionsWindow( Widget* parent )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/videooptions.gui" ) );

  setPosition( Point( parent->getWidth() - getWidth(), parent->getHeight() - getHeight() ) / 2 );
}

VideoOptionsWindow::~VideoOptionsWindow( void )
{
}

bool VideoOptionsWindow::onEvent(const NEvent& event)
{
  if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
  {
    switch( event.GuiEvent.Caller->getID() )
    {
    case 1: break;
    case 1024: break;
    case 1280: break;
    case 9999: break;

    default:
      deleteLater();
    break;
    }

    return true;
  }

  return Widget::onEvent( event );
}

Signal1<Size>& VideoOptionsWindow::onSreenSizeChange()
{
  return _d->onScreenSizeChangeSignal;
}
