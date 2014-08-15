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

#include "tutorial_window.hpp"
#include "core/saveadapter.hpp"
#include "listbox.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "events/playsound.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"

using namespace gfx;

namespace gui
{

TutorialWindow::TutorialWindow( Widget* p, vfs::Path tutorial )
  : Window( p, Rect( 0, 0, 590, 450 ), "" )
{
  _locker.activate();

  setupUI( ":/gui/tutorial_window.gui" );
  Size pSize = parent()->size() - size();
  setPosition( Point( pSize.width() / 2, pSize.height() / 2 ) );

  Label* lbTitle = findChildA<Label*>( "lbTitle", true, this );

  ListBox* lbx = findChildA<ListBox*>( "lbxHelp", true, this );
  TexturedButton* btn = findChildA<TexturedButton*>( "btnExit", true, this );
  CONNECT( btn, onClicked(), this, TutorialWindow::deleteLater );

  if( !lbx )
    return;

  VariantMap vm = SaveAdapter::load( tutorial );
  Logger::warningIf( vm.empty(), "Cannot load tutorial description from " + tutorial.toString() );

  StringArray items = vm.get( "items" ).toStringArray();
  std::string title = vm.get( "title" ).toString();
  std::string sound = vm.get( "sound" ).toString();

  if( lbTitle ) lbTitle->setText( _( title ) );
  if( !sound.empty() )
  {
    events::GameEventPtr e = events::PlaySound::create( sound, 100 );
    e->dispatch();
  }

  const std::string imgSeparator = "@img=";
  foreach( it, items )
  {
    std::string text = *it;
    if( text.substr( 0, imgSeparator.length() ) == imgSeparator )
    {
      Picture pic = Picture::load( text.substr( imgSeparator.length() ) );
      ListBoxItem& item = lbx->addItem( pic );
      item.setTextAlignment( align::center, align::upperLeft );
      int lineCount = pic.height() / lbx->itemHeight();
      StringArray lines;
      lines.resize( lineCount );
      lbx->addItems( lines );
    }
    else
    {
      lbx->fitText( _( text ) );
    }
  }
}

TutorialWindow::~TutorialWindow() {}

}//end namespace gui
