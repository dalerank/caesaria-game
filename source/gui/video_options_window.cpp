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

#include "video_options_window.hpp"
#include "game/settings.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "listbox.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"

namespace gui
{

class VideoOptionsWindow::Impl
{
public:
  Signal1<Size> onScreenSizeChangeSignal;
  Signal1<bool> onFullScreeChangeSignal;
  bool fullScreen;
};

VideoOptionsWindow::VideoOptionsWindow(Widget* parent, GfxEngine::Modes modes, bool fullscreen )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/videooptions.gui" ) );

  setPosition( Point( parent->getWidth() - getWidth(), parent->getHeight() - getHeight() ) / 2 );

  _d->fullScreen = fullscreen;
  if( ListBox* lbxModes = findChild<ListBox*>( "lbxModes", true ) )
  {
    std::string modeStr;
    foreach( GfxEngine::Mode mode, modes )
    {
      modeStr = StringHelper::format( 0xff, "%dx%d", mode.getWidth(), mode.getHeight() );
      ListBoxItem& item = lbxModes->addItem( modeStr );
      item.setTag( (mode.getWidth() << 16) + mode.getHeight());
    }
  }
}

VideoOptionsWindow::~VideoOptionsWindow( void )
{
}

bool VideoOptionsWindow::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui )

  switch( event.gui.type )
  {
  case guiButtonClicked:
  {
    switch( event.gui.caller->getID() )
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

  case guiListboxChanged:
  {
    ListBox* lbx = safety_cast< ListBox* >( event.gui.caller );

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


}//end namespace gui
