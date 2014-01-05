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

#ifndef __CAESARIA_PREDEFINITIONS_H_INCLUDED__
#define __CAESARIA_PREDEFINITIONS_H_INCLUDED__

#include "smartptr.hpp"
#include "position.hpp"

#include <list>
#include <vector>

#define PREDEFINE_CLASS_SMARTPOINTER(a) class a; typedef SmartPtr<a> a##Ptr;
#define PREDEFINE_CLASS_SMARTPOINTER_LIST(a,b) PREDEFINE_CLASS_SMARTPOINTER(a); typedef std::list< a##Ptr > a##b;

class TilePos;
class Size;
class VariantMap;
class DateTime;
typedef long unsigned int ThreadID;

typedef std::vector< TilePos > TilePosArray;
typedef std::vector< Point > PointsArray;

#endif //__CAESARIA_PREDEFINITIONS_H_INCLUDED__
