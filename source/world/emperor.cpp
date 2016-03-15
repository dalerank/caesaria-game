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
#include "game/funds.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "romechastenerarmy.hpp"
#include "city.hpp"
#include "core/saveadapter.hpp"
#include "core/utils.hpp"
#include "empire.hpp"
#include "game/gift.hpp"
#include "core/variant_map.hpp"
#include "city/states.hpp"
#include "config.hpp"
#include "core/logger.hpp"
#include "game/gift.hpp"
#include "relations.hpp"

using namespace config;

namespace world
{

namespace {
static const unsigned int legionSoldiersCount = 16;
static const int brokenTaxPenalty = 1;
static const int brokenMoreTaxPenalty = 2;
static const int taxBrokenFavourDecrease = 3;
static const int taxBrokenFavourDecreaseMax = 8;
static const int normalSalaryFavourUpdate = 1;
static const int minimumFavour4wrathGenerate = 20;
static const int maxWrathPointValue = 20;
}

class Emperor::Impl
{
public:
  Relations relations;
  Empire* empire;
  std::string name;

public:
  void resolveTroubleCities( const CityList& cities );
  void updateRelation( CityPtr city );
  CityList findTroubleCities();
};

Emperor::Emperor() : __INIT_IMPL(Emperor)
{
  __D_IMPL(d,Emperor)
  d->name = "Emperor";
}

Emperor::~Emperor(){}

void Emperor::updateRelation(const std::string& cityname, int value)
{
  Relation& relation = _dfunc()->relations[ cityname ];
  relation.change( value );
}

void Emperor::updateRelation(const std::string& cityname, const RelationAbility& ability)
{
  Relation& relation = _dfunc()->relations[ cityname ];
  relation.update( ability );
}

void Emperor::sendGift(const Gift& gift)
{
  Relation& relation = _dfunc()->relations[ gift.sender() ];
  relation.update( gift );
}

void Emperor::sendGift(const std::string & who, const std::string & type, int value)
{
  Gift gift(who, type, value, game::Date::current());
  sendGift(gift);
}

const Gift& Emperor::lastGift(const std::string& cityname) const
{
  auto it = _dfunc()->relations.find( cityname );
  return ( it == _dfunc()->relations.end() )
            ? Gift::invalid
            : it->second.gifts().last();
}

const Relation& Emperor::relation(const std::string& cityname) const
{
  auto it = _dfunc()->relations.find( cityname );
  return ( it == _dfunc()->relations.end() )
            ? Relation::invalid
            : it->second;
}

void Emperor::Impl::updateRelation( CityPtr cityp )
{
  if( !cityp->isAvailable() )
    return;

  Relation& relation = relations[ cityp->name() ];

  relation.soldiers.sent = 0;     //clear chasteners count
  relation.change( -config::emperor::yearlyFavorDecrease );

  int monthWithoutTax = relation.lastTaxDate.monthsTo( game::Date::current() );
  if( monthWithoutTax > DateTime::monthsInYear )
  {
    int decrease = math::clamp( taxBrokenFavourDecrease + monthWithoutTax / DateTime::monthsInYear * 2, 0, taxBrokenFavourDecreaseMax );
    relation.change( -decrease );
  }

  float salaryKoeff = EmpireHelper::governorSalaryKoeff( cityp );
  if( salaryKoeff > 1.f ) { salaryKoeff = -(int)salaryKoeff * salaryKoeff; }
  else if( salaryKoeff < 1.f ) { salaryKoeff = normalSalaryFavourUpdate; }
  else { salaryKoeff = 0; }

  relation.change( salaryKoeff );

  int brokenEmpireTax = cityp->treasury().getIssueValue( econ::Issue::overdueEmpireTax, econ::Treasury::lastYear );
  if( brokenEmpireTax > 0 )
  {
    relation.change( -brokenTaxPenalty );

    brokenEmpireTax = cityp->treasury().getIssueValue( econ::Issue::overdueEmpireTax, econ::Treasury::twoYearsAgo );
    if( brokenEmpireTax > 0 )
      relation.change( -brokenMoreTaxPenalty );
  }
}

CityList Emperor::Impl::findTroubleCities()
{
  CityList ret;
  for( auto& item : relations )
  {
    CityPtr city = empire->findCity( item.first );
    Relation& relation = item.second;

    if( !city.isValid() )
    {
      Logger::warning( "!!! WARNING: city not availaible " + item.first );
      continue;
    }

    bool emperorAngry = relation.value() < minimumFavour4wrathGenerate;
    if( emperorAngry  )
    {
      relation.wrathPoint += math::clamp( maxWrathPointValue - relation.value(), 0, maxWrathPointValue );
      if( relation.soldiers.sent == 0 )
        ret.push_back( city );
    }
    else
    {
      relation.wrathPoint = 0;
    }
  }

  return ret;
}

void Emperor::timeStep(unsigned int time)
{
  __D_REF(d,Emperor)
  if( game::Date::isYearChanged() )
  {
    for( auto cityp : d.empire->cities() )
      d.updateRelation( cityp );
  }

  if( game::Date::isMonthChanged() )
  {
    CityList troubleCities = d.findTroubleCities();
    d.resolveTroubleCities( troubleCities );
  }
}

void Emperor::Impl::resolveTroubleCities( const CityList& cities )
{
  for( auto& city : cities )
  {
    Relation& relation = relations[ city->name() ];
    float rule2destroy = utils::eventProbability( math::clamp( relation.wrathPoint / 100.f, 0.f, 1.f ),
                                                  math::clamp<int>( relation.tryCount, 0, DateTime::monthsInYear ),
                                                  DateTime::monthsInYear );

    relation.tryCount++;
    bool needSendLegion4destroyCity = (rule2destroy > 0.9);

    if( !needSendLegion4destroyCity )
      continue;

    relation.wrathPoint = 0;
    relation.tryCount = 0;
    relation.soldiers.sent = relation.soldiers.last * 2;

    unsigned int sldrNumber = std::max( legionSoldiersCount, relation.soldiers.sent );

    auto army = RomeChastenerArmy::create( empire );
    army->setCheckFavor( true );
    army->setBase( empire->capital() );
    army->setSoldiersNumber( sldrNumber );
    army->attack( ptr_cast<Object>( city ) );

    if( !army->isDeleted() )
    {
      relation.soldiers.last = sldrNumber;
    }
    else
    {
      relation.chastenerFailed++;
    }
  }
}

void Emperor::remSoldiers(const std::string& cityname, int value)
{
  __D_REF(d,Emperor)
  for( int i=0; i < value; i++ )
  {
    d.relations[ cityname ].removeSoldier();
  }
}

void Emperor::addSoldiers(const std::string& name, int value)
{
  __D_REF(d,Emperor)
  Relation& relation = d.relations[ name ];
  relation.soldiers.sent += value;
}

std::string Emperor::name() const { return _dfunc()->name; }
void Emperor::setName(const std::string& name){ _dfunc()->name = name; }

void Emperor::citySentTax(const std::string &cityname, unsigned int money)
{
  __D_REF(d,Emperor)
  d.relations[ cityname ].lastTaxDate = game::Date::current();
}

void Emperor::resetRelations(const StringArray& cities)
{
  __D_REF(d,Emperor)
  CityList empCities;
  if( !cities.empty() )
  {
    if( cities.front() == "all" )
    {
      empCities = d.empire->cities();
    }
    else
    {
      for( auto city : cities )
        empCities.addIfValid( d.empire->findCity( city ) );
    }

    for( auto city : empCities )
    {
      Relation& r = d.relations[ city->name() ];
      r.reset();
    }
  }

}

void Emperor::checkCities()
{
  __D_REF(d,Emperor)
  CityList empireCities = d.empire->cities();
  for( auto city : empireCities )
  {
    if( !city->isAvailable() )
      continue;

    if( d.relations.count( city->name() ) == 0 )
    {
      Relation& relation = d.relations[ city->name() ];
      relation.reset();
    }
  }
}

VariantMap Emperor::save() const
{
  __D_IMPL_CONST(d,Emperor)
  VariantMap ret;
  VARIANT_SAVE_CLASS_D( ret, d, relations )
  VARIANT_SAVE_STR_D  ( ret, d, name )
  return ret;
}

void Emperor::load(const VariantMap& stream)
{
  __D_IMPL(d,Emperor)
  checkCities();

  VARIANT_LOAD_CLASS_D( d, relations, stream )
  VARIANT_LOAD_STR_D( d, name, stream )
}

void Emperor::init(Empire &empire)
{
  _dfunc()->empire = &empire;
}

}//end namespace world
