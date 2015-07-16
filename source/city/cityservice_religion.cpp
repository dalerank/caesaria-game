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

#include "cityservice_religion.hpp"
#include "objects/construction.hpp"
#include "city/statistic.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "game/gamedate.hpp"
#include "objects/religion.hpp"
#include "religion/pantheon.hpp"
#include "core/foreach.hpp"
#include "core/variant_map.hpp"
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "core/logger.hpp"
#include "core/safetycast.hpp"
#include "events/showinfobox.hpp"
#include "cityservice_factory.hpp"
#include "city/states.hpp"

using namespace religion;
using namespace events;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Religion,religion)

struct CoverageInfo
{
  int smallTempleNum;
  int bigTempleNum;
  int parishionerNumber;

  CoverageInfo() : smallTempleNum( 0 ), bigTempleNum( 0 ), parishionerNumber( 0 ) {}
};

class TemplesCoverity : public std::map< std::string, CoverageInfo >
{
public:
  void update( TemplePtr temple )
  {
    if( temple->divinity().isValid() )
    {
      CoverageInfo& info = (*this)[ temple->divinity()->internalName() ];

      if( temple.is<BigTemple>() ) { info.bigTempleNum++; }
      else { info.smallTempleNum++; }

      info.parishionerNumber += temple->currentVisitors();
    }
  }

  void clear( const DivinityList& divns )
  {
    std::map< std::string, CoverageInfo >::clear();
    for( auto divinity : divns )
    {
      CoverageInfo& cvInfo = (*this)[ divinity->internalName() ];
      cvInfo.smallTempleNum = 0;
    }
  }

  void setOraclesParishioner( int parishioners )
  {
    for( auto item : *this )
      item.second.parishionerNumber += parishioners;
  }
};

class Religion::Impl
{
public:
  TemplesCoverity templesCoverity;
  DateTime lastMessageDate;
  StringArray reasons;

  void updateRelation(PlayerCityPtr city, DivinityPtr divn );
};

SrvcPtr Religion::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Religion( city ) );
  ret->drop();

  return ret;
}

std::string Religion::defaultName() { return CAESARIA_STR_EXT(Religion); }

Religion::Religion( PlayerCityPtr city )
  : Srvc( city, Religion::defaultName() ), _d( new Impl )
{
}

