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

#include "cityservice_peace.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"
#include "core/priorities.hpp"
#include "objects/house.hpp"
#include "cityservice_military.hpp"
#include "objects/house_spec.hpp"
#include "walker/rioter.hpp"
#include "events/disaster.hpp"
#include "events/showinfobox.hpp"
#include "core/logger.hpp"
#include "walker/mugger.hpp"
#include "core/variant_map.hpp"
#include "statistic.hpp"
#include "core/stacktrace.hpp"
#include "cityservice_factory.hpp"
#include <set>

using namespace events;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Peace,peace)

enum { minCh=1, middleCh=2, maxCh=5};
static const int longTimeWithoutWar = 2;

struct ThreatSeen
{
  bool protestor;
  bool mugger;
  bool rioter;
  bool criminal;

  inline void clear() { protestor = mugger = rioter = criminal = false; }
};

class Peace::Impl
{
public:
  ThreatSeen threats;
  unsigned int peaceYears;
  int value;
  bool significantBuildingsDestroyed;
  DateTime lastMessageDate;

  Vector<object::Type> unsignificantBuildings;
};

SrvcPtr Peace::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Peace( city ) );
  ret->drop();

  return ret;
}

Peace::Peace( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{
  _d->peaceYears = 0;
  _d->threats.clear();
  _d->value = 0;
  _d->significantBuildingsDestroyed = false;

  _d->unsignificantBuildings << object::prefecture
                             << object::engineering_post
                             << object::well
                             << object::fortArea
                             << object::fort_javelin
                             << object::fort_legionaries
                             << object::fort_horse
                             << object::gatehouse
                             << object::fortification
                             << object::road
                             << object::plaza
                             << object::high_bridge
                             << object::low_bridge
                             << object::tower;
}

void Peace::timeStep(const unsigned int time )
{
  if( !game::Date::isYearChanged() )
    return;

  MilitaryPtr ml = _city()->statistic().services.find<Military>();
  if( ml.isNull() )
  {
    Logger::warning( "!!! WARNING: not found military service" );
    crashhandler::printstack(false);
    return;
  }

  int change = (_d->threats.protestor || _d->threats.mugger) ? -minCh: 0;

  if( ml->haveNotification( Notification::chastener ) )
    change -= 1;

  if( ml->haveNotification( Notification::barbarian ) )
    change -= 1;

  change -= std::min( _d->threats.rioter ? -maxCh : 0, _d->value );
  change -= std::min( _d->significantBuildingsDestroyed ? -minCh : 0, _d->value );

  if( change == 0 )
  {
    change = _d->peaceYears > longTimeWithoutWar ? maxCh : middleCh;
  }

  if( _d->threats.protestor || _d->threats.mugger || _d->threats.rioter )
  {
    _d->peaceYears = 0;
  }
  else
  {
    _d->peaceYears++;
  }

  change = math::clamp<int>( change, -maxCh, maxCh );

  _d->value = math::clamp<int>( _d->value + change, 0, 100  );
  _d->threats.clear();
  _d->significantBuildingsDestroyed = false;
}

void Peace::addCriminal( WalkerPtr wlk )
{
  if( wlk.is<Rioter>() )   {    _d->threats.rioter = true;  }
  //else if( is_kind_of<Protestor>( wlk ) )   {    _d->threats.protestor = true;  }
  else if( wlk .is<Mugger>() ) { _d->threats.mugger = true; }
  else
  {
    Logger::warning( "Peace: addCriminal unknown walker %d", wlk->type() );
    _d->threats.criminal = true;
  }
}

void Peace::buildingDestroyed(OverlayPtr overlay, int why)
{
  if( overlay.isNull() )
  {
    Logger::warning( "WARNING!!! Peace::buildingDestroyed overlay is null" );
    return;
  }

  HousePtr house = overlay.as<House>();
  if( house.isValid() && house->spec().level() > HouseLevel::tent )
  {
    _d->significantBuildingsDestroyed = true;
  }
  else
  {
    _d->significantBuildingsDestroyed |= !_d->unsignificantBuildings.contain( overlay->type() );
  }

  if( _d->lastMessageDate.monthsTo( game::Date::current() ) > 1 )
  {
    std::string title;
    std::string text;
    std::string video;

    _d->lastMessageDate = game::Date::current();

    switch( why )
    {
    case Disaster::collapse:
      title = "##collapsed_building_title##";
      text = "##collapsed_building_text##";
    break;

    case Disaster::fire:
      title = "##city_fire_title##";
      text = "##city_fire_text##";
      video = "city_fire";
    break;

    case Disaster::riots:
      title = "##destroyed_building_title##";
      text = "##rioter_rampaging_accross_city##";
      video = "riot";
    break;
    }

    if( !title.empty() )
    {
      GameEventPtr e = ShowInfobox::create( title, text, false, video );
      e->dispatch();
    }
  }
}

int Peace::value() const { return _d->value; }
std::string Peace::defaultName() { return CAESARIA_STR_EXT(Peace); }

std::string Peace::reason() const
{
  if( _d->threats.rioter ) { return "##last_riots_bad_for_peace_rating##"; }

  return "";
}

VariantMap Peace::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, peaceYears )
  VARIANT_SAVE_ANY_D( ret, _d, threats.criminal )
  VARIANT_SAVE_ANY_D( ret, _d, threats.protestor )
  VARIANT_SAVE_ANY_D( ret, _d, threats.mugger )
  VARIANT_SAVE_ANY_D( ret, _d, threats.rioter )
  VARIANT_SAVE_ANY_D( ret, _d, value )
  VARIANT_SAVE_ANY_D( ret, _d, significantBuildingsDestroyed )

  return ret;
}

void Peace::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANY_D( _d, peaceYears, stream )
  VARIANT_LOAD_ANY_D( _d, threats.criminal, stream )
  VARIANT_LOAD_ANY_D( _d, threats.protestor, stream )
  VARIANT_LOAD_ANY_D( _d, threats.mugger, stream )
  VARIANT_LOAD_ANY_D( _d, threats.rioter, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )
  VARIANT_LOAD_ANY_D( _d, significantBuildingsDestroyed, stream )
}

}//end namespace city
