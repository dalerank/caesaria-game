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
#include "religion/pantheon.hpp"
#include "events/showfeastwindow.hpp"

using namespace religion;

namespace city
{

class Festival::Impl
{
public:
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

std::string Festival::getDefaultName() {  return "festival";}
DateTime Festival::getLastFestivalDate() const{  return _d->lastFestivalDate;}
DateTime Festival::getNextFestivalDate() const{  return _d->festivalDate; }

void Festival::assignFestival( RomeDivinityType name, int size )
{
  _d->festivalType = size;
  _d->festivalDate = GameDate::current();
  _d->festivalDate.appendMonth( size * 3 );
  _d->divinity = name;
}

Festival::Festival(PlayerCityPtr city )
: Srvc( *city.object(), getDefaultName() ), _d( new Impl )
{
  _d->lastFestivalDate = DateTime( -350, 0, 0 );
  _d->festivalDate = DateTime( -550, 0, 0 );
}

void Festival::update( const unsigned int time )
{
  if( time % (GameDate::ticksInMonth() / 2) != 1 )
    return;

  const DateTime current = GameDate::current();
  if( _d->festivalDate.year() == current.year()
      && _d->festivalDate.month() == current.month() )
  {
    _d->lastFestivalDate = GameDate::current();
    _d->festivalDate = DateTime( -550, 1, 1 );

    rome::Pantheon::doFestival( _d->divinity, _d->festivalType );

    const char* titles[3] = { "##small_festival##", "##middle_festival##", "##great_festival##" };
    const char* text[3] = { "##small_fest_description##", "##middle_fest_description##", "##big_fest_description##" };
    int id = math::clamp<int>( _d->festivalType, 0, 3 );
    events::GameEventPtr e = events::ShowFeastWindow::create( text[ id ], titles[ id ],
                                                              _city.player()->getName() );
    e->dispatch();
  }
}

VariantMap Festival::save() const
{
  VariantMap ret;
  ret[ "lastDate" ] = _d->lastFestivalDate;
  ret[ "nextDate" ] = _d->festivalDate;
  ret[ "divinity" ] = (int)_d->divinity;
  ret[ "festival" ] = _d->festivalType;

  return ret;
}

void Festival::load(VariantMap stream)
{
  _d->lastFestivalDate = stream[ "lastDate" ].toDateTime();
  _d->festivalDate = stream[ "nextDate" ].toDateTime();
  _d->divinity = (RomeDivinityType)stream[ "divinity" ].toInt();
  _d->festivalType = (int)stream[ "festival" ];
}

}//end namespace city
