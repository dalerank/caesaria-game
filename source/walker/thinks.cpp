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
#include "objects/construction.hpp"
#include "core/utils.hpp"
#include "city/statistic.hpp"

using namespace city;

class ThinksConstructor : public StringArray
{
public:
  ThinksConstructor( const std::string& type )
  {
    _type = type;
  }

  ThinksConstructor& addIf( bool condition, const std::string& past )
  {
    if( condition )
      push_back( "##" + _type + past + "##" );

    return *this;
  }

private:
  std::string _type;
};


std::string WalkerThinks::check(WalkerPtr walker, PlayerCityPtr city, const StringArray& own)
{
  city::InfoPtr info = city->statistic().services.find<Info>();

  if( info.isNull() )
  {
    Logger::warning( "CitizenIdea::check no city service info" );
    return "##unknown_reason##";
  }

  if( walker.is<Animal>() )
  {
    std::string text = fmt::format( "##animal_{0}_say##", WalkerHelper::getTypename( walker->type() ) );
    return text;
  }

  Info::Parameters params = info->lastParams();
  ThinksConstructor ret( WalkerHelper::getTypename( walker->type() ) );
  ret << own;

  ret.addIf( params[ Info::monthWithFood ] < 3,     "_so_hungry" )
     .addIf( params[ Info::godsMood ] < 3,          "_gods_angry" )
     .addIf( params[ Info::colloseumCoverage ] < 3, "_need_colloseum" )
     .addIf( params[ Info::theaterCoverage ] < 3,   "_need_theater" )
     .addIf( params[ Info::entertainment ] < 20,    "_low_entertainment" )
     .addIf( params[ Info::needWorkers ] > 0,       "_need_workers" )
     .addIf( params[ Info::workless ] > 15,         "_high_workless" )
     .addIf( params[ Info::tax ] > 10,              "_high_tax" )
     .addIf( params[ Info::payDiff ] < 0,           "_low_salary" );

  if( !ret.empty() )
    return ret.random();

  ret.clear();
  ret << own;
  ret.addIf( params[ Info::lifeValue ] > 90, "_good_life" )
     .addIf( params[ Info::lifeValue ] > 75, "_average_life" )
     .addIf( params[ Info::lifeValue ] > 50, "_normal_life" )
     .addIf( params[ Info::education ] > 90, "_good_education" );

  return ret.empty() ? "##unknown_reason##" : ret.random();
}
