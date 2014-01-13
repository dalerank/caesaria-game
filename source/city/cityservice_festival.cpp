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
#include "game/divinity.hpp"
#include "events/showfeastwindow.hpp"

class CityServiceFestival::Impl
{
public:
  PlayerCityPtr city;

  DateTime lastFestivalDate;
  DateTime festivalDate;
  RomeDivinityType divinity;  
  int festivalType;
};

CityServicePtr CityServiceFestival::create(PlayerCityPtr city )
{
  CityServicePtr ret( new CityServiceFestival( city ) );
  ret->drop();

  return ret;
}

std::string CityServiceFestival::getDefaultName()
{
  return "festival";
}

DateTime CityServiceFestival::getLastFestivalDate() const
{
  return _d->lastFestivalDate;
}

DateTime CityServiceFestival::getNextFestivalDate() const
{
  return _d->festivalDate;
}

void CityServiceFestival::assignFestival( RomeDivinityType name, int size )
{
  _d->festivalType = size;
  _d->festivalDate = GameDate::current();
  _d->festivalDate.appendMonth( size * 3 );
  _d->divinity = name;
}

CityServiceFestival::CityServiceFestival(PlayerCityPtr city )
: CityService( getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastFestivalDate = DateTime( -350, 0, 0 );
  _d->festivalDate = DateTime( -550, 0, 0 );
}

void CityServiceFestival::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  const DateTime current = GameDate::current();
  if( _d->festivalDate.year() == current.year()
      && _d->festivalDate.month() == current.month() )
  {
    _d->lastFestivalDate = GameDate::current();
    _d->festivalDate = DateTime( -550, 1, 1 );
    DivinePantheon::doFestival( _d->divinity, _d->festivalType );

    const char* titles[3] = { "##small_festival##", "##middle_festival##", "##great_festival##" };
    const char* text[3] = { "##small_fest_description##", "##middle_fest_description##", "##big_fest_description##" };
    int id = math::clamp( _d->festivalType, 0, 3 );
    events::GameEventPtr e = events::ShowFeastWindow::create( text[ id ], titles[ id ],
                                                              _d->city->getPlayer()->getName() );
    e->dispatch();
  }
}

VariantMap CityServiceFestival::save() const
{
  VariantMap ret;
  ret[ "lastDate" ] = _d->lastFestivalDate;
  ret[ "nextDate" ] = _d->festivalDate;
  ret[ "divinity" ] = (int)_d->divinity;
  ret[ "festival" ] = _d->festivalType;

  return ret;
}

void CityServiceFestival::load(VariantMap stream)
{
  _d->lastFestivalDate = stream[ "lastDate" ].toDateTime();
  _d->festivalDate = stream[ "nextDate" ].toDateTime();
  _d->divinity = (RomeDivinityType)stream[ "divinity" ].toInt();
  _d->festivalType = (int)stream[ "festival" ];
}
