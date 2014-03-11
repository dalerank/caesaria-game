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

#include "tutorial_window.hpp"
#include "core/saveadapter.hpp"
#include "listbox.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "game/settings.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "texturedbutton.hpp"
#include "label.hpp"

namespace gui
{

TutorialWindow::TutorialWindow( Widget* parent, vfs::Path tutorial )
  : Widget( parent, -1, Rect( 0, 0, 590, 450 ))
{
  _locker.activate();

  setupUI( GameSettings::rcpath( "/gui/tutorial_window.gui" ) );
  Size pSize = getParent()->size() - size();
  setPosition( Point( pSize.width() / 2, pSize.height() / 2 ) );

  Label* lbTitle = findChildA<Label*>( "lbTitle", true );

  ListBox* lbx = findChildA<ListBox*>( "lbxHelp", true, this );
  TexturedButton* btn = findChildA<TexturedButton*>( "btnExit", true, this );
  CONNECT( btn, onClicked(), this, TutorialWindow::deleteLater );

  if( !lbx )
    return;

  VariantMap vm = SaveAdapter::load( GameSettings::rcpath( tutorial.toString() ) );
  Logger::warningIf( vm.empty(), "Cannot load tutorial description from " + tutorial.toString() );

  StringArray items = vm.get( "items" ).toStringArray();
  std::string title = vm.get( "title" ).toString();
  if( lbTitle ) lbTitle->setText( _( title ) );

  const std::string imgSeparator = "@img=";
  foreach( it, items )
  {
    std::string text = *it;
    if( text.substr( 0, imgSeparator.length() ) == imgSeparator )
    {
      Picture pic = Picture::load( text.substr( imgSeparator.length() ) );
      ListBoxItem& item = lbx->addItem( pic );
      item.setItemTextAlignment( alignCenter, alignUpperLeft );
      int lineCount = pic.height() / lbx->getItemHeight();
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
