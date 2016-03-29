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

#include "infobox_granary.hpp"
#include "label.hpp"
#include "pushbutton.hpp"
#include "core/gettext.hpp"
#include "objects/granary.hpp"
#include "core/utils.hpp"
#include "good/store.hpp"
#include "core/logger.hpp"
#include "granary_orders_window.hpp"
#include "good/helper.hpp"
#include "game/infoboxmanager.hpp"
#include "widget_helper.hpp"
#include "core/metric.hpp"

using namespace gfx;
using namespace metric;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(granary,AboutGranary)

AboutGranary::AboutGranary(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 280 ), Rect( 16, 130, 510 - 16, 130 + 62) )
{
  setupUI( ":/gui/granaryinfo.gui" );

  _granary = tile.overlay<Granary>();

  if( _granary.isNull() )
  {
    deleteLater();
    return;
  }

  setBase( _granary );
  _setWorkingVisible( true );

  INIT_WIDGET_FROM_UI( Label*, lbUnits )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnOrders, onClicked(), AboutGranary::showSpecialOrdersWindow );

  setTitle( _( _granary->info().prettyName() ) );

  if( lbUnits )
  {
    // summary: total stock, free capacity
    int capacity = _granary->store().qty();
    int freeQty = _granary->store().freeQty();
    std::string desc = utils::format( 0xff, "%d %s, %s %d (%s)",
                                             Measure::convQty( capacity ),
                                             _("##units_in_stock##"), _("##freespace_for##"),
                                             Measure::convQty( freeQty ),
                                             Measure::measureType() );
    lbUnits->setPosition( _lbTitle()->leftbottom() + Point( 0, 5 ) );
    lbUnits->setText( desc );

    drawGood(good::wheat, 0, lbUnits->bottom() );
    drawGood(good::meat, 0, lbUnits->bottom() + 25);
    drawGood(good::fruit, 1, lbUnits->bottom() );
    drawGood(good::vegetable, 1, lbUnits->bottom() + 25);
  }

  _updateWorkersLabel( Point( 32, lbUnits->bottom() + 60 ), 542, _granary->maximumWorkers(), _granary->numberWorkers() );
}

AboutGranary::~AboutGranary() {}

void AboutGranary::showSpecialOrdersWindow()
{
  Point pos;
  if( top() > (int)parent()->height() / 2 )
  {
    pos = Point( screenLeft(), screenBottom() - GranarySpecialOrdersWindow::defaultHeight );
  }
  else
  {
    pos = absoluteRect().lefttop();
  }

  parent()->add<GranarySpecialOrdersWindow>( pos, _granary );
}

void AboutGranary::drawGood(good::Product goodType, int col, int paintY)
{
  good::Info info( goodType );
  int qty = _granary->store().qty(goodType);
  qty = Measure::convQty( qty );
  const char* measure = Measure::measureShort();

  std::string outText = fmt::format( "{} {} {}", qty, _(measure), _( "##" + info.name() + "##" ) );

  // pictures of goods
  Label& lb = add<Label>( Rect( Point( (col == 0 ? 31 : 250), paintY), Size( width()/2 - 15, 24 )) );
  lb.setIcon( info.picture() );
  lb.setFont( "FONT_2" );
  lb.setText( outText );
  lb.setTextOffset( Point( 30, 0 ) );
}

}//end namespace infobox

}//end namespace gui
