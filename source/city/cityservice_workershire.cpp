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
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "objects/engineer_post.hpp"
#include "objects/prefecture.hpp"
#include "walker/workerhunter.hpp"
#include "core/foreach.hpp"
#include "objects/constants.hpp"
#include "game/gamedate.hpp"
#include "objects/metadata.hpp"
#include "statistic.hpp"
#include "events/showinfobox.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"

using namespace constants;
using namespace std;
using namespace gfx;

namespace city
{

namespace {
CAESARIA_LITERALCONST(priorities)
CAESARIA_LITERALCONST(employers)
const unsigned int defaultHireDistance = 36;
}

class WorkersHire::Impl
{
public:
  typedef std::vector<TileOverlay::Type> BuildingsType;
  typedef std::map<TileOverlay::Group, BuildingsType> GroupBuildings;

  WalkerList hrInCity;
  unsigned int distance;
  DateTime lastMessageDate;
  HirePriorities priorities;
  GroupBuildings industryBuildings;
  std::set<TileOverlay::Type> excludeTypes;

public:
  void fillIndustryMap();
  bool haveRecruter( WorkingBuildingPtr objects );
  void hireWorkers( PlayerCityPtr city, WorkingBuildingPtr bld );
};

SrvcPtr WorkersHire::create( PlayerCityPtr city )
{
  SrvcPtr ret( new WorkersHire( city ) );
  ret->drop();

  return ret;
}

std::string WorkersHire::defaultName(){ return CAESARIA_STR_EXT(WorkersHire); }

WorkersHire::WorkersHire(PlayerCityPtr city)
  : Srvc( city, WorkersHire::defaultName() ), _d( new Impl )
{
  _d->lastMessageDate = game::Date::current();
  _d->excludeTypes.insert( objects::fountain );
  _d->fillIndustryMap();
  _d->distance = defaultHireDistance;

  load( config::load( ":workershire.model" ) );
}

void WorkersHire::Impl::fillIndustryMap()
{
  MetaDataHolder::OverlayTypes types = MetaDataHolder::instance().availableTypes();

  industryBuildings.clear();

  foreach(it, types)
  {
    const MetaData& info = MetaDataHolder::getData( *it );
    int workersNeed = info.getOption( lc_employers );
    if( workersNeed > 0 )
    {
      industryBuildings[ info.group() ].push_back( info.type() );
    }
  }
}

bool WorkersHire::Impl::haveRecruter( WorkingBuildingPtr building )
{
  foreach( w, hrInCity )
  {
    RecruterPtr hr = ptr_cast<Recruter>( *w );
    if( hr.isValid() )
    {
      if( hr->base() == building.object() )
        return true;
    }
  }

  return false;
}

void WorkersHire::Impl::hireWorkers(PlayerCityPtr city, WorkingBuildingPtr bld)
{
  if( excludeTypes.count( bld->type() ) > 0 )
    return;

  if( bld->numberWorkers() == bld->maximumWorkers() )
    return;

  if( haveRecruter( bld ) )
    return;

  if( bld->getAccessRoads().size() > 0 )
  {
    RecruterPtr hr = Recruter::create( city );
    hr->setPriority( priorities );
    hr->setMaxDistance( distance );

    hr->send2City( bld, bld->needWorkers() );
  }
}

void WorkersHire::timeStep( const unsigned int time )
{
  if( !game::Date::isWeekChanged() )
    return;

  if( _city()->population() == 0 )
    return;

  _d->hrInCity = _city()->walkers( walker::recruter );

  city::Helper helper( _city() );
  WorkingBuildingList buildings = helper.find< WorkingBuilding >( objects::any );

  if( !_d->priorities.empty() )
  {
    foreach( hireIt, _d->priorities )
    {
      industry::BuildingGroups groups = industry::toGroups( *hireIt );

      foreach( grIt, groups )
      {
        for( WorkingBuildingList::iterator it=buildings.begin(); it != buildings.end(); )
        {
          if( (*it)->group() == *grIt )
          {
            _d->hireWorkers( _city(), *it );
            it = buildings.erase( it );
          }
          else { ++it; }
        }
      }
    }
  }

  foreach( it, buildings )
  {    
    _d->hireWorkers( _city(), *it );
  }

  if( _d->lastMessageDate.monthsTo( game::Date::current() ) > DateTime::monthsInYear / 2 )
  {
    _d->lastMessageDate = game::Date::current();

    int workersNeed = statistic::getWorkersNeed( _city() );
    if( workersNeed > 20 )
    {
      events::GameEventPtr e = events::ShowInfobox::create( "##city_need_workers_title##", "##city_need_workers_text##",
                                                            events::ShowInfobox::send2scribe );
      e->dispatch();
    }
  }
}

void WorkersHire::setRecruterDistance(const unsigned int distance) {  _d->distance = distance; }

void WorkersHire::setIndustryPriority(industry::Type industry, int priority)
{
  foreach( i, _d->priorities )
  {
    if( *i == industry )
    {
      _d->priorities.erase( i );
      break;
    }
  }

  if( priority > 0 )
  {
    HirePriorities::iterator it = _d->priorities.begin();
    std::advance( it, math::clamp<int>( priority-1, 0, _d->priorities.size() ) );
    _d->priorities.insert( it, industry );
  }
}

int WorkersHire::getPriority(industry::Type industry)
{
  foreach( i, _d->priorities )
  {
    if( *i == industry )
      return (std::distance( _d->priorities.begin(), i )+1);
  }

  return 0;
}

const HirePriorities& WorkersHire::priorities() const {  return _d->priorities; }

VariantMap WorkersHire::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, distance );
  ret[ lc_priorities ] = _d->priorities.toVariantList();

  return ret;
}

void WorkersHire::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANY_D( _d, distance, stream );
  if( _d->distance == 0 ) _d->distance = defaultHireDistance;

  VariantList priorVl = stream.get( lc_priorities ).toList();

  if( !priorVl.empty() )
  {
    _d->priorities.clear();
    _d->priorities << priorVl;
  }
}

}//end namespace city
