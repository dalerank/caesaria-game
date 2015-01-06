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

#include "sentiment.hpp"
#include "game/gamedate.hpp"
#include "helper.hpp"
#include "objects/house.hpp"
#include "core/variant_map.hpp"

using namespace constants;

namespace city
{

struct BuffInfo
{
int value;
int finishValue;
bool relative;
DateTime finishDate;

BuffInfo()
{
  value = 0;
  finishValue = 0;
  relative = false;
}

VariantMap save()
{
  VariantMap ret;
  VARIANT_SAVE_ANY( ret, value )
  VARIANT_SAVE_ANY( ret, relative )
  VARIANT_SAVE_ANY( ret, finishDate )
  VARIANT_SAVE_ANY( ret, finishValue )

  return ret;
}

void load( const VariantMap& stream )
{
  VARIANT_LOAD_ANY( value, stream )
  VARIANT_LOAD_ANY( relative, stream )
  VARIANT_LOAD_TIME( finishDate, stream )
  VARIANT_LOAD_ANY( finishValue, stream )
}
};

class Sentiment::Impl
{
public:  
  typedef std::vector<BuffInfo> Buffs;

  int value;
  int finishValue;
  int affect;
  int buffValue;
  Buffs buffs;
};

city::SrvcPtr Sentiment::create( PlayerCityPtr city )
{
  SrvcPtr ret(new Sentiment( city ));
  ret->drop();
  return ret;
}

std::string Sentiment ::defaultName() { return CAESARIA_STR_EXT(Sentiment);}

Sentiment::Sentiment( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{
  _d->value = 50;
  _d->finishValue = 50;
  _d->affect = 0;
  _d->buffValue = 0;
}

void Sentiment::timeStep(const unsigned int time )
{
  if( game::Date::isWeekChanged() )
  {
    _d->value += math::signnum( _d->finishValue - _d->value );
  }

  if( game::Date::isMonthChanged() )
  {
    _d->buffValue = 0;
    DateTime current = game::Date::current();
    for( Impl::Buffs::iterator it=_d->buffs.begin(); it != _d->buffs.end(); )
    {
      BuffInfo& buff = *it;
      if( buff.finishDate > current )
      {
        it = _d->buffs.erase( it );
      }
      else
      {
        if( buff.relative ) { buff.finishValue += buff.value; }
        else { buff.finishValue = buff.value; }

        _d->buffValue += buff.value;
        ++it;
      }
    }

    HouseList houses;
    houses << _city()->overlays();

    unsigned int houseNumber = 0;
    _d->finishValue = 0;
    foreach( it, houses )
    {
      HousePtr h = *it;
      h->setState( House::happinessBuff, _d->buffValue );

      if( h->habitants().count() > 0 )
      {
        _d->finishValue += h->state( House::happiness );
        houseNumber++;
      }
    }

    if( houseNumber > 0 )
      _d->finishValue /= houseNumber;
    else
      _d->finishValue = 50;
  }
}

int Sentiment::value() const { return _d->value + _d->affect; }
int Sentiment::buff() const { return _d->buffValue; }

std::string Sentiment::reason() const
{
  std::string ret = "##unknown_sentiment_reason##";
  int v = value();
  if( v > 95 ) { ret = "##sentiment_people_idolize_you##"; }
  else if( v > 90 ) { ret = "##sentiment_people_love_you##"; }
  else if( v > 80 ) { ret = "##sentiment_people_extr_pleased_you##"; }
  else if( v > 70 ) { ret = "##sentiment_people_verypleased_you##"; }
  else if( v > 60 ) { ret = "##sentiment_people_pleased_you##"; }
  else if( v > 40 ) { ret = "##sentiment_people_indiffirent_you##"; }
  else if( v > 30 ) { ret = "##sentiment_people_annoyed_you##"; }
  else if( v > 22 ) { ret = "##sentiment_people_upset_you##"; }
  else if( v > 15 ) { ret = "##sentiment_people_veryupset_you##"; }
  else if( v > 10 ) { ret = "##sentiment_people_angry_you##"; }
  else if( v > 5 ) { ret = "##sentiment_people_veryangry_you##"; }
  else if( v > 0 ) { ret = "##city_loathed_you##"; }

  return ret;
}

VariantMap Sentiment::save() const
{
  VariantMap ret = Srvc::save();
  VARIANT_SAVE_ANY_D( ret, _d, value );
  VARIANT_SAVE_ANY_D( ret, _d, finishValue );
  VARIANT_SAVE_ANY_D( ret, _d, affect );

  VariantList vlBuffs;
  foreach( it, _d->buffs )
  {
    vlBuffs.push_back( (*it).save() );
  }

  ret[ "buffs" ] = vlBuffs;
  return ret;
}

void Sentiment::load(const VariantMap& stream)
{
  Srvc::load( stream );
  VARIANT_LOAD_ANY_D( _d, value, stream );
  VARIANT_LOAD_ANY_D( _d, finishValue, stream );
  VARIANT_LOAD_ANY_D( _d, affect, stream );

  VariantList vlBuffs = stream.get( "buffs" ).toList();
  foreach( it, vlBuffs )
  {
    BuffInfo buff;
    buff.load( (*it).toMap() );
  }
}

void Sentiment::addBuff(int value, bool relative, int month2finish)
{
  BuffInfo buff;
  buff.value = value;
  buff.relative = relative;
  buff.finishDate = game::Date::current();
  buff.finishDate.appendMonth( month2finish );

  _d->buffs.push_back( buff );
}

}//end namespace city
