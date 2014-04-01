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

#include "cityservice_fishplace.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "walker/fish_place.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace city
{

class Fishery::Impl
{
public:
  unsigned int maxFishPlace;
  int failedCounter;

  FishPlaceList places;
};

SrvcPtr Fishery::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Fishery( city ) );
  ret->drop();

  return ret;
}

std::string Fishery::getDefaultName() {  return "fishery";}

Fishery::Fishery( PlayerCityPtr city )
  : Srvc( *city.object(), Fishery::getDefaultName() ), _d( new Impl )
{
  _d->failedCounter = 0;
  _d->maxFishPlace = 1;
}

void Fishery::update( const unsigned int time )
{  
  if( time % (GameDate::ticksInMonth()/2) != 1 )
    return;

  if( _d->places.empty() )
  {
    Helper helper( &_city );
    _d->places = helper.find<FishPlace>( walker::fishPlace, TilePos(-1, -1) );
  }

  while( _d->places.size() < _d->maxFishPlace )
  {
    FishPlacePtr fishplace = FishPlace::create( &_city );
    fishplace->send2city( _city.borderInfo().boatEntry );

    if( fishplace->isDeleted() )
    {
      _d->failedCounter++;
      return;
    }

    _d->places.push_back( ptr_cast<FishPlace>( fishplace ) );
  }

  FishPlaceList::iterator fit = _d->places.begin();
  while( fit != _d->places.end() )
  {
    if( (*fit)->isDeleted() )     {      fit = _d->places.erase( fit );    }
    else {  ++fit;    }
  }
}

bool Fishery::isDeleted() const { return _d->failedCounter > 3; }

}//end namespace city
