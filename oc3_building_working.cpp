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

#include "oc3_building_working.hpp"
#include "oc3_picture.hpp"
#include "oc3_variant.hpp"
#include "oc3_walker.hpp"
#include "oc3_foreach.hpp"

class WorkingBuilding::Impl
{
public:
  int currentWorkers;
  int maxWorkers;
  bool isActive;
  WalkerList walkerList;
};

WorkingBuilding::WorkingBuilding(const LandOverlayType type, const Size& size)
: Building( type, size ), _d( new Impl )
{
  _d->currentWorkers = 0;
  _d->isActive = true;
}

void WorkingBuilding::setMaxWorkers(const int maxWorkers)
{
  _d->maxWorkers = maxWorkers;
}

int WorkingBuilding::getMaxWorkers() const
{
  return _d->maxWorkers;
}

void WorkingBuilding::setWorkers(const unsigned int currentWorkers)
{
  _d->currentWorkers = math::clamp<int>( currentWorkers, 0, _d->maxWorkers );
}

int WorkingBuilding::getWorkers() const
{
  return _d->currentWorkers;
}

void WorkingBuilding::setActive(const bool value)
{
  _d->isActive = value;
}

bool WorkingBuilding::isActive() const
{
  return _d->isActive;
}

void WorkingBuilding::save( VariantMap& stream ) const
{
  Building::save( stream );
  stream[ "currentWorkers" ] = _d->currentWorkers;
}

void WorkingBuilding::load( const VariantMap& stream)
{
  Building::load( stream );
  _d->currentWorkers = (int)stream.get( "currentWorkers", 0 );
}

void WorkingBuilding::addWorkers(const unsigned int workers )
{
  setWorkers( getWorkers() + workers );
}

void WorkingBuilding::removeWorkers(const unsigned int workers)
{
  setWorkers( getWorkers() - workers );
}

WorkingBuilding::~WorkingBuilding()
{

}

void WorkingBuilding::timeStep( const unsigned long time )
{
  Building::timeStep( time );

  WalkerList::iterator it=_d->walkerList.begin();
  while( it != _d->walkerList.end() )
  {
    if( (*it)->isDeleted() )
    {
      it = _d->walkerList.erase( it );
    }
    else
    {
      it++;
    }
  }
}

void WorkingBuilding::addWalker( WalkerPtr walker )
{
  if( walker.isValid() )
  {
    _d->walkerList.push_back( walker );
  }
}

const WalkerList& WorkingBuilding::getWalkerList() const
{
  return _d->walkerList;
}

void WorkingBuilding::destroy()
{
  Building::destroy();

  foreach( WalkerPtr walker, _d->walkerList )
  {
    walker->deleteLater();
  }
}
