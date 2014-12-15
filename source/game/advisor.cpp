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

#include "advisor.hpp"
#include "core/enumerator.hpp"

namespace constants
{

namespace advisor
{

Type findType(const std::string& advisorName)
{
  static EnumsHelper<Type> names( count );

#define ADD_ADVISOR(type) names.append(type,CAESARIA_STR_EXT(type) );

  if( names.empty() )
  {
    ADD_ADVISOR(employers)
    ADD_ADVISOR(military)
    ADD_ADVISOR(empire)
    ADD_ADVISOR(ratings)
    ADD_ADVISOR(trading)
    ADD_ADVISOR(population)
    ADD_ADVISOR(health)
    ADD_ADVISOR(education)
    ADD_ADVISOR(entertainment)
    ADD_ADVISOR(religion)
    ADD_ADVISOR(finance)
    ADD_ADVISOR(main)
  }

#undef ADD_ADVISOR

  return names.findType( advisorName );
}

} //end namespace advisor

} //end namespace constants
