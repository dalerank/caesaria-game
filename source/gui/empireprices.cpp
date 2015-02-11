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

#include "empireprices.hpp"
#include "city/trade_options.hpp"
#include "city/city.hpp"
#include "good/goodhelper.hpp"
#include "image.hpp"
#include "core/utils.hpp"
#include "label.hpp"
#include "core/event.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

EmpirePrices::EmpirePrices(Widget *parent, int id, const Rect &rectangle, PlayerCityPtr city)
  : Window( parent, rectangle, "", id )
{
  setupUI( ":/gui/empireprices.gui" );

  trade::Options& ctrade = city->tradeOptions();
  Font font = Font::create( FONT_1 );
  Point startPos( 140, 50 );
  for( good::Product gtype=good::wheat; gtype < good::prettyWine; ++gtype )
  {
    if( gtype == good::fish || gtype == good::denaries)
    {
      continue;
    }

    Picture goodIcon = good::Helper::picture( gtype );
    new Image( this, startPos, goodIcon );

    std::string priceStr = utils::i2str( ctrade.buyPrice( gtype ) );
    Label* lb = new Label( this, Rect( startPos + Point( 0, 34 ), Size( 24, 24 ) ), priceStr );
    lb->setFont( font );

    priceStr = utils::i2str( ctrade.sellPrice( gtype ) );
    lb = new Label( this, Rect( startPos + Point( 0, 58 ), Size( 24, 24 ) ), priceStr );
    lb->setFont( font );

    startPos += Point( 30, 0 );
  }
}

void EmpirePrices::draw(Engine &painter)
{
  if( !visible() )
    return;

  Window::draw( painter );
}

bool EmpirePrices::onEvent(const NEvent &event)
{
  if( event.EventType == sEventMouse && event.mouse.isRightPressed() )
    {
      deleteLater();
      return true;
    }

  return Window::onEvent( event );
}

}

}//end namespace gui
