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
#include "walker/protestor.hpp"
#include "walker/enemysoldier.hpp"
#include "walker/romesoldier.hpp"
#include "walker/lion.hpp"
#include "walker/lion_tamer.hpp"
#include <map>

using namespace constants;

template< class T >
class BaseCreator : public WalkerCreator
{
public:
  virtual WalkerPtr create( PlayerCityPtr city )
  {
    return T::create( city ).object();
  }
};

class ServicemanCreator : public WalkerCreator
{
public:
  WalkerPtr create( PlayerCityPtr city )
  {
    return ServiceWalker::create( city, serviceType ).object();
  }

  ServicemanCreator( const Service::Type type )
  {
    serviceType = type;
  }

  Service::Type serviceType;
};

template< class T >
class SoldierCreator : public WalkerCreator
{
public:
  WalkerPtr create( PlayerCityPtr city )
  {
    return T::create( city, rtype ).object();
  }

  SoldierCreator( const walker::Type type )
  {
    rtype = type;
  }

  walker::Type rtype;
};

class TraineeWalkerCreator : public WalkerCreator
{
public:
  WalkerPtr create( PlayerCityPtr city )
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
  addCreator( walker::emigrant, new BaseCreator<Emigrant>() );
  addCreator( walker::immigrant, new BaseCreator<Immigrant>() );
  addCreator( walker::cartPusher, new BaseCreator<CartPusher>() );
  addCreator( walker::prefect, new BaseCreator<Prefect>() );
  addCreator( walker::taxCollector, new BaseCreator<TaxCollector>() );
  addCreator( walker::engineer, new ServicemanCreator( Service::engineer ));
  addCreator( walker::doctor, new ServicemanCreator( Service::doctor ) );
  addCreator( walker::sheep, new BaseCreator< Sheep >() );
  addCreator( walker::bathlady, new ServicemanCreator( Service::baths ) );
  addCreator( walker::actor, new ServicemanCreator( Service::theater ) );
  addCreator( walker::gladiator, new ServicemanCreator( Service::amphitheater ) );
  addCreator( walker::barber, new ServicemanCreator( Service::barber ) );
  addCreator( walker::surgeon, new ServicemanCreator( Service::hospital ) );
  addCreator( walker::trainee, new TraineeWalkerCreator() );
  addCreator( walker::fishingBoat, new BaseCreator<FishingBoat>() );
  addCreator( walker::corpse, new BaseCreator<Corpse>() );
  addCreator( walker::protestor, new BaseCreator<Protestor>() );
  addCreator( walker::britonSoldier, new SoldierCreator<EnemySoldier>( walker::britonSoldier ) );
  addCreator( walker::legionary, new SoldierCreator<RomeSoldier>( walker::legionary ) );
  addCreator( walker::lion, new BaseCreator< Lion >() );
  addCreator( walker::lionTamer, new BaseCreator< LionTamer >() );
}

WalkerManager::~WalkerManager()
{

}

WalkerPtr WalkerManager::create(const walker::Type walkerType, PlayerCityPtr city)
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
