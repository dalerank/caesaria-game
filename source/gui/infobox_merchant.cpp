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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "infobox_merchant.hpp"
#include "walker/merchant.hpp"
#include "core/metric.hpp"
#include "label.hpp"
#include "good/helper.hpp"
#include "infobox_citizen_mgr.hpp"
#include "core/gettext.hpp"

namespace gui
{

namespace infobox
{

namespace citizen
{

REGISTER_CITIZEN_INFOBOX(merchant,AboutMerchant)
REGISTER_CITIZEN_INFOBOX(seaMerchant,AboutMerchant)

AboutMerchant::AboutMerchant( Widget* parent, PlayerCityPtr city, const TilePos& pos )
  : AboutPeople( parent, Rect( 0, 0, 460, 380 ), Rect( 18, 40, 460 - 18, 380 - 150 ) )
{
  _init( city, pos, ":/gui/infoboxmerchant.gui" );
}

void AboutMerchant::_updateExtInfo()
{
  WalkerPtr wlk = _walkers().valueOrEmpty( 0 );

  if( wlk.isNull() )
    return;

  switch( wlk->type() )
  {
  case walker::merchant:
  case walker::seaMerchant:
  {
    auto merchant = wlk.as<Merchant>();
    good::ProductMap mmap = merchant->bougth();
    good::ProductMap bmap = merchant->mayBuy();

    int index=0;
    add<Label>( Rect( Point( 16, _lbBlackFrame()->bottom() + 2 ), Size( 84, 24 ) ), _("##bougth##"));
    for( auto& it : bmap )
      _drawGood( it.first, mmap[ it.first ], index++, _lbBlackFrame()->bottom() + 2 );

    mmap = merchant->sold();
    index=0;
    add<Label>(Rect( Point( 16, _lbBlackFrame()->bottom() + 26 ), Size( 84, 24 ) ), _("##sold##"));
    for( auto& it : mmap )
      if( it.second > 0 )
        _drawGood( it.first, it.second, index++, _lbBlackFrame()->bottom() + 26 );
  }
  break;

  default:
  break;
  }
}

void AboutMerchant::_drawGood( const good::Product& goodType, int qty, int index, int paintY )
{
  int startOffset = 100;

  int offset = ( width() - startOffset * 2 ) / 4;
  //std::string goodName = good::Helper::name( goodType );
  std::string outText = utils::i2str( metric::Measure::convQty( qty ) );

  // pictures of goods
  gfx::Picture pic = good::Helper::picture( goodType );
  Point pos( index * offset + startOffset, paintY );

  auto& label = add<Label>( Rect( pos, pos + Point( 100, 24 )) );
  label.setFont( Font::create( FONT_2 ) );
  label.setIcon( pic );
  label.setText( outText );
  label.setTextOffset( Point( 30, 0 ) );
}

}//end namespace citizen

}//end namespace infobox

}//end namespace gui
