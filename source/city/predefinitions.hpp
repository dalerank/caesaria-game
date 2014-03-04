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

#ifndef __CAESARIA_CITY_PREDEFINITIONS_H_INCLUDED__
#define __CAESARIA_CITY_PREDEFINITIONS_H_INCLUDED__

#include "core/predefinitions.hpp"

PREDEFINE_CLASS_SMARTPOINTER(PlayerCity)

namespace city
{
PREDEFINE_CLASS_SMARTPOINTER_LIST(Srvc,List)

namespace request
{
PREDEFINE_CLASS_SMARTPOINTER_LIST(Request,List)
PREDEFINE_CLASS_SMARTPOINTER_LIST(RqGood,List)
PREDEFINE_CLASS_SMARTPOINTER(Dispatcher)
}

}

#endif //__CAESARIA_CITY_PREDEFINITIONS_H_INCLUDED__
