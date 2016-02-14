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

#include "fileinfo.hpp"
#include "core/platform_specific.hpp"

#if defined(GAME_PLATFORM_UNIX) || defined(GAME_PLATFORM_HAIKU)
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <limits.h>
  #include <sys/types.h>
  #include <dirent.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

namespace vfs
{

Info::Info(Path path) : _path( path )
{

}

Info::Info(const Info& other)
{
  _path = other._path;
}

DateTime Info::modified() const
{
#ifndef GAME_PLATFORM_WIN
  struct tm *foo;
  struct stat attrib;
  stat( _path.toCString(), &attrib);
  foo = gmtime((const time_t*)&(attrib.st_mtime));

  return DateTime( foo->tm_year, foo->tm_mon+1, foo->tm_mday+1,
                   foo->tm_hour, foo->tm_min, foo->tm_sec );
#else
  FILETIME creationTime,
           lpLastAccessTime,
           lastWriteTime;
  HANDLE h = CreateFile( _path.toCString(),
                         GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
  GetFileTime( h, &creationTime, &lpLastAccessTime, &lastWriteTime );
  SYSTEMTIME systemTime;
  FileTimeToSystemTime( &lastWriteTime, &systemTime );
  CloseHandle(h);
  return DateTime( systemTime.wYear, systemTime.wMonth, systemTime.wDay,
                   systemTime.wHour, systemTime.wMinute, systemTime.wSecond );
#endif
}

DateTime Info::created() const
{
#ifndef GAME_PLATFORM_WIN
  struct tm *foo;
  struct stat attrib;
  stat( _path.toCString(), &attrib);
  foo = gmtime((const time_t*)&(attrib.st_ctime));

  return DateTime( foo->tm_year, foo->tm_mon+1, foo->tm_mday+1,
                   foo->tm_hour, foo->tm_min, foo->tm_sec );
#else
  FILETIME creationTime,
           lpLastAccessTime,
           lastWriteTime;
  HANDLE h = CreateFile( _path.toCString(),
                         GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, 0, NULL);
  GetFileTime( h, &creationTime, &lpLastAccessTime, &lastWriteTime );
  SYSTEMTIME systemTime;
  FileTimeToSystemTime( &creationTime, &systemTime );
  CloseHandle(h);
  return DateTime( systemTime.wYear, systemTime.wMonth, systemTime.wDay,
                   systemTime.wHour, systemTime.wMinute, systemTime.wSecond );
#endif
}


} //end namespace vfs

