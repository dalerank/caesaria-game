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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_OSYSTEM_INCLUDE_HPP__
#define __CAESARIA_OSYSTEM_INCLUDE_HPP__

#include <string>

class OSystem
{
public:
  static void error( const std::string& title, const std::string& text );
  static void openUrl( const std::string& url );
  static void openDir( const std::string& path );
  static int  gmtOffsetMs();
};

#endif //__CAESARIA_OSYSTEM_INCLUDE_HPP__
