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

#include "granary_orders_window.hpp"
#include "core/gettext.hpp"
#include "good/helper.hpp"
#include "core/logger.hpp"
#include "good/store.hpp"
#include "order_good_widget.hpp"
#include "good/orders.hpp"
#include "objects/granary.hpp"

using namespace gfx;

namespace gui
{

class GranarySpecialOrdersWindow::Impl
{
public:
  GranaryPtr granary;
  PushButton* btnToggleDevastation;
};

GranarySpecialOrdersWindow::GranarySpecialOrdersWindow( Widget* parent, const Point& pos, GranaryPtr granary )
: BaseSpecialOrdersWindow( parent, pos, defaultHeight ), __INIT_IMPL(GranarySpecialOrdersWindow)
{
  __D_IMPL(d, GranarySpecialOrdersWindow)

  setupUI( ":/gui/granaryspecial.gui" );

  setTitle( _("##granary_orders##") );
  int index=0;
  d->granary = granary;
  for( auto& goodType : good::foods() )
  {
    const good::Orders::Order rule = granary->store().getOrder( goodType );
    
    if( rule != good::Orders::none )
    {
      OrderGoodWidget::create( index, goodType, _ordersArea(), granary->store() );
      index++;
    }
  }

  d->btnToggleDevastation = &add<PushButton>( Rect( 80, height() - 45, width() - 80, height() - 25 ),
                                              "", -1, false, PushButton::whiteBorderUp );

  CONNECT( d->btnToggleDevastation, onClicked(), this, GranarySpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

GranarySpecialOrdersWindow::~GranarySpecialOrdersWindow() {}

void GranarySpecialOrdersWindow::toggleDevastation()
{
  __D_IMPL(d, GranarySpecialOrdersWindow)
  d->granary->store().setDevastation( !d->granary->store().isDevastation() );
  _updateBtnDevastation();
}

void GranarySpecialOrdersWindow::_updateBtnDevastation()
{
  __D_IMPL(d, GranarySpecialOrdersWindow)
  d->btnToggleDevastation->setText( d->granary->store().isDevastation()
                                    ? _("##stop_granary_devastation##")
                                    : _("##devastate_granary##") );
}

}//end namespace gui
