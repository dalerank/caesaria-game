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

using namespace constants;

namespace gui
{

InfoBoxGranary::InfoBoxGranary( Widget* parent, const Tile& tile )
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 280 ), Rect( 16, 130, 510 - 16, 130 + 62) )
{
  _granary = tile.getOverlay().as<Granary>();
  Size btnOrdersSize( 350, 20 );
  PushButton* btnOrders = new PushButton( this, Rect( Point( (getWidth() - btnOrdersSize.getWidth())/ 2, getHeight() - 34 ), btnOrdersSize),
                                         _("##granary_orders##"), -1, false, PushButton::whiteBorderUp );
  CONNECT( btnOrders, onClicked(), this, InfoBoxGranary::showSpecialOrdersWindow );

  setTitle( MetaDataHolder::getPrettyName( _granary->getType() ) );

  // summary: total stock, free capacity
  std::string desc = StringHelper::format( 0xff, "%d %s %d",
                                           _granary->getGoodStore().getQty(),
                                           _("##units_in_stock_freespace_for##"),
                                           _granary->getGoodStore().getFreeQty() );

  Label* lbUnits = new Label( this, Rect( _getTitle()->getLeftdownCorner(), Size( getWidth() - 16, 40 )), desc );

  drawGood(Good::wheat, 0, lbUnits->getBottom() );
  drawGood(Good::meat, 0, lbUnits->getBottom() + 25);
  drawGood(Good::fruit, 1, lbUnits->getBottom() );
  drawGood(Good::vegetable, 1, lbUnits->getBottom() + 25);

  _updateWorkersLabel( Point( 32, lbUnits->getBottom() + 60 ), 542, _granary->getMaxWorkers(), _granary->getWorkersCount() );
}

InfoBoxGranary::~InfoBoxGranary()
{
}

void InfoBoxGranary::showSpecialOrdersWindow()
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

  new GranarySpecialOrdersWindow( getParent(), pos, _granary );
}

void InfoBoxGranary::drawGood( Good::Type goodType, int col, int paintY)
{
  std::string goodName = GoodHelper::getName( goodType );
  int qty = _granary->getGoodStore().getQty(goodType);
  std::string outText = StringHelper::format( 0xff, "%d %s", qty, goodName.c_str() );

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  Label* lb = new Label( this, Rect( Point( (col == 0 ? 31 : 250), paintY), Size( 100, 24 )) );
  lb->setIcon( pic );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setText( outText );
  lb->setTextOffset( Point( 30, 0 ) );
}

}//end namespace gui
