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
#include "walker/walkers_factory.hpp"
#include "walker/fish_place.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace city
{

class Fishery::Impl
{
public:
  PlayerCityPtr city;
  unsigned int maxFishPlace;

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
  : Srvc( Fishery::getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->maxFishPlace = 1;
}

void Fishery::update( const unsigned int time )
{  
  if( time % (GameDate::ticksInMonth()/2) != 1 )
    return;

  if( _d->places.empty() )
  {
    Helper helper( _d->city );
    _d->places = helper.find<FishPlace>( walker::fishPlace, TilePos(-1, -1) );
  }

  while( _d->places.size() < _d->maxFishPlace )
  {
    FishPlacePtr fishplace = ptr_cast<FishPlace>( WalkerManager::instance().create( walker::fishPlace, _d->city ) );

    if( fishplace.isValid() )
    {
      fishplace->send2city( _d->city->getBorderInfo().boatEntry );
      _d->places.push_back( ptr_cast<FishPlace>( fishplace ) );
    }
  }

  FishPlaceList::iterator fit = _d->places.begin();
  while( fit != _d->places.end() )
  {
    if( (*fit)->isDeleted() )     {      fit = _d->places.erase( fit );    }
    else    {      ++fit;    }
  }
}

}//end namespace city
