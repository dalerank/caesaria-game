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

#ifndef __OPENCAESAR3_FILE_SYSTEM_H_INCLUDED__
#define __OPENCAESAR3_FILE_SYSTEM_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "vfs/file.hpp"
#include "vfs/archive.hpp"
#include "vfs/path.hpp"

namespace vfs
{

class Entries;


/*!
	Virtual FS which uses normal files and archives
*/
class FileSystem 
{
public:
  typedef enum { fsNative=0, // Native OS FileSystem
                 fsVirtual  // Virtual FileSystem
  } Mode;

  static FileSystem& instance();

  //! destructor
  virtual ~FileSystem();

  //! opens a file for read access
  virtual NFile createAndOpenFile( const Path& filename,
                                                                       NFile::Mode mode );

  //! opens a file in archive, if not exists return 0
  virtual NFile loadFileFromArchive( const Path& filePath );

  //! Adds an archive to the file system.
  virtual ArchivePtr mountArchive( const Path& filename,
                                   Archive::Type archiveType=Archive::unknown,
                                   bool ignoreCase = true, bool ignorePaths = true,
                                   const std::string& password="" );

  //! Adds an archive to the file system.
  virtual ArchivePtr mountArchive( NFile file,
                                   Archive::Type archiveType=Archive::unknown,
                                   bool ignoreCase=true,
                                   bool ignorePaths=true,
                                   const std::string& password="" );

  //! Adds an archive to the file system.
  virtual ArchivePtr mountArchive( ArchivePtr archive );

  //! move the hirarchy of the filesystem. moves sourceIndex relative up or down
  virtual bool moveFileArchive( unsigned int sourceIndex, int relative);

  //! Adds an external archive loader to the engine.
  virtual void addArchiveLoader( ArchiveLoaderPtr loader);

  //! Returns the total number of archive loaders added.
  virtual unsigned int getArchiveLoaderCount() const;

  //! Gets the archive loader by index.
  virtual ArchiveLoaderPtr getArchiveLoader( unsigned int index) const;

  //! gets the file archive count
  virtual unsigned int getFileArchiveCount() const;

  //! gets an archive
  virtual ArchivePtr getFileArchive( unsigned int index);

  //! removes an archive from the file system.
  virtual bool unmountArchive( unsigned int index);

  //! removes an archive from the file system.
  virtual bool unmountArchive(const Path& filename);

  //! Removes an archive from the file system.
  virtual bool unmountArchive( ArchivePtr archive);

  //! Returns the string of the current working directory
  virtual const Path& getWorkingDirectory();

  //! Changes the current Working Directory to the string given.
  //! The string is operating system dependent. Under Windows it will look
  //! like this: "drive:\directory\sudirectory\"
  virtual bool changeWorkingDirectoryTo(const Path& newDirectory);

  //! Creates a list of files and directories in the current working directory
  //! and returns it.
  virtual Entries getFileList();

  //! determines if a file exists and would be able to be opened.
  virtual bool existFile(const Path& filename) const;

  Mode setFileListSystem( Mode listType);

private:
  //! constructor
  FileSystem();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namesapce io

#endif

