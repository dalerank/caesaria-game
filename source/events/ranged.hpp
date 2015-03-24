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

#ifndef _CAESARIA_RANGET_EVENT_H_INCLUDE_
#define _CAESARIA_RANGET_EVENT_H_INCLUDE_

#include "event.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"

namespace events
{

CAESARIA_LITERALCONST(population)

class Ranged : public GameEvent
{
public:
  struct Range
  {
    int min, max;

    inline bool contain( int value ) { return (value >= min && value <= max); }
    VariantList save() const
    {
      VariantList ret;
      ret << min << max;

      return ret;
    }

    void load( const VariantList& vl )
    {
      min = vl.get( 0, 0 );
      max = vl.get( 1, 999999 );
    }
  };

};

}//namespace events

#endif //_CAESARIA_RANGET_EVENT_H_INCLUDE_
