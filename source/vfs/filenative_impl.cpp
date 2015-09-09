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

#include "filenative_impl.hpp"
#include "core/logger.hpp"

#ifdef CAESARIA_PLATFORM_WIN
#define getline_def getline_fp
#include "windows.h"
#include <time.h>
#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
  #ifdef CAESARIA_PLATFORM_ANDROID
    #define getline_def getline_fp
  #else
    #define getline_def getline
    #include <sys/stat.h>
  #endif
#endif

namespace vfs
{

FileNative::FileNative(const Path& fileName, Entity::Mode mode)
: _file(0), _size(0), _name(fileName), _mode( mode )
{
  #ifdef _DEBUG
    setDebugName("NFile");
  #endif

  openFile();
}

FileNative::FileNative() : _file( 0 ), _size( 0 ), _mode( fmRead )
{
}

FileNative::~FileNative()
{
  if( _file )
  {
    fclose(_file);
  }
}

bool FileNative::isEof() const
{
  if (isOpen())
    return feof( _file ) != 0;

  return false;
}

//! returns how much was read
int FileNative::read(void* buffer, unsigned int sizeToRead)
{
  if (!isOpen())
    return 0;

  return (int)fread(buffer, 1, sizeToRead, _file);
}

//! changes position in file, returns true if successful
//! if relativeMovement==true, the pos is changed relative to current pos,
//! otherwise from begin of file
bool FileNative::seek(long finalPos, bool relativeMovement)
{
  if (!isOpen())
    return false;

  return fseek(_file, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

//! returns size of file
long FileNative::size() const{	return _size;}

//! returns where in the file we are.
long FileNative::getPos() const { return ftell(_file);}


#if defined(CAESARIA_PLATFORM_WIN) || defined(CAESARIA_PLATFORM_ANDROID)
size_t getline_fp(char **linebuf, size_t *linebufsz, FILE *file)
{
  int delimiter = '\n';
  //static const int GROWBY = 80; /* how large we will grow strings by */

  int ch;
  unsigned int idx = 0;

  if (file == NULL || linebuf==NULL || linebufsz == NULL || *linebuf == NULL || *linebufsz < 2)
  {
    return -1;
  }

  while (1)
  {
    ch = fgetc(file);
    if (ch == EOF)
      break;

    (*linebuf)[idx++] = (char)ch;
    if ((char)ch == delimiter || idx >= *linebufsz )
      break;
  }
	
  if( idx != 0 )
    (*linebuf)[idx] = 0;
  else if ( ch == EOF )
    return -1;

  return idx;
}
#endif //defined(CAESARIA_PLATFORM_WIN) || defined(CAESARIA_PLATFORM_ANDROID)

ByteArray FileNative::readLine()
{
  if (!isOpen())
  {
    return ByteArray();
  }

  size_t defaultBytesRead = 100;
  ByteArray ret;
  ret.resize( defaultBytesRead );

  bool endLineReaded = false;
  unsigned int readOneLineCounter = 0;
  while( !endLineReaded )
  {
    readOneLineCounter++;
    char* currentPos = ret.data() + ret.size() - defaultBytesRead;

    int reallyReadingBytes = getline_def( &currentPos, &defaultBytesRead, _file);

    if( reallyReadingBytes <= 0 )
      return ByteArray();

    if( readOneLineCounter > 1000 )
    {
      Logger::warning( "Too many iteration for read one line" );
      return ByteArray();
    }

    if( reallyReadingBytes != (int)defaultBytesRead )
      ret.resize( ret.size() - defaultBytesRead + reallyReadingBytes );

    if( ret.back() == '\n' )
    {
      ret.push_back( '\0' );
      return ret;
    }
  }

  return ret;
}

ByteArray FileNative::read(unsigned int sizeToRead)
{
  if (!isOpen())
  {
    return ByteArray();
  }

  ByteArray ret;
  ret.resize( sizeToRead );

  int reallyReadingBytes = fread( ret.data(), 1, sizeToRead, _file);
  ret.resize( reallyReadingBytes );

  return ret;
}

//! opens the file
void FileNative::openFile()
{
  if( _name.toString().empty() ) // bugfix posted by rt
  {
    _file = 0;
    return;
  }

  const char* modeStr[] = { "rb", "wb", "ab" };
  if( (unsigned int)_mode > Entity::fmAppend )
  {
    Logger::warning( "Unsupported file open mode for %s", _name.toCString() );
    _mode = Entity::fmRead;
  }

  _file = fopen( _name.toString().c_str(), modeStr[ _mode ] );

  if( _file )
  {
   // get FileSize
   fseek(_file, 0, SEEK_END);
   _size = getPos();
   fseek(_file, 0, SEEK_SET);
  }
  else
  {
    Logger::warning( "FileNative: Can't open file %s", _name.toCString() );
  }
}

//! returns name of file
const Path& FileNative::path() const {	return _name;}

//! returns how much was read
int FileNative::write(const void* buffer, unsigned int sizeToWrite)
{
  if( !isOpen() )
   return 0;

  return (int)fwrite(buffer, 1, sizeToWrite, _file);
}

int FileNative::write( const ByteArray& bArray )
{
  if( !isOpen() )
      return 0;

  return (int)fwrite( bArray.data(), 1, bArray.size(), _file );
}

bool FileNative::isOpen() const { return _file != 0;}

size_t FileNative::lastModify() const
{
#ifdef CAESARIA_PLATFORM_UNIX
  struct stat attr;
  stat(_name.toCString(), &attr);
  return attr.st_mtime;
#else
  FILETIME creationTime,
           lpLastAccessTime,
           lastWriteTime;
  HANDLE  hFile = CreateFile(_name.toCString(), GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, 0, NULL);
  bool err = GetFileTime( hFile, &creationTime, &lpLastAccessTime, &lastWriteTime );
  time_t ret = 0;
  if( !err )
  {
    SYSTEMTIME sysTime;
    FileTimeToSystemTime( &creationTime, &sysTime );

    struct tm tmtime = {0};

    tmtime.tm_year = sysTime.wYear - 1900;
    tmtime.tm_mon = sysTime.wMonth - 1;
    tmtime.tm_mday = sysTime.wDay;
    tmtime.tm_hour = sysTime.wHour;
    tmtime.tm_min = sysTime.wMinute;
    tmtime.tm_sec = sysTime.wSecond;
    tmtime.tm_wday = 0;
    tmtime.tm_yday = 0;
    tmtime.tm_isdst = -1;
    ret = mktime(&tmtime);
  }

  CloseHandle( hFile );
  return ret;
#endif
}

void FileNative::flush()
{
  if( !isOpen() )
    return;

  fflush( _file );
}

} //end namespace io
