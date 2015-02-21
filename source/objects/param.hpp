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

DEFINE_NAMEDTYPE(Param)

namespace pr
{
const Param fire( 0 );
const Param damage( 1 );
const Param inflammability(2);
const Param collapsibility(3);
const Param destroyable(4);
const Param health(5);
const Param happiness(6);
const Param happinessBuff(7);
const Param healthBuff(8);
const Param settleLock(9);
const Param lockTerrain(10);
const Param food(11);
const Param paramCount(12);
}

#endif //__CAESARIA_OBJECTPARAM_H_INCLUDED__
