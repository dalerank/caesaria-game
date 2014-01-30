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

#include "random_damage.hpp"
#include "game/game.hpp"
#include "city/helper.hpp"
#include "game/gamedate.hpp"
#include "objects/house.hpp"
#include "events/dispatcher.hpp"

using namespace constants;

CityServicePtr RandomDamage::create( PlayerCityPtr city )
{
  RandomDamage* e = new RandomDamage();
  e->_city = city;

  CityServicePtr ret( e );
  ret->drop();

  return ret;
}

void RandomDamage::update( const unsigned int time)
{
  if( time % GameDate::getTickInMonth() == 0 && !_isDeleted )
  {
    int population = _city->getPopulation();
    if( population > _minPopulation && population < _maxPopulation )
    {
      _isDeleted = true;
      CityHelper helper( _city );
      HouseList houses = helper.find<House>( building::house );
      for( unsigned int k=0; k < houses.size() / 4; k++ )
      {
        HouseList::iterator it = houses.begin();
        std::advance( it, math::random( houses.size() ) );
        (*it)->collapse();
      }     

      events::Dispatcher::instance().load( _events );
    }
  }
}

bool RandomDamage::isDeleted() const
{
  return _isDeleted;
}

void RandomDamage::load(const VariantMap& stream)
{
  VariantList vl = stream.get( "population" ).toList();
  _minPopulation = vl.get( 0, 0 ).toInt();
  _maxPopulation = vl.get( 1, 999999 ).toInt();
  _events = stream.get( "exec" ).toMap();
}

VariantMap RandomDamage::save() const
{
  return VariantMap();
}

RandomDamage::RandomDamage() : CityService("randomDamage")
{
  _isDeleted = false;
}
