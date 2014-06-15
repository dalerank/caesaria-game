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

#include "working.hpp"
#include "walker/walker.hpp"
#include "events/returnworkers.hpp"
#include "core/stringhelper.hpp"
#include "game/gamedate.hpp"

using namespace gfx;

namespace {
CAESARIA_LITERALCONST(currentWorkers)
CAESARIA_LITERALCONST(active)
CAESARIA_LITERALCONST(maxWorkers)
const int workersDescNum = 6;
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
  std::string factoryType = MetaDataHolder::findTypename( type() );
  float workKoeff = (numberWorkers() / (float)maximumWorkers()) * workersDescNum;

  const char* workKoeffStr[] = { "no_workers", "bad_work", "slow_work", "patrly_workers",
                                 "need_some_workers", "full_work" };
  workKoeff = math::clamp( (int)ceil(workKoeff), 0, workersDescNum-1 );

  return StringHelper::format( 0xff, "##%s_%s##", factoryType.c_str(), workKoeffStr[ (int)workKoeff ] );
}

std::string WorkingBuilding::troubleDesc() const
{
  std::string trouble = Building::troubleDesc();

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
    if( mayWork()  )
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
  if( pic.isValid() )
  {
     _fgPicturesRef().back() = _animationRef().currentFrame();
  }
}

void WorkingBuilding::_setClearAnimationOnStop(bool value)
{
  _d->clearAnimationOnStop = value;
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
    events::GameEventPtr e=events::ReturnWorkers::create( pos(), numberWorkers() );
    e->dispatch();
  }
}
