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

class StringArray;

class OSystem
{
public:
  typedef enum { unknown, windows, win32, win64, unix, linux, macos, bsd, haiku, beos, android } Type;
  static void error( const std::string& title, const std::string& text );
  static void openUrl(const std::string& url , const std::string& prefix="");
  static void openDir(const std::string& path , const std::string &prefix="");
  static int  gmtOffsetMs();
  static void markFileAsExecutable( const std::string& filename );
  static bool is( Type type );
  static void restartProcess(const std::string &filename, const std::string& dir, const StringArray& cmds);

  static bool isAndroid();
  static bool isLinux();
};

#endif //__CAESARIA_OSYSTEM_INCLUDE_HPP__
