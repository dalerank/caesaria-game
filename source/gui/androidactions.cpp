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

#include "androidactions.hpp"
#include "core/event.hpp"
#include "environment.hpp"
#include "texturedbutton.hpp"

using namespace gfx;

namespace gui
{

class AndroidActionsBar::Impl
{
public:
  TexturedButton* btnShowHelp;
  TexturedButton* btnExit;
  TexturedButton* btnMenu;
  TexturedButton* btnEnter;
};

AndroidActionsBar::AndroidActionsBar( Widget* parent)
  : Widget( parent, -1, Rect( parent->width() - 150, 480, parent->width(), parent->height() ) ), _d( new Impl )
{
  Picture pic = Picture::load( ResourceGroup::panelBackground, 657 );
  _d->btnMenu = new TexturedButton( this, Point( 0, 0 ), pic.size(), -1, 657 );

  pic = Picture::load( ResourceGroup::panelBackground, 651 );
  _d->btnShowHelp = new TexturedButton( this, _d->btnMenu->leftdown(), pic.size(), -1, 651 );

  pic = Picture::load( ResourceGroup::panelBackground, 661 );
  _d->btnEnter = new TexturedButton( this, _d->btnShowHelp->rightup(), pic.size(), -1, 661 );

  pic = Picture::load( ResourceGroup::panelBackground, 654 );
  _d->btnExit = new TexturedButton( this, _d->btnShowHelp->leftdown(), pic.size(), -1, 654 );
}

Signal0<>& AndroidActionsBar::onRequestTileHelp() { return _d->btnShowHelp->onClicked(); }
Signal0<>& AndroidActionsBar::onEscapeClicked() { return _d->btnExit->onClicked(); }
Signal0<>& AndroidActionsBar::onEnterClicked() { return _d->btnEnter->onClicked(); }
Signal0<>& AndroidActionsBar::onRequestMenu() { return _d->btnMenu->onClicked(); }

void AndroidActionsBar::beforeDraw(gfx::Engine& painter)
{
  if( parent()->children().back() != this )
  {
    bringToFront();
  }

  Widget::beforeDraw( painter );
}

}//end namespace gui
