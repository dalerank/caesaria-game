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

#include "traderoute.hpp"
#include "empire.hpp"
#include "merchant.hpp"
#include "city.hpp"
#include "game/goodstore_simple.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

namespace world
{

class TradeRoute::Impl
{
oc3_signals public:
  Signal1<MerchantPtr> onMerchantArrivedSignal;
public:
  CityPtr begin;
  CityPtr end;
  typedef std::vector<MerchantPtr> MerchantList;
  MerchantList merchants;

  void resolveMerchantArrived( MerchantPtr merchant )
  {
    foreach( MerchantPtr m, merchants )
    {
      if( m == merchant )
      {
        m->deleteLater();
        break;
      }
    }

    onMerchantArrivedSignal.emit( merchant );
  }
};

CityPtr TradeRoute::getBeginCity() const
{
  return _d->begin;  
}

CityPtr TradeRoute::getEndCity() const
{
  return _d->end;
}

std::string TradeRoute::getName() const
{
  return _d->begin->getName() + "<->" + _d->end->getName();
}

void TradeRoute::update( unsigned int time )
{
  Impl::MerchantList::iterator it=_d->merchants.begin();
  while( it != _d->merchants.end() )
  {
    if( (*it)->isDeleted() )
    {
      it = _d->merchants.erase( it );
    }
    else
    {
      (*it)->update( time );
      it++;
    }
  }
}

void TradeRoute::addMerchant( const std::string& begin, GoodStore& sell, GoodStore& buy )
{
  MerchantPtr merchant = Merchant::create( *this, begin, sell, buy );
  _d->merchants.push_back( merchant );  

  CONNECT( merchant, onDestination(), _d.data(), Impl::resolveMerchantArrived );
}

TradeRoute::TradeRoute(CityPtr begin, CityPtr end )
: _d( new Impl )
{
  _d->begin = begin;
  _d->end = end;
}

TradeRoute::~TradeRoute()
{

}

MerchantPtr TradeRoute::getMerchant( unsigned int index )
{
  if( index >= _d->merchants.size() )
    return 0;

  Impl::MerchantList::iterator it = _d->merchants.begin();
  std::advance( it, index );
  return *it;
}

VariantMap TradeRoute::save() const
{  
  VariantMap ret;
  int index=0;
  foreach( MerchantPtr m, _d->merchants )
  {
    ret[ StringHelper::format( 0xff, "%d", index++ ) ] = m->save();
  }

  return ret;
}

void TradeRoute::load(const VariantMap& stream)
{
  for( VariantMap::const_iterator it=stream.begin(); it != stream.end(); it++ )
  {
    SimpleGoodStore sell, buy;
    addMerchant( _d->begin->getName(), sell, buy );

    _d->merchants.back()->load( it->second.toMap() );
  }
}

Signal1<MerchantPtr>& TradeRoute::onMerchantArrived()
{
  return _d->onMerchantArrivedSignal;
}

} // end namespace world
