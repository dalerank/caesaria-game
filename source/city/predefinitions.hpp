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

#ifndef __CAESARIA_CITY_PREDEFINITIONS_H_INCLUDED__
#define __CAESARIA_CITY_PREDEFINITIONS_H_INCLUDED__

#include "core/predefinitions.hpp"

PREDEFINE_CLASS_SMARTPOINTER(PlayerCity)

namespace city
{
PREDEFINE_CLASS_SMARTLIST(Srvc,List)

namespace request
{
PREDEFINE_CLASS_SMARTPOINTER(RqBase)
PREDEFINE_CLASS_SMARTPOINTER(RqGood)
PREDEFINE_CLASS_SMARTPOINTER(Dispatcher)
}

typedef request::RqBase Request;
typedef request::RqGood GoodRequest;
typedef SmartPtr<Request> RequestPtr;
typedef SmartPtr<GoodRequest> GoodRequestPtr;
typedef SmartList<Request> RequestList;

}//end namespace city

#endif //__CAESARIA_CITY_PREDEFINITIONS_H_INCLUDED__
