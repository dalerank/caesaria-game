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

#ifndef __CAESARIA_WORLD_PREDEFINITIONS_H_INCLUDED__
#define __CAESARIA_WORLD_PREDEFINITIONS_H_INCLUDED__

#include "core/smartptr.hpp"
#include "core/predefinitions.hpp"

namespace world
{

PREDEFINE_CLASS_SMARTPOINTER(Empire)
PREDEFINE_CLASS_SMARTPOINTER(Emperor)
PREDEFINE_CLASS_SMARTPOINTER(Messenger)
PREDEFINE_CLASS_SMARTPOINTER(EmpireMap)
PREDEFINE_CLASS_SMARTPOINTER(ComputerCity)

PREDEFINE_CLASS_SMARTLIST(Traderoute,List)
PREDEFINE_CLASS_SMARTLIST(Merchant,List)
PREDEFINE_CLASS_SMARTLIST(Barbarian,List)
PREDEFINE_CLASS_SMARTLIST(PlayerArmy,List)
PREDEFINE_CLASS_SMARTLIST(RomeChastenerArmy,List)
PREDEFINE_CLASS_SMARTLIST(Object,List)
PREDEFINE_CLASS_SMARTLIST(Army,List)
PREDEFINE_CLASS_SMARTLIST(City,List)
PREDEFINE_CLASS_SMARTLIST(MovableObject,List)
PREDEFINE_CLASS_SMARTLIST(GoodCaravan,List)

}

#endif //__CAESARIA_WORLD_PREDEFINITIONS_H_INCLUDED__
