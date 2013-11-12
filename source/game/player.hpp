// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "game/enums.hpp"
#include "core/serializer.hpp"
#include "core/scopedptr.hpp"
#include "predefinitions.hpp"
#include "core/referencecounted.hpp"

class Player : public Serializable, public ReferenceCounted
{
public:
   static PlayerPtr create();

   void save( VariantMap& stream) const;
   void load( const VariantMap& stream);

   void setName( const std::string& name );
   std::string getName() const;

   int getSalary() const;
   void setSalary( const int value );

   void appendMoney( int money );
   int getMoney() const;

   ~Player();
private:
   Player();

   class Impl;
   ScopedPtr< Impl > _d;
};

#endif
