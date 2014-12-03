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

#include "filesystem.hpp"
#include "filenative_impl.hpp"
#include "archive.hpp"
#include "directory.hpp"
#include "core/foreach.hpp"
#include "entries.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"

#if defined (CAESARIA_PLATFORM_WIN)
	#include <direct.h> // for _chdir
	#include <io.h> // for _access
	#include <tchar.h>
	#include <windows.h>
	#include <stdio.h>
#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
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

class FileSystem::Impl
{
public:
  //! currently attached ArchiveLoaders
  std::vector< ArchiveLoaderPtr > archiveLoaders;
	//! currently attached Archives
  std::vector< ArchivePtr > openArchives;

  Directory resourdFolder;

  //! WorkingDirectory for Native and Virtual filesystems	
  Path workingDirectory[2];

  FileSystem::Mode fileSystemType;

public:
  ArchivePtr changeArchivePassword( const Path& filename, const std::string& password );
};

ArchivePtr FileSystem::Impl::changeArchivePassword(const Path& filename, const std::string& password )
{
  foreach( it, openArchives )
  {
    // TODO: This should go into a path normalization method
    // We need to check for directory names with trailing slash and without
    const Path absPath = filename.absolutePath();
    const Path arcPath = (*it)->entries()->getPath();
    if( (absPath == arcPath) || (arcPath == (absPath.toString() + "/")) )
    {
      if( password.size() )
      {
        (*it)->Password=password;
      }

      return *it;
    }
  }

  return ArchivePtr();
}

//! constructor
FileSystem::FileSystem() : _d( new Impl )
{
  setMode( fsNative );
  //! reset current working directory

  workingDirectory();
}


//! destructor
FileSystem::~FileSystem() {}

NFile FileSystem::loadFileFromArchive( const Path& filePath )
{
  foreach( it, _d->openArchives )
  {
    NFile file = (*it)->createAndOpenFile( filePath );

    if( file.isOpen() )
    {
      return file;
    }
  }

  return NFile();
}

//! opens a file for read access
NFile FileSystem::createAndOpenFile(const Path& filename, Entity::Mode mode)
{
  NFile file = loadFileFromArchive( filename );

  if( file.isOpen() )
  {
    return file;
  }

  //Logger::warning( "FileSystem: create the file using an absolute path " + filename.toString() );
  FSEntityPtr ptr( new FileNative( filename.absolutePath(), mode ) );
  ptr->drop();

  return NFile( ptr );
}

//! Adds an external archive loader to the engine.
void FileSystem::addArchiveLoader( ArchiveLoaderPtr loader)
{
  if( loader.isNull() )
  {
    return;
  }

  _d->archiveLoaders.push_back(loader);
}

//! Returns the total number of archive loaders added.
unsigned int FileSystem::archiveLoaderCount() const
{
	return _d->archiveLoaders.size();
}

//! Gets the archive loader by index.
ArchiveLoaderPtr FileSystem::getArchiveLoader(unsigned int index) const
{
	if (index < _d->archiveLoaders.size())
  {
		return _d->archiveLoaders[index];
  }
	else
		return ArchiveLoaderPtr();
}

//! move the hirarchy of the filesystem. moves sourceIndex relative up or down
bool FileSystem::moveArchive(unsigned int sourceIndex, int relative)
{
	bool r = false;
	const int dest = (int) sourceIndex + relative;
	const int dir = relative < 0 ? -1 : 1;
	const int sourceEnd = ((int) _d->openArchives.size() ) - 1;
	ArchivePtr t;

	for (int s = (int) sourceIndex;s != dest; s += dir)
	{
		if (s < 0 || s > sourceEnd || s + dir < 0 || s + dir > sourceEnd)
			continue;

		t = _d->openArchives[s + dir];
		_d->openArchives[s + dir] = _d->openArchives[s];
		_d->openArchives[s] = t;
		r = true;
	}

	return r;
}


