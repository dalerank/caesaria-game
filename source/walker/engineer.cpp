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

#include "engineer.hpp"
#include "city/helper.hpp"

using namespace constants;

class Engineer::Impl
{
public:
  int averageLevel;
  std::set<ConstructionPtr> _reachedBuildings;
};

WalkerPtr Engineer::create(PlayerCityPtr city)
{
  return ServiceWalker::create( city, Service::engineer ).object();
}

Engineer::~Engineer() {}

std::string Engineer::getThinks() const
{
  if( _d->averageLevel > 70 )
  {
    return "##engineer_have_trouble_buildings##";
  }

  if( _d->averageLevel < 30 )
  {
    return "##engineer_no_trouble_buildings##";
  }

  return ServiceWalker::getThinks();
}

void Engineer::_centerTile()
{
  city::Helper helper( _city() );
  TilePos offset( reachDistance(), reachDistance() );
  ConstructionList buildings = helper.find<Construction>( building::any, pos() - offset, pos() + offset );
  foreach( b, buildings )
  {
    if( !_d->_reachedBuildings.count( *b ) )
    {
      int damageLvl = (*b)->state( Construction::damage );
      _d->averageLevel = ( _d->averageLevel + damageLvl ) / 2;
      _d->_reachedBuildings.insert( *b );
    }
  }

  ServiceWalker::_centerTile();
}

Engineer::Engineer( PlayerCityPtr city )
  : ServiceWalker( city, Service::engineer ), _d( new Impl )
{
  _d->averageLevel = 0;
}
