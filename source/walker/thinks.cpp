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
    std::string text = StringHelper::format( 0xff, "##animal_%s_say##", WalkerHelper::getTypename( walker->getType() ).c_str() );
    return text;
  }

  StringArray troubles = own;
  std::string walkerTypename = WalkerHelper::getTypename( walker->getType() );
  city::Info::Parameters params = info->getLast();
  if( params.monthWithFood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_so_hungry##" );
    troubles << "##citizen_so_hungry##";
    troubles << "##citizen_so_hungry2##";
    troubles << "##citizen_so_hungry3##";
    troubles << "##citizen_so_hungry4##";
  }

  if( params.godsMood < 3 )
  {
    troubles.push_back( "##" + walkerTypename + "_gods_angry##" );
    troubles << "##citizen_gods_angry##";
    troubles << "##citizen_gods_angry2##";
    troubles << "##citizen_gods_angry3##";
    troubles << "##citizen_gods_angry4##";
    troubles << "##citizen_gods_angry5##";
    troubles << "##citizen_gods_angry6##";
    troubles << "##citizen_gods_angry7##";
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
    troubles << "##citizen_low_entertainment##";
    troubles << "##citizen_low_entertainment2##";
    troubles << "##citizen_low_entertainment3##";
    troubles << "##citizen_low_entertainment4##";
    troubles << "##citizen_low_entertainment5#";
  }
  else if( params.entertainment < 20 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_entertainment##" );
  }

  if( params.needWorkers > 0 )
  {
    troubles.push_back( "##" + walkerTypename + "_need_workers##" );
    troubles << "##citizen_need_workers##";
    troubles << "##citizen_need_workers2##";
    troubles << "##citizen_need_workers3##";
    troubles << "##citizen_need_workers4##";
    troubles << "##citizen_need_workers5##";
    troubles << "##citizen_need_workers6##";
    troubles << "##citizen_need_workers7##";
    troubles << "##citizen_need_workers8##";
    troubles << "##citizen_need_workers9##";
  }

  if( params.workless > 15 )
  {
    troubles.push_back( "##" + walkerTypename + "_high_workless##" );
    troubles << "##citizen_high_workless##" ;
    troubles << "##citizen_high_workless2##";
    troubles << "##citizen_high_workless3##";
    troubles << "##citizen_high_workless4##";
    troubles << "##citizen_high_workless5##";
    troubles << "##citizen_high_workless6##";
    troubles << "##citizen_high_workless7##";
    troubles << "##citizen_high_workless8##";
    troubles << "##citizen_high_workless9##";
    troubles << "##citizen_high_workless10##";
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
    return troubles.rand();
  }

  StringArray positiveIdeas = own;
  if( params.lifeValue > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_perfect_life##" );
  }
  else if( params.lifeValue > 75 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_life##" );
    troubles << "##citizen_good_life##";
  }
  else if( params.lifeValue > 50 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_average_life##" );
    troubles << "##citizen_average_life_1##";
  }

  if( params.education > 90 )
  {
    positiveIdeas.push_back( "##" + walkerTypename + "_good_education##" );
    positiveIdeas << "##citizen_good_education##";
  }

  if( !positiveIdeas.empty() )
  {
    return positiveIdeas.rand();
  }

  return "##unknown_reason##";
}
