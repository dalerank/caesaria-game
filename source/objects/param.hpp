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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_OBJECTPARAM_H_INCLUDED__
#define __CAESARIA_OBJECTPARAM_H_INCLUDED__

#include "core/namedtype.hpp"
#include "core/hash.hpp"

#define REGISTER_PARAM(name,id) static const Param name = Param(id);
#define REGISTER_PARAM_H(name) static const Param name = Param( (int)Hash(#name) );

DEFINE_NAMEDTYPE(Param,none)

namespace pr
{
REGISTER_PARAM(fire,0 )
REGISTER_PARAM(damage,1)
REGISTER_PARAM(inflammability,2)
REGISTER_PARAM(collapsibility,3)
REGISTER_PARAM(destroyable,4)
REGISTER_PARAM(health,5)
REGISTER_PARAM(happiness,6)
REGISTER_PARAM(happinessBuff,7)
REGISTER_PARAM(healthBuff,8)
REGISTER_PARAM(settleLock,9)
REGISTER_PARAM(lockTerrain,10)
REGISTER_PARAM(food,11)
REGISTER_PARAM(paramCount,12)
}

#endif //__CAESARIA_OBJECTPARAM_H_INCLUDED__
