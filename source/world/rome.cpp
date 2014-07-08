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

#include "rome.hpp"
#include "empire.hpp"
#include "good/goodstore_simple.hpp"
#include "city/funds.hpp"
#include "events/showinfobox.hpp"
#include "goodcaravan.hpp"
#include "core/gettext.hpp"

namespace world {

const char* Rome::defaultName = "Rome";

class Rome::Impl
{
public:
  city::Funds funds;
  SimpleGoodStore gstore;
};

Rome::Rome(EmpirePtr empire)
   : City( empire ), _d( new Impl )
{
  gfx::Picture pic = gfx::Picture::load( "roma", 1 );
  pic.setOffset( 0, 30 );
  setPicture( pic );

  setLocation( Point( 870, 545 ) );
}

unsigned int Rome::tradeType() const { return 0; }
city::Funds& Rome::funds() { return _d->funds; }

std::string Rome::name() const { return Rome::defaultName; }
unsigned int Rome::population() const { return 45000; }
bool Rome::isPaysTaxes() const { return true; }
bool Rome::haveOverduePayment() const { return false; }
const GoodStore& Rome::exportingGoods() const{ return _d->gstore; }
void Rome::delayTrade(unsigned int month) {}
void Rome::empirePricesChanged(Good::Type gtype, int bCost, int sCost){}
const GoodStore& Rome::importingGoods() const{ return _d->gstore; }

void Rome::addObject(ObjectPtr obj)
{
  if( is_kind_of<GoodCaravan>( obj ) )
  {
    GoodCaravanPtr caravan = ptr_cast<GoodCaravan>( obj );

    Good::Type gtype = Good::none;
    for( int i=Good::wheat; i < Good::goodCount; i++ )
    {
      if( caravan->store().qty( Good::Type(i) ) > 0 )
      {
        gtype = Good::Type(i);
        break;
      }
    }

    events::GameEventPtr e = events::ShowInfobox::create( _("##rome_gratitude_request_title##"),
                                                          _("##rome_gratitude_request_text##"),
                                                          gtype,
                                                          !events::ShowInfobox::send2scribe);
    e->dispatch();
  }
}

} // end namespace world
