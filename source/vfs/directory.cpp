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
// alo  ng with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.

#include "core/exception.hpp"
#include "directory.hpp"
#include "filesystem.hpp"
#include "entries.hpp"
#include "core/logger.hpp"

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

namespace vfs
{

bool Directory::create( std::string dir )
{
  Directory rdir( dir );
  if( rdir.isExist() )
  {
    Logger::warning( "Directory %s also exist", dir.c_str() );
    return false;
  }

  int result=0;
#ifdef CAESARIA_PLATFORM_WIN
    CreateDirectoryA( rdir.removeEndSlash().toString().c_str(), NULL );
#elif defined(CAESARIA_PLATFORM_UNIX)
  result = ::mkdir( rdir.toString().c_str(), S_IRWXU|S_IRWXG|S_IRWXO );
#endif

  if( result < 0 )
  {
    Logger::warning( "Cannot create directory %s error=%d", dir.c_str(), result );
  }
  return (result == 0);
}

bool Directory::createByPath( Directory dir )
{
  Path saveDir = getCurrent();
  bool result=true;

  StringArray path = StringHelper::split( dir.toString(), "/" );
  std::string current;
  try
  {
#ifdef CAESARIA_PLATFORM_LINUX
    changeCurrentDir( "/" );
#endif

    for( StringArray::iterator iter=path.begin(); iter != path.end(); iter++)
    {
      current += *iter;
      Path path = current;
      if( path.isExist() )
      {
        if( !path.isFolder() )
        {
          Logger::warning( "Current path %s not a directory " + current );
          result = false;
          break;
        }
      }
      else
      {
        if( !create( current ) )
        {
          Logger::warning( "Some error on create directory " + current );
        }
      }
      current += "/";
    }
  }
  catch(...)
  {

  }

  changeCurrentDir( saveDir );

  return result;
}

Path Directory::find( const Path& fileName ) const
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

  Path finalPath( addEndSlash().toString() + fileName.toString() );
  if( finalPath.isExist() )
  {
    return finalPath;
  }

  return fileName;
}

Entries Directory::getEntries() const
{
  FileSystem& fs = FileSystem::instance();
  Directory saveDir( fs.getWorkingDirectory() );
  Directory changeDd = *this;
  fs.changeWorkingDirectoryTo( changeDd );
    
  Entries fList( changeDd.toString(), false, false );
  fList = fs.getFileList();

  fs.changeWorkingDirectoryTo( saveDir );
  return fList;
}

Directory::Directory( const Path& pathTo ) : Path( pathTo )
{
}

Directory::Directory( const std::string& nPath ) : Path( nPath )
{

}

Directory::Directory( const Directory& nPath ) : Path( nPath.toString()  )
{

}

Path Directory::getFilePath( const Path& fileName )
{
  std::string ret = addEndSlash().toString();
  ret.append( fileName.removeBeginSlash().toString() );
  return Path( ret );
}

Directory Directory::operator/(const Directory& dir)
{
  std::string dr = addEndSlash().toString();
  return Directory( dr + dir.toString() );
}

Path Directory::operator/(const Path& filename)
{
  std::string dr = addEndSlash().toString();
  return Path( dr + filename.toString() );
}


bool Directory::changeCurrentDir( const Path& dirName )
{
  return FileSystem::instance().changeWorkingDirectoryTo( dirName );
}

Directory Directory::getCurrent()
{
  return FileSystem::instance().getWorkingDirectory();
}

Directory Directory::getApplicationDir()
{
#ifdef CAESARIA_PLATFORM_WIN
  unsigned int pathSize=512;
  ByteArray tmpPath;
  tmpPath.resize( pathSize );
  GetModuleFileName( 0, tmpPath.data(), pathSize);
  Directory tmp( std::string( tmpPath.data() ) );
  tmp = tmp.up();
  return tmp;
#elif defined(CAESARIA_PLATFORM_LINUX)
  char exe_path[PATH_MAX] = {0};
  char * dir_path;
  sprintf(exe_path, "/proc/%d/exe", getpid());
  readlink(exe_path, exe_path, sizeof(exe_path));
  dir_path = dirname(exe_path);
  return Path(dir_path);
#elif defined(CAESARIA_PLATFORM_MACOSX)
  char exe_path[PROC_PIDPATHINFO_MAXSIZE];
  int ret = proc_pidpath(getpid(), exe_path, sizeof(exe_path));
  if (ret <= 0)
  {
    THROW("Cannot get application executable file path");
  }
  return Path(dirname(exe_path));
#endif

  return Path( "." );
}

Directory::Directory()
{
}

Directory Directory::up() const
{
  if( toString().empty() )
    return Directory();

  Path pathToAny = removeEndSlash();
  std::string::size_type index = pathToAny.toString().find_last_of( "/" );

  if( index != std::string::npos )
  {
    return Path( pathToAny.toString().substr( 0, index ) );
  }

  _CAESARIA_DEBUG_BREAK_IF( !isExist() );
  return Directory();
}

Path Directory::getRelativePathTo(Path path) const
{
  if ( toString().empty() || path.toString().empty() )
    return *this;

  Path path1 = getAbsolutePath();
  Path path2( Directory( path.getDir() ).getAbsolutePath() );
  StringArray list1, list2;

  list1 = StringHelper::split( path1.toString(), "/\\");
  list2 = StringHelper::split( path2.toString(), "/\\");

  unsigned int i=0;
#if defined (CAESARIA_PLATFORM_WIN)
  char partition1 = 0, partition2 = 0;
  Path prefix1, prefix2;
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


  for (; i<list1.size() && i<list2.size(); ++i)
  {
    StringHelper::equaleMode emode = StringHelper::equaleIgnoreCase;
#ifdef CAESARIA_PLATFORM_UNIX
    emode = StringHelper::equaleCase;
#endif //CAESARIA_PLATFORM_UNIX

    if( !StringHelper::isEquale( list1[ i ], list2[ i ], emode ) )
    {
      break;
    }
  }

  path1="";
  for( unsigned int k=i; k<list1.size(); ++k)
  {
    path1 = path1.toString() + "../";
  }

  for( ; i < list2.size(); i++ )
  {
    path1 = path1.toString() + list2[ i ] + "/";
  }

  return path1.toString() + path.getBasename().toString();
}

} //end namespace io

