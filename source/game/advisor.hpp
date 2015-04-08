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

DEFINE_NAMEDTYPE(Type,none)

#define REGISTER_ADVISOR(name,index) static const Type name = Type(index);

Type fromString( const std::string& name );

REGISTER_ADVISOR(employers,     0)
REGISTER_ADVISOR(military,      1)
REGISTER_ADVISOR(empire,        2)
REGISTER_ADVISOR(ratings,       3)
REGISTER_ADVISOR(trading,       4)
REGISTER_ADVISOR(population,    5)
REGISTER_ADVISOR(health,        6)
REGISTER_ADVISOR(education,     7)
REGISTER_ADVISOR(entertainment, 8)
REGISTER_ADVISOR(religion,      9)
REGISTER_ADVISOR(finance,       10)
REGISTER_ADVISOR(main,          11)
REGISTER_ADVISOR(unknown,       12)


} //end namespace advisor

typedef advisor::Type Advisor;

#endif  //_CAESARIA_ADVISOR_INCLUDE_H_