//! Adds an archive to the file system.
ArchivePtr FileSystem::mountArchive(  const Path& filename,
				      Archive::Type archiveType,
				      bool ignoreCase,
				      bool ignorePaths,
				      const std::string& password)
{
  Logger::warning( "FileSystem: mountArchive(path) archive call for " + filename.toString() );
  ArchivePtr archive;

  // see if archive is already added
  archive = _d->changeArchivePassword(filename, password );
  if( archive.isValid() )
  {
    Logger::warning( "Open archive " + filename.absolutePath().toString() );
    return archive;
  }

  foreach( it, _d->openArchives )
  {
    if( filename == (*it)->entries()->getPath())
      return *it;
  }

  int i;

  // do we know what type it should be?
  if (archiveType == Archive::unknown || archiveType == Archive::folder)
  {
    Logger::warning( "FileSystem: try to load archive based on file name" );
    for (i = _d->archiveLoaders.size()-1; i >=0 ; --i)
    {        
      if (_d->archiveLoaders[i]->isALoadableFileFormat(filename) )
      {
        Logger::warning( "FileSystem: find archive reader by extension " + filename.toString() );
        archive = _d->archiveLoaders[i]->createArchive(filename, ignoreCase, ignorePaths);
        if( archive.isValid() )
        {
          Logger::warning( "FileSystem: succesful mount archive " + filename.toString() );
          break;
        }
      }
    }

    // try to load archive based on content
    if( archive.isNull() )
    {
      Logger::warning( "FileSystem: try to load archive based on content" );
      NFile file = createAndOpenFile( filename, Entity::fmRead );
      if( file.isOpen() )
      {
        for (i = _d->archiveLoaders.size()-1; i >= 0; --i)
        {
          file.seek(0);
          if (_d->archiveLoaders[i]->isALoadableFileFormat( file ) )
          {
            Logger::warning( "FileSystem: find archive reader by content " + filename.toString() );
            file.seek(0);
            archive = _d->archiveLoaders[i]->createArchive( file, ignoreCase, ignorePaths);
            if( archive.isValid() )
            {
              Logger::warning( "FileSystem: succesful create archive from " + filename.toString() );
              break;
            }
          }
        }
      }
    }
  }
  else
  {
    // try to open archive based on archive loader type
    NFile file;
    for (i = _d->archiveLoaders.size()-1; i >= 0; --i)
    {
      if (_d->archiveLoaders[i]->isALoadableFileFormat(archiveType))
      {
        // attempt to open file
        if( !file.isOpen() )
        {
          file = createAndOpenFile(filename, Entity::fmRead );
        }

        // is the file open?
        if( file.isOpen() )
        {
          // attempt to open archive
          file.seek(0);
          if (_d->archiveLoaders[i]->isALoadableFileFormat( file))
          {
            file.seek(0);
            archive = _d->archiveLoaders[i]->createArchive( file, ignoreCase, ignorePaths);
            if( archive.isValid() )
            {
                break;
            }
          }
        }
        else
        {
            // couldn't open file
            break;
        }
      }
    }
  }

  if( archive.isValid() )
  {
    const std::string arcType = archive->getTypeName();
    Logger::warning( "FileSystem: check archive:type-%s as opened %s", arcType.c_str(), filename.toString().c_str() );
    _d->openArchives.push_back( archive );
    if( password.size() )
    {
      archive->Password=password;
    }

    return archive;
  }
  else
  {
    Logger::warning( "FileSystem: could not archive reader for" + filename.toString() );
  }

  return ArchivePtr();
}

