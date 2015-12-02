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

#ifndef _GAME_PLATFORM_SPECIFIC_H_INCLUDE_
#define _GAME_PLATFORM_SPECIFIC_H_INCLUDE_

#include "platform.hpp"

#ifdef GAME_PLATFORM_WIN
  #include "windows.h"
  #include <io.h> // for _access
  #include <direct.h> // for _chdir
  #include <time.h>
  #include <tchar.h>
  #include <stdio.h>
#endif //GAME_PLATFORM_WIN

#ifdef GAME_PLATFORM_UNIX
  #include <sys/time.h>
#endif //GAME_PLATFORM_UNIX

#ifdef GAME_PLATFORM_LINUX
  #include <linux/limits.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <libgen.h>
  #include <stdlib.h>
  #include <string.h>
  #include <limits.h>
  #include <sys/types.h>
  #include <dirent.h>
#endif //GAME_PLATFORM_LINUX

#ifdef GAME_PLATFORM_MACOSX
  #include <libproc.h>
#endif //GAME_PLATFORM_MACOSX

#ifdef GAME_PLATFORM_HAIKU
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <limits.h>
  #include <sys/types.h>
  #include <dirent.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

#endif//_GAME_PLATFORM_SPECIFIC_H_INCLUDE_
