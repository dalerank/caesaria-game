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

#include "market_lady.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "pathway/pathway.hpp"
#include "objects/market.hpp"
#include "good/store.hpp"
#include "walkers_factory.hpp"

using namespace constants;

REGISTER_CLASS_IN_WALKERFACTORY(walker::marketLady, MarketLady)

ServiceWalkerPtr MarketLady::create(PlayerCityPtr city)
{
  ServiceWalkerPtr ret( new MarketLady( city ) );
  ret->drop();

  return ret;
}

MarketPtr MarketLady::market() const
{
  return ptr_cast<Market>( _city()->getOverlay( baseLocation() ) );
}

void MarketLady::_updateThoughts()
{
  if( pathway().isReverse() )
  {
    if( market().isValid() && market()->goodStore().empty() )
    {
      setThinks( "##marketLady_no_food_on_market##" );
      return;
    }
  }

  ServiceWalker::_updateThoughts();
}

void MarketLady::_centerTile()
{
  ServiceWalker::_centerTile();

  if( market().isValid() && market()->goodStore().empty() )
  {
    return2Base();
  }
}

MarketLady::MarketLady(PlayerCityPtr city)
  : ServiceWalker( city, Service::market )
{
}
