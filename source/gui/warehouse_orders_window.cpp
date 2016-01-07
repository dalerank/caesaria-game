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

#include "warehouse_orders_window.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "order_good_widget.hpp"
#include "good/helper.hpp"
#include "widget_helper.hpp"
#include "core/priorities.hpp"
#include "core/logger.hpp"
#include "good/orders.hpp"
#include "objects/warehouse.hpp"
#include <set>

using namespace gfx;

namespace gui
{

class WarehouseSpecialOrdersWindow::Impl
{
public:
  WarehousePtr warehouse;
};

WarehouseSpecialOrdersWindow::WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse )
: BaseSpecialOrdersWindow( parent, pos, defaultHeight ), __INIT_IMPL(WarehouseSpecialOrdersWindow)
{
  __D_REF(d, WarehouseSpecialOrdersWindow)

  setupUI( ":/gui/warehousespecial.gui");
  setTitle( _("##warehouse_orders##") );

  good::Products goods =  good::all();
  goods.exclude( good::none )
       .exclude( good::denaries );

  d.warehouse = warehouse;

  if( warehouse->isTradeCenter() )
  {
    setTitle( "##trade_center##" );
  }

  int index=0;
  for( auto& goodType : goods )
  {
    const good::Orders::Order rule = d.warehouse->store().getOrder( goodType );

    if( rule != good::Orders::none )
    {
      new OrderGoodWidget( _ordersArea(), index, goodType, d.warehouse->store() );
      index++;
    }
  }

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnToggleDevastation, onClicked(), WarehouseSpecialOrdersWindow::_toggleDevastation )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnTradeCenter,       onClicked(), WarehouseSpecialOrdersWindow::_toggleTradeCenter )

  _update();
}

WarehouseSpecialOrdersWindow::~WarehouseSpecialOrdersWindow() {}

void WarehouseSpecialOrdersWindow::_toggleTradeCenter()
{
  __D_REF(d, WarehouseSpecialOrdersWindow)
  d.warehouse->setTradeCenter( !d.warehouse->isTradeCenter() );
  _update();
}

void WarehouseSpecialOrdersWindow::_toggleDevastation()
{
  __D_REF(d, WarehouseSpecialOrdersWindow)
  d.warehouse->store().setDevastation( !d.warehouse->store().isDevastation() );
  _update();
}

void WarehouseSpecialOrdersWindow::_update()
{
  __D_REF(d, WarehouseSpecialOrdersWindow)

  INIT_WIDGET_FROM_UI( PushButton*, btnToggleDevastation )
  INIT_WIDGET_FROM_UI( PushButton*, btnTradeCenter )

  if( btnToggleDevastation )
    btnToggleDevastation->setText( d.warehouse->store().isDevastation()
                                   ? _("##stop_warehouse_devastation##")
                                   : _("##devastate_warehouse##") );

  if( btnTradeCenter )
    btnTradeCenter->setText( d.warehouse->isTradeCenter()
                             ? _("##become_warehouse##")
                             : _("##become_trade_center##") );
}

}//end namespace gui
