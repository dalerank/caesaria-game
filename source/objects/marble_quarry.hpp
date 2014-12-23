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

#ifndef __CAESARIA_FACTORYMARBLE_H_INCLUDED__
#define __CAESARIA_FACTORYMARBLE_H_INCLUDED__

#include "factory.hpp"

class MarbleQuarry : public Factory
{
public:
  MarbleQuarry();

  virtual bool canBuild(const CityAreaInfo& areaInfo) const;  // returns true if it can be built there
  virtual void timeStep(const unsigned long time);
};

#endif //__CAESARIA_FACTORYMARBLE_H_INCLUDED__
