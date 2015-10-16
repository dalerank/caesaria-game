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

#include <cstdio>

#include "special_orders_window.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "label.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "gfx/decorator.hpp"
#include "core/event.hpp"
#include "core/priorities.hpp"
#include "good/helper.hpp"
#include "good/store.hpp"
#include "good/orders.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

class BaseSpecialOrdersWindow::Impl
{
public:
  Pictures bgPicture;
  GroupBox* gbOrders;
  Widget* gbOrdersInsideArea;
  Label* lbTitle;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnEmpty;
};

BaseSpecialOrdersWindow::BaseSpecialOrdersWindow( Widget* parent, const Point& pos, int h )
  : Window( parent, Rect( pos, Size( 510, h ) ), "" ), _d( new Impl )
{
  // create the title
  _d->lbTitle = new Label( this, Rect( 50, 10, width()-50, 10 + 30 ), "", true );
  _d->lbTitle->setFont( Font::create( FONT_5 ) );
  _d->lbTitle->setTextAlignment( align::center, align::center );

  _d->btnExit = new TexturedButton( this, Point( 472, height() - 39 ), Size( 24 ), -1, config::id.menu.exitInf );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );

  _d->btnHelp = new TexturedButton( this, Point( 14, height() - 39 ), Size( 24 ), -1, config::id.menu.helpInf );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  _d->gbOrders = new GroupBox( this, Rect( 17, 42, width() - 17, height() - 70), -1, GroupBox::blackFrame );
  _d->gbOrdersInsideArea = new Widget( _d->gbOrders, -1, Rect( 5, 5, _d->gbOrders->width() -5, _d->gbOrders->height() -5 ) );

  CONNECT( _d->btnExit, onClicked(), this, BaseSpecialOrdersWindow::deleteLater );
}

BaseSpecialOrdersWindow::~BaseSpecialOrdersWindow() {}

Widget*BaseSpecialOrdersWindow::_ordersArea()
{
  return _d->gbOrdersInsideArea;
}

void BaseSpecialOrdersWindow::draw(gfx::Engine& engine )
{
  Window::draw( engine );
}

bool BaseSpecialOrdersWindow::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return parent()->absoluteRect().isPointInside( point );
}

bool BaseSpecialOrdersWindow::onEvent( const NEvent& event)
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

void BaseSpecialOrdersWindow::setTitle( const std::string& text ){  _d->lbTitle->setText( text );}

}//end namespace gui
