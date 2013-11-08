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

typedef int Type;

const Type unknown=0;
const Type immigrant=1;
const Type emigrant=2;
const Type soldier=3;
const Type cartPusher=4;
const Type WT_MARKETLADY=5;
const Type marketLady=6;
const Type WT_SERVICE=7;                     //unknown service
const Type trainee=8;
const Type recruter=9;
const Type prefect=10;
const Type taxCollector=11;
const Type engineer=12;
const Type doctor=13;
const Type bathlady=14;
const Type WT_MERCHANT=15;
const Type actor=16;
const Type gladiator=17;
const Type tamer=18;
const Type charioter=19;
const Type sheep=20;
const Type barber=21;
const Type surgeon=22;
const Type fishingBoat=23;
const Type WT_PRIEST=24;
const Type rioter=25;
const Type patrician=26;
const Type WT_ALL=0xff;

}

}

#endif //__OPENCAESAR3_WALKER_CONSTANTS_H_INCLUDED__
