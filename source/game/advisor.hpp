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

#ifndef _CAESARIA_ADVISOR_INCLUDE_H_
#define _CAESARIA_ADVISOR_INCLUDE_H_

#include <string>
#include "core/namedtype.hpp"

namespace advisor
{

BEGIN_NAMEDTYPE(Type,none)
APPEND_NAMEDTYPE(employers)
APPEND_NAMEDTYPE(military)
APPEND_NAMEDTYPE(empire)
APPEND_NAMEDTYPE(ratings)
APPEND_NAMEDTYPE(trading)
APPEND_NAMEDTYPE(population)
APPEND_NAMEDTYPE(health)
APPEND_NAMEDTYPE(education)
APPEND_NAMEDTYPE(entertainment)
APPEND_NAMEDTYPE(religion)
APPEND_NAMEDTYPE(finance)
APPEND_NAMEDTYPE(main)
APPEND_NAMEDTYPE(unknown)
END_NAMEDTYPE(Type)

Type fromString( const std::string& name );

} //end namespace advisor

typedef advisor::Type Advisor;

#endif  //_CAESARIA_ADVISOR_INCLUDE_H_
