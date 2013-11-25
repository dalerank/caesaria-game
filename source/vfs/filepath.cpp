
// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "core/exception.hpp"
#include "filepath.hpp"
#include "filesystem.hpp"
#include "filelist.hpp"

#ifdef CAESARIA_PLATFORM_WIN
  #include <windows.h>
  #include <io.h>
#elif defined(CAESARIA_PLATFORM_UNIX)
  #ifdef CAESARIA_PLATFORM_LINUX
    #include <sys/io.h>
    #include <linux/limits.h>
  #elif defined(CAESARIA_PLATFORM_MACOSX)
    #include <libproc.h>
  #endif
  #include <sys/stat.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <libgen.h>
#endif

namespace io
{

const char* FilePath::anyFile = "*.*";
const char* FilePath::firstEntry = ".";
const char* FilePath::secondEntry = "..";

class FilePath::Impl
{
public:
  std::string path;
};

void FilePath::splitToDirPathExt( FilePath* path,
                      FilePath* filename,
                      FilePath* extension )
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
              *extension = FilePath( name.substr( extpos + 1, name.size() - (extpos + 1) ) );
          }
      }
      else
          if ( name[i] == '/' || name[i] == '\\' )
          {
              if ( filename )
              {
                  *filename = FilePath( name.substr( i + 1, extpos - (i + 1) ) );
              }

              if ( path )
              {
                std::string rp = StringHelper::replace( name.substr( 0, i + 1 ), "\\", "/" ) ;   
                *path = FilePath( rp );
              }
              return;
          }
      i -= 1;
  }

  if ( filename )
  {
      *filename = FilePath( name.substr( 0, extpos ) );
  }
}


void FilePath::remove()
{
#ifdef CAESARIA_PLATFORM_WIN
    DeleteFile( _d->path.c_str() );
#elif defined(CAESARIA_PLATFORM_UNIX)
    ::remove( _d->path.c_str() );
#endif
}

FilePath FilePath::addEndSlash() const
{
  std::string pathTo = _d->path;

  if( pathTo.size() == 0 )
      return FilePath( "" );

  if( (*pathTo.rbegin()) != '/' && (*pathTo.rbegin()) != '\\' )
  {
      pathTo.append( "/" );
  }

  return pathTo;
}

FilePath FilePath::removeBeginSlash() const
{
  std::string pathTo = _d->path;
 
  if( pathTo.empty() )
      return FilePath( "" ); 

  wchar_t endsym = *pathTo.begin();
  if( endsym == '/' || endsym == '\\' )
      pathTo.erase( 0 );

  return pathTo;
}

FilePath FilePath::removeEndSlash() const
{
  std::string pathTo = _d->path;
    
  if( pathTo.empty() )
      return "";

  if( (*pathTo.rbegin()) == '/' || (*pathTo.rbegin()) == '\\' )
  {
      pathTo.erase( pathTo.rbegin().base() );
  }

  return pathTo;
}

void FileDir::_OsCreate( const FileDir& dirName )
{
#ifdef CAESARIA_PLATFORM_WIN
    CreateDirectory( removeEndSlash().toString().c_str(), NULL );
#elif defined(CAESARIA_PLATFORM_UNIX)
    ::mkdir( dirName.toString().c_str(), S_IRWXU|S_IRWXG|S_IRWXO );
#endif
}

void FileDir::create()
{
    if( isExist() )
        return;

    _OsCreate( *this );
}

bool FilePath::isExist() const
{
    return FileSystem::instance().existFile( *this );
}

