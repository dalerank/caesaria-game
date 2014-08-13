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
//
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "walkers_factory.hpp"
#include "trainee.hpp"
#include "market_buyer.hpp"
#include "market_kid.hpp"
#include "cart_pusher.hpp"
#include "prefect.hpp"
#include "emigrant.hpp"
#include "taxcollector.hpp"
#include "city/city.hpp"
#include "name_generator.hpp"
#include "core/logger.hpp"
#include "animals.hpp"
#include "fishing_boat.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "rioter.hpp"
#include "enemysoldier.hpp"
#include "romesoldier.hpp"
#include "lion.hpp"
#include "lion_tamer.hpp"
#include "fish_place.hpp"
#include "seamerchant.hpp"
#include "merchant.hpp"
#include "engineer.hpp"
#include "workerhunter.hpp"
#include "spear.hpp"
#include "bow_arrow.hpp"
#include "enemyarcher.hpp"
#include "helper.hpp"
#include "watergarbage.hpp"
#include "immigrant.hpp"
#include "dustcloud.hpp"
#include "mugger.hpp"
#include "indigene.hpp"
#include "chastener.hpp"
#include "chastener_elephant.hpp"
#include "romearcher.hpp"
#include "market_lady.hpp"
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
  WalkerPtr create( PlayerCityPtr city )   {  return ServiceWalker::create( city, serviceType ).object();  }
  ServicemanCreator( const Service::Type type )  {    serviceType = type;  }
  Service::Type serviceType;
};

template< class T >
class SoldierCreator : public WalkerCreator
{
public:
  WalkerPtr create( PlayerCityPtr city )   { return T::create( city, rtype ).object();  }
  SoldierCreator( const walker::Type type )  {    rtype = type;  }
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
  addCreator( walker::priest, new ServicemanCreator( Service::religionCeres ) );
  addCreator( walker::taxCollector, new BaseCreator<TaxCollector>() );
  addCreator( walker::engineer, new BaseCreator< Engineer >() );
  addCreator( walker::doctor, new ServicemanCreator( Service::doctor ) );
  addCreator( walker::sheep, new BaseCreator< Sheep >() );
  addCreator( walker::marketBuyer, new BaseCreator< MarketBuyer >() );
  addCreator( walker::marketKid, new BaseCreator< MarketKid >() );
  addCreator( walker::marketLady, new BaseCreator< MarketLady >() );
  addCreator( walker::bathlady, new ServicemanCreator( Service::baths ) );
  addCreator( walker::actor, new ServicemanCreator( Service::theater ) );
  addCreator( walker::gladiator, new ServicemanCreator( Service::amphitheater ) );
  addCreator( walker::barber, new ServicemanCreator( Service::barber ) );
  addCreator( walker::surgeon, new ServicemanCreator( Service::hospital ) );
  addCreator( walker::trainee, new TraineeWalkerCreator() );
  addCreator( walker::fishingBoat, new BaseCreator<FishingBoat>() );
  addCreator( walker::corpse, new BaseCreator<Corpse>() );
  addCreator( walker::merchant, new BaseCreator<Merchant>() );
  addCreator( walker::rioter, new BaseCreator<Rioter>() );
  addCreator( walker::britonSoldier, new SoldierCreator<EnemySoldier>( walker::britonSoldier ) );
  addCreator( walker::legionary, new SoldierCreator<RomeSoldier>( walker::legionary ) );
  addCreator( walker::romeHorseman, new SoldierCreator<RomeSoldier>( walker::romeHorseman ) );
  addCreator( walker::lion, new BaseCreator< Lion >() );
  addCreator( walker::lionTamer, new BaseCreator< LionTamer >() );
  addCreator( walker::fishPlace, new BaseCreator< FishPlace >() );
  addCreator( walker::seaMerchant, new BaseCreator<SeaMerchant>() );
  addCreator( walker::scholar, new ServicemanCreator( Service::school ) );
  addCreator( walker::librarian, new ServicemanCreator( Service::library ) );
  addCreator( walker::teacher, new ServicemanCreator( Service::academy ) );
  addCreator( walker::recruter, new BaseCreator<Recruter>() );
  addCreator( walker::spear, new BaseCreator<Spear>() );
  addCreator( walker::waterGarbage, new BaseCreator<WaterGarbage>() );
  addCreator( walker::bow_arrow, new BaseCreator<BowArrow>() );
  addCreator( walker::etruscanSoldier, new SoldierCreator<EnemySoldier>( walker::etruscanSoldier ) );
  addCreator( walker::etruscanArcher, new SoldierCreator<EnemyArcher>( walker::etruscanArcher ) );
  addCreator( walker::mugger, new BaseCreator<Mugger>() );
  addCreator( walker::dustCloud, new BaseCreator<DustCloud>() );
  addCreator( walker::wolf, new BaseCreator<Wolf>() );
  addCreator( walker::zebra, new BaseCreator<Zebra>() );
  addCreator( walker::missioner, new ServicemanCreator( Service::missionary ) );
  addCreator( walker::indigene, new BaseCreator<Indigene>() );
  addCreator( walker::romeSpearman, new SoldierCreator<RomeArcher>( walker::romeSpearman ) );
  addCreator( walker::romeChastenerSoldier, new SoldierCreator<Chastener>( walker::romeChastenerSoldier ) );
  addCreator( walker::romeChastenerElephant, new BaseCreator<ChastenerElephant>() );
}

WalkerManager::~WalkerManager(){}

WalkerPtr WalkerManager::create(const walker::Type walkerType, PlayerCityPtr city)
{
  Impl::WalkerCreators::iterator findConstructor = _d->constructors.find( walkerType );

  if( findConstructor != _d->constructors.end() )
  {
    return ptr_cast<Walker>( findConstructor->second->create( city ) );
  }

  Logger::warning( "WalkerManager: cannot create walker from type %d", walkerType );
  return WalkerPtr();
}

WalkerManager& WalkerManager::instance()
{
  static WalkerManager inst;
  return inst;
}

void WalkerManager::addCreator( const walker::Type type, WalkerCreator* ctor )
{
  std::string typeName = WalkerHelper::getTypename( type );

  bool alreadyHaveConstructor = _d->constructors.find( type ) != _d->constructors.end();
  if( alreadyHaveConstructor )
  {
    Logger::warning( "WalkerManager: also have constructor for type %s", typeName.c_str() );
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
