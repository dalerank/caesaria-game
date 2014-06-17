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

namespace city
{

class TradeOptions::Impl
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
    TradeOptions::Order order;
    bool vendor;

    VariantList save() 
    {
      VariantList ret;
      ret << sellPrice
          << buyPrice
          << exportLimit
          << importLimit
          << soldGoods
          << bougthGoods
          << stacking
          << order
          << vendor;

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
      order = (TradeOptions::Order)it->toInt(); ++it;
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
      case TradeOptions::importing:
        buys.setCapacity( gtype, 9999 );
        sells.setCapacity( gtype, 0 );
      break;

      case TradeOptions::exporting:
        buys.setCapacity( gtype, 0 );
        sells.setCapacity( gtype, 9999 );
      break;

      case TradeOptions::noTrade:
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
      goods[ gtype ].order = TradeOptions::noTrade;
      goods[ gtype ].vendor = true;
      goods[ gtype ].exportLimit = 0;
      goods[ gtype ].sellPrice = 0;
      goods[ gtype ].buyPrice = 0;
    }

    goods[ Good::fish ].order = TradeOptions::disabled;
    goods[ Good::denaries ].order = TradeOptions::disabled;
  }
};

TradeOptions::TradeOptions() : _d( new Impl )
{  
}

unsigned int TradeOptions::exportLimit(Good::Type type) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.exportLimit );
}

TradeOptions::Order TradeOptions::getOrder( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? TradeOptions::noTrade : it->second.order);
}

TradeOptions::~TradeOptions()
{

}

TradeOptions::Order TradeOptions::switchOrder( Good::Type type )
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

void TradeOptions::setExportLimit(Good::Type type, unsigned int qty)
{
  _d->goods[ type ].exportLimit = qty;

  _d->updateLists();
}

bool TradeOptions::isGoodsStacking( Good::Type type )
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.stacking );
}

bool TradeOptions::isExporting( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  if (it == _d->goods.end())
  {
    return false;
  }
  else
  {
    return !it->second.stacking && it->second.order == exporting ;
  }
}

bool TradeOptions::isImporting( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  if (it == _d->goods.end())
  {
    return false;
  }
  else
  {
    return it->second.order == importing ;
  }
}

void TradeOptions::setStackMode( Good::Type type, bool stackGoods )
{
  _d->goods[ type ].stacking = stackGoods;

  _d->updateLists();
}

unsigned int TradeOptions::sellPrice( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.sellPrice );
}

void TradeOptions::setSellPrice( Good::Type type, unsigned int price )
{
   _d->goods[ type ].sellPrice = price;
}

unsigned int TradeOptions::buyPrice( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.buyPrice );
}

void TradeOptions::setBuyPrice( Good::Type type, unsigned int price )
{
  _d->goods[ type ].buyPrice = price;
}

bool TradeOptions::isVendor( Good::Type type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.vendor );
}

void TradeOptions::setVendor( Good::Type type, bool available )
{
  _d->goods[ type ].vendor = available;

  _d->updateLists();
}

void TradeOptions::setOrder( Good::Type type, Order order )
{
  _d->goods[ type ].order = order;

  _d->updateLists();
}

void TradeOptions::load( const VariantMap& stream )
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

VariantMap TradeOptions::save() const
{
  VariantMap ret;

  for( Impl::GoodsInfo::iterator it=_d->goods.begin(); it != _d->goods.end(); ++it )
  {
    ret[ GoodHelper::getTypeName( it->first ) ] = it->second.save();
  }

  return ret;
}

const GoodStore& TradeOptions::importingGoods()
{
  return _d->buys;
}

const GoodStore& TradeOptions::exportingGoods()
{
  return _d->sells;
}

}//end namespace city
