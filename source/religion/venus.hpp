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

#ifndef __CAESARIA_ROME_DIVINITY_VENUS_H_INCLUDED__
#define __CAESARIA_ROME_DIVINITY_VENUS_H_INCLUDED__

#include "divinities.hpp"

namespace religion
{

namespace rome
{

class Venus : public RomeDivinity
{
public:
  static DivinityPtr create();
  virtual void updateRelation(float income, PlayerCityPtr city);

protected:
  void _doWrath( PlayerCityPtr city );
  void _doBlessing( PlayerCityPtr city );
  void _doSmallCurse( PlayerCityPtr city );
};

}//end namespace rome

}//end namespace religion

#endif //__CAESARIA_ROME_DIVINITY_VENUS_H_INCLUDED__
