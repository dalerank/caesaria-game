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

#ifndef _CAESARIA_WHARF_INCLUDE_H_
#define _CAESARIA_WHARF_INCLUDE_H_

#include "coastalbuilding.hpp"
#include "core/direction.hpp"

class Wharf : public CoastalFactory
{
public:
  Wharf();

  virtual void destroy();
  virtual void timeStep(const unsigned long time);

  ShipPtr getBoat() const;
  virtual void assignBoat( ShipPtr boat );

private:
  virtual void _updatePicture( constants::Direction direction );

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //_CAESARIA_WHARF_INCLUDE_H_
