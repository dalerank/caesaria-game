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
#include "widget_helper.hpp"
#include "core/logger.hpp"

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
  : Window( parent, Rect( 0, 0, 150, 480 ), "", -1, bgNone ), _d( new Impl )
{
  setupUI( ":/gui/android_actions_bar.gui" );

  GET_DWIDGET_FROM_UI( _d, btnMenu )
  GET_DWIDGET_FROM_UI( _d, btnShowHelp )
  GET_DWIDGET_FROM_UI( _d, btnEnter )
  GET_DWIDGET_FROM_UI( _d, btnExit )
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

  Window::beforeDraw( painter );
}

}//end namespace gui
