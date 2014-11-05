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

#ifndef _CAESARIA_APPOINTEDWAY_H_INCLUDE_
#define _CAESARIA_APPOINTEDWAY_H_INCLUDE_

#include "pathway.hpp"
#include "objects/predefinitions.hpp"
#include <map>

class DirectRoute : public std::pair<ConstructionPtr,Pathway>
{
public:
  DirectRoute() {}
  DirectRoute( ConstructionPtr dst, const Pathway& way )
  {
    first = dst;
    second = way;
  }

  ConstructionPtr dst() const { return first; }
  const Pathway& way() const { return second; }
};

typedef std::map < ConstructionPtr, PathwayPtr > DirectPRoutes;

#endif //_CAESARIA_APPOINTEDWAY_H_INCLUDE_
