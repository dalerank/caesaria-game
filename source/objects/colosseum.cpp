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

#include "colosseum.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/foreach.hpp"
#include "city/helper.hpp"
#include "training.hpp"
#include "core/utils.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"
#include "walker/serviceman.hpp"
#include "game/gamedate.hpp"
#include "walker/helper.hpp"
#include "objects_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(objects::colloseum, Colosseum)

class Colosseum::Impl
{
public:
  DateTime lastDateGl, lastDateLion;
};

Colosseum::Colosseum() : EntertainmentBuilding(Service::colloseum, objects::colloseum, Size(5) ), _d( new Impl )
{
  setPicture( Picture::load( ResourceGroup::entertaiment, 36));

  _animationRef().load( ResourceGroup::entertaiment, 37, 13);
  _animationRef().setOffset( Point( 122, 81 ) );

  _fgPicturesRef().resize(2);

  _addNecessaryWalker( walker::gladiator );
  _addNecessaryWalker( walker::lionTamer );
}

void Colosseum::deliverService()
{
  int saveWalkesNumber = walkers().size();
  Service::Type lastSrvc = serviceType();

  EntertainmentBuilding::deliverService();

  if( _animationRef().isRunning() )
  {
    _fgPicturesRef().front() = Picture::load( ResourceGroup::entertaiment, 50 );
    int currentWalkerNumber = walkers().size();
    if( saveWalkesNumber != currentWalkerNumber )
    {
      (lastSrvc == Service::colloseum
        ? _d->lastDateGl : _d->lastDateLion) = game::Date::current();
    }
  }
  else
  {
    _fgPicturesRef().front() = Picture::getInvalid();
    _fgPicturesRef().back() = Picture::getInvalid();
  }
}

Service::Type Colosseum::serviceType() const
{
  int lionValue = traineeValue( walker::lionTamer );
  return lionValue > 0 ? Service::colloseum : Service::amphitheater;
}

bool Colosseum::build( const CityAreaInfo& info )
{
  ServiceBuilding::build( info );

  city::Helper helper( info.city );
  GladiatorSchoolList glSchools = helper.find<GladiatorSchool>( objects::gladiatorSchool );
  LionsNurseryList lionsNs = helper.find<LionsNursery>( objects::lionsNursery );

  _d->lastDateGl = game::Date::current();
  _d->lastDateLion = game::Date::current();

  if( glSchools.empty() )
  {
    _setError( "##need_gladiator_school##" );
  }

  if( lionsNs.empty() )
  {
    _setError( "##need_gladiator_school##" );
  }

  return true;
}

std::string Colosseum::troubleDesc() const
{
  std::string ret = EntertainmentBuilding::troubleDesc();

  if( !ret.empty() )
    return ret;

  bool lions = isShowLionBattles();
  bool gladiators = isShowGladiatorBattles();
  if( !(lions && gladiators))
  {
    return "##trouble_colloseum_no_shows##";
  }

  if( lions && gladiators )  { return "##trouble_colloseum_full_work##";  }
  else if( lions ) { return "##colloseum_have_only_lions##"; }
  else             { return "##trouble_colloseum_have_only_gladiatros##"; }
}

bool Colosseum::isNeedGladiators() const
{
  city::Helper helper( _city() );
  GladiatorSchoolList colloseums = helper.find<GladiatorSchool>( objects::gladiatorSchool );

  return colloseums.empty();
}

Service::Type Colosseum::_getServiceManType() const
{
  ServiceWalkerList servicemen;
  servicemen << walkers();
  return (!servicemen.empty() ? servicemen.front()->serviceType() : Service::srvCount);
}

bool Colosseum::isShowGladiatorBattles() const {  return _getServiceManType() == Service::amphitheater; }
bool Colosseum::isShowLionBattles() const{  return _getServiceManType() == Service::colloseum; }
DateTime Colosseum::lastAnimalBoutDate() const { return _d->lastDateLion; }
DateTime Colosseum::lastGladiatorBoutDate() const { return _d->lastDateGl; }

void Colosseum::save(VariantMap& stream) const
{
  EntertainmentBuilding::save( stream );
  stream[ "lastGdate" ] = _d->lastDateGl;
  stream[ "lastLdate" ] = _d->lastDateLion;
}

void Colosseum::load(const VariantMap& stream)
{
  EntertainmentBuilding::load( stream );
  _d->lastDateGl = stream.get( "lastGdate" ).toDateTime();
  _d->lastDateLion = stream.get( "lastLdate" ).toDateTime();
}

WalkerList Colosseum::_specificWorkers() const
{
  WalkerList ret;

  foreach( i, walkers() )
  {
    if( (*i)->type() == walker::lionTamer || (*i)->type() == walker::gladiator )
      ret << *i;
  }

  return ret;
}
