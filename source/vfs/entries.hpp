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
// Copyright 2012-2014 Dalerank, dalerank@gmail.com

#ifndef __CAESARIA_ENTRIES_H_INCLUDED__
#define __CAESARIA_ENTRIES_H_INCLUDED__

#include "path.hpp"
#include "entryinfo.hpp"
#include "core/stringarray.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include <vector>

namespace vfs
{

//! Implementation of a file list
class Entries : public ReferenceCounted
{
public:
  typedef enum { file=0x1, directory=0x2, extFilter=0x4 } FilterFlag;

  class Items : public std::vector< EntryInfo >
  {
  public:
    StringArray names() const;
    StringArray files( const std::string& ext ) const;
    StringArray folders() const;
  };

  typedef Items::iterator ItemIt;
  typedef Items::const_iterator ConstItemIt;

  //! Constructor
  /** \param path The path of this file archive */
  Entries( const Path& path="", Path::SensType ignoreCase=Path::equaleCase, bool ignorePaths=false );

  Entries( const Entries& other );

  //! Destructor
  virtual ~Entries();

  //! Add as a file or folder to the list
  /** \param fullPath The file name including path, up to the root of the file list.
  \param isDirectory True if this is a directory rather than a file.
  \param offset The offset where the file is stored in an archive
  \param size The size of the file in bytes.
  \param id The ID of the file in the archive which owns it */
  unsigned int addItem(const Path& fullPath, unsigned int offset, unsigned int size, bool isDirectory, unsigned int id=0);

  //! Sorts the file list. You should call this after adding any items to the file list
  void sort();

  //! Returns the amount of files in the filelist.
  unsigned int getFileCount() const;

  const Items& items() const;

  const EntryInfo& item( unsigned int index ) const;

  //! Gets the name of a file in the list, based on an index.
  const Path& getFileName(unsigned int index) const;

  //! Gets the full name of a file in the list, path included, based on an index.
  const Path& getFullFileName(unsigned int index) const;

  //! Returns the ID of a file in the file list, based on an index.
  unsigned int getID(unsigned int index) const;

  //! Returns true if the file is a directory
  bool isDirectory(unsigned int index) const;

  //! Returns the size of a file
  unsigned int getFileSize(unsigned int index) const;

  //! Returns the offest of a file
  unsigned int getFileOffset(unsigned int index) const;

  //! Searches for a file or folder within the list, returns the index
  int findFile(const Path& filename, bool isFolder=false) const;

  //! Returns the base path of the file list
  const Path& getPath() const;

  void setSensType( Path::SensType type );

  Entries filter( int flags, const std::string& options );

  Entries& operator=( const Entries& other );

  ConstItemIt begin() const;
  ConstItemIt end() const;

protected:
  Items& _items();
  void _updateCache();

private:
  class Impl;
  ScopedPtr< Impl >_d;
};

}//end namespace io

inline StringArray& operator<<( StringArray& array, const vfs::Entries& flist )
{
  const vfs::Entries::Items& items = flist.items();
  for( auto item : items)
  {
    array.push_back( item.name.toString() );
  }

  return array;
}

#endif

