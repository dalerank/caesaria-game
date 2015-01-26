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

#include "cityservice_festival.hpp"
#include "game/gamedate.hpp"
#include "city.hpp"
#include "city/statistic.hpp"
#include "core/variant_map.hpp"
#include "religion/pantheon.hpp"
#include "events/showfeastwindow.hpp"
#include "events/updatecitysentiment.hpp"
#include "events/updatehouseservice.hpp"
#include "events/fundissue.hpp"
#include "city/funds.hpp"

using namespace religion;

namespace city
{

namespace {
  typedef enum { ftNone=0, ftSmall, ftMiddle, ftBig, ftCount } FestType;
  int firstFestivalSentinment[ftCount] = { 7, 9, 12 };
  int secondFesivalSentiment[ftCount] = { 2, 3, 5 };

  const char* festivalTitles[ftCount] = { "", "##small_festival##", "##middle_festival##", "##great_festival##" };
  const char* festivalDesc[ftCount] = { "", "##small_fest_description##", "##middle_fest_description##", "##big_fest_description##" };
  const char* festivalVideo[ftCount] = { "", ":/smk/festival1_feast.smk", ":/smk/festival3_Glad.smk", ":/smk/festival2_chariot.smk" };
}

class Festival::Impl
{
public:
  DateTime prevFestivalDate;
  DateTime lastFestivalDate;
  DateTime festivalDate;

  RomeDivinityType divinity;  
  int festivalType;
};

SrvcPtr Festival::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Festival( city ) );
  ret->drop();

  return ret;
}

std::string Festival::defaultName() {  return CAESARIA_STR_EXT(Festival); }
DateTime Festival::lastFestivalDate() const { return _d->lastFestivalDate; }
DateTime Festival::nextFestivalDate() const { return _d->festivalDate; }

void Festival::assignFestival( RomeDivinityType name, int size )
{
  _d->festivalType = size;
  _d->festivalDate = game::Date::current();
  _d->festivalDate.appendMonth( 2 + size );
  _d->divinity = name;

  events::GameEventPtr e = events::FundIssueEvent::create( city::Funds::sundries, city::statistic::getFestivalCost( _city(), (FestivalType)size ) );
  e->dispatch();
}

Festival::Festival(PlayerCityPtr city)
: Srvc( city, defaultName() ), _d( new Impl )
{
  _d->lastFestivalDate = game::Date::current();
  _d->festivalDate = DateTime( -550, 0, 0 );
  _d->prevFestivalDate = DateTime( -550, 0, 0 );
}

void Festival::timeStep(const unsigned int time )
{
  if( !game::Date::isWeekChanged() )
    return;

  const DateTime currentDate = game::Date::current();
  if( _d->festivalDate.year() == currentDate.year()
      && _d->festivalDate.month() == currentDate.month() )
  {
    int sentimentValue = 0;

    if( _d->prevFestivalDate.monthsTo( currentDate ) >= 12 )
    {
      int* sentimentValues = (_d->lastFestivalDate.monthsTo( game::Date::current() ) < 12)
                                  ? secondFesivalSentiment
                                  : firstFestivalSentinment;

      sentimentValue = sentimentValues[ _d->festivalType ];
    }

    _d->prevFestivalDate = _d->lastFestivalDate;
    _d->lastFestivalDate = currentDate;
    _d->festivalDate = DateTime( -550, 1, 1 );

    rome::Pantheon::doFestival( _d->divinity, _d->festivalType );

    int id = math::clamp<int>( _d->festivalType, 0, 3 );
    events::GameEventPtr e = events::ShowFeastival::create( festivalDesc[ id ], festivalTitles[ id ],
                                                              _city()->player()->name(), festivalVideo[ id ] );
    e->dispatch();

    e = events::UpdateCitySentiment::create( sentimentValue );
    e->dispatch();

    e = events::UpdateHouseService::create( Service::crime, -firstFestivalSentinment[ _d->festivalType ] );
    e->dispatch();
  }
}

VariantMap Festival::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, lastFestivalDate )
  VARIANT_SAVE_ANY_D( ret, _d, prevFestivalDate )
  VARIANT_SAVE_ANY_D( ret, _d, festivalDate )
  VARIANT_SAVE_ENUM_D( ret, _d, divinity )
  VARIANT_SAVE_ANY_D( ret, _d, festivalType )

  return ret;
}

void Festival::load( const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, lastFestivalDate, stream )
  VARIANT_LOAD_TIME_D( _d, prevFestivalDate, stream )
  VARIANT_LOAD_TIME_D( _d, festivalDate, stream )
  VARIANT_LOAD_ENUM_D( _d, divinity, stream )
  VARIANT_LOAD_ANY_D( _d, festivalType, stream )
}

}//end namespace city
