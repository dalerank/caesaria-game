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
#include "oc3_walker_trainee.hpp"
#include "oc3_walker_market_lady.hpp"
#include "oc3_walker_cart_pusher.hpp"
#include "oc3_walker_prefect.hpp"
#include "oc3_walker_emigrant.hpp"
#include "oc3_walker_taxcollector.hpp"
#include "oc3_city.hpp"
#include "oc3_name_generator.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_walker_animals.hpp"
#include <map>

template< class T >
class WalkerCreator : public AbstractWalkerCreator
{
public:
  virtual WalkerPtr create( CityPtr city )
  {
    return T::create( city ).object();
  }
};

class ServiceWalkerCreator : public AbstractWalkerCreator
{
public:
  WalkerPtr create( CityPtr city )
  {
    return ServiceWalker::create( city, serviceType ).object();
  }

  ServiceWalkerCreator( const Service::Type type )
  {
    serviceType = type;
  }

  Service::Type serviceType;
};

class WalkerManager::Impl
{
public:
  typedef std::map< WalkerType, AbstractWalkerCreator* > WalkerCreators;
  WalkerCreators constructors;
};

WalkerManager::WalkerManager() : _d( new Impl )
{
  addCreator( WT_EMIGRANT, new WalkerCreator<Emigrant>() );
  addCreator( WT_IMMIGRANT, new WalkerCreator<Immigrant>() );
  addCreator( WT_CART_PUSHER, new WalkerCreator<CartPusher>() );
  addCreator( WT_PREFECT, new WalkerCreator<WalkerPrefect>() );
  addCreator( WT_TAXCOLLECTOR, new WalkerCreator<TaxCollector>() );
  addCreator( WT_ENGINEER, new ServiceWalkerCreator( Service::S_ENGINEER ));
  addCreator( WT_DOCTOR, new ServiceWalkerCreator( Service::S_DOCTOR ) );
  addCreator( WT_ANIMAL_SHEEP, new WalkerCreator< Sheep >() );
}

WalkerManager::~WalkerManager()
{

}

WalkerPtr WalkerManager::create(const WalkerType walkerType , CityPtr city)
{
  Impl::WalkerCreators::iterator findConstructor = _d->constructors.find( walkerType );

  if( findConstructor != _d->constructors.end() )
  {
    return findConstructor->second->create( city ).as<Walker>();
  }

  StringHelper::debug( 0xff, "Can't create walker from type %d", walkerType );
  return WalkerPtr();
}

WalkerManager& WalkerManager::getInstance()
{
  static WalkerManager inst;
  return inst;
}

void WalkerManager::addCreator( const WalkerType type, AbstractWalkerCreator* ctor )
{
  std::string typeName = WalkerHelper::getName( type );

  bool alreadyHaveConstructor = _d->constructors.find( type ) != _d->constructors.end();
  if( alreadyHaveConstructor )
  {
    StringHelper::debug( 0xff, "Already have constructor for type %s", typeName.c_str() );
    return;
  }
  else
  {
    _d->constructors[ type ] = ctor;
  }
}

bool WalkerManager::canCreate( const WalkerType type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
