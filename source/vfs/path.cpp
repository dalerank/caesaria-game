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

#include "core/exception.hpp"
#include "path.hpp"
#include "filesystem.hpp"
#include "entries.hpp"
#include "directory.hpp"
#include "core/utils.hpp"

#ifdef CAESARIA_PLATFORM_WIN
  #include <windows.h>
  #include <io.h>
#elif defined(CAESARIA_PLATFORM_UNIX) 
  #ifdef CAESARIA_PLATFORM_LINUX
    //#include <sys/io.h>
    #include <linux/limits.h>
  #elif defined(CAESARIA_PLATFORM_MACOSX)
    #include <libproc.h>
  #endif
  #include <sys/stat.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <libgen.h>
#elif defined(CAESARIA_PLATFORM_HAIKU)
  #include <sys/stat.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <libgen.h>
#endif

namespace vfs
{

const char* Path::anyFile = "*.*";
const char* Path::firstEntry = ".";
const char* Path::secondEntry = "..";

class Path::Impl
{
public:
  std::string path;

  void checkRcPrefix()
  {
    if( !path.empty() && path[ 0 ] == ':' )
    {
      Path tp( path.substr( 1 ) );
      path = ( FileSystem::instance().rcFolder()/tp ).toString();
    }
  }
};

void Path::splitToDirPathExt( Path* path,
                      Path* filename,
                      Path* extension )
{
  std::string name = _d->path;;
  int i = name.size();
  int extpos = i;

  // search for path separator or beginning
  while ( i >= 0 )
  {
      if ( name[i] == '.' )
      {
          extpos = i;
          if( extension )
          {
              *extension = Path( name.substr( extpos + 1, name.size() - (extpos + 1) ) );
          }
      }
      else
          if ( name[i] == '/' || name[i] == '\\' )
          {
              if ( filename )
              {
                  *filename = Path( name.substr( i + 1, extpos - (i + 1) ) );
              }

              if ( path )
              {
                std::string rp = utils::replace( name.substr( 0, i + 1 ), "\\", "/" ) ;   
                *path = Path( rp );
              }
              return;
          }
      i -= 1;
  }

  if ( filename )
  {
      *filename = Path( name.substr( 0, extpos ) );
  }
}

Path Path::addEndSlash() const
{
  std::string pathTo = _d->path;

  if( pathTo.size() == 0 )
      return Path( "" );

  if( (*pathTo.rbegin()) != '/' && (*pathTo.rbegin()) != '\\' )
  {
      pathTo.append( "/" );
  }

  return pathTo;
}

Path Path::removeBeginSlash() const
{
  std::string pathTo = _d->path;
 
  if( pathTo.empty() )
      return Path( "" ); 

  char endsym = *pathTo.begin();
  if( endsym == '/' || endsym == '\\' )
      pathTo.erase( 0, 1 );

  return pathTo;
}

Path Path::removeEndSlash() const
{
  std::string pathTo = _d->path;
    
  if( pathTo.empty() )
      return "";

  char lastChar = *pathTo.rbegin();
  if( lastChar == '/' || lastChar == '\\' )
  {
      pathTo.resize( pathTo.size() - 1 );
  }

  return pathTo;
}

char Path::lastChar() const { return _d->path.empty() ? 0 : *_d->path.rbegin(); }
char Path::firstChar() const { return _d->path.empty() ? 0 : *_d->path.begin(); }

bool Path::exist(SensType sens) const
{
  return FileSystem::instance().existFile( *this, sens );
}

bool Path::isFolder() const
{
#ifdef CAESARIA_PLATFORM_WIN
  WIN32_FILE_ATTRIBUTE_DATA fad;
  if( ::GetFileAttributesExA( _d->path.c_str(), ::GetFileExInfoStandard, &fad )== 0 )
      return false;

  return (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
  struct stat path_stat;
  if ( ::stat( toString().c_str(), &path_stat) != 0 )
      return false;

  return S_ISDIR(path_stat.st_mode);
#endif //CAESARIA_PLATFORM_UNIX
}

bool Path::isDirectoryEntry() const
{
  std::string bn = baseName().toString();
  return (bn == firstEntry || bn == secondEntry);
}

std::string Path::extension() const
{
  if( isFolder() )
  {
    return "";
  }

  std::string::size_type index = _d->path.find_last_of( '.' );
  if( index != std::string::npos )
  {
    return _d->path.substr( index, 0xff );
  }

  return "";
}

std::string Path::suffix() const
{
  std::string ret = extension();
  return ret.empty() ? "" : ret.substr(1);
}

Path::Path( const std::string& nPath ) : _d( new Impl )
{
  _d->path = utils::replace( nPath, "\\", "/" );
  _d->checkRcPrefix();
}

Path::Path( const Path& nPath ) : _d( new Impl )
{
  _d->path = nPath._d->path;
  _d->checkRcPrefix();
}

Path::Path( const char* nPath ) : _d( new Impl )
{
  _d->path = utils::replace( nPath, "\\", "/" );
  _d->checkRcPrefix();
}

Path::Path() : _d( new Impl )
{
  _d->path = "";
}

const std::string& Path::toString() const { return _d->path; }

std::string Path::removeExtension() const
{
  std::string::size_type index = _d->path.find_last_of( '.' );
  if( index != std::string::npos )
  {
    return _d->path.substr( 0, index );
  }

  return toString();
}

Path Path::changeExtension( const std::string& newExtension ) const
{
  std::string ext = newExtension;
  if( !ext.empty() )
  {
    ext = ( ext[0] == '.' ? ext : ("." + ext) );
  }
  return Path( this->removeExtension() + ext );
}

Path::~Path(){}

Path Path::absolutePath() const
{
#if defined(CAESARIA_PLATFORM_WIN)
  char *p=0;
  char fpath[_MAX_PATH];

  p = _fullpath(fpath, _d->path.c_str(), _MAX_PATH);
  std::string tmp = utils::replace( p, "\\", "/");
  return tmp;
#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
  char* p=0;
  char fpath[4096];
  fpath[0]=0;
  p = realpath( _d->path.c_str(), fpath);
  if (!p)
  {
    // content in fpath is unclear at this point
    if (!fpath[0]) // seems like fpath wasn't altered, use our best guess
    {
      Path tmp(_d->path);
      return flattenFilename(tmp);
    }
    else
      return Path(fpath);
  }

  if( *(_d->path.rbegin())=='/')
    return Path( std::string(p) + "/" );
  else
    return Path( std::string(p) );
#endif // CAESARIA_PLATFORM_UNIX
}


//! flatten a path and file name for example: "/you/me/../." becomes "/you"
Path Path::flattenFilename( const Path& root ) const
{
  std::string directory = addEndSlash().toString();
  directory = utils::replace( directory, "\\", "/" );
  
  Directory dir;
  Path subdir;

  int lastpos = 0;
  std::string::size_type pos = 0;
  bool lastWasRealDir=false;

  while( ( pos = directory.find( '/', lastpos) ) != std::string::npos )
  {
    subdir = Path( directory.substr(lastpos, pos - lastpos + 1) );

    if( subdir.toString() == "../" )
    {
      if (lastWasRealDir)
      {
        dir = dir.up().up();
        lastWasRealDir=( dir.toString().size()!=0);
      }
      else
      {
        dir = Path( dir.toString() + subdir.toString() );
        lastWasRealDir=false;
      }
    }
    else if( subdir.toString() == "/")
    {
      dir = root;
    }
    else if( subdir.toString() != "./" )
    {
      dir = Path( dir.toString() + subdir.toString() );
      lastWasRealDir=true;
    }

    lastpos = pos + 1;
  }

  return dir;
}

//! Get the relative filename, relative to the given directory
Path Path::getRelativePathTo( const Directory& directory ) const
{
  if ( toString().empty() || directory.toString().empty() )
    return *this;

  Path path1, file, ext;
  absolutePath().splitToDirPathExt( &path1, &file, &ext );
  Path path2(directory.absolutePath());
  StringArray list1, list2;

  list1 = utils::split( path1.toString(), "/\\");
  list2 = utils::split( path2.toString(), "/\\");

  unsigned int i=0;
  unsigned int it1=0;
  unsigned int it2=0;

#if defined (CAESARIA_PLATFORM_WIN)
  char partition1 = 0, partition2 = 0;
  Path prefix1, prefix2;
  if ( it1 > 0 )
    prefix1 = list1[ it1 ];
  if ( it2 > 0 )
    prefix2 = list2[ it2 ];

  if ( prefix1.toString().size() > 1 && prefix1.toString()[1] == ':' )
  {
    partition1 = prefix1.canonical().toString()[0];
  }

  if ( prefix2.toString().size() > 1 && prefix2.toString()[1] == ':' )
  {
    partition2 = prefix2.canonical().toString()[0];
  }

  // must have the same prefix or we can't resolve it to a relative filename
  if ( partition1 != partition2 )
  {
    return *this;
  }
#endif //CAESARIA_PLATFORM_WIN


  for (; i<list1.size() && i<list2.size() 
#if defined (CAESARIA_PLATFORM_WIN)
    && ( utils::isEquale( list1[ it1 ], list2[ it2 ], utils::equaleIgnoreCase ) )
#elif defined(CAESARIA_PLATFORM_UNIX)
    && ( list1[ it1 ]== list2[ it2 ] )	
#endif //CAESARIA_PLATFORM_UNIX
    ; ++i)
  {
    ++it1;
    ++it2;
  }

  path1="";
  for (; i<list2.size(); ++i)
  {
    path1 = path1.toString() + "../";
  }

  while( it1 != list1.size() )
  {
    path1 = path1.toString() + list1[ it1 ] + "/";
    it1++;
  }

  path1 = path1.toString() + file.toString();
  if( ext.toString().size() )
  {
    path1 = path1.toString() + "." + ext.toString();
  }
  return path1;
}

//! returns the base part of a filename, i.e. all except for the directory
//! part. If no directory path is prefixed, the full name is returned.
Path Path::baseName(bool keepExtension) const
{
  // find last forward or backslash
  std::string::size_type lastSlash = toString().find_last_of('/');

  // get number of chars after last dot
  std::string::size_type end = 0;
  if( !keepExtension )
  {
    // take care to search only after last slash to check only for
    // dots in the filename
    end = toString().find_last_of('.');
    if( end == std::string::npos || end == lastSlash)
      end=0;
    else
      end = toString().size()-end;
  }

  if( lastSlash != std::string::npos )
  {
    return Path( toString().substr(lastSlash+1, toString().size()-lastSlash-1-end) );
  }
  else if (end != 0)
  {
    return Path( toString().substr(0, toString().size()-end) );
  }
  else
  {
    return *this;
  }
}

//! returns the directory part of a filename, i.e. all until the first
//! slash or backslash, excluding it. If no directory path is prefixed, a '.'
//! is returned.
std::string Path::directory() const
{
  // find last forward or backslash
  std::string::size_type lastSlash = toString().find_last_of( '/' );

  if( lastSlash != std::string::npos )
  {
    return toString().substr(0, lastSlash);
  }
  else
    return ".";
}

bool Path::operator==( const Path& other ) const
{
  return toString() == other.toString();
}

bool Path::operator==( const std::string& other ) const
{
  return toString() == other;
}

char &Path::operator [](const unsigned int index)
{
  return _d->path[index];
}

bool Path::isMyExtension(const std::string &ext, bool checkCase) const
{
  return utils::isEquale( extension(), ext, checkCase ? utils::equaleCase : utils::equaleIgnoreCase );
}

Path& Path::operator=( const Path& other )
{
  _d->path = other._d->path;
  return *this;
}

Path &Path::operator +=(char c)
{
  _d->path += c;
  return *this;
}

Path Path::operator +(const Path& other)
{
  return vfs::Path( _d->path + other._d->path );
}

Path Path::canonical() const
{
  return utils::localeLower( _d->path );
}

} //end namespace vfs

