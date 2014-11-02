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
#include "city/funds.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "romechastenerarmy.hpp"
#include "city.hpp"
#include "empire.hpp"
#include <map>

namespace world
{

namespace {
static const unsigned int legionSoldiersCount = 16;
}

struct Relation
{
  int value;
  unsigned int soldiersSent;
  unsigned int lastSoldiersSent;
  DateTime lastGiftDate;
  DateTime lastTaxDate;
  int lastGiftValue;
  bool debtMessageSent;

  Relation()
    : value( 0 ),
      soldiersSent( 0 ), lastSoldiersSent( 0 ),
      lastGiftDate( DateTime( -351, 1, 1 ) ), lastGiftValue( 0 ),
      debtMessageSent(false)
  {

  }

  void removeSoldier()
  {
    if( soldiersSent > 0)
      --soldiersSent;
  }

  VariantMap save() const
  {
    VariantMap ret;
    VARIANT_SAVE_ANY(ret, lastGiftDate )
    VARIANT_SAVE_ANY(ret, lastTaxDate )
    VARIANT_SAVE_ANY(ret, value)
    VARIANT_SAVE_ANY(ret, lastGiftValue )
    VARIANT_SAVE_ANY(ret, soldiersSent )
    VARIANT_SAVE_ANY(ret, lastSoldiersSent)
    VARIANT_SAVE_ANY(ret, debtMessageSent )

    return ret;
  }

