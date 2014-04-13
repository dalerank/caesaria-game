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
  DateTime lastDate;
  DateTime lastUnhappyMessageDate;
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
  _d->lastDate = GameDate::current();
  _d->lastUnhappyMessageDate = GameDate::current();
}

void Religion::update( const unsigned int time )
{  
  if( _d->lastDate.month() == GameDate::current().month() )
    return;

  Logger::warning( "Religion: start update relations" );
  _d->lastDate = GameDate::current();

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

  if( _d->templesCoverity.size() > 0 )
  {
    Impl::TemplesMap maxTemples;
    maxTemples.insert( *_d->templesCoverity.begin() );
    foreach( it, _d->templesCoverity )
    {
      if(  )
      divnList.push_back( );

      Impl::CoverageInfo& info = _d->templesCoverity[ *it ];

    }
  }

  foreach( it, divinities )
  {       
    _d->updateRelation( _city, *it );
  }
}

VariantMap Religion::save() const
{
  VariantMap ret = Srvc::save();

  ret[ "lastUnhappyDate" ] = _d->lastUnhappyMessageDate;

  return ret;
}

void Religion::load(const VariantMap& stream)
{
  Srvc::load( stream );

  _d->lastUnhappyMessageDate = stream.get( "lastUnhappyDate", GameDate::current() ).toDateTime();
}

void Religion::Impl::updateRelation(PlayerCity& city, DivinityPtr divn, )
{
  Impl::CoverageInfo& myTemples = templesCoverity[ divn ];
  float faithIncome = (float)myTemples.parishionerNumber / (float)(city.getPopulation()+1);

  Logger::warning( "Religion: faith income for %s is %f[r=%f]", divn->name().c_str(), faithIncome, divn->relation() );
  divn->updateRelation( faithIncome, &city );

  if( divn->relation() < 30 && lastUnhappyMessageDate.getDaysToDate( GameDate::current() ) > 6 )
  {
    lastUnhappyMessageDate = GameDate::current();
    events::GameEventPtr e = events::ShowInfobox::create( _("##gods_unhappy_title##"), _("##gods_unhappy_text##"),
                                                          events::ShowInfobox::send2scribe );
    e->dispatch();
  }
}

}//end namespace city
