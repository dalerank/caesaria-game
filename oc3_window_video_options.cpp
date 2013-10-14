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
#include "oc3_listbox.hpp"
#include "oc3_stringhelper.hpp"

class VideoOptionsWindow::Impl
{
public:
  Signal1<Size> onScreenSizeChangeSignal;
  Signal1<bool> onFullScreeChangeSignal;
  bool fullScreen;
};

VideoOptionsWindow::VideoOptionsWindow(Widget* parent, const std::vector<Size>& modes, bool fullscreen )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/videooptions.gui" ) );

  setPosition( Point( parent->getWidth() - getWidth(), parent->getHeight() - getHeight() ) / 2 );

  _d->fullScreen = fullscreen;
  if( ListBox* lbxModes = findChild<ListBox*>( "lbxModes", true ) )
  {
    std::string modeStr;
    for( std::vector<Size>::const_iterator mode=modes.begin(); mode != modes.end(); mode++ )
    {
      modeStr = StringHelper::format( 0xff, "%dx%d", (*mode).getWidth(), (*mode).getHeight() );
      ListBoxItem& item = lbxModes->addItem( modeStr );
      item.setTag( ((*mode).getWidth() << 16) + (*mode).getHeight());
    }
  }
}

VideoOptionsWindow::~VideoOptionsWindow( void )
{
}

bool VideoOptionsWindow::onEvent(const NEvent& event)
{
  if( event.EventType == OC3_GUI_EVENT )

  switch( event.GuiEvent.EventType )
  {
  case OC3_BUTTON_CLICKED:
  {
    switch( event.GuiEvent.Caller->getID() )
    {
    case 1:
    {
      _d->fullScreen = _d->fullScreen;
      _d->onFullScreeChangeSignal( _d->fullScreen );
      _update();
    }
    break;

    default:
      deleteLater();
    break;
    }

    return true;
  }
  break;

  case OC3_LISTBOX_CHANGED:
  {
    ListBox* lbx = safety_cast< ListBox* >( event.GuiEvent.Caller );

    int tag = lbx->getSelectedItem().getTag();

    _d->onScreenSizeChangeSignal.emit( Size( (tag>>16) & 0xffff, tag & 0xffff ) );
  }
  break;

  default: break;
  }

  return Widget::onEvent( event );
}

Signal1<Size>& VideoOptionsWindow::onSreenSizeChange()
{
  return _d->onScreenSizeChangeSignal;
}

Signal1<bool>&VideoOptionsWindow::onFullScreenChange()
{
  return _d->onFullScreeChangeSignal;
}

void VideoOptionsWindow::_update()
{

}
