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
#include "city.hpp"
#include "core/safetycast.hpp"
#include "building/engineer_post.hpp"
#include "building/prefecture.hpp"
#include "walker/workerhunter.hpp"
#include "core/foreach.hpp"
#include "building/constants.hpp"
#include <map>

using namespace constants;
using namespace std;

typedef map< int, TileOverlay::Type > Priorities;

class CityServiceWorkersHire::Impl
{
public:
  Priorities priorities;
  WalkerList hrInCity;
  PlayerCityPtr city;
};

CityServicePtr CityServiceWorkersHire::create(PlayerCityPtr city )
{
  CityServicePtr ret( new CityServiceWorkersHire( city ));
  ret->drop();

  return ret;
}

CityServiceWorkersHire::CityServiceWorkersHire(PlayerCityPtr city )
: CityService( "workershire" ), _d( new Impl )
{
  _d->city = city;
  _d->priorities[ 1 ] = building::prefecture;
  _d->priorities[ 2 ] = building::engineerPost;
  _d->priorities[ 3 ] = building::clayPit;
  _d->priorities[ 4 ] = building::wheatFarm;
  _d->priorities[ 5 ] = building::grapeFarm;
  _d->priorities[ 6 ] = building::granary;
  _d->priorities[ 7 ] = building::ironMine;
  _d->priorities[ 8 ] = building::templeCeres;
  _d->priorities[ 9 ] = building::pottery;
  _d->priorities[ 10 ] = building::warehouse;
  _d->priorities[ 11 ] = building::forum;
  _d->priorities[ 12 ] = building::doctor;
  _d->priorities[ 13 ] = building::hospital;
  _d->priorities[ 14 ] = building::barber;
  _d->priorities[ 15 ] = building::baths;
  _d->priorities[ 16 ] = building::fruitFarm;
  _d->priorities[ 17 ] = building::oliveFarm;
  _d->priorities[ 18 ] = building::vegetableFarm;
  _d->priorities[ 19 ] = building::pigFarm;
  _d->priorities[ 20 ] = building::senate;
  _d->priorities[ 21 ] = building::market;
  _d->priorities[ 22 ] = building::timberLogger;
  _d->priorities[ 23 ] = building::marbleQuarry;
  _d->priorities[ 24 ] = building::furnitureWorkshop;
  _d->priorities[ 25 ] = building::weaponsWorkshop;
  _d->priorities[ 26 ] = building::theater;
  _d->priorities[ 27 ] = building::actorColony;
  _d->priorities[ 28 ] = building::school;
  _d->priorities[ 29 ] = building::amphitheater;
  _d->priorities[ 30 ] = building::gladiatorSchool;
  _d->priorities[ 31 ] = building::wharf;
  _d->priorities[ 32 ] = building::barracks;
  _d->priorities[ 33 ] = building::tower;
  _d->priorities[ 34 ] = building::creamery;
  _d->priorities[ 35 ] = building::academy;
}

bool CityServiceWorkersHire::_haveHr( WorkingBuildingPtr building )
{
  foreach( WalkerPtr walker, _d->hrInCity )
  {
    RecruterPtr hr = walker.as<Recruter>();
    if( hr.isValid() )
    {
      if( hr->getBase() == building.as<Building>() )
        return true;
    }
  }

  return false;
}

void CityServiceWorkersHire::_hireByType(const TileOverlay::Type type )
{
  CityHelper hlp( _d->city );
  WorkingBuildingList buildings = hlp.find< WorkingBuilding >( type );
  foreach( WorkingBuildingPtr wrkbld, buildings )
  {
    if( _haveHr( wrkbld ) )
      continue;

    if( wrkbld->getAccessRoads().size() > 0 && wrkbld->getWorkersCount() < wrkbld->getMaxWorkers() )
    {
      RecruterPtr hr = Recruter::create( _d->city );
      hr->setMaxDistance( 20 );
      hr->send2City( wrkbld, wrkbld->getMaxWorkers() - wrkbld->getWorkersCount());
    }
  }
}

void CityServiceWorkersHire::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  //unsigned int vacantPop=0;

  _d->hrInCity = _d->city->getWalkers( walker::recruter );

  foreach( Priorities::value_type& pr, _d->priorities )
  {
    _hireByType( pr.second );
  }
}
