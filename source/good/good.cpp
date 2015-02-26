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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "good.hpp"

namespace good
{

static const Product __pAll[] = { none,
                                   wheat, fish, meat,fruit,vegetable,olive,oil,
                                   grape, wine, timber, furniture, clay,
                                   pottery, iron, weapon, marble, denaries,
                                   prettyWine
                                 };

static const Product __pFoods[] = { wheat, fish, meat, fruit, vegetable };
static const Product __pMaterials[] = { olive, grape, timber, clay, iron, marble };

#define ASSIGN(name,base) static const Products name = Products( base, sizeof(base) );
ASSIGN(foods, __pFoods)
ASSIGN(materials, __pMaterials )
ASSIGN(all, __pAll )

#undef ASSIGN

}
