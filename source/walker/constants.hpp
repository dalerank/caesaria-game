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

#ifndef __OPENCAESAR3_WALKER_CONSTANTS_H_INCLUDED__
#define __OPENCAESAR3_WALKER_CONSTANTS_H_INCLUDED__

namespace constants
{

namespace walker
{

enum Type{
  unknown=0,
  immigrant,
  emigrant,
  soldier,
  cartPusher,
  marketLady,
  marketKid,
  serviceman,                   //unknown service
  trainee,
  recruter,
  prefect,
  taxCollector,
  engineer,
  doctor,
  bathlady,
  merchant,
  actor,
  gladiator,
  tamer,
  charioter,
  sheep,
  barber,
  surgeon,
  fishingBoat,
  priest,
  protestor,
  patrician,
  corpse,
  enemySoldier,
  britonSoldier,
  all = 0xff,
  any = all
};

} //end namespace walker

} //end namespace constants

#endif //__OPENCAESAR3_WALKER_CONSTANTS_H_INCLUDED__