bool FilePath::isFolder() const
{
#ifdef CAESARIA_PLATFORM_WIN
  WIN32_FILE_ATTRIBUTE_DATA fad;
  if( ::GetFileAttributesEx( _d->path.c_str(), ::GetFileExInfoStandard, &fad )== 0 )
      return false;

  return (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
#elif defined(CAESARIA_PLATFORM_UNIX)
  struct stat path_stat;
  if ( ::stat( toString().c_str(), &path_stat) != 0 )
      return false;

  return S_ISDIR(path_stat.st_mode);
#endif //CAESARIA_PLATFORM_UNIX
}

std::string FilePath::getExtension() const 
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

FilePath FilePath::getUpDir() const
{
    if( !_d->path.size() )
        return "";

    FilePath pathToAny = removeEndSlash();
    std::string::size_type index = pathToAny._d->path.find_last_of( "/" );

    if( index != std::string::npos )
    {
        return FilePath( pathToAny._d->path.substr( 0, index ) );
    }

    _CAESARIA_DEBUG_BREAK_IF( !isExist() );
    return "";
}

FilePath FileDir::find( const FilePath& fileName ) const
{
    _CAESARIA_DEBUG_BREAK_IF( !isExist() );
    if( !fileName.toString().size() )
    {
        return "";
    }

    if( fileName.isExist() )
    {
        return fileName;
    }

    FilePath finalPath( addEndSlash().toString() + fileName.toString() );
    if( finalPath.isExist() )
    {
        return finalPath;
    }

    return fileName;
}

void FilePath::_OsRename( const FilePath& newName )
{
  ::rename( toString().c_str(), newName.toString().c_str() );
}

void FilePath::rename( const FilePath& pathNew )
{
  _CAESARIA_DEBUG_BREAK_IF( !isExist() );
  *this = pathNew;
}

FilePath::FilePath( const std::string& nPath ) : _d( new Impl )
{
  _d->path = StringHelper::replace( nPath, "\\", "/" );
}

FilePath::FilePath( const FilePath& nPath ) : _d( new Impl )
{
  _d->path = nPath._d->path;
}

FilePath::FilePath( const char* nPath ) : _d( new Impl )
{
  _d->path = StringHelper::replace( nPath, "\\", "/" );
}

FilePath::FilePath() : _d( new Impl )
{
  _d->path = "";
}

const std::string& FilePath::toString() const
{
    return _d->path;
}

std::string FilePath::removeExtension() const
{
    std::string::size_type index = _d->path.find_last_of( '.' );
    if( index != std::string::npos )
    {
        return _d->path.substr( 0, index );
    }

    return toString();
}

FilePath::~FilePath()
{

}

FilePath FilePath::getAbsolutePath() const
{
#if defined(CAESARIA_PLATFORM_WIN)
  char *p=0;
  char fpath[_MAX_PATH];

  p = _fullpath(fpath, _d->path.c_str(), _MAX_PATH);
  std::string tmp = StringHelper::replace( p, "\\", "/");
  return tmp;
#elif defined(CAESARIA_PLATFORM_UNIX)
  char* p=0;
  char fpath[4096];
  fpath[0]=0;
  p = realpath( _d->path.c_str(), fpath);
  if (!p)
  {
    // content in fpath is unclear at this point
    if (!fpath[0]) // seems like fpath wasn't altered, use our best guess
    {
      FilePath tmp(_d->path);
      return flattenFilename(tmp);
    }
    else
      return FilePath(fpath);
  }

  if( *(_d->path.rbegin())=='/')
    return FilePath( std::string(p) + "/" );
  else
    return FilePath( std::string(p) );
#endif // CAESARIA_PLATFORM_UNIX
}


//! flatten a path and file name for example: "/you/me/../." becomes "/you"
FilePath FilePath::flattenFilename( const FilePath& root ) const
{
  std::string directory = addEndSlash().toString();
  directory = StringHelper::replace( directory, "\\", "/" );
  
  FilePath dir;
  FilePath subdir;

  int lastpos = 0;
  std::string::size_type pos = 0;
  bool lastWasRealDir=false;

  while( ( pos = directory.find( '/', lastpos) ) != std::string::npos )
  {
    subdir = FilePath( directory.substr(lastpos, pos - lastpos + 1) );

    if( subdir.toString() == "../" )
    {
      if (lastWasRealDir)
      {
        dir = dir.getUpDir();
        dir = dir.getUpDir();
        lastWasRealDir=( dir.toString().size()!=0);
      }
      else
      {
        dir = FilePath( dir.toString() + subdir.toString() );
        lastWasRealDir=false;
      }
    }
    else if( subdir.toString() == "/")
    {
      dir = root;
    }
    else if( subdir.toString() != "./" )
    {
      dir = FilePath( dir.toString() + subdir.toString() );
      lastWasRealDir=true;
    }

    lastpos = pos + 1;
  }

  return dir;
}

//! Get the relative filename, relative to the given directory
FilePath FilePath::getRelativePathTo( const FilePath& directory ) const
{
  if ( toString().empty() || directory.toString().empty() )
    return *this;

  FilePath path1, file, ext;
  getAbsolutePath().splitToDirPathExt( &path1, &file, &ext );
  FilePath path2(directory.getAbsolutePath());
  StringArray list1, list2;

  list1 = StringHelper::split( path1.toString(), "/\\", 2);
  list2 = StringHelper::split( path2.toString(), "/\\", 2);

  unsigned int i=0;
  unsigned int it1=0;
  unsigned int it2=0;

#if defined (CAESARIA_PLATFORM_WIN)
  char partition1 = 0, partition2 = 0;
  FilePath prefix1, prefix2;
  if ( it1 > 0 )
    prefix1 = list1[ it1 ];
  if ( it2 > 0 )
    prefix2 = list2[ it2 ];

  if ( prefix1.toString().size() > 1 && prefix1.toString()[1] == ':' )
  {
    partition1 = StringHelper::localeLower( prefix1.toString()[0] );
  }

  if ( prefix2.toString().size() > 1 && prefix2.toString()[1] == ':' )
  {
    partition2 = StringHelper::localeLower( prefix2.toString()[0] );
  }

  // must have the same prefix or we can't resolve it to a relative filename
  if ( partition1 != partition2 )
  {
    return *this;
  }
#endif //CAESARIA_PLATFORM_WIN


  for (; i<list1.size() && i<list2.size() 
#if defined (CAESARIA_PLATFORM_WIN)
    && ( StringHelper::isEquale( list1[ it1 ], list2[ it2 ], StringHelper::equaleIgnoreCase ) )
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
FilePath FilePath::getBasename(bool keepExtension) const
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
    if( end == std::string::npos || end < lastSlash)
      end=0;
    else
      end = toString().size()-end;
  }

  if( lastSlash != std::string::npos )
  {
    return FilePath( toString().substr(lastSlash+1, toString().size()-lastSlash-1-end) );
  }
  else if (end != 0)
  {
    return FilePath( toString().substr(0, toString().size()-end) );
  }
  else
  {
    return *this;
  }
}

//! returns the directory part of a filename, i.e. all until the first
//! slash or backslash, excluding it. If no directory path is prefixed, a '.'
//! is returned.
FilePath FilePath::getFileDir() const
{
  // find last forward or backslash
  std::string::size_type lastSlash = toString().find_last_of( '/' );

  if( lastSlash != std::string::npos )
  {
    return FilePath( toString().substr(0, lastSlash) );
  }
  else
    return ".";
}

bool FilePath::operator==( const FilePath& other ) const
{
  return toString() == other.toString();
}

bool FilePath::operator==( const std::string& other ) const
{
    return toString() == other;
}

char &FilePath::operator [](const unsigned int index)
{
    return _d->path[index];
}

bool FilePath::isExtension(const std::string &ext, bool checkCase) const
{
    return StringHelper::isEquale( getExtension(), ext, checkCase ? StringHelper::equaleCase : StringHelper::equaleIgnoreCase );
}

FilePath& FilePath::operator=( const FilePath& other )
{
  _d->path = other._d->path;
  return *this;
}

FilePath &FilePath::operator +=(char c)
{
  _d->path += c;
  return *this;
}

FileList FileDir::getEntries() const
{
  FileSystem& fs = FileSystem::instance();
  FileDir saveDir( fs.getWorkingDirectory() );
  FileDir changeDd = *this;
  fs.changeWorkingDirectoryTo( changeDd );
    
  FileList fList( changeDd.toString(), false, false );
  fList = fs.getFileList();

  fs.changeWorkingDirectoryTo( saveDir );
  return fList;
}

FileDir::FileDir( const FilePath& pathTo ) : FilePath( pathTo )
{
}

FileDir::FileDir( const std::string& nPath ) : FilePath( nPath )
{

}

FileDir::FileDir( const FileDir& nPath ) : FilePath( nPath.toString()  )
{

}

FilePath FileDir::getFilePath( const FilePath& fileName )
{
  std::string ret = addEndSlash().toString();
  ret.append( fileName.removeBeginSlash().toString() );
  return FilePath( ret );
}


bool FileDir::changeCurrentDir( const FilePath& dirName )
{
  return FileSystem::instance().changeWorkingDirectoryTo( dirName );
}

FileDir FileDir::getCurrentDir()
{
  return FileSystem::instance().getWorkingDirectory();
}

FileDir FileDir::getApplicationDir()
{
#ifdef WIN32
  unsigned int pathSize=512;
  ByteArray tmpPath;
  tmpPath.resize( pathSize );
  GetModuleFileName( 0, tmpPath.data(), pathSize);
  FilePath tmp = tmpPath.data();

  tmp = tmp.getUpDir();
  //delete tmpPath;

  return tmp;
#elif defined(__linux__)
  char exe_path[PATH_MAX] = {0};
  char * dir_path;
  sprintf(exe_path, "/proc/%d/exe", getpid());
  readlink(exe_path, exe_path, sizeof(exe_path));
  dir_path = dirname(exe_path);
  return FilePath(dir_path);
#elif defined(__APPLE__)
  char exe_path[PROC_PIDPATHINFO_MAXSIZE];
  int ret = proc_pidpath(getpid(), exe_path, sizeof(exe_path));
  if (ret <= 0)
  {
    THROW("Cannot get application executable file path");
  }
  return FilePath(dirname(exe_path));
#endif

  return FilePath( "." );
}

} //end namespace io

