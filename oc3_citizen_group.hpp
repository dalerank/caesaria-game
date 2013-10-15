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

#ifndef _OPENCAESAR_HOUSE_HABITANT_INCLUDE_H_
#define _OPENCAESAR_HOUSE_HABITANT_INCLUDE_H_

#include "oc3_variant.hpp"

class CitizenGroup : public std::map< int, int >
{
public:
  typedef enum { child, young, student, mature, aged } Age;

  int count() const;
  int count( Age group ) const;

  CitizenGroup retrieve( int count );

  CitizenGroup& operator += ( const CitizenGroup& b );

  VariantMap save() const;
  void load( const VariantMap& stream );
};

#endif //_OPENCAESAR_HOUSE_HABITANT_INCLUDE_H_
