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

#include "oc3_filelist.hpp"

namespace io
{

static const FilePath emptyFileListEntry( "" );

class FileList::Impl
{
public:
  //! Ignore paths when adding or searching for files
  bool ignorePaths;
  bool ignoreCase;
  //! Path to the file list
  FilePath path;
  Items files;
};

FileList::FileList( const FilePath& path, bool ignoreCase, bool ignorePaths )
 : _d( new Impl )
{
	#ifdef _DEBUG
	  setDebugName( "FileList" );
	#endif
  _d->ignorePaths = ignorePaths;
  _d->path = StringHelper::replace( path.toString(), "\\", "/" );
	_d->ignoreCase = ignoreCase;
}

FileList::FileList( const FileList& other ) : _d( new Impl )
{
	*this = other;
}

FileList& FileList::operator=( const FileList& other )
{
	_d->ignoreCase = other._d->ignoreCase;
	_d->ignorePaths = other._d->ignorePaths;
	_d->path = other._d->path;

	_d->files.clear();

	ItemIterator it = other._d->files.begin();
	for( ; it != other._d->files.end(); it++ )
  {
		_d->files.push_back( *it );
  }

	return *this;
}

FileList::~FileList()
{
	delete _d;
}

unsigned int FileList::getFileCount() const
{
	return _d->files.size();
}

void FileList::sort()
{
  std::sort( _d->files.begin(), _d->files.end() );
}

const FilePath& FileList::getFileName(unsigned int index) const
{
	if (index >= _d->files.size())
		return emptyFileListEntry;

	return _d->files[index].name;
}


//! Gets the full name of a file in the list, path included, based on an index.
const FilePath& FileList::getFullFileName(unsigned int index) const
{
	if (index >= _d->files.size())
		return emptyFileListEntry;

	return _d->files[index].fullName;
}

//! adds a file or folder
unsigned int FileList::addItem( const FilePath& fullPath, unsigned int offset, unsigned int size, bool isDirectory, unsigned int id )
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

	//os::Printer::log(Path.c_str(), entry.fullName);

	_d->files.push_back(entry);

	return _d->files.size() - 1;
}

//! Returns the iD of a file in the file list, based on an index.
unsigned int FileList::getID(unsigned int index) const
{
	return index < _d->files.size() ? _d->files[index].iD : 0;	
}

bool FileList::isDirectory(unsigned int index) const
{
	bool ret = false;
	if (index < _d->files.size())
		ret = _d->files[index].isDirectory;

	return ret;
}

//! Returns the size of a file
unsigned int FileList::getFileSize(unsigned int index) const
{
	return index < _d->files.size() ? _d->files[index].size : 0;
}

//! Returns the size of a file
unsigned int FileList::getFileOffset(unsigned int index) const
{
	return index < _d->files.size() ? _d->files[index].Offset : 0;
}


//! Searches for a file or folder within the list, returns the index
int FileList::findFile(const FilePath& filename, bool isDirectory) const
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
const FilePath& FileList::getPath() const
{
	return _d->path;
}

void FileList::setIgnoreCase( bool ignore )
{
	_d->ignoreCase = ignore;
}

const FileList::Items& FileList::getItems() const
{
  return _d->files;
}

} //end namespace io