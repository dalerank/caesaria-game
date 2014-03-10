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
#include "gfx/picture.hpp"
#include "core/variant.hpp"
#include "walker/walker.hpp"
#include "core/foreach.hpp"
#include "events/returnworkers.hpp"
#include "core/stringhelper.hpp"

class WorkingBuilding::Impl
{
public:
  int currentWorkers;
  int maxWorkers;
  bool isActive;
  WalkerList walkerList;
  std::string errorStr;
};

WorkingBuilding::WorkingBuilding(const Type type, const Size& size)
: Building( type, size ), _d( new Impl )
{
  _d->currentWorkers = 0;
  _d->maxWorkers = 0;
  _d->isActive = true;
}

void WorkingBuilding::save( VariantMap& stream ) const
{
  Building::save( stream );
  stream[ "currentWorkers" ] = _d->currentWorkers;
  stream[ "active" ] = _d->isActive;
  stream[ "maxWorkers" ] = _d->maxWorkers;
}

void WorkingBuilding::load( const VariantMap& stream)
{
  Building::load( stream );
  _d->currentWorkers = (int)stream.get( "currentWorkers", 0 );
  _d->isActive = (bool)stream.get( "active", true );
  Variant value = stream.get( "maxWorkers" );

  if( !value.isNull() )
    _d->maxWorkers = value;
}

std::string WorkingBuilding::getWorkersProblem() const
{
  std::string factoryType = MetaDataHolder::getTypename( type() );
  float workKoeff = (numberWorkers() / (float)maxWorkers()) * 6.f;

  const char* workKoeffStr[] = { "no_workers", "bad_work", "slow_work", "patrly_workers",
                                 "need_some_workers", "full_work" };
  workKoeff = math::clamp( (int)ceil(workKoeff), 0, 5 );

  return StringHelper::format( 0xff, "##%s_%s##", factoryType.c_str(), workKoeffStr[ (int)workKoeff ] );
}

std::string WorkingBuilding::troubleDesc() const
{
  std::string trouble = Building::troubleDesc();

  if( trouble.empty() && numberWorkers() < maxWorkers() / 2 )
  {
    trouble = getWorkersProblem();
  }

  return trouble;
}

std::string WorkingBuilding::workersStateDesc() const { return ""; }
void WorkingBuilding::setMaxWorkers(const int maxWorkers) { _d->maxWorkers = maxWorkers; }
int WorkingBuilding::maxWorkers() const { return _d->maxWorkers; }
void WorkingBuilding::setWorkers(const unsigned int currentWorkers){  _d->currentWorkers = math::clamp<int>( currentWorkers, 0, _d->maxWorkers );}
int WorkingBuilding::numberWorkers() const { return _d->currentWorkers; }
void WorkingBuilding::setActive(const bool value) { _d->isActive = value; }
bool WorkingBuilding::isActive() const { return _d->isActive; }
void WorkingBuilding::addWorkers(const unsigned int workers ) { setWorkers( numberWorkers() + workers ); }
void WorkingBuilding::removeWorkers(const unsigned int workers) { setWorkers( numberWorkers() - workers ); }
WorkingBuilding::~WorkingBuilding(){}
const WalkerList& WorkingBuilding::getWalkers() const {  return _d->walkerList; }
std::string WorkingBuilding::getError() const { return _d->errorStr;}
void WorkingBuilding::_setError(const std::string& err) { _d->errorStr = err;}

void WorkingBuilding::timeStep( const unsigned long time )
{
  Building::timeStep( time );

  WalkerList::iterator it=_d->walkerList.begin();
  while( it != _d->walkerList.end() )
  {
    if( (*it)->isDeleted() ) { it = _d->walkerList.erase( it ); }
    else { it++; }
  }
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
