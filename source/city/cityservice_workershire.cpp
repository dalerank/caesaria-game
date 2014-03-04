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

#include "cityservice_workershire.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "objects/engineer_post.hpp"
#include "objects/prefecture.hpp"
#include "walker/workerhunter.hpp"
#include "core/foreach.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include <map>

using namespace constants;
using namespace std;

namespace city
{

typedef vector< TileOverlay::Type > Priorities;

class WorkersHire::Impl
{
public:
  Priorities priorities;
  WalkerList hrInCity;
  PlayerCityPtr city;
};

SrvcPtr WorkersHire::create(PlayerCityPtr city )
{
  SrvcPtr ret( new WorkersHire( city ));
  ret->drop();

  return ret;
}

string WorkersHire::getDefaultName(){ return "workershire"; }

WorkersHire::WorkersHire(PlayerCityPtr city )
  : Srvc( WorkersHire::getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->priorities.push_back( building::prefecture );
  _d->priorities.push_back( building::engineerPost );
  _d->priorities.push_back( building::clayPit );
  _d->priorities.push_back( building::wheatFarm );
  _d->priorities.push_back( building::grapeFarm );
  _d->priorities.push_back( building::granary );
  _d->priorities.push_back( building::ironMine );
  _d->priorities.push_back( building::templeCeres );
  _d->priorities.push_back( building::templeMars );
  _d->priorities.push_back( building::templeMercury );
  _d->priorities.push_back( building::templeNeptune );
  _d->priorities.push_back( building::templeVenus );
  _d->priorities.push_back( building::pottery );
  _d->priorities.push_back( building::warehouse );
  _d->priorities.push_back( building::forum );
  _d->priorities.push_back( building::doctor );
  _d->priorities.push_back( building::hospital );
  _d->priorities.push_back( building::barber );
  _d->priorities.push_back( building::baths );
  _d->priorities.push_back( building::fruitFarm );
  _d->priorities.push_back( building::oliveFarm );
  _d->priorities.push_back( building::vegetableFarm );
  _d->priorities.push_back( building::pigFarm );
  _d->priorities.push_back( building::senate );
  _d->priorities.push_back( building::market );
  _d->priorities.push_back( building::timberLogger );
  _d->priorities.push_back( building::marbleQuarry );
  _d->priorities.push_back( building::furnitureWorkshop );
  _d->priorities.push_back( building::weaponsWorkshop );
  _d->priorities.push_back( building::theater );
  _d->priorities.push_back( building::actorColony );
  _d->priorities.push_back( building::school );
  _d->priorities.push_back( building::amphitheater );
  _d->priorities.push_back( building::gladiatorSchool );
  _d->priorities.push_back( building::wharf );
  _d->priorities.push_back( building::barracks );
  _d->priorities.push_back( building::tower );
  _d->priorities.push_back( building::creamery );
  _d->priorities.push_back( building::academy );
  _d->priorities.push_back( building::colloseum );
  _d->priorities.push_back( building::lionsNursery );
  _d->priorities.push_back( building::shipyard );
  _d->priorities.push_back( building::dock );
  _d->priorities.push_back( building::library );
}

bool WorkersHire::_haveHr( WorkingBuildingPtr building )
{
  foreach( w, _d->hrInCity )
  {
    RecruterPtr hr = ptr_cast<Recruter>( *w );
    if( hr.isValid() )
    {
      if( hr->getBase() == building.object() )
        return true;
    }
  }

  return false;
}

void WorkersHire::_hireByType(const TileOverlay::Type type )
{
  CityHelper hlp( _d->city );
  WorkingBuildingList buildings = hlp.find< WorkingBuilding >( type );
  foreach( it, buildings )
  {
    WorkingBuildingPtr wrkbld = *it;
    if( _haveHr( wrkbld ) )
      continue;

    if( wrkbld->getAccessRoads().size() > 0 && wrkbld->numberWorkers() < wrkbld->maxWorkers() )
    {
      RecruterPtr hr = Recruter::create( _d->city );
      hr->setMaxDistance( 20 );
      hr->send2City( wrkbld, wrkbld->maxWorkers() - wrkbld->numberWorkers());
    }
  }
}

void WorkersHire::update( const unsigned int time )
{
  if( time % (GameDate::ticksInMonth()/2) != 1 )
    return;

  //unsigned int vacantPop=0;

  _d->hrInCity = _d->city->getWalkers( walker::recruter );

  foreach( pr, _d->priorities )
  {
    _hireByType( *pr );
  }
}

}//end namespace city
