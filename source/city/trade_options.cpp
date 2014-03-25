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

#include "trade_options.hpp"
#include "core/stringhelper.hpp"
#include "good/goodhelper.hpp"
#include "good/goodstore_simple.hpp"
#include "core/logger.hpp"

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

    VariantList save() 
    {
      VariantList ret;
      ret.push_back( sellPrice );
      ret.push_back( buyPrice );
      ret.push_back( exportLimit );
      ret.push_back( importLimit );
      ret.push_back( soldGoods );
      ret.push_back( bougthGoods );
      ret.push_back( stacking );
      ret.push_back( order );
      ret.push_back( vendor );

      return ret;
    }

    void load( const VariantList& stream )
    {
      if( stream.size() != 9 )
      {
        Logger::warning( "%s [%s %d]", "Incorrect argument number in ", __FILE__, __LINE__ );
        return;
      }

      VariantList::const_iterator it=stream.begin();
      sellPrice = it->toUInt(); ++it;
      buyPrice = it->toUInt(); ++it;
      exportLimit = it->toUInt(); ++it;
      importLimit = it->toUInt(); ++it;
      soldGoods = it->toUInt(); ++it;
      bougthGoods = it->toUInt(); ++it;
      stacking = it->toBool(); ++it;
      order = (CityTradeOptions::Order)it->toInt(); ++it;
      vendor = it->toBool();
    }
  };

  typedef std::map< Good::Type, GoodInfo > GoodsInfo;
  GoodsInfo goods;
  SimpleGoodStore buys, sells;

  void updateLists()
  {
    for( int i=Good::wheat; i < Good::goodCount; i++ )
    {
      Good::Type gtype = (Good::Type)i;
      const GoodInfo& info = goods[ gtype ];

      switch( info.order )
      {
      case CityTradeOptions::importing:
        buys.setCapacity( gtype, 9999 );
        sells.setCapacity( gtype, 0 );
      break;

      case CityTradeOptions::exporting:
        buys.setCapacity( gtype, 0 );
        sells.setCapacity( gtype, 9999 );
      break;

      case CityTradeOptions::noTrade:
        buys.setCapacity( gtype, 0 );
        buys.setCapacity( gtype, 0 );
      break;

      default: break;
      }
    }
  }

  Impl()
  {
    buys.setCapacity( 9999 );
    sells.setCapacity( 9999 );
    for( int i=Good::wheat; i < Good::goodCount; i++ )
    {
      Good::Type gtype = (Good::Type)i;
      goods[ gtype ].stacking = false;
      goods[ gtype ].order = CityTradeOptions::noTrade;
      goods[ gtype ].vendor = true;
      goods[ gtype ].exportLimit = 0;
    }

    goods[ Good::fish ].order = CityTradeOptions::disabled;
    goods[ Good::denaries ].order = CityTradeOptions::disabled;
  }

  void initStandartPrice( Good::Type type, int buy, int sell )
  {
    goods[ type ].buyPrice = buy; goods[ type ].sellPrice = sell;
  }

  void initStandartPrice()
  {
    initStandartPrice( Good::wheat, 28, 22 );
    initStandartPrice( Good::vegetable, 38, 30 );
    initStandartPrice( Good::fruit, 38, 30 );
    initStandartPrice( Good::olive, 42, 34 );
    initStandartPrice( Good::grape, 44, 36 );
    initStandartPrice( Good::meat, 44, 36 );
    initStandartPrice( Good::wine, 215, 160 );
    initStandartPrice( Good::oil, 180, 140 );
    initStandartPrice( Good::iron, 60, 40 );
    initStandartPrice( Good::timber, 50, 35 );
    initStandartPrice( Good::clay, 40, 30 );
    initStandartPrice( Good::marble, 200, 140 );
    initStandartPrice( Good::weapon, 250, 180 );
    initStandartPrice( Good::furniture, 200, 150 );
    initStandartPrice( Good::pottery, 180, 140 );
  }
};

CityTradeOptions::CityTradeOptions() : _d( new Impl )
{
  _d->initStandartPrice();
}

int CityTradeOptions::getExportLimit(Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.exportLimit );
}

CityTradeOptions::Order CityTradeOptions::getOrder( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? CityTradeOptions::noTrade : it->second.order);
}

CityTradeOptions::~CityTradeOptions()
{

}

CityTradeOptions::Order CityTradeOptions::switchOrder( Good::Type type )
{
  if( !isVendor( type ) )
  {
    switch( getOrder( type ) )
    {
    case noTrade: setOrder( type, importing ); break;
    case importing: setOrder( type, noTrade ); break;
    default: break;
    }
  }
  {
    switch( getOrder( type ) )
    {
    case noTrade: setOrder( type, exporting ); break;
    case exporting: setOrder( type, importing ); break;
    case importing: setOrder( type, noTrade ); break;
    default: break;
    }
  }

  _d->updateLists();

  return getOrder( type );
}

void CityTradeOptions::setExportLimit( Good::Type type, int qty )
{
  _d->goods[ type ].exportLimit = qty;

  _d->updateLists();
}

bool CityTradeOptions::isGoodsStacking( Good::Type type )
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.stacking );
}

void CityTradeOptions::setStackMode( Good::Type type, bool stackGoods )
{
  _d->goods[ type ].stacking = stackGoods;

  _d->updateLists();
}

unsigned int CityTradeOptions::getSellPrice( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.sellPrice );
}

void CityTradeOptions::setSellPrice( Good::Type type, unsigned int price )
{
   _d->goods[ type ].sellPrice = price;
}

unsigned int CityTradeOptions::getBuyPrice( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.buyPrice );
}

void CityTradeOptions::setBuyPrice( Good::Type type, unsigned int price )
{
  _d->goods[ type ].buyPrice = price;
}

bool CityTradeOptions::isVendor( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.vendor );
}

void CityTradeOptions::setVendor( Good::Type type, bool available )
{
  _d->goods[ type ].vendor = available;

  _d->updateLists();
}

void CityTradeOptions::setOrder( Good::Type type, Order order )
{
  _d->goods[ type ].order = order;

  _d->updateLists();
}

void CityTradeOptions::load( const VariantMap& stream )
{
  for( VariantMap::const_iterator it=stream.begin(); it != stream.end(); ++it )
  {
    Good::Type gtype = GoodHelper::getType( it->first );

    if( gtype == Good::none )
    {
      Logger::warning( "%s %s [%s %d]", "Can't convert type from ",
                           it->first.c_str(), __FILE__, __LINE__ );
      return;
    }

    _d->goods[ gtype ].load( it->second.toList() );
  }

  _d->updateLists();
}

VariantMap CityTradeOptions::save() const
{
  VariantMap ret;

  for( Impl::GoodsInfo::iterator it=_d->goods.begin(); it != _d->goods.end(); ++it )
  {
    ret[ GoodHelper::getTypeName( it->first ) ] = it->second.save();
  }

  return ret;
}

const GoodStore& CityTradeOptions::getBuys()
{
  return _d->buys;
}

const GoodStore& CityTradeOptions::getSells()
{
  return _d->sells;
}