  void load( const VariantMap& stream )
  {
    VARIANT_LOAD_TIME(lastGiftDate, stream )
    VARIANT_LOAD_TIME(lastTaxDate, stream )
    VARIANT_LOAD_ANY(value, stream)
    VARIANT_LOAD_ANY(lastGiftValue, stream);
    VARIANT_LOAD_ANY(lastSoldiersSent, stream);
    VARIANT_LOAD_ANY(soldiersSent, stream)
    VARIANT_LOAD_ANY(debtMessageSent, stream)
  }
};

class Emperor::Impl
{
public:
  typedef std::map< std::string, Relation > Relations;
  Relations relations;
  Empire* empire;
  std::string name;
};

Emperor::Emperor() : __INIT_IMPL(Emperor)
{
  __D_IMPL(d,Emperor)
  d->name = "Emperor";
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

void Emperor::sendGift(const std::string& cityname, unsigned int money)
{
  Relation relation;
  Impl::Relations::iterator it = _dfunc()->relations.find( cityname );
  if( it != _dfunc()->relations.end() )
  {
    relation = it->second;
  }

  int monthFromLastGift = math::clamp<int>( relation.lastGiftDate.monthsTo( GameDate::current() ),
                                            0, (int)DateTime::monthsInYear );
  const int maxFavourUpdate = 5;

  float timeKoeff = monthFromLastGift / (float)DateTime::monthsInYear;
  int affectMoney = relation.lastGiftValue / ( monthFromLastGift + 1 );
  float moneyKoeff = math::max<float>( money - affectMoney, 0.f ) / money;
  int favourUpdate = maxFavourUpdate * timeKoeff * moneyKoeff;

  updateRelation( cityname, favourUpdate );
}

DateTime Emperor::lastGiftDate(const std::string &cityname)
{
  Relation relation;
  Impl::Relations::iterator it = _dfunc()->relations.find( cityname );
  if( it != _dfunc()->relations.end() )
  {
    relation = it->second;
    return DateTime( -350, 1, 1 );
  }

  return relation.lastGiftDate;
}

void Emperor::timeStep(unsigned int time)
{
  if( GameDate::isYearChanged() )
  {
    __D_IMPL(d,Emperor)

    foreach( it, d->relations )
    {
      Relation& ref = it->second;
      ref.value -= 2;
      int monthWithoutTax = ref.lastTaxDate.monthsTo( GameDate::current() );
      if( monthWithoutTax > 12 )
      {
        int decrease = math::clamp( 3 + monthWithoutTax / DateTime::monthsInYear * 2, 0, 8 );
        ref.value -= decrease;
      }

      CityPtr cityp = d->empire->findCity( it->first );
      float salaryKoeff = EmpireHelper::governorSalaryKoeff( cityp );
      if( salaryKoeff > 1.f ) { ref.value -= (int)salaryKoeff * salaryKoeff; }
      else if( salaryKoeff < 1.f ) { ref.value += 1; }

      int brokenEmpireTax = cityp->funds().getIssueValue( city::Funds::overdueEmpireTax, city::Funds::lastYear );
      if( brokenEmpireTax > 0 )
      {
        ref.value -= 1;

        brokenEmpireTax = cityp->funds().getIssueValue( city::Funds::overdueEmpireTax, city::Funds::twoYearAgo );
        if( brokenEmpireTax > 0 )
          ref.value -= 2;
      }
    }
  }

  if( GameDate::isMonthChanged() )
  {
    __D_IMPL(d,Emperor)

    CityList troubleCities;
    foreach( it, d->relations )
    {
      CityPtr city = d->empire->findCity( it->first );
      Relation& relation = d->relations[ it->first ];

      if( ( city->funds().treasury() < -5500 || relation.value < 20 )
          && relation.soldiersSent == 0 )
      {
        if( city->funds().treasury() < -5500 && !relation.debtMessageSent )
        {
          relation.debtMessageSent = true;
          events::GameEventPtr e = events::ShowInfobox::create( "##emperor_wrath_by_debt_title##",
                                                                "##emperor_wrath_by_debt_text##",
                                                                true );
          e->dispatch();
        }

        if( city.isValid() )
        {
          troubleCities << city;
        }
      }

    }

    foreach( it, troubleCities )
    {
      Relation& relation = d->relations[ (*it)->name() ];
      relation.soldiersSent = relation.lastSoldiersSent * 2;

      unsigned int sldrNumber = std::max( legionSoldiersCount, relation.soldiersSent );
      relation.soldiersSent = sldrNumber;
      relation.lastSoldiersSent = sldrNumber;

      RomeChastenerArmyPtr army = RomeChastenerArmy::create( d->empire );
      army->setSoldiersNumber( sldrNumber );
      army->attack( ptr_cast<Object>( *it ) );
    }
  }
}

void Emperor::soldierDie(const std::string& cityname)
{
  __D_IMPL(d,Emperor)
  d->relations[ cityname ].removeSoldier();
}

std::string Emperor::name() const { return _dfunc()->name; }

void Emperor::cityTax(const std::string &cityname, unsigned int money)
{
  __D_IMPL(d,Emperor)
  d->relations[ cityname ].lastTaxDate = GameDate::current();
}

void Emperor::resetRelations(const StringArray& cities)
{
  __D_IMPL(d,Emperor)
  CityList empCities;
  if( !cities.empty() )
  {
    if( cities.front() == "all" )
    {
      empCities = d->empire->cities();
    }
    else
    {
      foreach( it, cities )
      {
        CityPtr pCity = d->empire->findCity( *it );
        if( pCity.isValid() )
          empCities << pCity;
      }
    }

    foreach( it, empCities )
    {
      Relation& r = d->relations[ (*it)->name() ];

      r.value = 50;
      r.lastGiftValue = 0;
      r.lastTaxDate = GameDate::current();
      r.lastGiftDate = GameDate::current();
    }
  }

}

VariantMap Emperor::save() const
{
  __D_IMPL_CONST(d,Emperor)
  VariantMap ret;

  Impl::Relations r = d->relations;
  VariantMap vm_relations;
  foreach( it, r )
  {
    vm_relations[ it->first ] = it->second.save();
  }

  ret[ "relations" ] = vm_relations;
  VARIANT_SAVE_STR_D( ret, d, name )
  return ret;
}

void Emperor::load(const VariantMap& stream)
{
  __D_IMPL(d,Emperor)
  VariantMap vm_relations = stream.get( "relations" ).toMap();

  Impl::Relations& relations = d->relations;
  foreach( it, vm_relations )
  {
    Relation r;
    r.load( it->second.toMap() );
    relations[ it->first ] = r;
  }

  VARIANT_LOAD_STR_D( d, name, stream )
}

void Emperor::init(Empire &empire) { _dfunc()->empire = &empire; }

}
