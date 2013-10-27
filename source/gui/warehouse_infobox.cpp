// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "warehouse_infobox.hpp"
#include "gfx/tile.hpp"
#include "pushbutton.hpp"
#include "core/gettext.hpp"
#include "building/metadata.hpp"
#include "building/warehouse.hpp"
#include "label.hpp"
#include "game/goodhelper.hpp"
#include "special_orders_window.hpp"
#include "game/goodstore.hpp"
#include "core/stringhelper.hpp"

namespace gui
{

InfoBoxWarehouse::InfoBoxWarehouse( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 360 ), Rect( 16, 225, 510 - 16, 225 + 62 ) )
{
  _warehouse = tile.getOverlay().as<Warehouse>();

  Size btnOrdersSize( 350, 20 );
  PushButton* btnOrders = new PushButton( this, Rect( Point( (getWidth() - btnOrdersSize.getWidth()) / 2, getHeight() - 34 ), btnOrdersSize ),
                                          _("##special_orders##"), -1, false, PushButton::whiteBorderUp );

  CONNECT( btnOrders, onClicked(), this, InfoBoxWarehouse::showSpecialOrdersWindow );

  setTitle( BuildingDataHolder::getPrettyName( _warehouse->getType() ) );

  // summary: total stock, free capacity
  int _paintY = _getTitle()->getBottom();

  drawGood(Good::wheat,     0, _paintY+25);
  drawGood(Good::vegetable, 0, _paintY+50);
  drawGood(Good::fruit,     0, _paintY+75);
  drawGood(Good::olive,     0, _paintY+100);
  drawGood(Good::grape,     0, _paintY+125);
  drawGood(Good::fish,      0, _paintY+150);

  drawGood(Good::meat,      1, _paintY+25);
  drawGood(Good::wine,      1, _paintY+50);
  drawGood(Good::oil,       1, _paintY+75);
  drawGood(Good::iron,      1, _paintY+100);
  drawGood(Good::timber,    1, _paintY+125);

  drawGood(Good::clay,      2, _paintY+25);
  drawGood(Good::marble,    2, _paintY+50);
  drawGood(Good::weapon,    2, _paintY+75);
  drawGood(Good::furniture, 2, _paintY+100);
  drawGood(Good::pottery,   2, _paintY+125);

  _drawWorkers( Point( 16 + 20, 225 + 10 ), 542, _warehouse->getMaxWorkers(), _warehouse->getWorkers() );
}

InfoBoxWarehouse::~InfoBoxWarehouse()
{
}

void InfoBoxWarehouse::showSpecialOrdersWindow()
{
  Point pos;
  if( getTop() > (int)getParent()->getHeight() / 2 )
  {
    pos = Point( getScreenLeft(), getScreenBottom() - 450 );
  }
  else
  {
    pos = getAbsoluteRect().UpperLeftCorner;
  }

  new WarehouseSpecialOrdersWindow( getParent(), pos, _warehouse );
}

void InfoBoxWarehouse::drawGood( const Good::Type &goodType, int col, int paintY )
{
  std::string goodName = GoodHelper::getName( goodType );

  Font font = Font::create( FONT_2 );
  int qty = _warehouse->getGoodStore().getCurrentQty(goodType);

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  getBgPicture().draw( pic, col * 150 + 15, paintY );

  std::string outText = StringHelper::format( 0xff, "%d %s", qty, goodName.c_str() );
  font.draw( _getBgPicture(), outText, col * 150 + 45, paintY, false );
}

}//end namespace gui
