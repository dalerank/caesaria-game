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

#include "emperor.hpp"
#include "core/foreach.hpp"
#include <map>

namespace world
{

class Emperor::Impl
{
public:
  typedef std::map< std::string, int > Relations;
  Relations relations;
};

Emperor::Emperor() : __INIT_IMPL(Emperor)
{

}

Emperor::~Emperor()
{

}

int Emperor::relation(const std::string& cityname)
{
  __D_IMPL(d,Emperor)
  Impl::Relations::iterator i = d->relations.find( cityname );
  return ( i == d->relations.end() ? 0 : i->second );
}

void Emperor::updateRelation(const std::string& cityname, int value)
{
  __D_IMPL(d,Emperor)
  Impl::Relations::iterator i = d->relations.find( cityname );
  if( i != d->relations.end() )
  {
    i->second = math::clamp<int>( i->second + value, 0, 100 );
  }
}

VariantMap Emperor::save() const
{
  VariantMap ret;

  Impl::Relations r = _dfunc()->relations;
  VariantMap vm_relations;
  foreach( it, r )
  {
    vm_relations[ it->first ] = it->second;
  }

  ret[ "relations" ] = vm_relations;
  return ret;
}

void Emperor::load(const VariantMap& stream)
{
  VariantMap vm_relations = stream.get( "relations" ).toMap();

  Impl::Relations& relations = _dfunc()->relations;
  foreach( it, vm_relations )
  {
    relations[ it->first ] = it->second;
  }
}

}
