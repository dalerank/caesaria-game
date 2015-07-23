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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "building.hpp"

#include "gfx/tile.hpp"
#include "walker/serviceman.hpp"
#include "core/exception.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "walker/trainee.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "gfx/tilemap.hpp"
#include "city/statistic.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "constants.hpp"
#include "game/gamedate.hpp"
#include "city/states.hpp"
#include "walker/typeset.hpp"

using namespace gfx;
using namespace city;

namespace {
static Renderer::PassQueue buildingPassQueue=Renderer::PassQueue(1,Renderer::overlayAnimation);
}

struct CityKoeffs
{
  float fireRisk;
  float collapseRisk;

  CityKoeffs() : fireRisk( 1.f), collapseRisk( 1.f ) {}
};

class ReservedServices : public std::map<Service::Type, DateTime>
{
public:
  bool contain( Service::Type type ) const
  {
    return count( type ) > 0;
  }

  VariantList save() const
  {
    VariantList ret;
    for( auto item : *this )
      ret.emplace_back( VariantList(item.first, item.second) );

    return ret;
  }

  void load( const VariantList& stream )
  {
    for( auto item : stream )
    {
      VariantList vl = item.toList();
      Service::Type stype = vl.get( 0 ).toEnum<Service::Type>();
      DateTime value = vl.get( 1 ).toDateTime();
      (*this)[ stype ] = value;
    }
  }

  void reserve( Service::Type type )
  {
    (*this)[ type ] = game::Date::current();
  }

  void removeExpired( int days=1 )
  {
    DateTime current = game::Date::current();
    for( iterator it = begin(); it != end(); )
    {
      if( it->second.daysTo( current ) > days ) { it = erase( it );}
      else { ++it; }
    }
  }
};

class TraineeMap : public std::map<walker::Type,int>
{
public:
  VariantList save() const
  {
    VariantList ret;
    for( auto item : *this )
      ret.emplace_back( VariantList(item.first, item.second) );

    return ret;
  }

  void load( const VariantList& stream )
  {
    for( auto item : stream )
    {
      VariantList vl = item.toList();
      walker::Type wtype = vl.get( 0 ).toEnum<walker::Type>();
      int value = vl.get( 1 );
      (*this)[ wtype ] = value;
    }
  }
};

class Building::Impl
{
public:
  TraineeMap trainees;  // current level of trainees working in the building (0..200)
  WalkerTypeSet reservedTrainees;  // a trainee is on the way
  ReservedServices reservedServices;  // a serviceWalker is on the way

  int stateDecreaseInterval;
  CityKoeffs cityKoeffs;
};

Building::Building(const object::Type type, const Size& size )
: Construction( type, size ), _d( new Impl )
{
  setState( pr::inflammability, 1 );
  setState( pr::collapsibility, 1 );
  _d->stateDecreaseInterval = game::Date::days2ticks( 1 );
}

Building::~Building() {}

void Building::initTerrain( Tile& ) {}

void Building::timeStep(const unsigned long time)
{
  if( game::Date::isWeekChanged() )
  {
    _updateBalanceKoeffs();
  }

  if( time % _d->stateDecreaseInterval == 1 )
  {
    updateState( pr::fire,   _d->cityKoeffs.fireRisk     * state( pr::inflammability ) );
    updateState( pr::damage, _d->cityKoeffs.collapseRisk * state( pr::collapsibility ) );
  }

  Construction::timeStep(time);
}

void Building::storeGoods(good::Stock &stock, const int amount)
{
  std::string bldType = debugName();
  Logger::warning( "This building should not store any goods %s at [%d,%d]",
                   bldType.c_str(), pos().i(), pos().j() );
}

float Building::evaluateService(ServiceWalkerPtr walker)
{
   float res = 0.0;
   Service::Type service = walker->serviceType();
   if(_d->reservedServices.contain(service))
   {
      // service is already reserved
      return 0.0;
   }

   switch(service)
   {
   case Service::engineer: res = state( pr::damage ); break;
   case Service::prefect: res = state( pr::fire ); break;
   default: break;
   }
   return res;
}

