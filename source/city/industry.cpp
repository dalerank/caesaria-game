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

namespace city
{

namespace industry
{

object::Groups toGroups( Type type )
{
  object::Groups ret;
  switch( type )
  {
  case factoryAndTrade: ret.push_back( object::group::industry ); ret.push_back( object::group::trade ); break;
  case food: ret.push_back( object::group::food ); break;
  case engineering: ret.push_back( object::group::engineering ); break;
  case water: ret.push_back( object::group::water ); break;
  case prefectures: ret.push_back( object::group::security ); break;
  case military: ret.push_back( object::group::military ); break;
  case entertainment: ret.push_back( object::group::entertainment ); break;
  case healthAndEducation: ret.push_back( object::group::health ); ret.push_back( object::group::education ); break;
  case administrationAndReligion: ret.push_back( object::group::administration ); ret.push_back( object::group::religion ); break;
    default: break;
  }

  return ret;
}

}//end namespace industry

}//end namespace city
