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
#include "animals.hpp"
#include "helper.hpp"
#include "core/stringhelper.hpp"

using namespace constants;

std::string WalkerThinks::check(WalkerPtr walker, PlayerCityPtr city, const StringArray& own)
{
  SmartPtr< city::Info > info = ptr_cast< city::Info>( city->findService( city::Info::getDefaultName() ) );

  if( info.isNull() )
  {
    Logger::warning( "CitizenIdea::check no city service info" );
    return "##unknown_reason##";
  }

  if( is_kind_of<Animal>( walker ) )
  {
    std::string text = StringHelper::format( 0xff, "##animal_%s_say##", WalkerHelper::getTypename( walker->type() ).c_str() );
    return text;
  }

  StringArray troubles = own;
  std::string walkerTypename = WalkerHelper::getTypename( walker->type() );
  city::Info::Parameters params = info->getLast();
  if( params.monthWithFood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_so_hungry##" );
  }

  if( params.godsMood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_gods_angry##" );
  }

  if( params.colloseumCoverage < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_colloseum##" );
  }

  if( params.theaterCoverage < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_theater##" );
  }

  if( params.entertainment < 20 )
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

  if( params.payDiff < 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_low_salary##" );
  }

  if( !troubles.empty() )
  {
    return troubles.rand();
  }

  StringArray positiveIdeas = own;
  if( params.lifeValue > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_life##" );
  }
  else if( params.lifeValue > 75 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_average_life##" );
  }
  else if( params.lifeValue > 50 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_normal_life##" );
  }

  if( params.education > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_education##" );
  }

  std::string ret;
  if( !positiveIdeas.empty() )
  {
    ret = positiveIdeas.rand();
  }
  else
  {
    ret = positiveIdeas.rand();
  }

  return ret.empty() ? "##unknown_reason##" : ret;
}
