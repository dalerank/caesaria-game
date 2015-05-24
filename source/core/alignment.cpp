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


#include "alignment.hpp"

namespace align
{

Helper::Helper() : EnumsHelper<Type>( automatic )
{
  append( upperLeft,  CAESARIA_STR_A(upperLeft) );
  append( lowerRight, CAESARIA_STR_A(lowerRight) );
  append( center,     CAESARIA_STR_A(center) );
  append( scale,      CAESARIA_STR_A(scale) );
  append( automatic, "auto" );
}

}//end namespace alig
