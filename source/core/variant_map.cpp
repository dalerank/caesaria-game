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

#include "variant_map.hpp"

VariantMap::VariantMap(const VariantMap& other)
{
  *this = other;
}

VariantMap& VariantMap::operator+=(const VariantMap& other)
{
  for (auto& it : other)
    {
      (*this)[ it.first ] = it.second;
    }

  return *this;
}

void VariantMap::visitEach(VariantMap::Visitor visitor)
{
  for (auto& it : *this)
    {
      visitor( it.first, it.second );
    }
}

VariantMap& VariantMap::operator=(const VariantMap& other)
{
  clear();

  for( auto& item : other)
    {
      (*this)[ item.first ] = item.second;
    }

  return *this;
}

Variant VariantMap::get(const std::string& name, Variant defaultVal) const
{
  VariantMap::const_iterator it = find( name );
  return (it != end() ? it->second : defaultVal );
}

Variant VariantMap::toVariant() const
{
  return Variant( *this );
}
