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
#include "oc3_traineewalker.hpp"
#include "oc3_walker_market_buyer.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_emigrant.hpp"
#include "oc3_scenario.hpp"
#include <map>

template< class T >
class WalkerMigrantCreator : public AbstractWalkerCreator
{
public:
  T* create()
  {
    SmartPtr<T> ret = T::create( Scenario::instance().getCity() ); 
    ret->grab();

    return ret.object();
  }
};

class WalkerManager::Impl
{
public:
  typedef std::map< WalkerType, AbstractWalkerCreator* > WalkerCreators;
  std::map< std::string, WalkerType > name2typeMap;
  WalkerCreators constructors;
};

WalkerManager::WalkerManager() : _d( new Impl )
{
  addCreator( WT_EMIGRANT, OC3_STR_EXT(WT_EMIGRANT), new WalkerMigrantCreator<Emigrant>() );
  addCreator( WT_IMMIGRANT, OC3_STR_EXT(WT_IMMIGRANT), new WalkerMigrantCreator<Immigrant>() );
}

WalkerManager::~WalkerManager()
{

}

WalkerPtr WalkerManager::create( const WalkerType walkerType )
{
  Impl::WalkerCreators::iterator findConstructor = _d->constructors.find( walkerType );

  if( findConstructor != _d->constructors.end() )
  {
    WalkerPtr ret( findConstructor->second->create() );
    ret->drop();
    return ret;
  }

  return WalkerPtr();
}

WalkerManager& WalkerManager::getInstance()
{
  static WalkerManager inst;
  return inst;
}

void WalkerManager::addCreator( const WalkerType type, const std::string& typeName, AbstractWalkerCreator* ctor )
{
  bool alreadyHaveConstructor = _d->name2typeMap.find( typeName ) != _d->name2typeMap.end();
  _OC3_DEBUG_BREAK_IF( alreadyHaveConstructor && "already have constructor for this type");

  if( !alreadyHaveConstructor )
  {
    _d->name2typeMap[ typeName ] = type;
    _d->constructors[ type ] = ctor;
  }
}

bool WalkerManager::canCreate( const WalkerType type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}