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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "cityservice_festival.hpp"
#include "game/gamedate.hpp"
#include "city.hpp"
#include "city/statistic.hpp"
#include "religion/pantheon.hpp"
#include "events/showfeastwindow.hpp"
#include "events/updatecitysentiment.hpp"
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

SrvcPtr Festival::create(PlayerCityPtr city )
{
  SrvcPtr ret( new Festival( city ) );
  ret->drop();

  return ret;
}

std::string Festival::defaultName() {  return "festival";}
DateTime Festival::lastFestivalDate() const{  return _d->lastFestivalDate;}
DateTime Festival::nextFestivalDate() const{  return _d->festivalDate; }

void Festival::assignFestival( RomeDivinityType name, int size )
{
  _d->festivalType = size;
  _d->festivalDate = GameDate::current();
  _d->festivalDate.appendMonth( 2 + size );
  _d->divinity = name;

  events::GameEventPtr e = events::FundIssueEvent::create( city::Funds::sundries, city::Statistic::getFestivalCost( &_city, (FestivalType)size ) );
  e->dispatch();
}

Festival::Festival(PlayerCityPtr city )
: Srvc( *city.object(), defaultName() ), _d( new Impl )
{
  _d->lastFestivalDate = GameDate::current();
  _d->festivalDate = DateTime( -550, 0, 0 );
  _d->prevFestivalDate = DateTime( -550, 0, 0 );
}

void Festival::update( const unsigned int time )
{
  if( !GameDate::isWeekChanged() )
    return;

  const DateTime currentDate = GameDate::current();
  if( _d->festivalDate.year() == currentDate.year()
      && _d->festivalDate.month() == currentDate.month() )
  {
    int sentimentValue = 0;

    if( _d->prevFestivalDate.monthsTo( currentDate ) >= 12 )
    {
      int* sentimentValues = (_d->lastFestivalDate.monthsTo( GameDate::current() ) < 12)
                                  ? secondFesivalSentiment
                                  : firstFestivalSentinment;

      sentimentValue = sentimentValues[ _d->festivalType ];
    }

    _d->prevFestivalDate = _d->lastFestivalDate;
    _d->lastFestivalDate = currentDate;
    _d->festivalDate = DateTime( -550, 1, 1 );

    rome::Pantheon::doFestival( _d->divinity, _d->festivalType );

    int id = math::clamp<int>( _d->festivalType, 0, 3 );
    events::GameEventPtr e = events::ShowFeastWindow::create( festivalDesc[ id ], festivalTitles[ id ], _city.player()->name() );
    e->dispatch();

    e = events::UpdateCitySentiment::create( sentimentValue );
    e->dispatch();
  }
}

VariantMap Festival::save() const
{
  VariantMap ret;
  ret[ "lastDate" ] = _d->lastFestivalDate;
  ret[ "prevDate" ] = _d->prevFestivalDate;
  ret[ "nextDate" ] = _d->festivalDate;
  ret[ "divinity" ] = (int)_d->divinity;
  ret[ "festival" ] = _d->festivalType;

  return ret;
}

void Festival::load( const VariantMap& stream)
{
  _d->lastFestivalDate = stream.get( "lastDate" ).toDateTime();
  _d->prevFestivalDate = stream.get( "prevDate" ).toDateTime();
  _d->festivalDate = stream.get( "nextDate" ).toDateTime();
  _d->divinity = (RomeDivinityType)stream.get( "divinity" ).toInt();
  _d->festivalType = (int)stream.get( "festival" );
}

}//end namespace city
