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

#include "oc3_city_trade_options.hpp"
#include "oc3_goodstore_simple.hpp"

class CityTradeOptions::Impl
{
public:
  SimpleGoodStore trading;
};

CityTradeOptions::CityTradeOptions() : _d( new Impl )
{

}

int CityTradeOptions::getExportLimit( GoodType type ) const
{
  return _d->trading.getMaxQty( type );
}

CityTradeOptions::Order CityTradeOptions::getOrder( GoodType type ) const
{
  return noTrade;
}

CityTradeOptions::~CityTradeOptions()
{

}

CityTradeOptions::Order CityTradeOptions::switchOrder( GoodType type )
{
  return noTrade;
}

void CityTradeOptions::setExportLimit( GoodType type, int qty )
{
  _d->trading.setMaxQty( type, qty );
}

bool CityTradeOptions::isGoodsStacking( GoodType type )
{
  return false;
}

void CityTradeOptions::setStackMode( GoodType type, bool stackGoods )
{
  
}