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

#include "filelist.hpp"

namespace vfs
{

static const Path emptyFileListEntry( "" );

class Entries::Impl
{
public:
  //! Ignore paths when adding or searching for files
  bool ignorePaths;
  bool ignoreCase;
  //! Path to the file list
  Path path;
  Items files;
};

Entries::Entries( const Path& path, bool ignoreCase, bool ignorePaths )
 : _d( new Impl )
{
#ifdef _DEBUG
  setDebugName( "FileList" );
#endif
  _d->ignorePaths = ignorePaths;
  _d->path = StringHelper::replace( path.toString(), "\\", "/" );
  _d->ignoreCase = ignoreCase;
}

Entries::Entries( const Entries& other ) : _d( new Impl )
{
  *this = other;
}

Entries& Entries::operator=( const Entries& other )
{
  _d->ignoreCase = other._d->ignoreCase;
  _d->ignorePaths = other._d->ignorePaths;
  _d->path = other._d->path;

  _d->files.clear();

  ItemIt it = other._d->files.begin();
  for( ; it != other._d->files.end(); it++ )
  {
    _d->files.push_back( *it );
  }

  return *this;
}

Entries::ConstItemIt Entries::begin() const
{
  return _d->files.begin();
}

Entries::ConstItemIt Entries::end() const
{
  return _d->files.end();
}

Entries::Items &Entries::_getItems()
{
  return _d->files;
}

Entries::~Entries()
{
}

unsigned int Entries::getFileCount() const
{
  return _d->files.size();
}

void Entries::sort()
{
  std::sort( _d->files.begin(), _d->files.end() );
}

const Path& Entries::getFileName(unsigned int index) const
{
  if (index >= _d->files.size())
    return emptyFileListEntry;

  return _d->files[index].name;
}


//! Gets the full name of a file in the list, path included, based on an index.
const Path& Entries::getFullFileName(unsigned int index) const
{
  if (index >= _d->files.size())
    return emptyFileListEntry;

  return _d->files[index].fullName;
}

//! adds a file or folder
unsigned int Entries::addItem( const Path& fullPath, unsigned int offset, unsigned int size, bool isDirectory, unsigned int id )
{
  FileListItem entry;
  entry.iD   = id ? id : _d->files.size();
  entry.Offset = offset;
  entry.size = size;
  entry.name = StringHelper::replace( fullPath.toString(), "\\", "/" );
  entry.isDirectory = isDirectory;

  // remove trailing slash
  if( *(entry.name.toString().rbegin()) == '/')
  {
    entry.isDirectory = true;
    entry.name = entry.name.removeEndSlash();
    //entry.name.validate();
  }

  if( _d->ignoreCase)
  {
    entry.name = StringHelper::localeLower( entry.name.toString() );
  }

  entry.fullName = entry.name;

  entry.name = entry.name.getBasename();

  if(_d->ignorePaths )
  {
    entry.fullName = entry.name;
  }

  _d->files.push_back(entry);

  return _d->files.size() - 1;
}

//! Returns the iD of a file in the file list, based on an index.
unsigned int Entries::getID(unsigned int index) const
{
  return index < _d->files.size() ? _d->files[index].iD : 0;
}

bool Entries::isDirectory(unsigned int index) const
{
  bool ret = false;
  if (index < _d->files.size())
    ret = _d->files[index].isDirectory;

  return ret;
}

//! Returns the size of a file
unsigned int Entries::getFileSize(unsigned int index) const
{
  return index < _d->files.size() ? _d->files[index].size : 0;
}

//! Returns the size of a file
unsigned int Entries::getFileOffset(unsigned int index) const
{
  return index < _d->files.size() ? _d->files[index].Offset : 0;
}

//! Searches for a file or folder within the list, returns the index
int Entries::findFile(const Path& filename, bool isDirectory) const
{
  FileListItem entry;
  // we only need fullName to be set for the search
  entry.fullName = StringHelper::replace( filename.toString(), "\\", "/" );
  entry.isDirectory = isDirectory;

  // remove trailing slash
  if( *entry.fullName.toString().rbegin() == '/' )
  {
    entry.isDirectory = true;
  }
  entry.fullName = entry.fullName.removeEndSlash();

  if( _d->ignoreCase )
  {
    entry.fullName = StringHelper::localeLower( entry.fullName.toString() );
  }

  if( _d->ignorePaths )
  {
    entry.fullName = entry.fullName.getBasename();
  }

  for( Items::iterator it=_d->files.begin(); it != _d->files.end(); it++ )
  {
    if( (*it).fullName == entry.fullName )
    {
      return std::distance( _d->files.begin(), it );
    }
  }

  return -1;
}


//! Returns the base path of the file list
const Path& Entries::getPath() const
{
  return _d->path;
}

void Entries::setIgnoreCase( bool ignore )
{
  _d->ignoreCase = ignore;
}

Entries Entries::filter(int flags, const std::string &options)
{
  Entries ret;
  bool isFile = (flags & Entries::file) > 0;
  bool isDirectory = (flags & Entries::directory) > 0;
  bool checkFileExt = (flags & Entries::extFilter) > 0;

  for( Items::iterator it=_d->files.begin(); it != _d->files.end(); it++ )
  {
    bool mayAdd = true;
    if( isFile ) { mayAdd = !(*it).isDirectory; }
    if( !mayAdd && isDirectory ) { mayAdd = (*it).isDirectory; }

    if( mayAdd && !(*it).isDirectory && checkFileExt )
    {
      mayAdd = (*it).fullName.isExtension( options );
    }

    if( mayAdd )
    {
      ret._d->files.push_back( *it );
    }
  }

  return ret;
}

const Entries::Items& Entries::getItems() const
{
  return _d->files;
}

} //end namespace io
