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
#include "city/helper.hpp"
#include "training.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "walker/helper.hpp"

using namespace constants;

namespace {
static const int idleDecreaseLevel = 10;
static const int workDecreaseLevel = 25;
}

class EntertainmentBuilding::Impl
{
public:
  EntertainmentBuilding::NecessaryWalkers necWalkers;
  unsigned int showCounter;
};

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const TileOverlay::Type type,
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
    _animationRef().stop();
    return;
  }

  bool isWalkerReady = _isWalkerReady();

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
        _animationRef().start();
        decreaseLevel = workDecreaseLevel;
      }
    }
  }

  if( _specificWorkers().empty() )
  {
    _animationRef().stop(); //have no actors for the show
  }

  foreach( item, _d->necWalkers )
  {
    int level = traineeValue( *item );
    setTraineeValue( *item, math::clamp( level - decreaseLevel, 0, 100) );
  }
}

int EntertainmentBuilding::visitorsNumber() const{  return 0;}
unsigned int EntertainmentBuilding::walkerDistance() const {  return 35; }

float EntertainmentBuilding::evaluateTrainee(walker::Type traineeType)
{
  if( numberWorkers() == 0 )
    return 0.0;

  return ServiceBuilding::evaluateTrainee( traineeType );
}

unsigned int EntertainmentBuilding::showsCount() const { return _d->showCounter; }
bool EntertainmentBuilding::isShow() const {   return animation().isRunning(); }

void EntertainmentBuilding::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, showCounter );
}

void EntertainmentBuilding::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );
  VARIANT_LOAD_ANY_D( _d, showCounter, stream );
}

std::string EntertainmentBuilding::troubleDesc() const
{
  std::string ret = ServiceBuilding::troubleDesc();

  if( ret.empty() )
  {
    foreach( item, _d->necWalkers )
    {
      int level = traineeValue( *item );
      if( level == 0 )
      {
        ret = utils::format( 0xff, "##need_trainee_%s##", WalkerHelper::getTypename( *item ).c_str() );
        break;
      }
    }
  }

  return ret;
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
  foreach( item, _d->necWalkers )
  {  maxLevel = std::max( maxLevel, traineeValue( *item ) ); }

  return maxLevel;
}
