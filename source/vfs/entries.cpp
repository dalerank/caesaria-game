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

#include "entries.hpp"
#include "core/foreach.hpp"

namespace vfs
{

static const Path emptyFileListEntry( "" );

class Entries::Impl
{
public:
  //! Ignore paths when adding or searching for files
  bool ignorePaths;
  Path::SensType sensType;
  //! Path to the file list
  Path path;
  Items files;

  Path checkCase( Path p )
  {
    switch( sensType )
    {
    case Path::ignoreCase:
      return StringHelper::localeLower( p.toString() );
      break;
    case Path::equaleCase: break;
    case Path::nativeCase:
  #ifdef CAESARIA_PLATFORM_WIN
      return StringHelper::localeLower( p.toString() );
  #elif defined(CAESARIA_PLATFORM_UNIX)
      return p;
  #endif
    break;
    }

    return p;
  }
};

Entries::Entries( const Path& path, Path::SensType type, bool ignorePaths )
 : _d( new Impl )
{
#ifdef _DEBUG
  setDebugName( "FileList" );
#endif
  _d->ignorePaths = ignorePaths;
  _d->path = StringHelper::replace( path.toString(), "\\", "/" );
  _d->sensType = type;
}

Entries::Entries( const Entries& other ) : _d( new Impl )
{
  *this = other;
}

Entries& Entries::operator=( const Entries& other )
{
  _d->sensType = other._d->sensType;
  _d->ignorePaths = other._d->ignorePaths;
  _d->path = other._d->path;

  _d->files.clear();

  foreach( it, other._d->files )
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

Entries::Items &Entries::_items()
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

  return _d->files[index].abspath();
}

//! adds a file or folder
unsigned int Entries::addItem( const Path& fullPath, unsigned int offset, unsigned int size, bool isDirectory, unsigned int id )
{
  EntryInfo entry;
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

  entry.name = _d->checkCase( entry.name );

  entry.setAbspath( entry.name );

  entry.name = entry.name.baseName();

  if(_d->ignorePaths )
  {
    entry.setAbspath( entry.name );
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
  EntryInfo entry;
  // we only need fullName to be set for the search
  entry.setAbspath( StringHelper::replace( filename.toString(), "\\", "/" ) );
  entry.isDirectory = isDirectory;

  // remove trailing slash
  if( entry.abspath().lastChar() == '/' )
  {
    entry.isDirectory = true;
  }
  entry.setAbspath( entry.abspath().removeEndSlash() );
  entry.setAbspath( _d->checkCase( entry.abspath() ) );

  if( _d->ignorePaths )
  {
    entry.setAbspath( entry.abspath().baseName() );
  }

  foreach( it, _d->files )
  {
    if( (*it).isAbspathEquale( entry ) )
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

void Entries::setSensType( Path::SensType type )
{
  _d->sensType = type;
}

Entries Entries::filter(int flags, const std::string &options)
{
  Entries ret;
  bool isFile = (flags & Entries::file) > 0;
  bool isDirectory = (flags & Entries::directory) > 0;
  bool checkFileExt = (flags & Entries::extFilter) > 0;

  foreach( it, _d->files )
  {
    bool mayAdd = true;
    if( isFile ) { mayAdd = !(*it).isDirectory; }
    if( !mayAdd && isDirectory ) { mayAdd = (*it).isDirectory; }

    if( mayAdd && !(*it).isDirectory && checkFileExt )
    {
      mayAdd = (*it).abspath().isMyExtension( options );
    }

    if( mayAdd )
    {
      ret._d->files.push_back( *it );
    }
  }

  return ret;
}

const Entries::Items& Entries::items() const {  return _d->files; }

const EntryInfo& Entries::item(unsigned int index) const
{
  return _d->files[ index ];
}

} //end namespace io
