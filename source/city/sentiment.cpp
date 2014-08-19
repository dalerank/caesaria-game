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

using namespace constants;

namespace city
{

class Sentiment::Impl
{
public:
  int value;
  int finishValue;
  int affect;
};

city::SrvcPtr Sentiment::create(PlayerCityPtr city )
{
  Sentiment* ret = new Sentiment( city );
  return city::SrvcPtr( ret );
}

std::string Sentiment ::defaultName() { return CAESARIA_STR_EXT(Sentiment);}

Sentiment::Sentiment(PlayerCityPtr city )
  : city::Srvc( *city.object(), defaultName() ), _d( new Impl )
{
  _d->value = 50;
  _d->finishValue = 50;
  _d->affect = 0;
}

void Sentiment::update( const unsigned int time )
{
  if( GameDate::isWeekChanged() )
  {
    _d->value += math::signnum( _d->finishValue - _d->value );
  }

  if( GameDate::isMonthChanged() )
  {
    Helper helper( &_city );
    HouseList houses = helper.find<House>( building::house );

    unsigned int houseNumber = 0;
    _d->finishValue = 0;
    foreach( it, houses )
    {
      HousePtr h = *it;

      if( h->habitants().count() > 0 )
      {
        _d->finishValue += h->state( House::happiness );
        houseNumber++;
      }
    }

    if( houseNumber > 0 )
      _d->finishValue /= houseNumber;
  }
}

int Sentiment::value() const { return _d->value + _d->affect; }

void Sentiment::append(int value)
{
  _d->finishValue = math::clamp( _d->value + value, 0, 100 );
}

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
  else if( v > 7 ) { ret = "##sentiment_people_angry_you##"; }
  else if( v >=0 ) { ret = "##sentiment_people_veryangry_you##"; }

  return ret;
}

VariantMap Sentiment::save() const
{
  VariantMap ret = Srvc::save();
  VARIANT_SAVE_ANY_D( ret, _d, value );
  VARIANT_SAVE_ANY_D( ret, _d, finishValue );
  VARIANT_SAVE_ANY_D( ret, _d, affect );
  return ret;
}

void Sentiment::load(const VariantMap& stream)
{
  Srvc::load( stream );
  VARIANT_LOAD_ANY_D( _d, value, stream );
  VARIANT_LOAD_ANY_D( _d, finishValue, stream );
  VARIANT_LOAD_ANY_D( _d, affect, stream );
}

}//end namespace city
