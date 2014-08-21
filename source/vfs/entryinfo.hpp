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

#ifndef _CAESARIA_FILELIST_ITEM_INCLUDE_H_
#define _CAESARIA_FILELIST_ITEM_INCLUDE_H_

#include "path.hpp"

namespace vfs
{

//! An entry in a list of files, can be a folder or a file.
class EntryInfo
{
public:
  //! The size of the file in bytes
  unsigned int size;

  //! The iD of the file in an archive
  /** This is used to link the FileList entry to extra info held about this
  file in an archive, which can hold things like data offset and CRC. */
  unsigned int uid;

  //! FileOffset inside an archive
  unsigned int offset;

  //! True if this is a folder, false if not.
  bool isDirectory;

  //! The name of the file
  /** If this is a file or folder in the virtual filesystem and the archive
  was created with the ignoreCase flag then the file name will be lower case. */
  Path name;

  //! The name of the file including the path
  /** If this is a file or folder in the virtual filesystem and the archive was
  created with the ignoreDirs flag then it will be the same as name. */
  Path fullpath;
  unsigned int fphash, nhash, nihash;
};

//! The < operator is provided so that list can sort and quickly search the list.
inline bool operator<(const EntryInfo& a, const EntryInfo& b)
{
  if (a.isDirectory != b.isDirectory)
    return a.isDirectory;

  std::string m = a.fullpath.canonical().toString();
  std::string o = b.fullpath.canonical().toString();

  return m < o;
}

} // end namspace vfs

#endif //_CAESARIA_FILELIST_ITEM_INCLUDE_H_
