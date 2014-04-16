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

#include "cityservice_sentiment.hpp"
#include "city.hpp"
#include "game/gamedate.hpp"

namespace city
{

class Sentiment::Impl
{
public:
  int value;
};

city::SrvcPtr Sentiment::create(PlayerCityPtr city )
{
  Sentiment* ret = new Sentiment( city );
  return city::SrvcPtr( ret );
}

std::string Sentiment ::getDefaultName() { return "sentiment";}

Sentiment::Sentiment(PlayerCityPtr city )
  : city::Srvc( *city.object(), getDefaultName() ), _d( new Impl )
{
  _d->value = 50;
}

void Sentiment::update( const unsigned int time )
{
  if( !GameDate::isWeekChanged() )
    return;


}

void Sentiment::append(int value)
{
  _d->value = math::clamp( _d->value + value, 0, 100 );
}

VariantMap Sentiment::save() const
{
  VariantMap ret = Srvc::save();
  ret[ "value" ] = _d->value;
  return ret;
}

void Sentiment::load(VariantMap stream)
{
  Srvc::load( stream );
  _d->value = stream.get( "value" );
}



}//end namespace city
