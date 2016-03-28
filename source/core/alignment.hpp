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

#ifndef __CAESARIA_ALIGNMENT_H_INCLUDED__
#define __CAESARIA_ALIGNMENT_H_INCLUDED__

#include "enumerator.hpp"

namespace align
{

enum Type
{
  //! Aligned to parent's top or left side (default)
  upperLeft=0,
  //! Aligned to parent's bottom or right side
  lowerRight,
  //! Aligned to the center of parent
  center,
  //! Stretched to fit parent
  scale,
  //!
  automatic
};

class Helper : public EnumsHelper<align::Type>
{
public:
  Helper();
};

} // end namespacce align

namespace margin
{

enum Type
{
  //!
  left=0,
  top,
  right,
  bottom,
  count
};

}

typedef margin::Type Margin;
typedef align::Type Alignment;

#endif // __CAESARIA_ALIGNMENT_H_INCLUDED__

