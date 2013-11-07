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

#include "cityservice_animals.hpp"
#include "city.hpp"
#include "gfx/tile.hpp"
#include "gamedate.hpp"
#include "tilemap.hpp"
#include "walker/animals.hpp"
#include "walker/constants.hpp"

using namespace constants;

class CityServiceAnimals::Impl
{
public:
  static const unsigned int maxSheeps = 10;
  CityPtr city;
  DateTime lastTimeUpdate;
};

CityServicePtr CityServiceAnimals::create(CityPtr city)
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
    TilemapTiles border = tmap.getRectangle( TilePos( 0, 0 ), Size( tmap.getSize() ) );
    TilemapTiles::iterator it=border.begin();
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

    WalkerList sheeps = _d->city->getWalkerList( walker::sheep );
    if( sheeps.size() < Impl::maxSheeps )
    {
      WalkerPtr sheep = Sheep::create( _d->city );
      if( sheep.isValid() )
      {
        TilemapTiles::iterator it = border.begin();
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