bool Building::build( const city::AreaInfo& info )
{
  Construction::build( info );

  switch( info.city->climate() )
  {
  case game::climate::northen: setState( pr::inflammability, 0.5 ); break;
  case game::climate::desert: setState( pr::inflammability, 2 ); break;
  default: break;
  }

  _updateBalanceKoeffs();

  return true;
}

void Building::reserveService(const Service::Type service) { _d->reservedServices.reserve(service); }
bool Building::isServiceReserved( const Service::Type service ) { return _d->reservedServices.contain(service); }
void Building::cancelService(const Service::Type service){ _d->reservedServices.erase(service);}

void Building::applyService( ServiceWalkerPtr walker)
{
  // std::cout << "apply service" << std::endl;
  // remove service reservation
  Service::Type service = walker->serviceType();
  _d->reservedServices.erase(service);

  switch( service )
  {
  case Service::engineer: setState( pr::damage, 0 ); break;
  case Service::prefect: setState( pr::fire, 0 ); break;
  default: break;
  }
}

float Building::evaluateTrainee(walker::Type traineeType)
{
   float res = 0.0;

   if( _d->reservedTrainees.count(traineeType) == 1 )
   {
      // don't allow two reservations of the same type
      return 0.0;
   }

   int trValue = traineeValue( traineeType );
   if( trValue >= 0 )
   {
      res = (float)( 100 - trValue );
   }

   return res;
}

void Building::reserveTrainee(walker::Type traineeType) { _d->reservedTrainees.insert(traineeType); }
void Building::cancelTrainee(walker::Type traineeType) { _d->reservedTrainees.erase(traineeType);}

void Building::updateTrainee(  TraineeWalkerPtr walker )
{
   _d->reservedTrainees.erase( walker->type() );
   _d->trainees[ walker->type() ] += walker->value() ;
}

void Building::setTraineeValue(walker::Type type, int value)
{
  _d->trainees[ type ] = value;
}

void Building::initialize(const MetaData &mdata)
{
  Construction::initialize( mdata );

  Variant inflammabilityV = mdata.getOption( "inflammability" );
  if( inflammabilityV.isValid() ) setState( pr::inflammability, inflammabilityV.toDouble() );

  Variant collapsibilityV = mdata.getOption( "collapsibility" );
  if( collapsibilityV.isValid() ) setState( pr::collapsibility, collapsibilityV.toDouble() );
}

void Building::save(VariantMap& stream) const
{
  Construction::save( stream );
  VARIANT_SAVE_CLASS_D( stream, _d, trainees )
  VARIANT_SAVE_CLASS_D( stream, _d, reservedTrainees )
  VARIANT_SAVE_CLASS_D( stream, _d, reservedServices )
}

void Building::load(const VariantMap& stream)
{
  Construction::load( stream );
  VARIANT_LOAD_CLASS_D_LIST( _d, trainees, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, reservedTrainees, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, reservedServices, stream )
}

int Building::traineeValue(walker::Type traineeType) const
{
  TraineeMap::iterator i = _d->trainees.find( traineeType );
  return i != _d->trainees.end() ? i->second : -1;
}

Renderer::PassQueue Building::passQueue() const {  return buildingPassQueue;}

void Building::_updateBalanceKoeffs()
{
  if( !_city().isValid() )
    return;

  float balance = std::max<float>( statistic::getBalanceKoeff( _city() ), 0.1f );

  float fireKoeff = balance * _city()->getOption( PlayerCity::fireKoeff ) / 100.f;
  if( !_city()->getOption(PlayerCity::c3gameplay) )
  {
    int anyWater = tile().param( Tile::pWellWater ) + tile().param( Tile::pFountainWater ) + tile().param( Tile::pReservoirWater );
    if( anyWater > 0 )
      fireKoeff -= 0.3f;
  }

  _d->cityKoeffs.fireRisk = math::clamp( fireKoeff, 0.f, 9.f );
  _d->cityKoeffs.collapseRisk = balance * _city()->getOption( PlayerCity::collapseKoeff ) / 100.f;
}
