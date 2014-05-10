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

#ifndef __CAESARIA_ARCHIVE_LOADER_H_INCLUDED__
#define __CAESARIA_ARCHIVE_LOADER_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "vfs/file.hpp"
#include "core/smartptr.hpp"

namespace vfs
{

class Entries;

//! The FileArchive manages archives and provides access to files inside them.
class Archive : public virtual ReferenceCounted
{
public:
  typedef enum
  {
    zip=0,
    gzip,
    unknown,
    folder,
    sg2
  } Type;

  //! Opens a file based on its name
  /** Creates and returns a new IReadFile for a file in the archive.
  \param filename The file to open
  \return Returns A pointer to the created file on success,
  or 0 on failure. */
  virtual NFile createAndOpenFile(const Path& filename) =0;

  //! Opens a file based on its position in the file list.
  /** Creates and returns
  \param index The zero based index of the file.
  \return Returns a pointer to the created file on success, or 0 on failure. */
  virtual NFile createAndOpenFile( unsigned int index) =0;

  //! Returns the complete file tree
  /** \return Returns the complete directory tree for the archive,
  including all files and folders */
  virtual const Entries* entries() const =0;

  //! get the archive type
  virtual std::string getTypeName() const { return ""; }

  //! An optionally used password string
  /** This variable is publicly accessible from the interface in order to
  avoid single access patterns to this place, and hence allow some more
  obscurity.
  */
  std::string Password;
};

typedef SmartPtr< Archive > ArchivePtr;

//! Class which is able to create an archive from a file.
/** If you want load archives of
currently unsupported file formats (e.g .wad), then implement
this and add your new Archive loader with
IFileSystem::addArchiveLoader() to the engine. */
class ArchiveLoader : public virtual ReferenceCounted
{
public:
  //! Check if the file might be loaded by this class
  /** Check based on the file extension (e.g. ".zip")
    \param filename Name of file to check.
    \return True if file seems to be loadable.
  */
  virtual bool isALoadableFileFormat(const Path& filename) const =0;

  //! Check if the file might be loaded by this class
  /** This check may look into the file.
    \param file File handle to check.
    \return True if file seems to be loadable.
  */
  virtual bool isALoadableFileFormat( NFile file) const =0;

  //! Check to see if the loader can create archives of this type.
  /** Check based on the archive type.
    \param fileType The archive type to check.
    \return True if the archile loader supports this type, false if not
  */
  virtual bool isALoadableFileFormat( Archive::Type fileType) const =0;

  //! Creates an archive from the filename
  /**
    \param filename File to use.
    \param ignoreCase Searching is performed without regarding the case
    \param ignorePaths Files are searched for without checking for the directories
    \return Pointer to newly created archive, or 0 upon error.
  */
  virtual ArchivePtr createArchive(const Path& filename, bool ignoreCase, bool ignorePaths) const =0;

  //! Creates an archive from the file
  /**
    \param file File handle to use.
    \param ignoreCase Searching is performed without regarding the case
    \param ignorePaths Files are searched for without checking for the directories
    \return Pointer to newly created archive, or 0 upon error.
  */
  virtual ArchivePtr createArchive( NFile file, bool ignoreCase, bool ignorePaths) const =0;
};

typedef SmartPtr< ArchiveLoader > ArchiveLoaderPtr;

} //end namespace vfs

#endif //__CAESARIA_ARCHIVE_LOADER_H_INCLUDED__

