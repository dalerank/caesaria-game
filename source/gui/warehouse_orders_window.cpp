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
  PushButton* btnToggleDevastation;
  PushButton* btnTradeCenter;

public:
  void update();
  void toggleTradeCenter();
  void toggleDevastation();
};

WarehouseSpecialOrdersWindow::WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse )
: BaseSpecialOrdersWindow( parent, pos, defaultHeight ), __INIT_IMPL(WarehouseSpecialOrdersWindow)
{
  __D_IMPL(d, WarehouseSpecialOrdersWindow)

  setupUI( ":/gui/warehousespecial.gui");
  setTitle( _("##warehouse_orders##") );

  std::set<good::Product> excludeGoods;
  excludeGoods << good::none << good::denaries;

  d->warehouse = warehouse;
  int index=0;
  foreach( goodType, good::all() )
  {
    if( excludeGoods.count( *goodType ) > 0 )
      continue;

    const good::Orders::Order rule = d->warehouse->store().getOrder( *goodType );

    if( rule != good::Orders::none )
    {
      OrderGoodWidget::create( index, *goodType, _ordersArea(), d->warehouse->store() );
      index++;
    }
  }

  GET_DWIDGET_FROM_UI( d, btnToggleDevastation )
  GET_DWIDGET_FROM_UI( d, btnTradeCenter )

  CONNECT( d->btnToggleDevastation, onClicked(), d.data(), Impl::toggleDevastation );
  CONNECT( d->btnTradeCenter,       onClicked(), d.data(), Impl::toggleTradeCenter );

  d->update();
}

WarehouseSpecialOrdersWindow::~WarehouseSpecialOrdersWindow() {}

void WarehouseSpecialOrdersWindow::Impl::toggleTradeCenter()
{
  warehouse->setTradeCenter( !warehouse->isTradeCenter() );
  update();
}

void WarehouseSpecialOrdersWindow::Impl::toggleDevastation()
{
  warehouse->store().setDevastation( !warehouse->store().isDevastation() );
  update();
}

void WarehouseSpecialOrdersWindow::Impl::update()
{
  if( btnToggleDevastation )
    btnToggleDevastation->setText( warehouse->store().isDevastation()
                                   ? _("##stop_warehouse_devastation##")
                                   : _("##devastate_warehouse##") );

  if( btnTradeCenter )
    btnTradeCenter->setText( warehouse->isTradeCenter()
                             ? _("##stop_warehouse_tradepost##")
                             : _("##tradepost_warehouse##") );
}

}//end namespace gui
