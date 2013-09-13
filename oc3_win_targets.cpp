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

#include "oc3_win_targets.hpp"
#include "oc3_house_level.hpp"
#include "oc3_variant.hpp"

class CityWinTargets::Impl
{
public:
  int maxHouseLevel;
  int population;
  int culture;
  int prosperity;
  int favour;
  int peace;
  bool success;
  std::string overview;
};

CityWinTargets::CityWinTargets() : _d( new Impl )
{
  _d->success = false;
  _d->maxHouseLevel = 30;
  _d->population = 0;
  _d->culture = 0;
  _d->prosperity = 0;
  _d->favour = 0;
  _d->peace = 0;
}

CityWinTargets::~CityWinTargets()
{

}

bool CityWinTargets::isSuccess() const
{
  return _d->success;
}

void CityWinTargets::load( const VariantMap& stream )
{
  _d->maxHouseLevel = HouseSpecHelper::getInstance().getHouseLevel( stream.get( "maxHouseLevel" ).toString() );
  _d->success = stream.get( "success" ).toBool();
  _d->population = (int)stream.get( "population" );
  _d->culture = (int)stream.get( "culture" );
  _d->prosperity = (int)stream.get( "prosperity" );
  _d->favour = (int)stream.get( "favour" );
  _d->peace = (int)stream.get( "peace" );
  _d->overview = stream.get( "overview" ).toString();
}

CityWinTargets&CityWinTargets::operator=(const CityWinTargets& a)
{
  _d->maxHouseLevel = a._d->maxHouseLevel;
  _d->success = a._d->success;
  _d->population = a._d->population;
  _d->culture = a._d->culture;
  _d->prosperity = a._d->prosperity;
  _d->favour = a._d->favour;
  _d->peace = a._d->peace;
  _d->overview = a._d->overview;

  return *this;
}

int CityWinTargets::getCulture() const
{
  return _d->culture;
}

int CityWinTargets::getProsperity() const
{
  return _d->prosperity;
}

int CityWinTargets::getFavour() const
{
  return _d->favour;
}

int CityWinTargets::getPeace() const
{
  return _d->peace;
}

int CityWinTargets::getPopulation() const
{
  return _d->population;
}

const std::string&CityWinTargets::getOverview() const
{
  return _d->overview;
}
