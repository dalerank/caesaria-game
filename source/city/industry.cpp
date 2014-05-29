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

std::vector<constants::building::Group> city::Industry::toGroups( Type type )
{
  std::vector<constants::building::Group> ret;
  switch( type )
  {
  case factoryAndTrade: ret.push_back( building::industryGroup ); ret.push_back( building::tradeGroup ); break;
  case food: ret.push_back( building::foodGroup ); break;
  case engineering: ret.push_back( building::engineeringGroup ); break;
  case water: ret.push_back( building::waterGroup ); break;
  case prefectures: ret.push_back( building::securityGroup ); break;
  case military: ret.push_back( building::militaryGroup ); break;
  case entertainment: ret.push_back( building::entertainmentGroup ); break;
  case healthAndEducation: ret.push_back( building::healthGroup ); ret.push_back( building::educationGroup ); break;
  case administrationAndReligion: ret.push_back( building::administrationGroup ); ret.push_back( building::religionGroup ); break;
    default: break;
  }

  return ret;
}
