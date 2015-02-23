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

BEGIN_NAMEDTYPE(Type)
explicit Type( const std::string& name );
bool operator>=(const Type& a) const;
END_NAMEDTYPE(Type)

const Type employers(0);
const Type military(1);
const Type empire(2);
const Type ratings(3);
const Type trading(4);
const Type population(5);
const Type health(6);
const Type education(7);
const Type entertainment(8);
const Type religion(9);
const Type finance(10);
const Type main(11);
const Type unknown(12);

} //end namespace advisor

typedef advisor::Type Advisor;

#endif  //_CAESARIA_ADVISOR_INCLUDE_H_
