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

#include "productmap.hpp"
#include "core/foreach.hpp"
#include "core/position.hpp"

namespace good
{

ProductMap& ProductMap::operator+=(const ProductMap& other)
{
  foreach( it, other )
    (*this)[ it->first ] += it->second;

  return *this;
}

VariantList ProductMap::save() const
{
  VariantList ret;
  foreach( it, *this )
    ret << Point( it->first, it->second );

  return ret;
}

void ProductMap::load(const VariantList &stream)
{
  foreach( it, stream )
  {
    Point t = *it;
    (*this)[ (Product)t.x() ] = t.y();
  }
}

}//end namespace good
    
