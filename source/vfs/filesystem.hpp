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
#include "core/time.hpp"

namespace vfs
{

class Entries;


/*!
	Virtual FS which uses normal files and archives
*/
class FileSystem 
{
public:
  typedef enum
  {
    fsNative=0, // Native OS FileSystem
    fsVirtual  // Virtual FileSystem
  } Mode;

  static FileSystem& instance();

  //! destructor
  virtual ~FileSystem();

  //! opens a file for read access
  NFile createAndOpenFile( const Path& filename, NFile::Mode mode );

  //! opens a file in archive, if not exists return 0
  NFile loadFileFromArchive( const Path& filePath );

  //! Adds an archive to the file system.
  ArchivePtr mountArchive( const Path& filename,
                                   Archive::Type archiveType=Archive::unknown,
                                   bool ignoreCase = true, bool ignorePaths = true,
                                   const std::string& password="" );

  //! Adds an archive to the file system.
  ArchivePtr mountArchive( NFile file,
                                   Archive::Type archiveType=Archive::unknown,
                                   bool ignoreCase=true,
                                   bool ignorePaths=true,
                                   const std::string& password="" );

  void mountFolder( const vfs::Directory& folder );

  //! Adds an archive to the file system.
  ArchivePtr mountArchive( ArchivePtr archive );

  //! move the hirarchy of the filesystem. moves sourceIndex relative up or down
  bool moveArchive( unsigned int sourceIndex, int relative);

  //! Adds an external archive loader to the engine.
  void addArchiveLoader( ArchiveLoaderPtr loader);

  //! Returns the total number of archive loaders added.
  unsigned int archiveLoaderCount() const;

  //! Gets the archive loader by index.
  ArchiveLoaderPtr getArchiveLoader( unsigned int index) const;

  //! gets the file archive count
  unsigned int archiveCount() const;

  //! gets an archive
  ArchivePtr getFileArchive( unsigned int index);

  //! removes an archive from the file system.
  bool unmountArchive( unsigned int index);

  //! removes an archive from the file system.
  bool unmountArchive(const Path& filename);

  //! Removes an archive from the file system.
  bool unmountArchive( ArchivePtr archive);

  //! Returns the string of the current working directory
  const Path& workingDirectory();

  //! Changes the current Working Directory to the string given.
  //! The string is operating system dependent. Under Windows it will look
  //! like this: "drive:\directory\sudirectory\"
  bool changeWorkingDirectoryTo(Path newDirectory);

  //! Creates a list of files and directories in the current working directory
  //! and returns it.
  Entries getFileList();

  //! determines if a file exists and would be able to be opened.
  bool existFile(const Path& filename, Path::SensType sens=Path::nativeCase) const;

  DateTime getFileUpdateTime( const Path& filename ) const;

  Mode setMode( Mode listType );

private:
  FileSystem();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namesapce io

#endif