void Religion::timeStep( const unsigned int time )
{  
  if( game::Date::isWeekChanged() )
  {   
    _d->reasons.clear();

    if( _city()->getOption( PlayerCity::godEnabled ) == 0 )
      return;

    Logger::warning( "Religion: start update relations" );
    DivinityList divinities = rome::Pantheon::instance().all();

    //clear temples info
    _d->templesCoverity.clear( divinities );

    //update temples info
    TempleList temples = statistic::getObjects<Temple>( _city(), object::group::religion );
    foreach( it, temples )
      _d->templesCoverity.update( *it );

    TempleOracleList oracles = _city()->statistic().objects.find<TempleOracle>( object::oracle );

    //add parishioners to all divinities by oracles
    int oraclesParishionerNumber = 0;
    for( auto oracle : oracles )
      oraclesParishionerNumber += oracle->currentVisitors();

    _d->templesCoverity.setOraclesParishioner( oraclesParishionerNumber );

    for( auto divn :  divinities )
      divn->setEffectPoint( 0 );

    std::map< int, StringArray > templesByGod;

    for( auto coverity : _d->templesCoverity )
    {
      const CoverageInfo& info = coverity.second;
      int maxTemples = info.bigTempleNum + info.smallTempleNum;
      templesByGod[ maxTemples ].push_back( coverity.first );
    }

    if( !templesByGod.empty() )
    {
      const StringArray& awardedGods = templesByGod.rbegin()->second;
      //if we have award god with most temples number
      if( awardedGods.size() == 1 )
      {
        DivinityPtr god = rome::Pantheon::get( awardedGods.front() );
        if( god.isValid() )
        {
          god->setEffectPoint( award::admiredGod );
          _d->reasons << utils::format( 0xff, "##%s_god_admired##", god->internalName().c_str() );
        }
      }

      if( templesByGod.size() > 1 )
      {
        const StringArray& unhappyGods = templesByGod.begin()->second;
        //if we have penalty god with less temples number, then set him -25 points
        if( unhappyGods.size() == 1 )
        {
          DivinityPtr god = rome::Pantheon::get( unhappyGods.front() );
          if( god.isValid() )
          {
            god->setEffectPoint( -penalty::brokenGod );
            _d->reasons << utils::format( 0xff, "##%s_god_broken##", god->internalName().c_str() );
          }
        }
      }
    }

    for( auto divn : divinities )
      _d->updateRelation( _city(), divn );
  }

  if( game::Date::isMonthChanged() )
  {
    if( _city()->getOption( PlayerCity::godEnabled ) == 0 )
      return;

    int goddesRandom = math::random( 20 );
    //only for trird, seven, ace event
    if( !(goddesRandom == 3 || goddesRandom == 7 || goddesRandom == 11) )
      return;

    DivinityList divinities = rome::Pantheon::instance().all();

    //check gods wrath and mood
    std::map< int, DivinityList > godsWrath;
    std::map< int, DivinityList > godsUnhappy;
    for( auto god : divinities )
    {
      if( god->wrathPoints() > 0 )
      {
        godsWrath[ god->wrathPoints() ].push_back( god );
        _d->reasons << utils::format( 0xff, "##%s_god_wrath##", god->internalName().c_str() );
      }      
      else if( god->relation() < relation::minimum4wrath )
      {
        godsUnhappy[ god->relation() ].push_back( god );
        _d->reasons << utils::format( 0xff, "##%s_god_unhappy##", god->internalName().c_str() );
      }
    }       

    //find wrath god
    DivinityList someGods = divinities;
    if( !godsWrath.empty() ) { someGods = godsWrath.rbegin()->second; }
    else if( !godsUnhappy.empty() ) { someGods = godsUnhappy.rbegin()->second; }

    DivinityPtr randomGod = someGods.random();

    if( randomGod.isValid() )
    {
      randomGod->checkAction( _city() );
    }
  }
}

VariantMap Religion::save() const
{
  VariantMap ret = Srvc::save();

  VARIANT_SAVE_ANY_D( ret, _d, lastMessageDate)

  return ret;
}

void Religion::load(const VariantMap& stream)
{
  Srvc::load( stream );

  VARIANT_LOAD_TIME_D( _d, lastMessageDate, stream )
}

std::string Religion::reason() const
{
  if( _city()->getOption( PlayerCity::godEnabled ) == 0 )
    return "##no_gods_in_your_city##";

  if( _d->reasons.empty() )
    return "##no_gods_info_for_city##";

  return _d->reasons.random();
}

void Religion::Impl::updateRelation( PlayerCityPtr city, DivinityPtr divn )
{
  if( !divn.isValid() )
  {
    Logger::warning( "!!! WARNING: Cant update relation for null god" );
    return;
  }

  CoverageInfo& myTemples = templesCoverity[ divn->internalName() ];
  unsigned int faithValue = 0;
  if( city->states().population > 0 )
  {
    faithValue = math::clamp<unsigned int>( math::percentage( myTemples.parishionerNumber, city->states().population ), 0u, 100u );
  }

  Logger::warning( "Religion: set faith income for %s is %d [r=%f]", divn->name().c_str(), faithValue, divn->relation() );
  divn->updateRelation( faithValue, city );

  bool unhappy = divn->relation() < relation::negative;
  bool maySendNotification = lastMessageDate.monthsTo( game::Date::current() ) > DateTime::monthsInYear/2;

  if( unhappy && maySendNotification )
  {
    lastMessageDate = game::Date::current();
    std::string text = divn->relation() < relation::wrathfull ? "##gods_wrathful_text##" : "##gods_unhappy_text##";
    std::string title = divn->relation() < relation::wrathfull ? "##gods_wrathful_title##" : "##gods_unhappy_title##";

    GameEventPtr e = ShowInfobox::create( _(title), _(text), ShowInfobox::send2scribe );
    e->dispatch();
  }
}

}//end namespace city
