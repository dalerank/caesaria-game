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

#include "params.hpp"
#include "core/variant_list.hpp"

namespace pr
{

VariantList Array::save() const
{
  VariantList ret;
  for( auto& item : *this )
    ret.push_back( VariantList( item.first, item.second ) );

  return ret;
}

void Array::load(const VariantList& stream)
{
  for( auto& item : stream )
  {
    const VariantList& vl = item.toList();
    Param param = vl.get( 0 ).toEnum<Param>();
    double value = vl.get( 1, 0.f ).toDouble();
    (*this)[ param ] = value;
  }
}

}//end namespace pr
