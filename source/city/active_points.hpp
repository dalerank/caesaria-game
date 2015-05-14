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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_SAVED_POINTS_H_INCLUDED__
#define __CAESARIA_SAVED_POINTS_H_INCLUDED__

#include "core/tilepos_array.hpp"

namespace city
{  

/*
 * Points for camera, which user can set
 */
class ActivePoints : TilePosArray
{
  enum { maxPoints = 10 };
public:
  ActivePoints();

  TilePos get( unsigned int index ) const;
  void    set( unsigned int index, const TilePos& pos );

  /*
   * Serialize section
   */
  VariantList save() const;
  void load(const VariantList& stream );
};

}//end namespace city
#endif //__CAESARIA_SAVED_POINTS_H_INCLUDED__
