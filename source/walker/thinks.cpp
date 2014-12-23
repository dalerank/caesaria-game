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

#include "thinks.hpp"
#include "city/cityservice_info.hpp"
#include "core/logger.hpp"
#include "walker.hpp"
#include "constants.hpp"
#include "animals.hpp"
#include "helper.hpp"
#include "core/utils.hpp"

using namespace constants;
using namespace city;

std::string WalkerThinks::check(WalkerPtr walker, PlayerCityPtr city, const StringArray& own)
{
  city::InfoPtr info;
  info << city->findService( city::Info::defaultName() );

  if( info.isNull() )
  {
    Logger::warning( "CitizenIdea::check no city service info" );
    return "##unknown_reason##";
  }

  if( is_kind_of<Animal>( walker ) )
  {
    std::string text = utils::format( 0xff, "##animal_%s_say##", WalkerHelper::getTypename( walker->type() ).c_str() );
    return text;
  }

  StringArray troubles = own;
  std::string walkerTypename = WalkerHelper::getTypename( walker->type() );
  city::Info::Parameters params = info->lastParams();
  if( params[ Info::monthWithFood ] < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_so_hungry##" );
  }

  if( params[ Info::godsMood ] < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_gods_angry##" );
  }

  if( params[ Info::colloseumCoverage ] < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_colloseum##" );
  }

  if( params[ Info::theaterCoverage ] < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_theater##" );
  }

  if( params[ Info::entertainment ] < 20 )
  {
    troubles.push_back( "##" + walkerTypename + "_low_entertainment##" );
  }

  if( params[ Info::needWorkers ] > 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_workers##" );
  }

  if( params[ Info::workless ] > 15 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_workless##" );
  }

  if( params[ Info::tax ] > 10 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_tax##" );
  }

  if( params[ Info::payDiff ] < 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_low_salary##" );
  }

  if( !troubles.empty() )
  {
    return troubles.random();
  }

  StringArray positiveIdeas = own;
  if( params[ Info::lifeValue ] > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_life##" );
  }
  else if( params[ Info::lifeValue ] > 75 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_average_life##" );
  }
  else if( params[ Info::lifeValue ] > 50 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_normal_life##" );
  }

  if( params[ Info::education ] > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_education##" );
  }

  std::string ret;
  if( !positiveIdeas.empty() )
  {
    ret = positiveIdeas.random();
  }
  else
  {
    ret = positiveIdeas.random();
  }

  return ret.empty() ? "##unknown_reason##" : ret;
}
