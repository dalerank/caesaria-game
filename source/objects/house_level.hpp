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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_HOUSE_LEVEL_H_INCLUDE_
#define _CAESARIA_HOUSE_LEVEL_H_INCLUDE_

class HouseLevel
{
public:
  typedef enum { vacantLot=0,
                 hovel=1, tent,
                 shack, hut, //4
                 domus, bigDomus, //6
                 mansion, bigMansion, //8
                 insula, middleInsula,   //10
                 bigInsula, beatyfullInsula, //12
                 smallVilla,  middleVilla,  bigVilla,  greatVilla,
                 smallPalace, middlePalace, bigPalace, greatPalace,
                 count } ID;

  typedef enum { maxSize2=2, maxSize3, maxSize4 } HouseMaxSize;
};

#endif //_CAESARIA_HOUSE_LEVEL_H_INCLUDE_
