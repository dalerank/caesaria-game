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

#include "working.hpp"
#include "city/helper.hpp"
#include "walker/walker.hpp"
#include "events/returnworkers.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "events/removecitizen.hpp"

using namespace gfx;
using namespace events;
using namespace constants;

class WorkingBuilding::Impl
{
public:
  unsigned int currentWorkers;
  unsigned int maxWorkers;
  bool isActive;
  WalkerList walkerList;
  std::string errorStr;
  bool clearAnimationOnStop;
  float laborAccessKoeff;
};

WorkingBuilding::WorkingBuilding(const Type type, const Size& size)
: Building( type, size ), _d( new Impl )
{
  _d->currentWorkers = 0;
  _d->maxWorkers = 0;
  _d->isActive = true;
  _d->clearAnimationOnStop = true;
  _d->laborAccessKoeff = 100;
  _animationRef().stop();
}

void WorkingBuilding::save( VariantMap& stream ) const
{
  Building::save( stream );
  VARIANT_SAVE_ANY_D( stream, _d, currentWorkers );
  VARIANT_SAVE_ANY_D( stream, _d, isActive );
  VARIANT_SAVE_ANY_D( stream, _d, maxWorkers );
  VARIANT_SAVE_ANY_D( stream, _d, laborAccessKoeff );
}

void WorkingBuilding::load( const VariantMap& stream)
{
  Building::load( stream );
  VARIANT_LOAD_ANY_D( _d, currentWorkers, stream );
  VARIANT_LOAD_ANY_D( _d, isActive, stream );
  VARIANT_LOAD_ANY_D( _d, maxWorkers, stream );
  VARIANT_LOAD_ANY_D( _d, laborAccessKoeff, stream );

  if( !_d->maxWorkers )
  {
    _d->maxWorkers = MetaDataHolder::getData( type() ).getOption( MetaDataOptions::employers );
  }
}

std::string WorkingBuilding::workersProblemDesc() const
{
  return WorkingBuildingHelper::productivity2desc( const_cast<WorkingBuilding*>( this ) );
}

std::string WorkingBuilding::sound() const
{
  return (isActive() && numberWorkers() > 0
            ? Building::sound()
            : "");
}

std::string WorkingBuilding::troubleDesc() const
{
  std::string trouble = Building::troubleDesc();

  if( isNeedRoadAccess() && getAccessRoads().empty() )
  {
    trouble = "##working_building_need_road##";
  }

  if( trouble.empty() )
  {
    if( numberWorkers() < maximumWorkers() / 2)
    {
      trouble = workersProblemDesc();
    }
    else if( _d->laborAccessKoeff < 50 )
    {
      trouble = "##working_build_poor_labor_warning##";
    }
  }

  return trouble;
}

void WorkingBuilding::initialize(const MetaData& mdata)
{
  Building::initialize( mdata );

  setMaximumWorkers( (unsigned int)mdata.getOption( "employers" ) );
}

std::string WorkingBuilding::workersStateDesc() const { return ""; }
void WorkingBuilding::setMaximumWorkers(const unsigned int maxWorkers) { _d->maxWorkers = maxWorkers; }
unsigned int WorkingBuilding::maximumWorkers() const { return _d->maxWorkers; }
void WorkingBuilding::setWorkers(const unsigned int currentWorkers){  _d->currentWorkers = math::clamp( currentWorkers, 0u, _d->maxWorkers );}
unsigned int WorkingBuilding::numberWorkers() const { return _d->currentWorkers; }
unsigned int WorkingBuilding::needWorkers() const { return maximumWorkers() - numberWorkers(); }
unsigned int WorkingBuilding::productivity() const { return math::percentage( numberWorkers(), maximumWorkers() ); }
unsigned int WorkingBuilding::laborAccessPercent() const { return _d->laborAccessKoeff; }
bool WorkingBuilding::mayWork() const { return numberWorkers() > 0; }
void WorkingBuilding::setActive(const bool value) { _d->isActive = value; }
bool WorkingBuilding::isActive() const { return _d->isActive; }
WorkingBuilding::~WorkingBuilding(){}
const WalkerList& WorkingBuilding::walkers() const {  return _d->walkerList; }
std::string WorkingBuilding::errorDesc() const { return _d->errorStr;}
void WorkingBuilding::_setError(const std::string& err) { _d->errorStr = err;}

unsigned int WorkingBuilding::addWorkers(const unsigned int workers )
{
  unsigned int maxAdd = std::min( workers, needWorkers() );
  setWorkers( numberWorkers() + maxAdd );
  return maxAdd;
}

