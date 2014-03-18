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

#include "video_options_window.hpp"
#include "game/settings.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "listbox.hpp"
#include "core/stringhelper.hpp"
#include "dialogbox.hpp"
#include "core/gettext.hpp"
#include "environment.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"

namespace gui
{

class VideoOptionsWindow::Impl
{
public:
  GameAutoPause locker;
  Signal1<Size> onScreenSizeChangeSignal;
  Signal1<bool> onFullScreeChangeSignal;
  Signal0<> onCloseSignal;
  PushButton* btnSwitchMode;
  bool fullScreen;
  bool haveChanges;
};

VideoOptionsWindow::VideoOptionsWindow(Widget* parent, GfxEngine::Modes modes, bool fullscreen )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  _d->locker.activate();
  setupUI( GameSettings::rcpath( "/gui/videooptions.gui" ) );

  setPosition( Point( parent->width() - width(), parent->height() - height() ) / 2 );
  _d->btnSwitchMode = findChildA<PushButton*>( "btnSwitchMode", true, this );

  _d->fullScreen = fullscreen;
  _d->haveChanges = false;
  if( ListBox* lbxModes = findChildA<ListBox*>( "lbxModes", true, this ) )
  {
    std::string modeStr;
    foreach( mode, modes )
    {
      modeStr = StringHelper::format( 0xff, "%dx%d", mode->width(), mode->height() );
      ListBoxItem& item = lbxModes->addItem( modeStr );
      item.setTag( (mode->width() << 16) + mode->height());
    }
  }

  _update();
}

VideoOptionsWindow::~VideoOptionsWindow( void ){}

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
      _d->fullScreen = !_d->fullScreen;
      _d->onFullScreeChangeSignal( _d->fullScreen );
      _d->haveChanges = true;
      _update();
    }
    break;

    default:
      if( _d->haveChanges )
      {
        Widget* parent = getEnvironment()->rootWidget();
        DialogBox* dlg = new DialogBox( parent, Rect(), "",
                                        _("##need_restart_for_apply_changes##"), DialogBox::btnOk );
        CONNECT( dlg, onOk(), dlg, DialogBox::deleteLater );
      }
      deleteLater();
    break;
    }

    return true;
  }
  break;

  case guiListboxChanged:
  {
    _d->haveChanges = true;
    ListBox* lbx = safety_cast< ListBox* >( event.gui.caller );

    int tag = lbx->getSelectedItem().getTag();

    _d->onScreenSizeChangeSignal.emit( Size( (tag>>16) & 0xffff, tag & 0xffff ) );
  }
  break;

  default: break;
  }

  return Widget::onEvent( event );
}

Signal1<Size>& VideoOptionsWindow::onSreenSizeChange() {  return _d->onScreenSizeChangeSignal; }
Signal1<bool>&VideoOptionsWindow::onFullScreenChange(){  return _d->onFullScreeChangeSignal; }
Signal0<>&VideoOptionsWindow::onClose(){  return _d->onCloseSignal; }

void VideoOptionsWindow::_update()
{
  if( _d->btnSwitchMode )
  {
    _d->btnSwitchMode->setText( _d->fullScreen ? _("##fullscreen_on##") : _("##fullscreen_off##") );
  }
}

}//end namespace gui