ArchivePtr FileSystem::mountArchive(NFile file, Archive::Type archiveType,
                                    bool ignoreCase,
                                    bool ignorePaths,
                                    const std::string& password)
{
  Logger::warning( "FileSystem: mountArchive call for " + file.path().absolutePath().toString() );
  if( !file.isOpen() || archiveType == Archive::folder)
  {
    Logger::warning( "FileSystem: cannot open archive " + file.path().absolutePath().toString() );
    return ArchivePtr();
  }

  if( file.isOpen() )
  {
    ArchivePtr archive = _d->changeArchivePassword( file.path(), password );

    if( archive.isValid() )
    {
        return archive;
    }

    int i;

    if (archiveType == Archive::unknown)
    {
      // try to load archive based on file name
      for (i = _d->archiveLoaders.size()-1; i >=0 ; --i)
      {
        if (_d->archiveLoaders[i]->isALoadableFileFormat( file.path() ) )
        {
          archive = _d->archiveLoaders[i]->createArchive( file, ignoreCase, ignorePaths );
          if (archive.isValid())
          {
            break;
          }
        }
      }

      // try to load archive based on content
      if( archive.isNull() )
      {
        for (i = _d->archiveLoaders.size()-1; i >= 0; --i)
        {
          file.seek(0);
          if (_d->archiveLoaders[i]->isALoadableFileFormat( file ) )
          {
            file.seek(0);
            archive = _d->archiveLoaders[i]->createArchive( file, ignoreCase, ignorePaths);

            if( archive.isValid() )
            {
              break;
            }
          }
        }
      }
    }
    else
    {
      // try to open archive based on archive loader type
      for (i = _d->archiveLoaders.size()-1; i >= 0; --i)
      {
        if (_d->archiveLoaders[i]->isALoadableFileFormat(archiveType))
        {
          // attempt to open archive
          file.seek(0);

          if (_d->archiveLoaders[i]->isALoadableFileFormat( file))
          {
            file.seek(0);
            archive = _d->archiveLoaders[i]->createArchive( file, ignoreCase, ignorePaths);
            if( archive.isValid() )
            {
              break;
            }
          }
        }
      }
    }

    if( archive.isValid() )
    {
      Logger::warning( "Mount archive %s", file.path().toString().c_str() );
      _d->openArchives.push_back(archive);

      if (password.size())
      {
        archive->Password=password;
      }

      return archive;
    }
    else
    {
      Logger::warning( "Could not create archive for %s", file.path().toString().c_str() );
    }
  }

  return ArchivePtr();
}

Directory FileSystem::rcFolder() const { return _d->resourdFolder; }

void FileSystem::setRcFolder( const Directory &folder) { _d->resourdFolder = folder; }

//! Adds an archive to the file system.
ArchivePtr FileSystem::mountArchive( ArchivePtr archive)
{
  Logger::warning( "FileSystem: mountArchive call for " + archive->getTypeName() );
	for (unsigned int i=0; i < _d->openArchives.size(); ++i)
	{
		if( archive == _d->openArchives[i])
		{
			return archive;
		}
	}

	_d->openArchives.push_back(archive);
  return archive;
}


//! removes an archive from the file system.
bool FileSystem::unmountArchive(unsigned int index)
{
	bool ret = false;
	if (index < _d->openArchives.size())
	{
    Logger::warning( "FileSystem: unmountArchive %d", index );
		_d->openArchives.erase( _d->openArchives.begin() + index );
		ret = true;
	}

	return ret;
}


//! removes an archive from the file system.
bool FileSystem::unmountArchive(const Path& filename)
{
  for (unsigned int i=0; i < _d->openArchives.size(); ++i)
  {
    if (filename == _d->openArchives[i]->entries()->getPath())
    {
      Logger::warning( "FileSystem: unmountArchive " + filename.toString() );
      return unmountArchive(i);
    }
  }

  return false;
}


//! Removes an archive from the file system.
bool FileSystem::unmountArchive( ArchivePtr archive)
{
	for (unsigned int i=0; i < _d->openArchives.size(); ++i)
	{
		if( archive == _d->openArchives[i] )
		{
			return unmountArchive(i);
		}
	}

	return false;
}

//! gets an archive
unsigned int FileSystem::archiveCount() const {	return _d->openArchives.size(); }

ArchivePtr FileSystem::getFileArchive(unsigned int index)
{
	return index < archiveCount() ? _d->openArchives[index] : 0;
}

