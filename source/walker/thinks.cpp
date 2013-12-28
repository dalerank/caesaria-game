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

using namespace constants;

std::string WalkerThinks::check(WalkerPtr walker, PlayerCityPtr city)
{
  SmartPtr< CityServiceInfo > info = city->findService( CityServiceInfo::getDefaultName() ).as<CityServiceInfo>();

  if( info.isNull() )
  {
    Logger::warning( "CitizenIdea::check no city service info" );
    return _("##unknown_reason##");
  }

  StringArray troubles;
  std::string walkerTypename = WalkerHelper::getTypename( walker->getType() );
  CityServiceInfo::Parameters params = info->getLast();
  if( params.monthWithFood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_so_hungry##" );
  }

  if( params.godsMood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_gods_angry" );
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
  }

  if( params.needWorkers > 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_workers##" );
  }

  if( params.workless > 15 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_workless##" );
  }

  if( params.tax > 10 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_tax##" );
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
