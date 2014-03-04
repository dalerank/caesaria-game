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

#include "cityservice_religion.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "game/gamedate.hpp"
#include "objects/religion.hpp"
#include "game/divinity.hpp"
#include "events/event.hpp"
#include "core/foreach.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"

using namespace constants;

namespace city
{

class Religion::Impl
{
public:
  PlayerCityPtr city;
  DateTime lastDate;

  void updateRelation( RomeDivinityPtr divn );
};

SrvcPtr Religion::create(PlayerCityPtr city)
{
  SrvcPtr ret( new Religion( city ) );
  ret->drop();

  return ret;
}

std::string Religion::getDefaultName() { return "religion"; }

Religion::Religion(PlayerCityPtr city )
  : Srvc( Religion::getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastDate = GameDate::current();
}

void Religion::update( const unsigned int time )
{
  if( _d->lastDate.month() == GameDate::current().month() )
    return;

  _d->lastDate = GameDate::current();

  DivinePantheon::Divinities divinities = DivinePantheon::getInstance().getAll();

  foreach( it, divinities ) { _d->updateRelation( *it ); }
}

void Religion::Impl::updateRelation(RomeDivinityPtr divn)
{
  CityHelper helper( city );
  int peopleReached = 0;
  TempleList temples = helper.find<Temple>( building::religionGroup );

  foreach( temple, temples ) { peopleReached += ( (*temple)->getDivinity() == divn ? (*temple)->getParishionerNumber() : 0 ); }

  float faithIncome = (float)peopleReached / (float)(city->getPopulation()+1);
  divn->updateRelation( faithIncome, city );
}

}//end namespace city
