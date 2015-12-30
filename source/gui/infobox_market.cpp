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

#include "infobox_market.hpp"
#include "label.hpp"
#include "objects/market.hpp"
#include "good/store.hpp"
#include "core/gettext.hpp"
#include "good/helper.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/metric.hpp"
#include "core/color_list.hpp"
#include "core/logger.hpp"
#include "game/infoboxmanager.hpp"
#include "gfx/maskstate.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(market,AboutMarket)

AboutMarket::AboutMarket(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 155, 510 - 16, 155 + 50) )
{
  setupUI( ":/gui/infoboxmarket.gui" );

  MarketPtr market = tile.overlay<Market>();

  if( !market.isValid() )
  {
    Logger::warning( "AboutMarket: market is null tile at [{},{}]", tile.i(), tile.j() );
    return;
  }

  setBase( market );
  _setWorkingVisible( true );

  Label& lbAbout = add<Label>( Rect( 15, 30, width() - 15, 50) );
  lbAbout.setWordwrap( true );
  lbAbout.setFont( FONT_1 );
  lbAbout.setTextAlignment( align::upperLeft, align::upperLeft );

  setTitle( _( market->info().prettyName() ) );

  if( market->numberWorkers() > 0 )
  {
    good::Store& goods = market->goodStore();
    int furageSum = 0;
    // for all furage types of good
    for( good::Product pr=good::none; pr<good::olive; ++pr )
    {
      furageSum += goods.qty( pr );
    }

    int paintY = 100;
    if( 0 < furageSum )
    {
      drawGood( market, good::wheat, 0, paintY );
      drawGood( market, good::fish, 1, paintY);
      drawGood( market, good::meat, 2, paintY);
      drawGood( market, good::fruit, 3, paintY);
      drawGood( market, good::vegetable, 4, paintY);
      lbAbout.setHeight( 60 );
    }
    else
    {
      lbAbout.setHeight( 90 );
      lbAbout.setWordwrap( true );
      lbAbout.setTextAlignment( align::upperLeft, align::center );
    }

    paintY += 24;
    drawGood( market, good::pottery, 0, paintY);
    drawGood( market, good::furniture, 1, paintY);
    drawGood( market, good::oil, 2, paintY);
    drawGood( market, good::wine, 3, paintY);

    lbAbout.setText( 0 == furageSum ? _("##market_search_food_source##") : _("##market_about##"));
  }
  else
  {
    lbAbout.setHeight( 50 );
    lbAbout.setText( _("##market_no_workers##") );
  }

  _updateWorkersLabel( Point( 32, 8 ), 542, market->maximumWorkers(), market->numberWorkers() );
}

AboutMarket::~AboutMarket() {}

class MarketGoodButton : public PushButton
{
public:
  MarketGoodButton( Widget* parent, const Rect& rect, const good::Product &goodType,
                    int qty, good::Orders::Order order)
    : PushButton( parent, rect, "", -1, false, PushButton::noBackground )
  {
    setText( utils::i2str( metric::Measure::convQty( qty ) ) );
    setIcon( good::Helper::picture( goodType ) );
    setFont( FONT_2 );
    setTextOffset( { 30, 0 } );

    _order = order;
    _goodType = goodType;
  }

  virtual void drawIcon(Engine &painter)
  {
    if( _order == good::Orders::reject )
    {
      MaskState lock( painter, ColorList::red );
      PushButton::drawIcon( painter );
    }

    PushButton::drawIcon( painter );
  }

  Signal2<good::Product,good::Orders::Order> onSwitchOrder;
protected:
  virtual void _btnClicked()
  {
    PushButton::_btnClicked();
    _order = (_order == good::Orders::accept
                      ? good::Orders::reject
                      : good::Orders::accept );
    emit onSwitchOrder( _goodType, _order );
  }

  good::Product _goodType;
  good::Orders::Order _order;
};

void AboutMarket::drawGood( MarketPtr market, const good::Product &goodType,
                            int index, int paintY )
{
  int startOffset = 25;

  int offset = ( width() - startOffset * 2 ) / 5;
  good::Store& store = market->goodStore();
  good::Orders::Order order = store.getOrder( goodType );

  // pictures of goods
  Point pos( index * offset + startOffset, paintY );

  auto& btn = add<MarketGoodButton>( Rect( pos, Size( 100, 24 )),
                                     goodType, store.qty( goodType ), order );
  CONNECT( &btn, onSwitchOrder, &store, good::Store::setOrder )
}

}//end namespace infobox

}//end namespace gui
