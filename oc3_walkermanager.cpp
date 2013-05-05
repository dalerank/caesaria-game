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

#include "oc3_walkermanager.hpp"
#include "oc3_walker.hpp"
#include "oc3_walker_market_buyer.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include <map>

class WalkerManager::Impl
{
public:
  std::map<WalkerType, Walker*> mapWalkerByID;  // key=walkerType, value=instance
};

WalkerManager::WalkerManager() : _d( new Impl )
{
  // first call to this method
  //_d->mapWalkerByID[WT_SERVICE] = new ServiceWalker(S_MAX);  // dummy serviceType
  _d->mapWalkerByID[WT_MARKET_BUYER] = new MarketBuyer();
  _d->mapWalkerByID[WT_CART_PUSHER] = new CartPusher();
  //_d->mapWalkerByID[WT_IMMIGRANT] = new Immigrant();
  //_d->mapWalkerByID[WT_EMIGRANT] = new Emigrant();
  _d->mapWalkerByID[WT_TRAINEE] = new TraineeWalker(WTT_NONE);
}

WalkerManager::~WalkerManager()
{

}

Walker* WalkerManager::create( const WalkerType walkerType, const TilePos& pos )
{
  std::map<WalkerType, Walker*>::iterator mapIt;
  mapIt = _d->mapWalkerByID.find(walkerType);
  Walker *res;

  if( mapIt == _d->mapWalkerByID.end() )
  {
    // THROW("Unknown walker type:" << walkerType);
    res = NULL;
  }
  else
  {
    //res = mapIt->second->clone();
  }

  return res;
}

WalkerManager& WalkerManager::getInstance()
{
  static WalkerManager inst;
  return inst;
}