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
#include "core/utils.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"
#include "core/variant_map.hpp"
#include "core/saveadapter.hpp"
#include "dictionary_text.hpp"
#include "core/logger.hpp"
#include "objects/metadata.hpp"
#include "core/event.hpp"
#include "widget_helper.hpp"
#include "core/gettext.hpp"
#include "environment.hpp"

using namespace gfx;

namespace gui
{

static const char* defaultExt = "en";

class DictionaryWindow::Impl
{
public:
  Label* lbTitle;
  DictionaryText* lbText;
  TexturedButton* btnExit;

  typedef std::map<std::string,std::string> Aliases;
  Aliases aliases;
};

DictionaryWindow::DictionaryWindow( Widget* p )
  : Window( p->ui()->rootWidget(), Rect( 0, 0, 1, 1 ), "" ), _d( new Impl )
{
  setupUI( ":/gui/dictionary.gui" );

  setPosition( Point( parent()->width() - width(), parent()->height() - height() ) / 2 );

  GET_DWIDGET_FROM_UI( _d, lbTitle )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  _d->lbText = new DictionaryText( this, Rect( 20, 40, width() - 20, height() - 40 ) );
  _d->lbText->setFont( Font::create( FONT_1 ) );

  CONNECT( _d->btnExit, onClicked(), this, DictionaryWindow::deleteLater )
  CONNECT( _d->lbText, onWordClick(), this, DictionaryWindow::_handleUriChange )
}

void DictionaryWindow::_handleUriChange(std::string value)
{
  Impl::Aliases::iterator it = _d->aliases.find( value );

  value = (it != _d->aliases.end()
            ? it->second
            : "table_content");

  load( value );
}

vfs::Path DictionaryWindow::_convUri2path(std::string uri)
{
  vfs::Path fpath = ":/help/" + uri + "." + Locale::current();

  if( !fpath.exist() )
    fpath = fpath.changeExtension( defaultExt );

  return fpath;
}

void DictionaryWindow::show(Widget* parent, TileOverlay::Type type)
{
  DictionaryWindow* wnd = new DictionaryWindow( parent );
  if( wnd->_d->lbText )
  {
    wnd->load( MetaDataHolder::findTypename( type ) );
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
  vfs::Path filePath = _convUri2path( uri );

  VariantMap vm = config::load( filePath );

  std::string text = vm.get( "text" ).toString();
  std::string title = vm.get( "title" ).toString();

  _d->aliases.clear();
  VariantMap uris = vm.get( "uri" ).toMap();
  foreach( it, uris )
  {
    _d->aliases[ it->first ] = it->second.toString();
  }

  _d->lbTitle->setText( _(title) );
  _d->lbText->setText( text );
}

}//end namespace gui