//! Returns the string of the current working directory
const Path& FileSystem::workingDirectory()
{
	int type = 0;

	if (type != fsNative)
	{
		type = fsVirtual;
	}
	else
	{
		#if defined(CAESARIA_PLATFORM_WIN)
			char tmp[_MAX_PATH];
			_getcwd(tmp, _MAX_PATH);
      _d->workingDirectory[type] = utils::replace( tmp, "\\", "/" );
		#elif defined(CAESARIA_PLATFORM_UNIX)
			// getting the CWD is rather complex as we do not know the size
			// so try it until the call was successful
			// Note that neither the first nor the second parameter may be 0 according to POSIX
			unsigned int pathSize=256;
            ScopedPtr< char > tmpPath( new char[pathSize] );
      
            while( (pathSize < (1<<16)) && !( getcwd( tmpPath.data(), pathSize)))
			{
				pathSize *= 2;
				tmpPath.reset( new char[pathSize] );
			}

            if( tmpPath )
			{
								_d->workingDirectory[fsNative] = Path( tmpPath.data() );
			}
		#endif //CAESARIA_PLATFORM_UNIX

		//_d->workingDirectory[type].validate();
	}

	return _d->workingDirectory[type];
}


//! Changes the current Working Directory to the given string.
bool FileSystem::changeWorkingDirectoryTo(Path newDirectory)
{
	bool success=false;

    if ( _d->fileSystemType != fsNative)
    {
      // is this empty string constant really intended?
      _d->workingDirectory[fsVirtual] = newDirectory.flattenFilename( "" );
      success = true;
    }
    else
    {
        _d->workingDirectory[ fsNative ] = newDirectory;
#if defined(CAESARIA_PLATFORM_WIN)
        success = ( _chdir( newDirectory.toString().c_str() ) == 0 );
#elif defined(CAESARIA_PLATFORM_UNIX)
        success = ( chdir( newDirectory.toString().c_str() ) == 0 );
#endif //CAESARIA_PLATFORM_UNIX
    }

    return success;
}

//! Sets the current file systen type
FileSystem::Mode FileSystem::setMode( Mode listType)
{
	Mode current = _d->fileSystemType;
	_d->fileSystemType = listType;
	return current;
}

//! looks if file is in the same directory of path. returns offset of directory.
//! 0 means in same directory. 1 means file is direct child of path
inline int isInSameDirectory ( const Path& path, const Path& file )
{
	int subA = 0;
	int subB = 0;
	int pos;

  if ( path.toString().size() && !utils::isEqualen( path.toString(), file.toString(), path.toString().size() ) )
		return -1;

	pos = 0;
	while ( (pos = path.toString().find( '/', pos )) >= 0 )
	{
		subA += 1;
		pos += 1;
	}

	pos = 0;
	while ( (pos = file.toString().find ( '/', pos )) >= 0 )
	{
		subB += 1;
		pos += 1;
	}

	return subB - subA;
}

