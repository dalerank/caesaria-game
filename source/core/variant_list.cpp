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

#include "variant_list.hpp"

Variant VariantList::get(const unsigned int index, Variant defaultVal) const
{
  VariantList::const_iterator it = begin();
  if( index >= size() )
    {
      return defaultVal;
    }
  else
    {
      std::advance( it, index );
      return *it;
    }
}

void VariantList::visitEach(VariantList::Visitor visitor)
{
  for( auto& item : *this )
    visitor( item );
}

VariantList&VariantList::operator <<(const Variant& v)
{
  push_back( v );
  return *this;
}

VariantListReader::VariantListReader(const VariantList& list)
  : _list( list )
{
  _it = _list.begin();
}

const Variant&VariantListReader::next()
{
  const Variant& value = *_it;
  ++_it;

  return value;
}

bool VariantListReader::atEnd() { return _it == _list.end(); }
