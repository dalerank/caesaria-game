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

#ifndef __CAESARIA_OBJECTS_CONFIG_H_INCLUDED__
#define __CAESARIA_OBJECTS_CONFIG_H_INCLUDED__

namespace config
{

namespace fgpic
{
enum { idxMainPic = 0 };
}

namespace servicebld
{
enum { defaultWorkers=5, defaultRange=30 };
}

namespace educationbld
{
enum { maxSchoolVisitors=75, maxAcademyVisitors=100, maxLibraryVisitors=800 };
}

}//end namespace city

#endif //__CAESARIA_OBJECTS_CONFIG_H_INCLUDED__
