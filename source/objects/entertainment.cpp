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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "entertainment.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/foreach.hpp"
#include "training.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "walker/trainee.hpp"
#include "walker/helper.hpp"

namespace {
static const int idleDecreaseLevel = 10;
static const int workDecreaseLevel = 25;
}

class EntertainmentBuilding::Impl
{
public:
  EntertainmentBuilding::NecessaryWalkers necWalkers;
  unsigned int showCounter;
  TraineeWays incomes;
};

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const object::Type type,
                                             Size size )
  : ServiceBuilding(service, type, size), _d( new Impl )
{
  _d->showCounter = 0;
}

EntertainmentBuilding::~EntertainmentBuilding() {}

void EntertainmentBuilding::deliverService()
{
  // we need all trainees types for the show
  if( !mayWork() )
  {
    _animation().stop();
    return;
  }

  bool isWalkerReady = _isWalkerReady();
  _d->incomes.removeExpired( game::Date::current() ); //remove expired incomes for last 2 months

  int decreaseLevel = idleDecreaseLevel;
  // all trainees are there for the show!
  if( isWalkerReady )
  {
    if( _specificWorkers().empty() )
    {
      ServiceBuilding::deliverService();

      if( !_specificWorkers().empty() )
      {
        _d->showCounter++;
        _animation().start();
        decreaseLevel = workDecreaseLevel;
      }
    }
  }

  if( _specificWorkers().empty() )
  {
    _animation().stop(); //have no actors for the show
  }

  for( auto item : _d->necWalkers )
  {
    int level = traineeValue( item );
    setTraineeValue( item, math::clamp( level - decreaseLevel, 0, 100) );
  }
}

int EntertainmentBuilding::currentVisitors() const{  return 0;}
int EntertainmentBuilding::maxVisitors() const { return 0; }
unsigned int EntertainmentBuilding::walkerDistance() const { return 35; }

float EntertainmentBuilding::evaluateTrainee(walker::Type traineeType)
{
  if( numberWorkers() == 0 )
    return 0.0;

  return ServiceBuilding::evaluateTrainee( traineeType );
}

const EntertainmentBuilding::IncomeWays& EntertainmentBuilding::incomes() const { return _d->incomes; }
unsigned int EntertainmentBuilding::showsCount() const { return _d->showCounter; }
bool EntertainmentBuilding::isShow() const {   return animation().isRunning(); }

void EntertainmentBuilding::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, showCounter )
  VARIANT_SAVE_CLASS_D( stream, _d, incomes )
}

void EntertainmentBuilding::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );
  VARIANT_LOAD_ANY_D( _d, showCounter, stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, incomes, stream )
}

std::string EntertainmentBuilding::troubleDesc() const
{
  std::string ret = ServiceBuilding::troubleDesc();

  if( ret.empty() )
  {
    for( auto item : _d->necWalkers )
    {
      int level = traineeValue( item );
      if( level == 0 )
      {
        ret = fmt::format( "##need_trainee_{}##", WalkerHelper::getTypename( item ));
        break;
      }
    }
  }

  return ret;
}

void EntertainmentBuilding::updateTrainee(TraineeWalkerPtr walker)
{
  ServiceBuilding::updateTrainee( walker );

  TraineeWayInfo info{ game::Date::current(),
                       walker->places( Walker::plOrigin ),
                       pos() };

  _d->incomes.push_back( info );
}

EntertainmentBuilding::NecessaryWalkers EntertainmentBuilding::necessaryWalkers() const { return _d->necWalkers; }
WalkerList EntertainmentBuilding::_specificWorkers() const { return walkers(); }

void EntertainmentBuilding::_addNecessaryWalker(walker::Type type)
{
  _d->necWalkers.push_back( type );
  setTraineeValue( type, 0 );
}

bool EntertainmentBuilding::_isWalkerReady()
{
  int maxLevel = 0;
  for( auto item : _d->necWalkers )
    maxLevel = std::max( maxLevel, traineeValue( item ) );

  return (maxLevel>0);
}

VariantList TraineeWays::save() const
{
  VariantList ret;
  for( const auto& item : *this )
  {
    VariantList vl;
    vl << item.time << item.base << item.destination;
    ret.push_back( vl );
  }

  return ret;
}

void TraineeWays::load(const VariantList& stream)
{
  for( const auto& item : stream )
  {
    VariantList vl = item.toList();
    TraineeWayInfo info{ vl.get( 0 ).toDateTime(),
                         vl.get( 1 ).toTilePos(),
                         vl.get( 2 ).toTilePos() };
    push_back( info );
    }
}

void TraineeWays::removeExpired(const DateTime& current, int expsMonth)
{
  for( auto it = begin(); it != end(); )
  {
    if( it->time.monthsTo( current ) > expsMonth ) { it = erase( it ); }
    else { ++it; }
  }
}
