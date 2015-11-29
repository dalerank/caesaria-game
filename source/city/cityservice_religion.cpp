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

namespace
{
static SimpleLogger LOG( "Religion" );
}

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Religion,religion)

struct CoverageInfo
{
  struct {
    int small_n;
    int big_n;
  } temples;

  int parishionerNumber;

  CoverageInfo() : temples({0,0}), parishionerNumber( 0 ) {}
};

class TemplesCoverity : public std::map< std::string, CoverageInfo >
{
public:
  void update( TemplePtr temple )
  {
    if( temple->divinity().isValid() )
    {
      CoverageInfo& info = (*this)[ temple->divinity()->internalName() ];

      if( temple.is<BigTemple>() ) { info.temples.big_n++; }
      else { info.temples.small_n++; }

      info.parishionerNumber += temple->currentVisitors();
    }
  }

  void clear( const DivinityList& divinities )
  {
    std::map< std::string, CoverageInfo >::clear();
    for( auto divinity : divinities)
    {
      CoverageInfo& cvInfo = (*this)[divinity->internalName()];
      cvInfo.temples.small_n = 0;
      cvInfo.temples.big_n = 0;
    }
  }

  void setOraclesParishioner( int parishioners )
  {
    for( auto& it : *this)
    {
      it.second.parishionerNumber += parishioners;
    }
  }
};

class Religion::Impl
{
public:
  TemplesCoverity templesCoverity;
  DateTime lastMessageDate;
  StringArray reasons;
};

std::string Religion::defaultName() { return CAESARIA_STR_EXT(Religion); }

Religion::Religion( PlayerCityPtr city )
  : Srvc( city, Religion::defaultName() ), _d( new Impl )
{
  LOG.warn( "Initialized" );
}

void Religion::timeStep( const unsigned int time )
{  
  if( game::Date::isWeekChanged() )
  {   
    _d->reasons.clear();

    if( _city()->getOption( PlayerCity::godEnabled ) == 0 )
      return;

    LOG.info( "Update relations started" );
    DivinityList divinities = rome::Pantheon::instance().all();

    //clear temples info
    _d->templesCoverity.clear( divinities );

    //update temples info
    TempleList temples = _city()->statistic().religion.temples();
    for (auto temple : temples)
      _d->templesCoverity.update( temple );

    TempleOracleList oracles = _city()->statistic().religion.oracles();

    //add parishioners to all divinities by oracles
    int oraclesParishionerNumber = 0;
    for (auto oracle : oracles)
    {
      oraclesParishionerNumber += oracle->currentVisitors();
    }

    _d->templesCoverity.setOraclesParishioner( oraclesParishionerNumber );

    for (auto divinity : divinities)
    {
      divinity->setEffectPoint( 0 );
    }

    std::map< int, StringArray > templesByGod;

    for (auto coverity : _d->templesCoverity)
    {
      const CoverageInfo &info = coverity.second;
      int maxTemples = info.temples.big_n + info.temples.small_n;
      templesByGod[maxTemples].push_back( coverity.first );
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
          _d->reasons << fmt::format( "##{0}_god_admired##", god->internalName() );
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
            _d->reasons << fmt::format( "##{0}_god_broken##", god->internalName() );
          }
        }
      }
    }

    for( auto divinity : divinities)
      _updateRelation( divinity );
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
    for (auto god : divinities)
    {
      if( god->wrathPoints() > 0 )
      {
        godsWrath[ god->wrathPoints() ].push_back( god );
        _d->reasons << fmt::format( "##{0}_god_wrath##", god->internalName() );
      }      
      else if( god->relation() < relation::minimum4wrath )
      {
        godsUnhappy[ god->relation() ].push_back( god );
        _d->reasons << fmt::format( "##{0}_god_unhappy##", god->internalName() );
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

Religion::~Religion() {}

void Religion::_updateRelation( DivinityPtr divinity )
{
  if (divinity.isNull())
  {
    LOG.warn( "Can't update relation for null god" );
    return;
  }

  CoverageInfo &myTemples = _d->templesCoverity[ divinity->internalName() ];
  unsigned int faithValue = 0;
  if( _city()->states().population > 0 )
  {
    faithValue = math::clamp<unsigned int>( math::percentage( myTemples.parishionerNumber, _city()->states().population ), 0u, 100u );
  }

  LOG.info( "Faith income for {0} is {1} [r={2:.2f}]", divinity->name(), faithValue, divinity->relation());
  divinity->updateRelation( faithValue, _city() );

  bool unhappy = divinity->relation() < relation::negative;
  bool maySendNotification = _d->lastMessageDate.monthsTo( game::Date::current() ) > DateTime::monthsInYear/2;

  if( unhappy && maySendNotification )
  {
    _d->lastMessageDate = game::Date::current();
    bool wrathfull = divinity->relation() < relation::wrathfull;
    std::string text = wrathfull ? "##gods_wrathful_text##" : "##gods_unhappy_text##";
    std::string title = wrathfull ? "##gods_wrathful_title##" : "##gods_unhappy_title##";

    auto event = ShowInfobox::create( _(title), _(text), ShowInfobox::send2scribe ).as<ShowInfobox>();

    if( !wrathfull )
    {
      bool showWarn = _city()->getOption( PlayerCity::showGodsUnhappyWarn ) > 0;
      event->setDialogVisible( showWarn );
      event->addCallback( "##hide_godunhappy_warn##", makeDelegate( this, &Religion::_hideWarnings ) );
    }

    event->dispatch();
  }
}

void Religion::_hideWarnings() { _city()->setOption( PlayerCity::showGodsUnhappyWarn, 0 ); }

}//end namespace city
