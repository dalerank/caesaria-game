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

#include "industry.hpp"

using namespace constants;

namespace city
{

namespace industry
{

BuildingGroups toGroups( Type type )
{
  std::vector<constants::objects::Group> ret;
  switch( type )
  {
  case factoryAndTrade: ret.push_back( objects::industryGroup ); ret.push_back( objects::tradeGroup ); break;
  case food: ret.push_back( objects::foodGroup ); break;
  case engineering: ret.push_back( objects::engineeringGroup ); break;
  case water: ret.push_back( objects::waterGroup ); break;
  case prefectures: ret.push_back( objects::securityGroup ); break;
  case military: ret.push_back( objects::militaryGroup ); break;
  case entertainment: ret.push_back( objects::entertainmentGroup ); break;
  case healthAndEducation: ret.push_back( objects::healthGroup ); ret.push_back( objects::educationGroup ); break;
  case administrationAndReligion: ret.push_back( objects::administrationGroup ); ret.push_back( objects::religionGroup ); break;
    default: break;
  }

  return ret;
}

}//end namespace industry

}//end namespace city
