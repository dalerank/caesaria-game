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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "cityservice_workershire.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "objects/engineer_post.hpp"
#include "objects/prefecture.hpp"
#include "walker/workerhunter.hpp"
#include "core/foreach.hpp"
#include "objects/constants.hpp"
#include "core/priorities.hpp"
#include "game/gamedate.hpp"
#include <map>

using namespace constants;
using namespace std;

namespace city
{

typedef Priorities<TileOverlay::Type> HirePriorities;

class WorkersHire::Impl
{
public:
  HirePriorities priorities;
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
  : Srvc( *city.object(), WorkersHire::getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->priorities  << building::prefecture
                  << building::engineerPost
                  << building::clayPit
                  << building::wheatFarm
                  << building::grapeFarm
                  << building::granary
                  << building::ironMine
                  << building::templeCeres
                  << building::templeMars
                  << building::templeMercury
                  << building::templeNeptune
                  << building::templeVenus
                  << building::pottery
                  << building::warehouse
                  << building::forum
                  << building::doctor
                  << building::hospital
                  << building::barber
                  << building::baths
                  << building::fruitFarm
                  << building::oliveFarm
                  << building::vegetableFarm
                  << building::pigFarm
                  << building::senate
                  << building::market
                  << building::timberLogger
                  << building::marbleQuarry
                  << building::furnitureWorkshop
                  << building::weaponsWorkshop
                  << building::theater
                  << building::actorColony
                  << building::school
                  << building::amphitheater
                  << building::gladiatorSchool
                  << building::wharf
                  << building::barracks
                  << building::tower
                  << building::creamery
                  << building::academy
                  << building::colloseum
                  << building::lionsNursery
                  << building::shipyard
                  << building::dock
                  << building::library
                  << building::hippodrome
                  << building::chariotSchool
                  << building::winery;
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
  Helper hlp( _d->city );
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
