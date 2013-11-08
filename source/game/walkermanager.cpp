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
#include "core/logger.hpp"
#include "walker/animals.hpp"
#include "walker/fishing_boat.hpp"
#include "walker/constants.hpp"
#include "walker/corpse.hpp"
#include "walker/rioter.hpp"
#include <map>

using namespace constants;

template< class T >
class BaseWalkerCreator : public WalkerCreator
{
public:
  virtual WalkerPtr create( CityPtr city )
  {
    return T::create( city ).object();
  }
};

class ServiceWalkerCreator : public WalkerCreator
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

class TraineeWalkerCreator : public WalkerCreator
{
public:
  WalkerPtr create( CityPtr city )
  {
    return TraineeWalker::create( city, walker::trainee ).object();
  }
};

class WalkerManager::Impl
{
public:
  typedef std::map< walker::Type, WalkerCreator* > WalkerCreators;
  WalkerCreators constructors;
};

WalkerManager::WalkerManager() : _d( new Impl )
{
  addCreator( walker::emigrant, new BaseWalkerCreator<Emigrant>() );
  addCreator( walker::immigrant, new BaseWalkerCreator<Immigrant>() );
  addCreator( walker::cartPusher, new BaseWalkerCreator<CartPusher>() );
  addCreator( walker::prefect, new BaseWalkerCreator<Prefect>() );
  addCreator( walker::taxCollector, new BaseWalkerCreator<TaxCollector>() );
  addCreator( walker::engineer, new ServiceWalkerCreator( Service::engineer ));
  addCreator( walker::doctor, new ServiceWalkerCreator( Service::doctor ) );
  addCreator( walker::sheep, new BaseWalkerCreator< Sheep >() );
  addCreator( walker::bathlady, new ServiceWalkerCreator( Service::baths ) );
  addCreator( walker::actor, new ServiceWalkerCreator( Service::theater ) );
  addCreator( walker::gladiator, new ServiceWalkerCreator( Service::amphitheater ) );
  addCreator( walker::barber, new ServiceWalkerCreator( Service::barber ) );
  addCreator( walker::surgeon, new ServiceWalkerCreator( Service::hospital ) );
  addCreator( walker::trainee, new TraineeWalkerCreator() );
  addCreator( walker::fishingBoat, new BaseWalkerCreator<FishingBoat>() );
  addCreator( walker::corpse, new BaseWalkerCreator<Corpse>() );
  addCreator( walker::rioter, new BaseWalkerCreator<Rioter>() );
}

WalkerManager::~WalkerManager()
{

}

WalkerPtr WalkerManager::create(const walker::Type walkerType , CityPtr city)
{
  Impl::WalkerCreators::iterator findConstructor = _d->constructors.find( walkerType );

  if( findConstructor != _d->constructors.end() )
  {
    return findConstructor->second->create( city ).as<Walker>();
  }

  Logger::warning( "Can't create walker from type %d", walkerType );
  return WalkerPtr();
}

WalkerManager& WalkerManager::getInstance()
{
  static WalkerManager inst;
  return inst;
}

void WalkerManager::addCreator( const walker::Type type, WalkerCreator* ctor )
{
  std::string typeName = WalkerHelper::getName( type );

  bool alreadyHaveConstructor = _d->constructors.find( type ) != _d->constructors.end();
  if( alreadyHaveConstructor )
  {
    Logger::warning( "Already have constructor for type %s", typeName.c_str() );
    return;
  }
  else
  {
    _d->constructors[ type ] = ctor;
  }
}

bool WalkerManager::canCreate( const walker::Type type ) const
{
  return _d->constructors.find( type ) != _d->constructors.end();   
}
