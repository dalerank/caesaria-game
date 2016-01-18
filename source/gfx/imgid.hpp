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

#ifndef __CAESARIA_IMGID_HELPER_H_INCLUDED__
#define __CAESARIA_IMGID_HELPER_H_INCLUDED__

#include "tile.hpp"

namespace gfx
{

class Tilemap;

namespace imgid
{  
  std::string toResource( const unsigned int imgId );
  int fromResource( const std::string &pic_name);
  Picture toPicture( const unsigned int imgId );
}

}//end namespace gfx

#endif //__CAESARIA_IMGID_HELPER_H_INCLUDED__
