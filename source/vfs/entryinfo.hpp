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

#include "core/stringhelper.hpp"
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
  unsigned int iD;

  //! FileOffset inside an archive
  unsigned int Offset;

  //! True if this is a folder, false if not.
  bool isDirectory;

  //! The == operator is provided so that list can slowly search the list!
  bool operator == (const EntryInfo& other) const
  {
    if (isDirectory != other.isDirectory)
      return false;

    return StringHelper::isEquale( _fullpath.toString(), other._fullpath.toString(), StringHelper::equaleIgnoreCase );
  }

  //! The < operator is provided so that list can sort and quickly search the list.
  bool operator <(const EntryInfo& other) const
  {
    if (isDirectory != other.isDirectory)
      return isDirectory;

    return StringHelper::compare( _fullpath.toString(), other._fullpath.toString(), StringHelper::equaleIgnoreCase );
  }

  inline bool isFolder() const { return _fullpath.isFolder(); }
  void setAbsolutePath( const Path& p )
  {
    _fullpath = p;
    _fphash = StringHelper::hash( p.toString() );
  }

  void setName( const Path& name )
  {
    _name = name;
    _nhash = StringHelper::hash( name.toString() );
    _nihash = StringHelper::hash( StringHelper::localeLower( name.toString() ) );
  }

  inline const Path& absolutePath() const { return _fullpath; }
  inline const Path& name() const { return _name; }

  inline unsigned int abspathhash() const { return _fphash; }
  inline unsigned int nameihash() const { return _nihash; }
  inline unsigned int namehash() const { return _nhash; }
private:
  //! The name of the file
  /** If this is a file or folder in the virtual filesystem and the archive
  was created with the ignoreCase flag then the file name will be lower case. */
  Path _name;

  //! The name of the file including the path
  /** If this is a file or folder in the virtual filesystem and the archive was
  created with the ignoreDirs flag then it will be the same as name. */
  Path _fullpath;
  unsigned int _fphash, _nhash, _nihash;
};

} // end namspace vfs

#endif //_CAESARIA_FILELIST_ITEM_INCLUDE_H_
