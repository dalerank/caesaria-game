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

#include "cityservice_animals.hpp"
#include "city.hpp"
#include "gfx/tile.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilemap.hpp"
#include "walker/animals.hpp"
#include "walker/constants.hpp"

using namespace constants;

class CityServiceAnimals::Impl
{
public:
  static const unsigned int maxSheeps = 10;
  PlayerCityPtr city;
  DateTime lastTimeUpdate;
};

CityServicePtr CityServiceAnimals::create(PlayerCityPtr city)
{
  CityServiceAnimals* ret = new CityServiceAnimals();
  ret->_d->city = city;
  ret->_d->lastTimeUpdate = GameDate::current();

  return ret;
}

void CityServiceAnimals::update(const unsigned int time)
{
  if( time % 16 != 1 )
    return;

  if( _d->lastTimeUpdate.getMonth() != GameDate::current().getMonth() )
  {
    _d->lastTimeUpdate = GameDate::current();
    Tilemap& tmap = _d->city->getTilemap();
    TilesArray border = tmap.getRectangle( TilePos( 0, 0 ), Size( tmap.getSize() ) );
    TilesArray::iterator it=border.begin();
    while( it != border.end() )
    {
      if( !(*it)->isWalkable(true) )
      {
        it = border.erase( it );
      }
      else
      {
        it++;
      }
    }

    WalkerList sheeps = _d->city->getWalkers( walker::sheep );
    if( sheeps.size() < Impl::maxSheeps )
    {
      WalkerPtr sheep = Sheep::create( _d->city );
      if( sheep.isValid() )
      {
        TilesArray::iterator it = border.begin();
        std::advance( it, std::rand() % border.size() );
        sheep.as<Sheep>()->send2City( (*it)->getIJ() );
      }
    }
  }
}

CityServiceAnimals::CityServiceAnimals()
  : CityService( "animals" ), _d( new Impl )
{

}