//! Creates a list of files and directories in the current working directory
Entries FileSystem::getFileList()
{
  Entries ret;
  Path rpath = utils::replace( workingDirectory().toString(), "\\", "/" );
  rpath = rpath.addEndSlash();
  
  //Logger::warning( "FileSystem: start listing directory" );

	//! Construct from native filesystem
  if ( _d->fileSystemType == fsNative )
	{
		// --------------------------------------------
		//! Windows version
		#if defined(CAESARIA_PLATFORM_WIN)
			struct _finddata_t c_file;
			long hFile;

			if( (hFile = _findfirst( "*", &c_file )) != -1L )
			{
				do
				{
					ret.addItem( Path( rpath.toString() + c_file.name ), 0, c_file.size, (_A_SUBDIR & c_file.attrib) != 0, 0);
				}
				while( _findnext( hFile, &c_file ) == 0 );

				_findclose( hFile );
			}

			//TODO add drives
			//entry.Name = "E:\\";
			//entry.isDirectory = true;
			//Files.push_back(entry);
		#elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
			//Logger::warning( "FileSystem: start listing directory on unix" );
			// --------------------------------------------
			//! Linux version
			//ret.addItem( Path( rpath.toString() + ".." ), 0, 0, true, 0);

			//! We use the POSIX compliant methods instead of scandir
			DIR* dirHandle=opendir( rpath.toString().c_str() );
			if( dirHandle )
			{
				struct dirent *dirEntry;
				while ((dirEntry=readdir(dirHandle)))
				{
					unsigned int size = 0;
					bool isDirectory = false;

					if((strcmp(dirEntry->d_name, ".")==0) || (strcmp(dirEntry->d_name, "..")==0))
					{
						continue;
					}
					struct stat buf;
					if (stat(dirEntry->d_name, &buf)==0)
					{
						size = buf.st_size;
						isDirectory = S_ISDIR(buf.st_mode);
					}
					
					ret.addItem( Path( rpath.toString() + dirEntry->d_name ), 0, size, isDirectory, 0);
				}
				closedir(dirHandle);
			}
		#endif //CAESARIA_PLATFORM_UNIX
	}
	else
	{
		//! create file list for the virtual filesystem
		ret.setSensType( Path::equaleCase );

		//! add relative navigation
		EntryInfo e2;
		EntryInfo e3;

		//! PWD
		ret.addItem( Path( rpath.toString() + "." ), 0, 0, true, 0);

		//! parent
		ret.addItem( Path( rpath.toString() + ".." ), 0, 0, true, 0);

		//! merge archives
		for (unsigned int i=0; i < _d->openArchives.size(); ++i)
		{
		  const Entries *merge = _d->openArchives[i]->entries();

		  for (unsigned int j=0; j < merge->getFileCount(); ++j)
		  {
		    if ( isInSameDirectory(rpath, merge->getFullFileName(j)) == 0)
		    {
		      ret.addItem(merge->getFullFileName(j), merge->getFileOffset(j), merge->getFileSize(j), merge->isDirectory(j), 0);
		    }
		  }
		}
	}

	ret.sort();

  return ret;
}

//! determines if a file exists and would be able to be opened.
bool FileSystem::existFile(const Path& filename, Path::SensType sens) const
{
  for (unsigned int i=0; i < _d->openArchives.size(); ++i)
    if (_d->openArchives[i]->entries()->findFile(filename)!=-1)
      return true;

  #if defined(CAESARIA_PLATFORM_WIN)
    if( sens == Path::nativeCase || sens == Path::ignoreCase )
    {
      return ( _access( filename.toString().c_str(), 0) != -1);
    }
  #elif defined(CAESARIA_PLATFORM_UNIX) || defined(CAESARIA_PLATFORM_HAIKU)
    if( sens == Path::nativeCase || sens == Path::equaleCase )
    {
      return ( access( filename.toString().c_str(), 0 ) != -1);
    }
  #endif //CAESARIA_PLATFORM_UNIX

  Entries files = Directory( filename.directory() ).getEntries();
  files.setSensType( sens );
  int index = files.findFile( filename );
  return index != -1;
}

DateTime FileSystem::getFileUpdateTime(const Path& filename) const
{ 
#ifndef CAESARIA_PLATFORM_WIN
  struct tm *foo;
  struct stat attrib;
  stat( filename.toString().c_str(), &attrib);
  foo = gmtime((const time_t*)&(attrib.st_mtime));

  return DateTime( foo->tm_year, foo->tm_mon+1, foo->tm_mday+1,
                   foo->tm_hour, foo->tm_min, foo->tm_sec );
#else
  FILETIME creationTime,
           lpLastAccessTime,
           lastWriteTime;
  HANDLE h = CreateFile( filename.toString().c_str(),
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

FileSystem& FileSystem::instance()
{
  static FileSystem _instanceFileSystem;
  return _instanceFileSystem;
}

} //end namespace io
