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
#include "config.hpp"
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
static const int minimumFabvour4wrathGenerate = 20;
static const int maxWrathPointValue = 20;
}

class Emperor::Impl
{
public:
  Relations relations;
  Empire* empire;
  std::string name;

  void resolveTroubleCities( const CityList& cities );
};

Emperor::Emperor() : __INIT_IMPL(Emperor)
{
  __D_IMPL(d,Emperor)
  d->name = "Emperor";
}

Emperor::~Emperor(){}

int Emperor::relation(const std::string& cityname) const
{
  Relations::const_iterator i = _dfunc()->relations.find( cityname );
  return ( i == _dfunc()->relations.end() ? 0 : i->second.value() );
}

void Emperor::updateRelation(const std::string& cityname, int value)
{
  Relation& relation = _dfunc()->relations[ cityname ];
  relation.change( value );
}

void Emperor::sendGift(const Gift& gift)
{
  Relation& relation = _dfunc()->relations[ gift.sender() ];

  relation.update( gift );
}

DateTime Emperor::lastGiftDate(const std::string& cityname) const
{
  Relations::const_iterator it = _dfunc()->relations.find( cityname );
  if( it == _dfunc()->relations.end() )
    return DateTime( -350, 1, 1 );
  else
    return it->second.lastGift.date();
}

void Emperor::timeStep(unsigned int time)
{
  if( game::Date::isYearChanged() )
  {
    __D_IMPL(d,Emperor)

    CityList empireCities = d->empire->cities();
    foreach( it, empireCities )
    {
      CityPtr cityp = *it;

      if( !cityp->isAvailable() )
        continue;

      Relation& relation = d->relations[ cityp->name() ];

      relation.soldiersSent = 0;     //clear chasteners count
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

        brokenEmpireTax = cityp->treasury().getIssueValue( econ::Issue::overdueEmpireTax, econ::Treasury::twoYearAgo );
        if( brokenEmpireTax > 0 )
          relation.change( -brokenMoreTaxPenalty );
      }
    }
  }

  if( game::Date::isMonthChanged() )
  {
    __D_IMPL(d,Emperor)

    CityList troubleCities;
    foreach( it, d->relations )
    {
      CityPtr city = d->empire->findCity( it->first );
      Relation& relation = d->relations[ it->first ];

      if( city.isValid() )
      {
        if( relation.value() < minimumFabvour4wrathGenerate  )
        {
          relation.wrathPoint += math::clamp( maxWrathPointValue - relation.value(), 0, maxWrathPointValue );
          if( relation.soldiersSent == 0 )
            troubleCities << city;
        }
        else
        {
          relation.wrathPoint = 0;
        }
      }
    }

    d->resolveTroubleCities( troubleCities );
  }
}

void Emperor::Impl::resolveTroubleCities( const CityList& cities )
{
  foreach( it, cities )
  {
    Relation& relation = relations[ (*it)->name() ];
    float rule2destroy = utils::eventProbability( math::clamp( relation.wrathPoint / 100.f, 0.f, 1.f ),
                                                  math::clamp<int>( relation.tryCount, 0, DateTime::monthsInYear ),
                                                  DateTime::monthsInYear );

    relation.tryCount++;

    if( rule2destroy < 0.9 )
      continue;

    relation.wrathPoint = 0;
    relation.tryCount = 0;
    relation.soldiersSent = relation.lastSoldiersSent * 2;

    unsigned int sldrNumber = std::max( legionSoldiersCount, relation.soldiersSent );

    RomeChastenerArmyPtr army = RomeChastenerArmy::create( empire );
    army->setCheckFavor( true );
    army->setBase( empire->rome() );
    army->setSoldiersNumber( sldrNumber );
    army->attack( ptr_cast<Object>( *it ) );

    if( !army->isDeleted() )
    {
      relation.lastSoldiersSent = sldrNumber;
    }
    else
    {
      relation.chastenerFailed++;
    }
  }
}

void Emperor::remSoldiers(const std::string& cityname, int value)
{
  __D_IMPL(d,Emperor)
  for( int i=0; i < value; i++ )
  {
    d->relations[ cityname ].removeSoldier();
  }
}

void Emperor::addSoldiers(const std::string& name, int value)
{
  __D_IMPL(d,Emperor)
  Relation& relation = d->relations[ name ];
  relation.soldiersSent += value;
}

std::string Emperor::name() const { return _dfunc()->name; }
void Emperor::setName(const std::string& name){ _dfunc()->name = name; }

void Emperor::cityTax(const std::string &cityname, unsigned int money)
{
  __D_IMPL(d,Emperor)
  d->relations[ cityname ].lastTaxDate = game::Date::current();
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
        empCities.addIfValid( d->empire->findCity( *it ) );
    }

    foreach( it, empCities )
    {
      Relation& r = d->relations[ (*it)->name() ];
      r.reset();
    }
  }

}

void Emperor::checkCities()
{
  __D_IMPL(d,Emperor)
  CityList empireCities = d->empire->cities();
  foreach( it, empireCities )
  {
    if( !(*it)->isAvailable() )
      continue;

    if( d->relations.count( (*it)->name() ) == 0 )
    {
      Relation& relation = d->relations[ (*it)->name() ];
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
  __D_IMPL(d,Emperor)
  d->empire = &empire;
}

struct EmperorInfo
{
  std::string name;
  DateTime beginReign;
  VariantMap options;

  void load( const VariantMap& stream )
  {
    beginReign = stream.get( "date" ).toDateTime();
    name = stream.get( "name" ).toString();
    options = stream;
  }
};

class EmperorLine::Impl
{
public:
  typedef std::map< DateTime, EmperorInfo> ChangeInfo;

  ChangeInfo changes;
};

EmperorLine& EmperorLine::instance()
{
  static EmperorLine inst;
  return inst;
}

std::string EmperorLine::getEmperor(DateTime time)
{
  foreach( it, _d->changes )
  {
    if( it->first >= time )
      return it->second.name;
  }

  return "";
}

VariantMap EmperorLine::getInfo(std::string name) const
{
  foreach( it, _d->changes )
  {
    if( name == it->second.name )
      return it->second.options;
  }

  return VariantMap();
}

void EmperorLine::load(vfs::Path filename)
{
  _d->changes.clear();

  VariantMap opts = config::load( filename );
  foreach( it, opts )
  {
    EmperorInfo info;
    info.load( it->second.toMap() );

    _d->changes[ info.beginReign ] = info;
  }
}

EmperorLine::EmperorLine() : _d( new Impl )
{

}

}
