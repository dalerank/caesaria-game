#include "directory.hpp"
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
#include "directory.hpp"
#include "filesystem.hpp"
#include "entries.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "core/osystem.hpp"

#ifdef GAME_PLATFORM_WIN
  #include <windows.h>
  #include <io.h>
  #include <shlobj.h>
#elif defined(GAME_PLATFORM_UNIX)
  #if defined(GAME_PLATFORM_LINUX) || defined(GAME_PLATFORM_HAIKU)
    //#include <sys/io.h>
    #include <linux/limits.h>
    #include <pwd.h>
  #elif defined(GAME_PLATFORM_MACOSX)
    #include <libproc.h>
    #include <pwd.h>    
  #endif
  #include <sys/stat.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <libgen.h>
#endif

namespace vfs
{

Directory::~Directory() {}
  
bool Directory::create( std::string dir )
{
  Directory rdir( dir );
  if( rdir.exist() )
  {
    Logger::warning( "Directory {0} also exist", dir );
    return false;
  }

  int result=0;
#ifdef GAME_PLATFORM_WIN
  CreateDirectoryA( rdir.removeEndSlash().toCString(), NULL );
#elif defined(GAME_PLATFORM_UNIX)
  result = ::mkdir( rdir.toCString(), S_IRWXU|S_IRWXG|S_IRWXO );
#endif

  if( result < 0 )
  {
    Logger::warning( "Cannot create directory {0} error={1}", dir, result );
  }
  return (result == 0);
}

bool Directory::createByPath( Directory dir )
{
  Path saveDir = current();
  bool result=true;

  StringArray path = utils::split( dir.toString(), "/" );
  std::string current;
  try
  {
#if defined(GAME_PLATFORM_UNIX) || defined(GAME_PLATFORM_HAIKU)
    if( dir.toString().front() == '/' )
      switchTo( "/" );
#endif

    foreach( iter, path )
    {
      current += *iter;
      Path path = current;
      if( path.exist() )
      {
        if( !path.isFolder() )
        {
          Logger::warning( "Current path {} not a directory ", current );
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

  switchTo( saveDir );

  return result;
}

Path Directory::find(const Path& fileName, SensType sens) const
{
  if( fileName.toString().empty() )
  {
    Logger::warning( "!!! Directory: cannot try find zero lenght name" );
    return "";
  }

  Entries files = entries();
  files.setSensType( sens );
  int index = files.findFile( fileName.baseName() );
  if( index >= 0 )
  {
    return files.item( index ).fullpath;
  }

  return "";
}

Entries Directory::entries() const
{
  FileSystem& fs = FileSystem::instance();
  Directory saveDir( fs.workingDirectory() );
  Directory changeDd = *this;
  fs.changeWorkingDirectoryTo( changeDd );
    
  Entries fList( changeDd.toString(), Path::nativeCase, false );
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

vfs::Directory::Directory(const char * nPath)
  : Path( std::string(nPath) )
{
}

Path Directory::getFilePath( const Path& fileName )
{
  std::string ret = addEndSlash().toString();
  ret.append( fileName.removeBeginSlash().toString() );
  return Path( ret );
}

std::string _concat( const Path& p1, const Path& p2 )
{
  std::string p1str = p1.addEndSlash().toString();
  std::string p2str = p2.removeBeginSlash().toString();
  return p1str + p2str;
}

Directory Directory::operator/(const Directory& dir) const
{
  return Directory( _concat( *this, dir ) );
}

Path Directory::operator/(const Path& filename) const
{
  return Path( _concat( *this, filename ) );
}

Path Directory::operator/(const std::string& filename) const
{
  return Path( _concat( *this, filename ) );
}

Path Directory::operator/(const char* filename) const
{
  return Path( _concat( *this, filename ) );
}

bool Directory::switchTo( const Path& dirName ){  return FileSystem::instance().changeWorkingDirectoryTo( dirName );}
Directory Directory::current(){  return FileSystem::instance().workingDirectory();}

Directory Directory::applicationDir()
{
#ifdef GAME_PLATFORM_WIN
  unsigned int pathSize=512;
  ByteArray tmpPath;
  tmpPath.resize( pathSize );
  GetModuleFileNameA( 0, tmpPath.data(), pathSize);
  Directory tmp( std::string( tmpPath.data() ) );
  tmp = tmp.up();
  return tmp;
#elif defined(GAME_PLATFORM_LINUX)
  char exe_path[PATH_MAX] = {0};
  sprintf(exe_path, "/proc/%d/exe", ::getpid());
  readlink(exe_path, exe_path, sizeof(exe_path));
  vfs::Directory wdir = vfs::Path( exe_path ).directory();
  //dir_path = dirname(exe_path);
  return wdir;
/*#elif defined(GAME_PLATFORM_HAIKU)
  char exe_path[PATH_MAX] = {0};
  sprintf(exe_path, "/proc/%d/exe", getpid());
  readlink(exe_path, exe_path, sizeof(exe_path));
  dirname(exe_path);
  return Path( exe_path );*/
#elif defined(GAME_PLATFORM_MACOSX)
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

Directory Directory::userDir()
{
  std::string mHomePath;
#ifdef GAME_PLATFORM_MACOSX
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
    Logger::error( "Cannot find home user directory" );
  }
#elif defined(GAME_PLATFORM_LINUX)
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
    Logger::error( "Cannot find home user directory" );    
  }
#elif defined(GAME_PLATFORM_HAIKU)
   mHomePath = getenv("HOME");
   if( mHomePath.empty() )
   {
   	 mHomePath = "/boot/home";
   }
#elif defined(GAME_PLATFORM_WIN)
  TCHAR path[MAX_PATH];
  if( SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, path)) )
  {
     // need to convert to OEM codepage so that fstream can use
     // it properly on international systems.
     TCHAR oemPath[MAX_PATH];
     CharToOem(path, oemPath);
     mHomePath = oemPath;
     // create Home subdir
     mHomePath += "\\Home\\";
  }

  if (mHomePath.empty())
  {
     // couldn't create dir in home directory, fall back to cwd
     mHomePath = "";
  }
#endif

  return vfs::Directory( mHomePath );
}

Directory::Directory(){}

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

  _GAME_DEBUG_BREAK_IF( !exist() );
  return Directory();
}

Path Directory::relativePathTo(Path path) const
{
  if ( toString().empty() || path.toString().empty() )
    return *this;

  Path path1 = absolutePath();
  Path path2( Directory( path.directory() ).absolutePath() );
  StringArray list1, list2;

  list1 = utils::split( path1.toString(), "/\\");
  list2 = utils::split( path2.toString(), "/\\");

  unsigned int i=0;
  utils::equaleMode emode = utils::equaleIgnoreCase;
  if( OSystem::isUnix() )
    emode = utils::equaleCase;

  for (; i<list1.size() && i<list2.size(); ++i)
  {
    if( !utils::isEquale( list1[ i ], list2[ i ], emode ) )
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

  return path1.toString() + path.baseName().toString();
}

} //end namespace io

