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

#include "cityservice_religion.hpp"
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "game/gamedate.hpp"
#include "objects/religion.hpp"
#include "religion/pantheon.hpp"
#include "core/foreach.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "core/logger.hpp"
#include "core/safetycast.hpp"
#include "events/showinfobox.hpp"

using namespace constants;
using namespace religion;

namespace city
{

class Religion::Impl
{
public:
  struct CoverageInfo
  {
    int smallTempleNum;
    int bigTempleNum;
    int parishionerNumber;

    CoverageInfo() : smallTempleNum( 0 ), bigTempleNum( 0 ), parishionerNumber( 0 ) {}
  };

  typedef std::map< DivinityPtr, CoverageInfo > TemplesMap;
  TemplesMap templesCoverity;
  int oraclesNumber;
  DateTime lastMessageDate;

  void updateRelation( PlayerCity& city, DivinityPtr divn );
};

SrvcPtr Religion::create(PlayerCityPtr city)
{
  SrvcPtr ret( new Religion( city ) );
  ret->drop();

  return ret;
}

std::string Religion::getDefaultName() { return "religion"; }

Religion::Religion(PlayerCityPtr city )
  : Srvc( *city.object(), Religion::getDefaultName() ), _d( new Impl )
{
}

void Religion::update( const unsigned int time )
{  
  if( GameDate::isWeekChanged() )
  {
    Logger::warning( "Religion: start update relations" );
    DivinityList divinities = rome::Pantheon::instance().all();

    //clear temples info
    _d->templesCoverity.clear();

    //update temples info
    Helper helper( &_city );
    TempleList temples = helper.find<Temple>( building::religionGroup );
    foreach( it, temples)
    {
      if( (*it)->getDivinity().isValid() )
      {
        Impl::CoverageInfo& info = _d->templesCoverity[ (*it)->getDivinity() ];

        if( is_kind_of<BigTemple>( *it ) ) { info.bigTempleNum++; }
        else { info.smallTempleNum++; }

        info.parishionerNumber += (*it)->parishionerNumber();
      }
    }

    TempleOracleList oracles;
    oracles << temples;

    //add parishioners to all divinities by oracles
    foreach( itDivn, divinities )
    {
      Impl::CoverageInfo& info = _d->templesCoverity[ *itDivn ];

      foreach( itOracle, oracles )
      {
        info.parishionerNumber += (*itOracle)->parishionerNumber();
      }
    }

    foreach( it, divinities )
    {
      (*it)->setEffectPoint( 0 );
    }

    std::map< int, DivinityList > templesByGod;

    foreach( it, _d->templesCoverity )
    {
      const Impl::CoverageInfo& info = it->second;
      int maxTemples = info.bigTempleNum + info.smallTempleNum;
      templesByGod[ maxTemples ].push_back( it->first );
    }

    if( !templesByGod.empty() )
    {
      const DivinityList& dl = templesByGod.rbegin()->second;
      //if we have award god with most temples number
      if( dl.size() == 1 )
      {
        dl.front()->setEffectPoint( 50 );
      }

      if( templesByGod.size() > 1 )
      {
        const DivinityList& ml = templesByGod.begin()->second;
        //if we have penalty god with less temples number, then set him -25 points
        if( ml.size() == 1 )
        {
          ml.front()->setEffectPoint( -25 );
        }
      }
    }

    foreach( it, divinities )
    {
      _d->updateRelation( _city, *it );
    }
  }

  if( GameDate::isMonthChanged() )
  {
    DivinityList divinities = rome::Pantheon::instance().all();

    //check gods wrath and mood
    std::map< int, DivinityList > godsWrath;
    std::map< int, DivinityList > godsUnhappy;
    foreach( it, divinities )
    {
      DivinityPtr god = *it;
      if( god->wrathPoints() > 0 )
      {
        godsWrath[ god->wrathPoints() ].push_back( god );
      }
      else if( god->relation() < 40 )
      {
        godsUnhappy[ god->relation() ].push_back( god );
      }
    }

    //find wrath god
    DivinityList someGods = divinities;
    if( !godsWrath.empty() )
    {
      someGods = godsWrath.rbegin()->second;
    }
    else if( !godsUnhappy.empty() )
    {
      someGods = godsUnhappy.rbegin()->second;
    }

    DivinityPtr randomGod;
    if( !someGods.empty() )
    {
      DivinityList::const_iterator it = someGods.begin();
      if( someGods.size() > 1 )
      {
        std::advance( it, math::random( someGods.size() ) );
      }
      randomGod = *it;
    }

    if( randomGod.isValid() )
    {
      randomGod->checkAction( &_city );
    }
  }
}

VariantMap Religion::save() const
{
  VariantMap ret = Srvc::save();

  ret[ "lastMessageDate" ] = _d->lastMessageDate;

  return ret;
}

void Religion::load(const VariantMap& stream)
{
  Srvc::load( stream );

  _d->lastMessageDate = stream.get( "lastMessageDate", GameDate::current() ).toDateTime();
}

void Religion::Impl::updateRelation( PlayerCity& city, DivinityPtr divn )
{
  Impl::CoverageInfo& myTemples = templesCoverity[ divn ];
  float faithIncome = (float)myTemples.parishionerNumber / (float)(city.population()+1);

  Logger::warning( "Religion: faith income for %s is %f[r=%f]", divn->name().c_str(), faithIncome, divn->relation() );
  divn->updateRelation( faithIncome, &city );

  if( divn->relation() < 30 && lastMessageDate.monthsTo( GameDate::current() ) > 6 )
  {
    lastMessageDate = GameDate::current();
    std::string text = divn->relation() < 10 ? "##gods_wrathful_text##" : "##gods_unhappy_text##";
    std::string title = divn->relation() < 10 ? "##gods_wrathful_title##" : "##gods_unhappy_title##";

    events::GameEventPtr e = events::ShowInfobox::create( _(title), _(text),
                                                          events::ShowInfobox::send2scribe );
    e->dispatch();
  }
}

}//end namespace city
