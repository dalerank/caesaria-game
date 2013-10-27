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

#include "walkermanager.hpp"
#include "walker/trainee.hpp"
#include "walker/market_lady.hpp"
#include "walker/cart_pusher.hpp"
#include "walker/prefect.hpp"
#include "walker/emigrant.hpp"
#include "walker/taxcollector.hpp"
#include "city.hpp"
#include "name_generator.hpp"
#include "core/stringhelper.hpp"
#include "walker/animals.hpp"
#include "walker/fishing_boat.hpp"
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

class TraineeWalkerCreator : public AbstractWalkerCreator
{
public:
  WalkerPtr create( CityPtr city )
  {
    return TraineeWalker::create( city, WT_TRAINEE ).object();
  }
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
  addCreator( WT_ENGINEER, new ServiceWalkerCreator( Service::engineer ));
  addCreator( WT_DOCTOR, new ServiceWalkerCreator( Service::doctor ) );
  addCreator( WT_ANIMAL_SHEEP, new WalkerCreator< Sheep >() );
  addCreator( WT_BATHLADY, new ServiceWalkerCreator( Service::baths ) );
  addCreator( WT_ACTOR, new ServiceWalkerCreator( Service::theater ) );
  addCreator( WT_GLADIATOR, new ServiceWalkerCreator( Service::amphitheater ) );
  addCreator( WT_BARBER, new ServiceWalkerCreator( Service::barber ) );
  addCreator( WT_SURGEON, new ServiceWalkerCreator( Service::hospital ) );
  addCreator( WT_TRAINEE, new TraineeWalkerCreator() );
  addCreator( WT_FISHING_BOAT, new WalkerCreator<FishingBoat>() );
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
