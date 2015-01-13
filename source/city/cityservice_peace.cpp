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
#include "objects/house_level.hpp"
#include "walker/rioter.hpp"
#include "events/disaster.hpp"
#include "events/showinfobox.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"
#include <set>

using namespace constants;

namespace city
{

class Peace::Impl
{
public:
  unsigned int peaceYears;
  bool protestorOrMugglerSeen;
  bool rioterSeen;
  bool someCriminalSeen;
  int value;
  bool significantBuildingsDestroyed;
  DateTime lastMessageDate;

  Priorities<int> unsignificantBuildings;
};

city::SrvcPtr Peace::create( PlayerCityPtr city )
{
  city::SrvcPtr ret( new Peace( city ) );
  ret->drop();

  return ret;
}

Peace::Peace( PlayerCityPtr city )
  : city::Srvc( city, defaultName() ), _d( new Impl )
{
  _d->peaceYears = 0;
  _d->protestorOrMugglerSeen = false;
  _d->someCriminalSeen = false;
  _d->rioterSeen = false;
  _d->value = 0;
  _d->significantBuildingsDestroyed = false;

  _d->unsignificantBuildings << objects::prefecture
                         << objects::engineering_post
                         << objects::well
                         << objects::fortArea
                         << objects::fort_javelin
                         << objects::fort_legionaries
                         << objects::fort_horse
                         << objects::gatehouse
                         << objects::fortification
                         << objects::road
                         << objects::plaza
                         << objects::high_bridge
                         << objects::low_bridge
                         << objects::tower;
}

void Peace::timeStep(const unsigned int time )
{
  if( !game::Date::isYearChanged() )
    return;

  city::MilitaryPtr ml;
  ml << _city()->findService( city::Military::defaultName() );

  int change= _d->protestorOrMugglerSeen ? -1: 0;

  if( ml.isValid() )
  {
    if( ml->haveNotification( city::Military::Notification::chastener ) )
      change -= 1;

    if( ml->haveNotification( city::Military::Notification::barbarian ) )
      change -= 1;
  }

  change -= std::min( _d->rioterSeen ? -5 : 0, _d->value );
  change -= std::min( _d->significantBuildingsDestroyed ? -1 : 0, _d->value );

  if( change == 0 )
  {
    change = _d->peaceYears > 2 ? 5 : 2;
  }

  if( _d->protestorOrMugglerSeen || _d->rioterSeen )
  {
    _d->peaceYears = 0;
  }
  else
  {
    _d->peaceYears++;
  }

  change = math::clamp<int>( change, -5, 5 );

  _d->value = math::clamp<int>( _d->value + change, 0, 100  );
  _d->protestorOrMugglerSeen  = false;
  _d->rioterSeen = false;
  _d->significantBuildingsDestroyed = false;
}

void Peace::addCriminal( WalkerPtr wlk )
{
  if( is_kind_of<Rioter>( wlk ) )
  {
    _d->rioterSeen = true;
  }
  /*else if( is_kind_of<Protestor>( wlk ) )
  {
    _d->protestorOrMugglerSeen = true;
  }*/
  else
  {
    Logger::warning( "Peace:addCriminal unknown walker %d", wlk->type() );
    _d->someCriminalSeen = true;
  }
}

void Peace::buildingDestroyed(gfx::TileOverlayPtr overlay, int why)
{
  if( overlay.isNull() )
  {
    Logger::warning( "WARNING!!! Peace::buildingDestroyed overlay is null" );
    return;
  }

  HousePtr house = ptr_cast<House>( overlay );
  if( house.isValid() && house->spec().level() > HouseLevel::tent )
  {
    _d->significantBuildingsDestroyed = true;
  }
  else
  {
    _d->significantBuildingsDestroyed |= !_d->unsignificantBuildings.count( overlay->type() );
  }

  if( _d->lastMessageDate.monthsTo( game::Date::current() ) > 1 )
  {
    std::string title;
    std::string text;
    std::string video;

    _d->lastMessageDate = game::Date::current();

    switch( why )
    {
    case events::Disaster::collapse:
      title = "##collapsed_building_title##";
      text = "##collapsed_building_text##";
    break;

    case events::Disaster::fire:
      title = "##city_fire_title##";
      text = "##city_fire_text##";
      video = ":/smk/city_fire.smk";
    break;

    case events::Disaster::riots:
      title = "##destroyed_building_title##";
      text = "##rioter_rampaging_accross_city##";
      video = ":/smk/riot.smk";
    break;
    }

    if( !title.empty() )
    {
      events::GameEventPtr e = events::ShowInfobox::create( title, text, false, video );
      e->dispatch();
    }
  }
}

int Peace::value() const { return _d->value; }
std::string Peace::defaultName() { return CAESARIA_STR_EXT(Peace); }

std::string Peace::reason() const
{
  if( _d->rioterSeen ) { return "##last_riots_bad_for_peace_rating##"; }

  return "";
}

VariantMap Peace::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, peaceYears )
  VARIANT_SAVE_ANY_D( ret, _d, someCriminalSeen )
  VARIANT_SAVE_ANY_D( ret, _d, protestorOrMugglerSeen)
  VARIANT_SAVE_ANY_D( ret, _d, rioterSeen )
  VARIANT_SAVE_ANY_D( ret, _d, value )
  VARIANT_SAVE_ANY_D( ret, _d, significantBuildingsDestroyed )

  return ret;
}

void Peace::load(const VariantMap& stream)
{
  VARIANT_LOAD_ANY_D( _d, peaceYears, stream )
  VARIANT_LOAD_ANY_D( _d, someCriminalSeen, stream )
  VARIANT_LOAD_ANY_D( _d, protestorOrMugglerSeen, stream )
  VARIANT_LOAD_ANY_D( _d, rioterSeen, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )
  VARIANT_LOAD_ANY_D( _d, significantBuildingsDestroyed, stream )
}

}
