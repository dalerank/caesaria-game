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

#include "dictionary.hpp"
#include "game/settings.hpp"
#include "pushbutton.hpp"
#include "core/stringhelper.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "core/logger.hpp"
#include "objects/metadata.hpp"
#include "core/event.hpp"

using namespace gfx;

namespace gui
{

class DictionaryWindow::Impl
{
public:
  Label* lbTitle;
  Label* lbText;
  TexturedButton* btnExit;
};

DictionaryWindow::DictionaryWindow( Widget* parent )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/dictionary.gui" ) );

  setPosition( Point( parent->width() - width(), parent->height() - height() ) / 2 );

  _d->btnExit = findChildA<TexturedButton*>( "btnExit", true, this );
  _d->lbText = findChildA<Label*>( "lbText", true, this );

  CONNECT( _d->btnExit, onClicked(), this, DictionaryWindow::deleteLater );
}

void DictionaryWindow::show(Widget* parent, TileOverlay::Type type)
{
  DictionaryWindow* wnd = new DictionaryWindow( parent );
  if( wnd->_d->lbText )
  {
    wnd->_d->lbText->setText( MetaDataHolder::getDescription( type ) );
  }
}

DictionaryWindow::~DictionaryWindow( void )
{
}

bool DictionaryWindow::onEvent(const NEvent& event)
{
  switch( event.EventType )
  {
  case sEventMouse:
    if( event.mouse.type == mouseRbtnRelease )
    {
      deleteLater();
      return true;
    }
    else if( event.mouse.type == mouseLbtnRelease )
    {
      return true;
    }
  break;

  default:
  break;
  }

  return Widget::onEvent( event );
}

}//end namespace gui
