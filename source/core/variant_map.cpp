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
// Copyright 2012-2016 Dalerank, dalerankn8@gmail.com

#include "variant_map.hpp"

VariantMap::VariantMap() {}

const Variant VariantMap::defValue = Variant();

VariantMap::VariantMap(const VariantMap& other)
{
  _data = other._data;
}

VariantMap::Collection::iterator VariantMap::erase(VariantMap::Collection::iterator it)
{
  return _data.erase(it);
}

void VariantMap::erase(const std::string& name)
{
  _data.erase(name);
}

VariantMap& VariantMap::operator+=(const VariantMap& other)
{
  for (auto& it : other) {
    _data[ it.first ] = it.second;
  }

  return *this;
}

void VariantMap::visitEach(VariantMap::Visitor visitor)
{
  for (auto& it : _data) {
    visitor( it.first, it.second );
  }
}

Variant& VariantMap::operator[](const std::string& name) { return _data[name]; }

VariantMap& VariantMap::operator=(const VariantMap& other)
{
  clear();

  for (const auto& item : other._data)
    _data[item.first] = item.second;

  return *this;
}

Variant VariantMap::get(const std::string& name,const Variant& defaultVal) const
{
  auto it = _data.find( name );
  return (it != _data.end() ? it->second : defaultVal);
}

bool VariantMap::has(const std::string& name) const
{
  return _data.count(name) > 0;
}

Variant VariantMap::toVariant() const
{
  return Variant(*this);
}
