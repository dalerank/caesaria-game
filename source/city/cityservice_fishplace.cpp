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
#include "gfx/tilemap.hpp"
#include "core/position.hpp"
#include "gfx/tile.hpp"
#include "objects/objects_factory.hpp"
#include "walker/fish_place.hpp"
//#include "constants.hpp"

using namespace constants;

class CityServiceFishPlace::Impl
{
public:
  PlayerCityPtr city;
  unsigned int maxFishPlace;

  FishPlaceList places;
};

CityServicePtr CityServiceFishPlace::create( PlayerCityPtr city )
{
  CityServicePtr ret( new CityServiceFishPlace( city ) );
  ret->drop();

  return ret;
}

CityServiceFishPlace::CityServiceFishPlace( PlayerCityPtr city )
: CityService( "fishplace" ), _d( new Impl )
{
  _d->city = city;
  _d->maxFishPlace = 1;
}

void CityServiceFishPlace::update( const unsigned int time )
{  
  if( time % 44 != 1 )
    return;

  if( _d->places.empty() )
  {
    CityHelper helper( _d->city );
    _d->places = helper.find<FishPlace>( walker::fishPlace );
  }

  while( _d->places.size() < _d->maxFishPlace )
  {
    TileOverlayPtr fishplace = TileOverlayFactory::getInstance().create( walker::fishPlace );

    if( fishplace != 0 )
    {
      fishplace->build( _d->city, _d->city->getBorderInfo().boatEntry );
      _d->city->addOverlay( fishplace );
      _d->places.push_back( fishplace.as<FishPlace>() );
    }
  }

  FishPlaceList::iterator fit = _d->places.begin();
  while( fit != _d->places.end() )
  {
    if( (*fit)->isDeleted() )
    {
      fit = _d->places.erase( fit );
    }
    else
    {
      fit++;
    }
  }
}
