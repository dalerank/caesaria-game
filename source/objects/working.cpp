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
#include "walker/walker.hpp"
#include "events/returnworkers.hpp"
#include "core/stringhelper.hpp"
#include "game/gamedate.hpp"
#include "events/removecitizen.hpp"

using namespace gfx;
using namespace events;

namespace {
CAESARIA_LITERALCONST(currentWorkers)
CAESARIA_LITERALCONST(active)
CAESARIA_LITERALCONST(maxWorkers)
}

class WorkingBuilding::Impl
{
public:
  unsigned int currentWorkers;
  unsigned int maxWorkers;
  bool isActive;
  WalkerList walkerList;
  std::string errorStr;
  bool clearAnimationOnStop;
};

WorkingBuilding::WorkingBuilding(const Type type, const Size& size)
: Building( type, size ), _d( new Impl )
{
  _d->currentWorkers = 0;
  _d->maxWorkers = 0;
  _d->isActive = true;
  _d->clearAnimationOnStop = true;
  _animationRef().stop();
}

void WorkingBuilding::save( VariantMap& stream ) const
{
  Building::save( stream );
  stream[ lc_currentWorkers ] = _d->currentWorkers;
  stream[ lc_active ] = _d->isActive;
  stream[ lc_maxWorkers ] = _d->maxWorkers;
}

void WorkingBuilding::load( const VariantMap& stream)
{
  Building::load( stream );
  _d->currentWorkers = (unsigned int)stream.get( lc_currentWorkers, 0 );
  _d->isActive = (bool)stream.get( lc_active, true );
  Variant value = stream.get( lc_maxWorkers );

  if( !value.isNull() )
    _d->maxWorkers = value;
}

std::string WorkingBuilding::workersProblemDesc() const
{
  return WorkingBuildingHelper::productivity2desc( const_cast<WorkingBuilding*>( this ) );
}

std::string WorkingBuilding::troubleDesc() const
{
  std::string trouble = Building::troubleDesc();

  if( isNeedRoadAccess() && getAccessRoads().empty() )
  {
    trouble = "##working_building_need_road##";
  }

  if( trouble.empty() && numberWorkers() < maximumWorkers() / 2 )
  {
    trouble = workersProblemDesc();
  }

  return trouble;
}

std::string WorkingBuilding::workersStateDesc() const { return ""; }
void WorkingBuilding::setMaximumWorkers(const unsigned int maxWorkers) { _d->maxWorkers = maxWorkers; }
unsigned int WorkingBuilding::maximumWorkers() const { return _d->maxWorkers; }
void WorkingBuilding::setWorkers(const unsigned int currentWorkers){  _d->currentWorkers = math::clamp( currentWorkers, 0u, _d->maxWorkers );}
unsigned int WorkingBuilding::numberWorkers() const { return _d->currentWorkers; }
unsigned int WorkingBuilding::needWorkers() const { return maximumWorkers() - numberWorkers(); }
unsigned int WorkingBuilding::productivity() const { return numberWorkers() * 100u / maximumWorkers(); }
bool WorkingBuilding::mayWork() const {  return numberWorkers() > 0; }
void WorkingBuilding::setActive(const bool value) { _d->isActive = value; }
bool WorkingBuilding::isActive() const { return _d->isActive; }
void WorkingBuilding::addWorkers(const unsigned int workers ) { setWorkers( numberWorkers() + workers ); }
void WorkingBuilding::removeWorkers(const unsigned int workers) { setWorkers( numberWorkers() - workers ); }
WorkingBuilding::~WorkingBuilding(){}
const WalkerList& WorkingBuilding::walkers() const {  return _d->walkerList; }
std::string WorkingBuilding::errorDesc() const { return _d->errorStr;}
void WorkingBuilding::_setError(const std::string& err) { _d->errorStr = err;}

void WorkingBuilding::timeStep( const unsigned long time )
{
  Building::timeStep( time );

  WalkerList::iterator it=_d->walkerList.begin();
  while( it != _d->walkerList.end() )
  {
    if( (*it)->isDeleted() ) { it = _d->walkerList.erase( it ); }
    else { ++it; }
  }

  _updateAnimation( time );
}

void WorkingBuilding::_updateAnimation(const unsigned long time )
{
  if( GameDate::isDayChanged() )
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

  foreach( walker, _d->walkerList ) { (*walker)->deleteLater(); }

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

std::string WorkingBuildingHelper::productivity2desc( WorkingBuildingPtr w)
{
  std::string factoryType = MetaDataHolder::findTypename( w->type() );
  unsigned int workKoeff = w->productivity() * productivityDescriptionCount / 100;

  workKoeff = math::clamp( workKoeff, 0u, productivityDescriptionCount );

  return StringHelper::format( 0xff, "##%s_%s##", factoryType.c_str(), productivityDescription[ workKoeff ] );
}

std::string WorkingBuildingHelper::productivity2str( WorkingBuildingPtr w )
{
  unsigned int workKoeff = w->productivity() * productivityDescriptionCount / 100;
  workKoeff = math::clamp( workKoeff, 0u, productivityDescriptionCount );

  return productivityDescription[ workKoeff ];
}