unsigned int WorkingBuilding::removeWorkers(const unsigned int workers)
{
  unsigned int maxRemove = std::min( numberWorkers(), workers );
  setWorkers( numberWorkers() - maxRemove );
  return maxRemove;
}

void WorkingBuilding::timeStep( const unsigned long time )
{
  Building::timeStep( time );

  for( WalkerList::iterator it=_d->walkerList.begin(); it != _d->walkerList.end(); )
  {
    if( (*it)->isDeleted() ) { it = _d->walkerList.erase( it ); }
    else { ++it; }
  }

  if( game::Date::isMonthChanged() && numberWorkers() > 0 )
  {
    city::Helper helper( _city() );
    TilePos offset( 8, 8 );
    TilePos myPos = pos();
    HouseList houses = helper.find<House>( objects::house, myPos - offset, myPos + offset );
    float averageDistance = 0;
    foreach( it, houses )
    {
      if( (*it)->spec().level() < HouseLevel::smallVilla )
      {
        averageDistance += myPos.distanceFrom( (*it)->pos() );
      }
    }

    if( houses.size() > 0 )
      averageDistance /= houses.size();

    _d->laborAccessKoeff = math::clamp( math::percentage( averageDistance, 8 ) * 2, 25, 100 );
  }

  if( isActive() )
    _updateAnimation( time );
}

void WorkingBuilding::_updateAnimation(const unsigned long time )
{
  if( game::Date::isDayChanged() )
  {
    if( mayWork() )
    {
      if( _animationRef().isStopped() )
      {
        _animationRef().start();
      }      
    }
    else
    {
      if( _animationRef().isRunning() )
      {
        if( _d->clearAnimationOnStop && !_fgPicturesRef().empty() )
        {
          _fgPicturesRef().back() = Picture::getInvalid();
        }

        _animationRef().stop();
      }
    }
  }

  _animationRef().update( time );
  const Picture& pic = _animationRef().currentFrame();
  if( pic.isValid() && !_fgPicturesRef().empty() )
  {
    _fgPicturesRef().back() = _animationRef().currentFrame();
  }
}

void WorkingBuilding::_setClearAnimationOnStop(bool value) {  _d->clearAnimationOnStop = value; }

void WorkingBuilding::_disaster()
{
  unsigned int buriedCitizens = math::random( numberWorkers() );

  GameEventPtr e = ReturnWorkers::create( pos(), numberWorkers() - buriedCitizens );
  e->dispatch();

  e = RemoveCitizens::create( pos(), buriedCitizens );
  e->dispatch();

  setWorkers( 0 );
}

void WorkingBuilding::addWalker( WalkerPtr walker )
{
  if( walker.isValid() )
  {
    _d->walkerList.push_back( walker );
  }
}

void WorkingBuilding::destroy()
{
  Building::destroy();

  foreach( it, _d->walkerList )
  {
    walker::Type wt = (*it)->type();
    if( wt == walker::cartPusher || wt == walker::supplier )
      continue;

    (*it)->deleteLater();
  }

  if( numberWorkers() > 0 )
  {
    GameEventPtr e = ReturnWorkers::create( pos(), numberWorkers() );
    e->dispatch();
  }
}

void WorkingBuilding::collapse()
{
  Building::collapse();

  _disaster();
}

void WorkingBuilding::burn()
{
  Building::burn();

  _disaster();
}

namespace {

static const unsigned int productivityDescriptionCount = 6;
static const char* productivityDescription[] =
{
  "no_workers", "bad_work",
  "slow_work", "patrly_workers",
  "need_some_workers", "full_work"
};

}

std::string WorkingBuildingHelper::productivity2desc( WorkingBuildingPtr w, const std::string& prefix )
{
  std::string factoryType = MetaDataHolder::findTypename( w->type() );
  unsigned int workKoeff = w->productivity() * productivityDescriptionCount / 100;

  workKoeff = math::clamp( workKoeff, 0u, productivityDescriptionCount-1 );

  if( prefix.empty() )
  {
    return utils::format( 0xff, "##%s_%s##",
                                 factoryType.c_str(), productivityDescription[ workKoeff ] );
  }
  else
  {
    return utils::format( 0xff, "##%s_%s_%s##",
                                 factoryType.c_str(), prefix.c_str(), productivityDescription[ workKoeff ] );
  }
}

std::string WorkingBuildingHelper::productivity2str( WorkingBuildingPtr w )
{
  unsigned int workKoeff = w->productivity() * productivityDescriptionCount / 100;
  workKoeff = math::clamp( workKoeff, 0u, productivityDescriptionCount-1 );

  return productivityDescription[ workKoeff ];
}
