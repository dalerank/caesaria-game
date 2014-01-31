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

#include <cstdio>

#include "tutorial_window.hpp"
#include "core/saveadapter.hpp"
#include "listbox.hpp"
#include "core/foreach.hpp"

namespace gui
{

TutorialWindow::TutorialWindow( Widget* parent, vfs::Path tutorial )
  : Widget( parent, -1, Rect( 0, 0, 590, 450 ))
{
  setupUI( "/gui/tutorial_window.gui" );

  ListBox* lbx = findChild<ListBox*>( "lbxHelp", true );

  if( !lbx )
    return;

  VariantMap vm = SaveAdapter::load( tutorial );
  VariantList items = vm.get( "items" ).toList();

  foreach( it, items )
  {
    lbx->addItem( it->toString() );
  }
}

TutorialWindow::~TutorialWindow()
{
}

}//end namespace gui
