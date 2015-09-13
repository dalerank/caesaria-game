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
  int firstFestivalSentinment[ festival::count] = { 7, 9, 12 };
  int secondFesivalSentiment[ festival::count] = { 2, 3, 5 };

  const char* festivalTitles[ festival::count] = { "", "##small_festival##", "##middle_festival##", "##great_festival##" };
  const char* festivalDesc[ festival::count] = { "", "##small_fest_description##", "##middle_fest_description##", "##big_fest_description##" };
  const char* festivalVideo[ festival::count] = { "", "festival1_feast", "festival3_glad", "festival2_chariot" };
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
  divinity = (religion::RomeDivinityType)stream.get( 1 ).toInt();
  size = stream.get( 2 ).toInt();
}

class Festival::Impl
{
public:
  FestivalInfo nextfest;
  FestivalInfo lastfest;
  FestivalInfo oldfest;

  History history;
};

SrvcPtr Festival::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Festival( city ) );
  ret->drop();

  return ret;
}

std::string Festival::defaultName() {  return CAESARIA_STR_EXT(Festival); }

void Festival::now() { _d->nextfest.date = game::Date::current(); }
DateTime Festival::lastFestival() const { return _d->lastfest.date; }
DateTime Festival::nextFestival() const { return _d->nextfest.date; }

void Festival::assign( RomeDivinityType name, int size )
{
  _d->nextfest.size = (festival::Type)size;
  _d->nextfest.date= game::Date::current();
  _d->nextfest.date.appendMonth( festival::prepareMonthsDelay + size );
  _d->nextfest.divinity = name;

  GameEventPtr e = Payment::create( econ::Issue::sundries, -_city()->statistic().festival.calcCost( (FestivalType)size ) );
  e->dispatch();
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

  const DateTime currentDate = game::Date::current();
  if( _d->oldfest.date.monthsTo( currentDate ) >= DateTime::monthsInYear )
  {
    int* sentimentValues = (_d->lastfest.date.monthsTo( game::Date::current() ) < DateTime::monthsInYear )
                                ? secondFesivalSentiment
                                : firstFestivalSentinment;

    sentimentValue = sentimentValues[ _d->nextfest.size ];
  }

  _d->oldfest = _d->lastfest;
  _d->lastfest.date = currentDate;
  _d->nextfest.date = DateTime( -550, 1, 1 );

  _d->history.push_back( _d->oldfest );

  rome::Pantheon::doFestival( _d->nextfest.divinity, _d->nextfest.size );

  int id = math::clamp<int>( _d->nextfest.size, festival::none, festival::big );
  GameEventPtr e = ShowFeastival::create( _(festivalDesc[ id ]), _(festivalTitles[ id ]),
                                          _city()->mayor()->name(), festivalVideo[ id ] );
  e->dispatch();

  e = UpdateCitySentiment::create( sentimentValue );
  e->dispatch();

  e = UpdateHouseService::create( Service::crime, -firstFestivalSentinment[ _d->nextfest.size ] );
  e->dispatch();
}

}//end namespace city
