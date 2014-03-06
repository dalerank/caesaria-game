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

InfoboxGranary::InfoboxGranary( Widget* parent, const Tile& tile )
  : InfoboxConstruction( parent, Rect( 0, 0, 510, 280 ), Rect( 16, 130, 510 - 16, 130 + 62) )
{
  _granary = ptr_cast<Granary>( tile.overlay() );

  setConstruction( ptr_cast<Construction>( _granary ) );

  Size btnOrdersSize( 350, 20 );
  PushButton* btnOrders = new PushButton( this, Rect( Point( (width() - btnOrdersSize.width())/ 2, height() - 34 ), btnOrdersSize),
                                         _("##granary_orders##"), -1, false, PushButton::whiteBorderUp );
  CONNECT( btnOrders, onClicked(), this, InfoboxGranary::showSpecialOrdersWindow );

  std::string title = MetaDataHolder::getPrettyName( _granary->type() );
  setTitle( _(title) );

  // summary: total stock, free capacity
  std::string desc = StringHelper::format( 0xff, "%d %s, %s %d",
                                           _granary->store().qty(),
                                           _("##units_in_stock##"), _("##freespace_for##"),
                                           _granary->store().freeQty() );

  Label* lbUnits = new Label( this, Rect( _getTitle()->leftdownCorner(), Size( width() - 16, 40 )), desc );

  drawGood(Good::wheat, 0, lbUnits->bottom() );
  drawGood(Good::meat, 0, lbUnits->bottom() + 25);
  drawGood(Good::fruit, 1, lbUnits->bottom() );
  drawGood(Good::vegetable, 1, lbUnits->bottom() + 25);

  _updateWorkersLabel( Point( 32, lbUnits->bottom() + 60 ), 542, _granary->maxWorkers(), _granary->numberWorkers() );
}

InfoboxGranary::~InfoboxGranary()
{
}

void InfoboxGranary::showSpecialOrdersWindow()
{
  Point pos;
  if( getTop() > (int)getParent()->height() / 2 )
  {
    pos = Point( screenLeft(), screenBottom() - GranarySpecialOrdersWindow::defaultHeight );
  }
  else
  {
    pos = absoluteRect().UpperLeftCorner;
  }

  new GranarySpecialOrdersWindow( getParent(), pos, _granary );
}

void InfoboxGranary::drawGood( Good::Type goodType, int col, int paintY)
{
  std::string goodName = GoodHelper::getTypeName( goodType );
  int qty = _granary->store().qty(goodType);
  std::string outText = StringHelper::format( 0xff, "%d %s", qty, _( "##" + goodName + "##" ) );

  // pictures of goods
  const Picture& pic = GoodHelper::getPicture( goodType );
  Label* lb = new Label( this, Rect( Point( (col == 0 ? 31 : 250), paintY), Size( 150, 24 )) );
  lb->setIcon( pic );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setText( outText );
  lb->setTextOffset( Point( 30, 0 ) );
}

}//end namespace gui
