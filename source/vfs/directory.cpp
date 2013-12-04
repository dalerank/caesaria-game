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
    #include <pwd.h>
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

Entries Directory::getEntries(SensType sens) const
{
  FileSystem& fs = FileSystem::instance();
  Directory saveDir( fs.getWorkingDirectory() );
  Directory changeDd = *this;
  fs.changeWorkingDirectoryTo( changeDd );
    
  Entries fList( changeDd.toString(), sens, false );
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
  GetModuleFileNameA( 0, tmpPath.data(), pathSize);
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

Directory Directory::getUserDir()
{
  std::string mHomePath;
#ifdef CAESARIA_PLATFORM_MACOSX
  struct passwd* pwd = getpwuid(getuid());
  if (pwd)
  {
    mHomePath = pwd->pw_dir;
  }
  else
  {
    // try the $HOME environment variable
    mHomePath = getenv("HOME");
  }

  if( mHomePath.empty() )
  {
    // couldn't create dir in home directory, fall back to cwd
    mHomePath = "./";
    Logger::warning( "Cannot find home user directory" );
  }
#elif defined(CAESARIA_PLATFORM_LINUX)
  struct passwd* pwd = getpwuid(getuid());
  if (pwd)
  {
    mHomePath = pwd->pw_dir;
  }
  else
  {
    // try the $HOME environment variable
    mHomePath = getenv("HOME");
  }

  if( mHomePath.empty() )
  {
    // couldn't create dir in home directory, fall back to cwd
    mHomePath = "./";
    Logger::warning( "Cannot find home user directory" );
  }
#elif defined(CAESARIA_PLATFORM_WIN)
  TCHAR path[MAX_PATH];
  if( SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, path)) )
  {
     // need to convert to OEM codepage so that fstream can use
     // it properly on international systems.
     TCHAR oemPath[MAX_PATH];
     CharToOem(path, oemPath);
     mHomePath = oemPath;
     // create Ogre subdir
     mHomePath += "\\Ogre\\";
  }

  if (mHomePath.empty())
  {
     // couldn't create dir in home directory, fall back to cwd
     mHomePath = "";
  }
#endif

  return vfs::Directory( mHomePath );
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

