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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "cityservice_festival.hpp"
#include "game/gamedate.hpp"
#include "city.hpp"
#include "game/player.hpp"
#include "city/statistic.hpp"
#include "objects/construction.hpp"
#include "core/gettext.hpp"
#include "core/variant_map.hpp"
#include "religion/pantheon.hpp"
#include "events/showfeastwindow.hpp"
#include "events/updatecitysentiment.hpp"
#include "events/updatehouseservice.hpp"
#include "events/fundissue.hpp"
#include "game/funds.hpp"
#include "cityservice_factory.hpp"
#include "config.hpp"

using namespace religion;
using namespace config;
using namespace events;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Festival,festival)

namespace {
  struct FestivalDesc
  {
    int first;
    int second;
    std::string title;
    std::string desc;
    std::string video;
  };

  static std::map< FestivalInfo::Type, FestivalDesc > festivalDesc =
  {
   { FestivalInfo::none,   { 0, 0, "##unk_festival##",    "##unk_festival_description##", ""                  } },
   { FestivalInfo::small,  { 7, 2, "##small_festival##",  "##small_fest_description##",   "festival1_feast"   } },
   { FestivalInfo::middle, { 9, 3, "##middle_festival##", "##middle_fest_description##",  "festival3_glad"    } },
   { FestivalInfo::big,    {12, 5, "##great_festival##",  "##big_fest_description##",     "festival2_chariot" } },
  };

  FestivalDesc& findDesc( FestivalInfo::Type type )
  {
    auto it = festivalDesc.find( type );
    return it != festivalDesc.end() ? it->second : festivalDesc[ FestivalInfo::none ];
  }
}

class History : public std::vector<FestivalInfo>
{
public:
  VariantList save() const
  {
    VariantList ret;

    return ret;
  }

  void load( const VariantList& stream )
  {

  }
};

VariantList FestivalInfo::save() const
{
  VariantList ret;
  ret << date << (int)divinity << size;

  return ret;
}

void FestivalInfo::load(const VariantList& stream)
{
  date = stream.get( 0 ).toDateTime();
  divinity = stream.get( 1 ).toEnum<religion::RomeDivinity::Type>();
  size = stream.get( 2 ).toEnum<FestivalInfo::Type>();
}

class Festival::Impl
{
public:
  FestivalInfo nextfest;
  FestivalInfo lastfest;
  FestivalInfo oldfest;

  History history;
};

std::string Festival::defaultName() {  return CAESARIA_STR_EXT(Festival); }

void Festival::doFestivalNow() { _d->nextfest.date = game::Date::current(); }
DateTime Festival::last() const { return _d->lastfest.date; }
DateTime Festival::next() const { return _d->nextfest.date; }

void Festival::assign(RomeDivinity::Type name, int size )
{
  _d->nextfest.size = (FestivalInfo::Type)size;
  _d->nextfest.date= game::Date::current();
  _d->nextfest.date.appendMonth( festival::prepareMonthsDelay + size );
  _d->nextfest.divinity = name;

  events::dispatch<Payment>( econ::Issue::sundries, -_city()->statistic().festival.calcCost( (FestivalType)size ) );
}

Festival::Festival(PlayerCityPtr city)
: Srvc( city, defaultName() ), _d( new Impl )
{
  _d->lastfest.date = game::Date::current();
  _d->nextfest.date = DateTime( -550, 0, 0 );
  _d->oldfest.date = DateTime( -550, 0, 0 );
}

void Festival::timeStep(const unsigned int time )
{
  if( !game::Date::isWeekChanged() )
    return;

  const DateTime currentDate = game::Date::current();
  if( _d->nextfest.date.year() == currentDate.year()
      && _d->nextfest.date.month() == currentDate.month() )
  {
    _doFestival();
  }
}

VariantMap Festival::save() const
{
  VariantMap ret;
  VARIANT_SAVE_CLASS_D ( ret, _d, oldfest )
  VARIANT_SAVE_CLASS_D ( ret, _d, lastfest )
  VARIANT_SAVE_CLASS_D ( ret, _d, nextfest )
  VARIANT_SAVE_CLASS_D ( ret, _d, history )

  return ret;
}

void Festival::load( const VariantMap& stream)
{
  VARIANT_LOAD_CLASS_D_LIST( _d, lastfest,     stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, oldfest,      stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, nextfest,     stream )
  VARIANT_LOAD_CLASS_D_LIST( _d, history,      stream )
}

void Festival::_doFestival()
{
  int sentimentValue = 0;
  int festSize = math::clamp<int>( _d->nextfest.size, FestivalInfo::none, FestivalInfo::big );
  const FestivalDesc& info = findDesc( FestivalInfo::Type( festSize ) );

  const DateTime currentDate = game::Date::current();
  if( _d->oldfest.date.monthsTo( currentDate ) >= DateTime::monthsInYear )
  {
    sentimentValue = (_d->lastfest.date.monthsTo( game::Date::current() ) < DateTime::monthsInYear )
                                ? info.second
                                : info.first;
  }

  _d->oldfest = _d->lastfest;
  _d->lastfest.date = currentDate;
  _d->nextfest.date = DateTime( -550, 1, 1 );

  _d->history.push_back( _d->oldfest );

  rome::Pantheon::doFestival( _d->nextfest.divinity, festSize );

  events::dispatch<ShowFeastival>( _(info.desc), _(info.title),
                                   _city()->mayor()->name(), info.video );

  events::dispatch<UpdateCitySentiment>( sentimentValue );
  events::dispatch<UpdateHouseService>( Service::crime, -sentimentValue );
}

}//end namespace city
