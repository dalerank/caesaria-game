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

#include "dictionary.hpp"
#include "pushbutton.hpp"
#include "core/stringhelper.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "core/saveadapter.hpp"
#include "dictionary_text.hpp"
#include "core/logger.hpp"
#include "objects/metadata.hpp"
#include "core/event.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

class DictionaryWindow::Impl
{
public:
  Label* lbTitle;
  DictionaryText* lbText;
  TexturedButton* btnExit;
};

DictionaryWindow::DictionaryWindow( Widget* parent )
  : Window( parent, Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  setupUI( ":/gui/dictionary.gui" );

  setPosition( Point( parent->width() - width(), parent->height() - height() ) / 2 );

  GET_DWIDGET_FROM_UI( _d, btnExit )
  _d->lbText = new DictionaryText( this, Rect( 20, 40, width() - 20, height() - 40 ) );

  CONNECT( _d->btnExit, onClicked(), this, DictionaryWindow::deleteLater );
}

void DictionaryWindow::show(Widget* parent, TileOverlay::Type type)
{
  DictionaryWindow* wnd = new DictionaryWindow( parent );
  if( wnd->_d->lbText )
  {
    wnd->_d->lbText->setText( MetaDataHolder::findDescription( type ) );
  }
}

void DictionaryWindow::show(Widget* parent, const std::string& uri)
{
  DictionaryWindow* wnd = new DictionaryWindow( parent );
  if( wnd->_d->lbText )
  {
    wnd->load( uri );
  }
}

DictionaryWindow::~DictionaryWindow( void ) {}

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

void DictionaryWindow::load(const std::string& uri)
{
  std::string ruri = ":/help/" + uri + ".en";
  VariantMap vm = SaveAdapter::load( ruri );

  std::string text = vm.get( "text" ).toString();

  _d->lbText->setText( text );
}

}//end namespace gui
