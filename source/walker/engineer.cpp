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

#include "engineer.hpp"
#include "city/helper.hpp"

using namespace constants;

WalkerPtr Engineer::create(PlayerCityPtr city)
{
  return ServiceWalker::create( city, Service::engineer ).object();
}

Engineer::~Engineer()
{

}

std::string Engineer::getThinks() const
{
  city::Helper helper( _getCity() );

  TilePos offset( 2, 2 );

  ConstructionList buildings = helper.find<Construction>( building::any, pos() - offset, pos() + offset );
  foreach( b, buildings )
  {
    int damage = (*b)->getState( Construction::damage );
    if( damage > 90 )
    {
      return "##engineer_have_trouble_buildings##";
    }
  }

  return ServiceWalker::getThinks();
}

Engineer::Engineer( PlayerCityPtr city ) : ServiceWalker( city, Service::engineer )
{
}
