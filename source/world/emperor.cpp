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

#include "emperor.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include <map>

namespace world
{

struct Relation
{
  int value;
  DateTime lastGiftDate;
  int lastGiftValue;

  Relation() : value( 0 ), lastGiftDate( DateTime( -351, 1, 1 ) ), lastGiftValue( 0 )
  {

  }

  VariantMap save() const
  {
    VariantMap ret;
    ret[ "value" ] = value;
    ret[ "lastGiftDate" ] = lastGiftDate;
    ret[ "lastGiftValue" ] = lastGiftValue;

    return ret;
  }

  void load( const VariantMap& stream )
  {
    value = stream.get( "value" );
    lastGiftDate = stream.get( "lastGiftDate" ).toDateTime();
    lastGiftValue = stream.get( "lastGiftValue" );
  }
};

class Emperor::Impl
{
public:
  typedef std::map< std::string, Relation > Relations;
  Relations relations;
};

Emperor::Emperor() : __INIT_IMPL(Emperor)
{

}

Emperor::~Emperor(){}

int Emperor::relation(const std::string& cityname)
{
  __D_IMPL(d,Emperor)
  Impl::Relations::iterator i = d->relations.find( cityname );
  return ( i == d->relations.end() ? 0 : i->second.value );
}

void Emperor::updateRelation(const std::string& cityname, int value)
{
  __D_IMPL(d,Emperor)
  int current = d->relations[ cityname ].value;
  d->relations[ cityname ].value = math::clamp<int>( current + value, 0, 100 );
}

void Emperor::sendGift(const std::string& cityname, int money)
{
  Relation relation;
  Impl::Relations::iterator it = _dfunc()->relations.find( cityname );
  if( it != _dfunc()->relations.end() )
  {
    relation = it->second;
  }

  int monthFromLastGift = math::clamp<int>( relation.lastGiftDate.getMonthToDate( GameDate::current() ), 0, (int)DateTime::monthInYear );
  const int maxFavourUpdate = 5;

  float timeKoeff = monthFromLastGift / (float)DateTime::monthInYear;
  float priceKoeff = math::max<float>( money - relation.lastGiftValue, 0.f ) / money;
  int favourUpdate = maxFavourUpdate * timeKoeff * priceKoeff;

  updateRelation( cityname, favourUpdate );
}

VariantMap Emperor::save() const
{
  VariantMap ret;

  Impl::Relations r = _dfunc()->relations;
  VariantMap vm_relations;
  foreach( it, r )
  {
    vm_relations[ it->first ] = it->second.save();
  }

  ret[ "relations" ] = vm_relations;
  return ret;
}

void Emperor::load(const VariantMap& stream)
{
  VariantMap vm_relations = stream.get( "relations" ).toMap();

  Impl::Relations& relations = _dfunc()->relations;
  foreach( it, vm_relations )
  {
    Relation r;
    r.load( it->second.toMap() );
    relations[ it->first ] = r;
  }
}

}
