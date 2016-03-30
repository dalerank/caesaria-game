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
  Label* lbTitle;
  Widget* gbOrdersInsideArea;
  PushButton* btnEmpty;
};

BaseSpecialOrdersWindow::BaseSpecialOrdersWindow( Widget* parent, const Point& pos, int h )
  : Window( parent, Rect( pos, Size( 510, h ) ), "" ), _d( new Impl )
{
  // create the title
  auto& lbTitle = add<Label>( Rect( 50, 10, width()-50, 10 + 30 ), "", true );
  lbTitle.setFont( "FONT_5" );
  lbTitle.setTextAlignment( align::center, align::center );
  _d->lbTitle = &lbTitle;

  auto& btnExit = add<ExitButton>( Point( 472, height() - 39 ), Widget::noId );
  btnExit.setTooltipText( _("##infobox_tooltip_exit##") );

  auto& btnHelp = add<HelpButton>( Point( 14, height() - 39 ), "" );
  btnHelp.setTooltipText( _("##infobox_tooltip_help##") );

  auto& gbOrders = add<GroupBox>( Rect( 17, 42, width() - 17, height() - 70), -1, GroupBox::blackFrame );
  _d->gbOrdersInsideArea = &gbOrders.add<Widget>( -1, Rect( 5, 5, gbOrders.width()-5, gbOrders.height()-5 ) );
}

BaseSpecialOrdersWindow::~BaseSpecialOrdersWindow() {}

Widget* BaseSpecialOrdersWindow::_ordersArea()
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
    if( event.mouse.type == NEvent::Mouse::mouseRbtnRelease )
    {
      deleteLater();
      return true;
    }
    else if( event.mouse.type == NEvent::Mouse::mouseLbtnRelease )
    {
      return true;
    }
  break;

  default:
  break;
  }

  return Widget::onEvent( event );
}

void BaseSpecialOrdersWindow::setTitle( const std::string& text )
{
  if( _d->lbTitle )
    _d->lbTitle->setText( text );
}

}//end namespace gui
