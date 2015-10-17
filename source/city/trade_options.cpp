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
#include "core/utils.hpp"
#include "good/helper.hpp"
#include "good/storage.hpp"
#include "core/variant_map.hpp"
#include "core/variant_list.hpp"
#include "core/logger.hpp"

namespace city
{

namespace trade
{

class Options::Impl
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
    trade::Order order;
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
        Logger::warning( "{0} [{1} {2}]", "Incorrect argument number in ", __FILE__, __LINE__ );
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
      order = (trade::Order)it->toInt(); ++it;
      vendor = it->toBool();
    }
  };

  typedef std::map< good::Product, GoodInfo > GoodsInfo;
  GoodsInfo goods;
  good::Storage buys, sells;

  void updateLists()
  {
    for( auto& gtype : good::all() )
    {
      const GoodInfo& info = goods[ gtype ];

      switch( info.order )
      {
      case trade::importing:
        buys.setCapacity( gtype , 9999 );
        sells.setCapacity( gtype , 0 );
      break;

      case trade::exporting:
        buys.setCapacity( gtype , 0 );
        sells.setCapacity( gtype , 9999 );
      break;

      case trade::noTrade:
        buys.setCapacity( gtype , 0 );
        buys.setCapacity( gtype , 0 );
      break;

      default: break;
      }
    }
  }

  Impl()
  {
    buys.setCapacity( 9999 );
    sells.setCapacity( 9999 );
    for( auto& gtype : good::all() )
    {
      goods[ gtype  ].stacking = false;
      goods[ gtype  ].order = trade::noTrade;
      goods[ gtype  ].vendor = true;
      goods[ gtype  ].exportLimit = 0;
      goods[ gtype  ].sellPrice = 0;
      goods[ gtype  ].buyPrice = 0;
    }

    goods[ good::fish ].order = trade::disabled;
    goods[ good::denaries ].order = trade::disabled;
  }
};

Options::Options() : _d( new Impl )
{  
}

metric::Unit Options::tradeLimit( Order state, good::Product type) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  metric::Unit ret = metric::Unit::fromQty( 0 );
  if( it == _d->goods.end() )
    return ret;

  switch( state )
  {
  case importing: ret = metric::Unit::fromValue( it->second.importLimit ); break;
  case exporting: ret = metric::Unit::fromValue( it->second.exportLimit ); break;
  default: break;
  }

  return ret;
}

Order Options::getOrder( good::Product type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? trade::noTrade : it->second.order);
}

Options::~Options() {}

Order Options::switchOrder( good::Product type )
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
  else
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

void Options::setTradeLimit(Order o, good::Product type, metric::Unit qty)
{
  switch( o )
  {
  case exporting:
    _d->goods[ type ].exportLimit = qty.ivalue();
    _d->sells.setCapacity( type, qty.ivalue() );
  break;

  case importing:
    _d->goods[ type ].importLimit = qty.ivalue();
    _d->buys.setCapacity( type, qty.ivalue() );
  break;

  default: break;
  }

  _d->updateLists();
}

bool Options::isStacking( good::Product type )
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.stacking );
}

good::Products Options::locked() const
{
  good::Products ret;
  for( auto& g : _d->goods )
  {
    if( g.second.stacking )
      ret.insert( g.first );
  }

  return ret;
}

bool Options::isExporting( good::Product type ) const
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

bool Options::isImporting( good::Product type ) const
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

void Options::setStackMode( good::Product type, bool stackGoods )
{
  _d->goods[ type ].stacking = stackGoods;

  _d->updateLists();
}

unsigned int Options::sellPrice( good::Product type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.sellPrice );
}

void Options::setSellPrice( good::Product type, unsigned int price )
{
   _d->goods[ type ].sellPrice = price;
}

unsigned int Options::buyPrice( good::Product type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? 0 : it->second.buyPrice );
}

void Options::setBuyPrice( good::Product type, unsigned int price )
{
  _d->goods[ type ].buyPrice = price;
}

bool Options::isVendor( good::Product type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.vendor );
}

void Options::setVendor( good::Product type, bool available )
{
  _d->goods[ type ].vendor = available;

  _d->updateLists();
}

void Options::setOrder( good::Product type, Order order )
{
  _d->goods[ type ].order = order;

  _d->updateLists();
}

void Options::load( const VariantMap& stream )
{
  for( auto& it : stream )
  {
    good::Product gtype = good::Helper::getType( it.first );

    if( gtype == good::none )
    {
      Logger::warning( "{0} {1} [{2} {3}]",
                       "Can't convert type from ",
                       it.first, __FILE__, __LINE__ );
    }

    _d->goods[ gtype ].load( it.second.toList() );
  }

  _d->updateLists();
}

VariantMap Options::save() const
{
  VariantMap ret;

  for( auto& product : _d->goods )
  {
    ret[ good::Helper::getTypeName( product.first ) ] = product.second.save();
  }

  return ret;
}

const good::Store& Options::buys() {  return _d->buys; }
const good::Store& Options::sells() { return _d->sells; }

}//end namespace trade

}//end namespace city
