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
#include "good/goodstore.hpp"
#include "core/gettext.hpp"
#include "good/goodhelper.hpp"
#include "core/stringhelper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutMarket::AboutMarket(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 155, 510 - 16, 155 + 45) )
{
   MarketPtr market = ptr_cast<Market>( tile.overlay() );

   setBase( ptr_cast<Construction>( market ));

   Label* lbAbout = new Label( this, Rect( 15, 30, width() - 15, 50) );
   lbAbout->setWordwrap( true );
   lbAbout->setFont( Font::create( FONT_1 ) );
   lbAbout->setTextAlignment( align::upperLeft, align::upperLeft );

   std::string title = MetaDataHolder::findPrettyName( market->type() );
   setTitle( _( title ) );

   if( market->numberWorkers() > 0 )
   {
     GoodStore& goods = market->goodStore();
     int furageSum = 0;
     // for all furage types of good
     for (int goodType = 0; goodType<Good::olive; ++goodType)
     {
       furageSum += goods.qty( (Good::Type)goodType );
     }

     int paintY = 100;
     if( 0 < furageSum )
     {
       drawGood( market, Good::wheat, 0, paintY );
       drawGood( market, Good::fish, 1, paintY);
       drawGood( market, Good::meat, 2, paintY);
       drawGood( market, Good::fruit, 3, paintY);
       drawGood( market, Good::vegetable, 4, paintY);
       lbAbout->setHeight( 60 );
     }
     else
     {
       lbAbout->setHeight( 90 );
       lbAbout->setWordwrap( true );
     }

     paintY += 24;
     drawGood( market, Good::pottery, 0, paintY);
     drawGood( market, Good::furniture, 1, paintY);
     drawGood( market, Good::oil, 2, paintY);
     drawGood( market, Good::wine, 3, paintY);

     lbAbout->setText( 0 == furageSum ? _("##market_search_food_source##") : _("##market_about##"));
   }
   else
   {
     lbAbout->setHeight( 50 );
     lbAbout->setText( _("##market_no_workers##") );
   }

   _updateWorkersLabel( Point( 32, 8 ), 542, market->maximumWorkers(), market->numberWorkers() );
}

AboutMarket::~AboutMarket() {}

void AboutMarket::drawGood( MarketPtr market, const Good::Type &goodType, int index, int paintY )
{
  int startOffset = 25;

  int offset = ( width() - startOffset * 2 ) / 5;
  std::string goodName = GoodHelper::name( goodType );
  std::string outText = StringHelper::format( 0xff, "%d", market->goodStore().qty( goodType ) );

  // pictures of goods
  Picture pic = GoodHelper::picture( goodType );
  Point pos( index * offset + startOffset, paintY );

  Label* lb = new Label( this, Rect( pos, pos + Point( 100, 24 )) );
  lb->setFont( Font::create( FONT_2 ) );
  lb->setIcon( pic );
  lb->setText( outText );
  lb->setTextOffset( Point( 30, 0 ) );
}

}

}//end namespace gui
