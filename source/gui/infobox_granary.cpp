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
#include "core/stringhelper.hpp"
#include "good/goodstore.hpp"
#include "core/logger.hpp"
#include "special_orders_window.hpp"
#include "good/goodhelper.hpp"
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutGranary::AboutGranary(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 280 ), Rect( 16, 130, 510 - 16, 130 + 62) )
{
  setupUI( ":/gui/granaryinfo.gui" );
  _granary = ptr_cast<Granary>( tile.overlay() );

  setBase( ptr_cast<Construction>( _granary ) );

  PushButton* btnOrders;
  Label* lbUnits;
  GET_WIDGET_FROM_UI( btnOrders )
  GET_WIDGET_FROM_UI( lbUnits )

  CONNECT( btnOrders, onClicked(), this, AboutGranary::showSpecialOrdersWindow );

  std::string title = MetaDataHolder::findPrettyName( _granary->type() );
  setTitle( _(title) ); 

  if( lbUnits )
  {
    // summary: total stock, free capacity
    std::string desc = StringHelper::format( 0xff, "%d %s, %s %d",
                                             _granary->store().qty(),
                                             _("##units_in_stock##"), _("##freespace_for##"),
                                             _granary->store().freeQty() );
    lbUnits->setPosition( _lbTitleRef()->leftbottom() + Point( 0, 5 ) );
    lbUnits->setText( desc );

    drawGood(Good::wheat, 0, lbUnits->bottom() );
    drawGood(Good::meat, 0, lbUnits->bottom() + 25);
    drawGood(Good::fruit, 1, lbUnits->bottom() );
    drawGood(Good::vegetable, 1, lbUnits->bottom() + 25);
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
    pos = absoluteRect().UpperLeftCorner;
  }

  new GranarySpecialOrdersWindow( parent(), pos, _granary );
}

void AboutGranary::drawGood( Good::Type goodType, int col, int paintY)
{
  std::string goodName = GoodHelper::getTypeName( goodType );
  int qty = _granary->store().qty(goodType);
  std::string outText = StringHelper::format( 0xff, "%d %s", qty, _( "##" + goodName + "##" ) );

  // pictures of goods
  const Picture& pic = GoodHelper::picture( goodType );
  Label* lb = new Label( this, Rect( Point( (col == 0 ? 31 : 250), paintY), Size( 150, 24 )) );
  lb->setIcon( pic );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setText( outText );
  lb->setTextOffset( Point( 30, 0 ) );
}

}

}//end namespace gui
