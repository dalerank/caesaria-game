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

class CityTradeOptions::Impl
{
public:
  struct GoodInfo 
  { 
    unsigned int sellPrice; 
    unsigned int buyPrice;
    unsigned int exportLimit;
    unsigned int importLimit;
    unsigned int soldGoods;
    unsigned int bougthGoods;
    bool stacking;
    CityTradeOptions::Order order;
    bool vendor;
  };

  Impl()
  {
    for( int i=G_WHEAT; i < G_MAX; i++ )
    {
      GoodType gtype = (GoodType)i;
      goods[ gtype ].stacking = false;
      goods[ gtype ].order = CityTradeOptions::noTrade;
      goods[ gtype ].vendor = true;
      goods[ gtype ].exportLimit = 0;
    }

    goods[ G_FISH ].order = CityTradeOptions::disabled;
    goods[ G_DENARIES ].order = CityTradeOptions::disabled;
  }

  void initStandartPrice()
  {
    goods[ G_WHEAT ].buyPrice = 28; goods[ G_WHEAT ].sellPrice = 22;
    goods[ G_VEGETABLE ].buyPrice = 38; goods[ G_VEGETABLE ].sellPrice = 30;
    goods[ G_FRUIT ].buyPrice = 38; goods[ G_FRUIT ].sellPrice = 30;
    goods[ G_OLIVE ].buyPrice = 42; goods[ G_OLIVE ].sellPrice = 34;
    goods[ G_GRAPE ].buyPrice = 44; goods[ G_GRAPE ].sellPrice = 36;
    goods[ G_MEAT ].buyPrice = 44; goods[ G_MEAT ].sellPrice = 36;
    goods[ G_WINE ].buyPrice = 215; goods[ G_WINE ].sellPrice = 160;
    goods[ G_OIL ].buyPrice = 180; goods[ G_OIL ].sellPrice = 140;
    goods[ G_IRON ].buyPrice = 60; goods[ G_IRON ].sellPrice = 40;
    goods[ G_TIMBER ].buyPrice = 50; goods[ G_TIMBER ].sellPrice = 35;
    goods[ G_CLAY ].buyPrice = 40; goods[ G_CLAY ].sellPrice = 30;
    goods[ G_MARBLE ].buyPrice = 200; goods[ G_MARBLE ].sellPrice = 140;
    goods[ G_WEAPON ].buyPrice = 250; goods[ G_WEAPON ].sellPrice = 180;
    goods[ G_FURNITURE ].buyPrice = 200; goods[ G_FURNITURE ].sellPrice = 150;
    goods[ G_POTTERY ].buyPrice = 180; goods[ G_POTTERY ].sellPrice = 140;
  }

  typedef std::map< GoodType, GoodInfo > GoodsInfo;
  GoodsInfo goods;
};

CityTradeOptions::CityTradeOptions() : _d( new Impl )
{
  _d->initStandartPrice();
}

int CityTradeOptions::getExportLimit( GoodType type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.exportLimit );
}

CityTradeOptions::Order CityTradeOptions::getOrder( GoodType type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? CityTradeOptions::noTrade : it->second.order);
}

CityTradeOptions::~CityTradeOptions()
{

}

CityTradeOptions::Order CityTradeOptions::switchOrder( GoodType type )
{
  if( !isVendor( type ) )
  {
    switch( getOrder( type ) )
    {
    case noTrade: setOrder( type, importing ); break;
    case importing: setOrder( type, noTrade ); break;
    }
  }
  {
    switch( getOrder( type ) )
    {
    case noTrade: setOrder( type, exporting ); break;
    case exporting: setOrder( type, importing ); break;
    case importing: setOrder( type, noTrade ); break;
    }
  }

  return getOrder( type );
}

void CityTradeOptions::setExportLimit( GoodType type, int qty )
{
  _d->goods[ type ].exportLimit = qty;
}

bool CityTradeOptions::isGoodsStacking( GoodType type )
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.stacking );
}

void CityTradeOptions::setStackMode( GoodType type, bool stackGoods )
{
  _d->goods[ type ].stacking = stackGoods;
}

unsigned int CityTradeOptions::getSellPrice( GoodType type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.sellPrice );
}

void CityTradeOptions::setSellPrice( GoodType type, unsigned int price )
{
   _d->goods[ type ].sellPrice = price;
}

unsigned int CityTradeOptions::getBuyPrice( GoodType type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.buyPrice );
}

void CityTradeOptions::setBuyPrice( GoodType type, unsigned int price )
{
  _d->goods[ type ].buyPrice = price;
}

bool CityTradeOptions::isVendor( GoodType type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.vendor );
}

void CityTradeOptions::setVendor( GoodType type, bool available )
{
  _d->goods[ type ].vendor = available;
}

void CityTradeOptions::setOrder( GoodType type, Order order )
{
  _d->goods[ type ].order = order;
}