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

#include "market_lady_helper.hpp"
#include "market_lady.hpp"
#include "building/market.hpp"
#include "game/city.hpp"
#include "game/pathway.hpp"
#include "game/goodstore.hpp"
#include "game/resourcegroup.hpp"
#include "game/name_generator.hpp"
#include "constants.hpp"
#include "corpse.hpp"

using namespace constants;

class MarketLadyHelper::Impl
{
public:
  GoodStock basket;
  unsigned long delay;
  TilePos marketPos;
  unsigned long birthTime;
};

MarketLadyHelperPtr MarketLadyHelper::create( CityPtr city, MarketLadyPtr lady )
{
  MarketLadyHelperPtr ret( new MarketLadyHelper( city ) );
  ret->setPathWay( lady->getPathway() );
  ret->setIJ( lady->getIJ() );
  ret->_getPathway().rbegin();
  ret->onMidTile();
  ret->drop();

  return ret;
}

MarketLadyHelper::MarketLadyHelper( CityPtr city )
  : Walker( city ), _d( new Impl )
{
  _d->delay = 0;
  _d->birthTime = 0;
  _d->basket._maxQty = 100;
  _setGraphic( WG_MARKETLADY_HELPER );
  _setType( walker::marketLady );

  setName( NameGenerator::rand( NameGenerator::male ) );
}

void MarketLadyHelper::setDelay( int delay )
{
  _d->delay = delay;
}

void MarketLadyHelper::send2City( MarketPtr destination )
{
  if( destination.isValid() )
  {
    _d->marketPos = destination->getTilePos();
    _getCity()->addWalker( this );
  }
  else
  {
    deleteLater();
  }
}

void MarketLadyHelper::timeStep( const unsigned long time )
{
  if( _d->birthTime == 0 )
  {
    _d->birthTime = time;
  }

  if( time - _d->birthTime < _d->delay )
  {
    return;
  }

  Walker::timeStep( time );
}

void MarketLadyHelper::onDestination()
{
  Walker::onDestination();

  deleteLater();

  CityHelper cityh( _getCity() );
  MarketPtr market = cityh.find< Market >( _d->marketPos );
  if( market.isValid() )
  {
    market->getGoodStore().store( _d->basket, _d->basket._currentQty );
  }
}

void MarketLadyHelper::die()
{
  Walker::die();

  Corpse::create( _getCity(), getIJ(), ResourceGroup::carts, 465, 472 );
}

GoodStock& MarketLadyHelper::getBasket()
{
  return _d->basket;
}
