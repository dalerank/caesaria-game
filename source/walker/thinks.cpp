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

#include "thinks.hpp"
#include "city/cityservice_info.hpp"
#include "core/logger.hpp"
#include "walker.hpp"
#include "constants.hpp"
#include "core/gettext.hpp"
#include "animals.hpp"

using namespace constants;

std::string WalkerThinks::check(WalkerPtr walker, PlayerCityPtr city)
{
  SmartPtr< CityServiceInfo > info = ptr_cast<CityServiceInfo>( city->findService( CityServiceInfo::getDefaultName() ) );

  if( info.isNull() )
  {
    Logger::warning( "CitizenIdea::check no city service info" );
    return _("##unknown_reason##");
  }

  if( is_kind_of<Animal>( walker ) )
  {
    std::string text = StringHelper::format( 0xff, "##animal_%s_say##", WalkerHelper::getTypename( walker->getType() ).c_str() );
    return _( text );
  }

  StringArray troubles;
  std::string walkerTypename = WalkerHelper::getTypename( walker->getType() );
  CityServiceInfo::Parameters params = info->getLast();
  if( params.monthWithFood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_so_hungry##" );
    troubles.push_back( "##citizen_so_hungry##" );
    troubles.push_back( "##citizen_so_hungry2##" );
    troubles.push_back( "##citizen_so_hungry3##" );
  }

  if( params.godsMood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_gods_angry##" );
    troubles.push_back( "##citizen_gods_angry##" );
    troubles.push_back( "##citizen_gods_angry2##" );
  }

  if( params.colloseumCoverage < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_colloseum##" );
  }

  if( params.theaterCoverage < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_theater##" );
  }

  if( params.entertainment < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_low_entertainment##" );
    troubles.push_back( "##citizen_low_entertainment##" );
  }
  else if( params.entertainment < 20 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_entertainment##" );
  }

  if( params.needWorkers > 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_workers##" );
    troubles.push_back( "##citizen_need_workers##" );
    troubles.push_back( "##citizen_need_workers2##" );
  }

  if( params.workless > 15 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_workless##" );
    troubles.push_back( "##citizen_high_workless##" );
    troubles.push_back( "##citizen_high_workless2##" );
    troubles.push_back( "##citizen_high_workless3##" );
    troubles.push_back( "##citizen_high_workless4##" );
  }

  if( params.tax > 10 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_tax##" );
  }

  if( params.payDiff < 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_low_salary##" );
    troubles.push_back( "##citizen_low_salary##" );
  }

  if( !troubles.empty() )
  {
    std::string trouble = troubles.at( rand() % troubles.size() );
    return _( trouble.c_str() );
  }

  StringArray positiveIdeas;
  if( params.lifeValue > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_perfect_life##" );
  }
  else if( params.lifeValue > 75 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_life##" );
  }
  else if( params.lifeValue > 50 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_average_life##" );
  }

  if( !positiveIdeas.empty() )
  {
    std::string idea = positiveIdeas.at( rand() % positiveIdeas.size() );
    return _( idea.c_str() );
  }

  return _("##unknown_reason##");
}
