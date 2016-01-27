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
#include "delegate.hpp"

class VariantMap : public std::map<std::string, Variant>
{
public:
  typedef Delegate2< const std::string&, const Variant& > Visitor;

  VariantMap() {}

  VariantMap( const VariantMap& other );

  VariantMap& operator+=(const VariantMap& other );

  void visitEach( Visitor visitor );

  VariantMap& operator=(const VariantMap& other );

  Variant get( const std::string& name, Variant defaultVal=Variant() ) const;

  Variant toVariant() const;
};

#endif // __CAESARIA_VARIANT_MAP_H_INCLUDED__
