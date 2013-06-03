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

#include "oc3_working_building.hpp"
#include "oc3_building_data.hpp"
#include "oc3_picture.hpp"
#include "oc3_variant.hpp"

WorkingBuilding::WorkingBuilding(const BuildingType type, const Size& size)
: Building( type, size )
{
  const BuildingData& data = BuildingDataHolder::instance().getData( type );

  _maxWorkers = data.getEmployers();

  if( data.getResourceIndex() > 0 )
    setPicture( Picture::load( data.getResouceGroup(), data.getResourceIndex() ) );

  _currentWorkers = 0;
  _isActive = true;
}

void WorkingBuilding::setMaxWorkers(const int maxWorkers)
{
  _maxWorkers = maxWorkers;
}

int WorkingBuilding::getMaxWorkers() const
{
  return _maxWorkers;
}

void WorkingBuilding::setWorkers(const int currentWorkers)
{
  _currentWorkers = currentWorkers;
}

int WorkingBuilding::getWorkers() const
{
  return _currentWorkers;
}

void WorkingBuilding::setActive(const bool value)
{
  _isActive = value;
}

bool WorkingBuilding::isActive()
{
  return _isActive;
}

void WorkingBuilding::save( VariantMap& stream ) const
{
  Building::save( stream );
  stream[ "currentWorkers" ] = _currentWorkers;
}

void WorkingBuilding::load( const VariantMap& stream)
{
  //    Building::unserialize(stream);
  //    _currentWorkers = stream.read_int(1, 0, 100);
}

void WorkingBuilding::addWorkers( const int workers )
{
  _currentWorkers += workers;
}