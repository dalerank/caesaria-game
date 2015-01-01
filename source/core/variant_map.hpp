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

#ifndef __CAESARIA_VARIANT_MAP_H_INCLUDED__
#define __CAESARIA_VARIANT_MAP_H_INCLUDED__

#include "variant.hpp"
#include "signals.hpp"

class VariantMap : public std::map<std::string, Variant>
{
public:
  typedef Delegate2< const std::string&, const Variant& > Visitor;

  VariantMap() {}

  VariantMap( const VariantMap& other )
  {
    *this = other;
  }

  VariantMap& operator+=(const VariantMap& other )
  {
    foreach( it, other )
    {
      (*this)[ it->first ] = it->second;
    }

    return *this;
  }

  void visitEach( Visitor visitor )
  {
    foreach( it, *this )
      visitor( it->first, it->second );
  }

  VariantMap& operator=(const VariantMap& other )
  {
    clear();

    foreach( it, other )
    {
      (*this)[ it->first ] = it->second;
    }

    return *this;
  }

  Variant get( const std::string& name, Variant defaultVal=Variant() ) const
  {
    VariantMap::const_iterator it = find( name );
    return (it != end() ? it->second : defaultVal );
  }

  Variant toVariant() const
  {
    return Variant( *this );
  }
};

#endif // __OPENCAESAR3_VARIANTPRIVATE_H_INCLUDED__
