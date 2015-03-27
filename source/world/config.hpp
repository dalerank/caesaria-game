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

#ifndef __CAESARIA_WORLD_CONFIG_INCLUDE_H__
#define __CAESARIA_WORLD_CONFIG_INCLUDE_H__

namespace config
{

namespace army
{
enum { viewRange=20 };
}

namespace barbarian
{
enum { attackRange=20, viewRange=60 };
}

namespace chastener
{
enum { brokeAttack=35 };
}

namespace trade
{
enum { maxMerchantsInRoute = 2, minMonthsMerchantSend = 2 };
}

namespace emperor
{
enum { yearlyFavorDecrease=2, maxFavourUpdate=5, defaultFavor=50, maxFavor=100 };
}

} // end namespace config

namespace econ
{
enum { cityAge4tax=2, defaultSalary=30 };
}

#endif //__CAESARIA_WORLD_CONFIG_INCLUDE_H__
