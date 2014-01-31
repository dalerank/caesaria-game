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

#include "random_fire.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "gui/tutorial_window.hpp"

namespace events
{

GameEventPtr RandomFire::create( PlayerCityPtr city, const VariantMap& options )
{
  RandomFire* e = new RandomFire();
  e->_city = city;
  e->load( options );

  CityServicePtr ret( e );
  ret->drop();

  return ret;
}

void RandomFire::update(unsigned int time)
{
  if( time % GameDate::getTickInMonth() == 0 )
  {
    int population = _city->getPopulation();
    if( population > _minPopulation && population < _maxPopulation )
    {


    }
  }
}

void RandomFire::load(const VariantMap& stream)
{
  VariantList vl = stream.get( "population" ).toList();
  _minPopulation = vl.get( 0, 0 ).toInt();
  _maxPopulation = vl.get( 1, 999999 ).toInt();
}

}
