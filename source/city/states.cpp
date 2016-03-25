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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "states.hpp"
#include "core/variant_map.hpp"

namespace city
{

VariantMap States::save() const
{
  VariantMap ret;

  VARIANT_SAVE_ANY(ret,age)
  VARIANT_SAVE_ENUM(ret,nation);
  VARIANT_SAVE_ANY(ret,population)
  VARIANT_SAVE_ANY(ret,birth)
  VARIANT_SAVE_ANY(ret,favor)
  VARIANT_SAVE_ANY(ret,money)

  return ret;
}

}//end namespace city
