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
#include "good/goodhelper.hpp"
#include "good/goodstore_simple.hpp"
#include "core/variant_map.hpp"
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
      order = (trade::Order)it->toInt(); ++it;
      vendor = it->toBool();
    }
  };

  typedef std::map< good::Product, GoodInfo > GoodsInfo;
  GoodsInfo goods;
  good::SimpleStore buys, sells;

  void updateLists()
  {
    for( good::Product gtype=good::wheat; gtype < good::goodCount; ++gtype )
    {
      const GoodInfo& info = goods[ gtype ];

      switch( info.order )
      {
      case trade::importing:
        buys.setCapacity( gtype, 9999 );
        sells.setCapacity( gtype, 0 );
      break;

      case trade::exporting:
        buys.setCapacity( gtype, 0 );
        sells.setCapacity( gtype, 9999 );
      break;

      case trade::noTrade:
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
    for( good::Product gtype=good::wheat; gtype < good::goodCount; ++gtype )
    {
      goods[ gtype ].stacking = false;
      goods[ gtype ].order = trade::noTrade;
      goods[ gtype ].vendor = true;
      goods[ gtype ].exportLimit = 0;
      goods[ gtype ].sellPrice = 0;
      goods[ gtype ].buyPrice = 0;
    }

    goods[ good::fish ].order = trade::disabled;
    goods[ good::denaries ].order = trade::disabled;
  }
};

Options::Options() : _d( new Impl )
{  
}

unsigned int Options::tradeLimit( Order state, good::Product type) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  if( it == _d->goods.end() )
    return 0;

  switch( state )
  {
  case importing: return it->second.importLimit;
  case exporting: return it->second.exportLimit;
  default: return 0;
  }
}

Order Options::getOrder( good::Product type ) const
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? trade::noTrade : it->second.order);
}

Options::~Options()
{

}

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

void Options::setTradeLimit( Order o, good::Product type, unsigned int qty)
{
  switch( o )
  {
  case exporting: _d->goods[ type ].exportLimit = qty; break;
  case importing: _d->goods[ type ].importLimit = qty; break;
  default: break;
  }

  _d->updateLists();
}

bool Options::isGoodsStacking( good::Product type )
{
  Impl::GoodsInfo::const_iterator it = _d->goods.find( type );
  return ( it == _d->goods.end() ? false : it->second.stacking );
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
  for( VariantMap::const_iterator it=stream.begin(); it != stream.end(); ++it )
  {
    good::Product gtype = good::Helper::getType( it->first );

    if( gtype == good::none )
    {
      Logger::warning( "%s %s [%s %d]", "Can't convert type from ",
                           it->first.c_str(), __FILE__, __LINE__ );
    }

    _d->goods[ gtype ].load( it->second.toList() );
  }

  _d->updateLists();
}

VariantMap Options::save() const
{
  VariantMap ret;

  for( Impl::GoodsInfo::iterator it=_d->goods.begin(); it != _d->goods.end(); ++it )
  {
    ret[ good::Helper::getTypeName( it->first ) ] = it->second.save();
  }

  return ret;
}

const good::Store& Options::importingGoods() {  return _d->buys; }
const good::Store& Options::exportingGoods() { return _d->sells; }

}//end namespace trade

}//end namespace city
