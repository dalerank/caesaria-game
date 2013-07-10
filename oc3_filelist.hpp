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


#ifndef __NRP_FILE_LIST_H_INCLUDED__
#define __NRP_FILE_LIST_H_INCLUDED__

#include "oc3_filepath.hpp"
#include "oc3_filelist_item.hpp"
#include "oc3_referencecounted.hpp"
#include <vector>

namespace io
{

//! Implementation of a file list
class FileList : public ReferenceCounted
{
public:
  typedef std::vector< FileListItem > Items;
	typedef Items::iterator ItemIterator;

	//! Constructor
	/** \param path The path of this file archive */
	FileList( const FilePath& path="", bool ignoreCase=false, bool ignorePaths=false );

	FileList( const FileList& other );

	//! Destructor
	~FileList();

	//! Add as a file or folder to the list
	/** \param fullPath The file name including path, up to the root of the file list.
	\param isDirectory True if this is a directory rather than a file.
	\param offset The offset where the file is stored in an archive
	\param size The size of the file in bytes.
	\param id The ID of the file in the archive which owns it */
	unsigned int addItem(const FilePath& fullPath, unsigned int offset, unsigned int size, bool isDirectory, unsigned int id=0);

	//! Sorts the file list. You should call this after adding any items to the file list
	void sort();

	//! Returns the amount of files in the filelist.
	unsigned int getFileCount() const;

  const Items& getItems() const;  

	//! Gets the name of a file in the list, based on an index.
	const FilePath& getFileName(unsigned int index) const;

	//! Gets the full name of a file in the list, path included, based on an index.
	const FilePath& getFullFileName(unsigned int index) const;

	//! Returns the ID of a file in the file list, based on an index.
	unsigned int getID(unsigned int index) const;

	//! Returns true if the file is a directory
	bool isDirectory(unsigned int index) const;

	//! Returns the size of a file
	unsigned int getFileSize(unsigned int index) const;

	//! Returns the offest of a file
	unsigned int getFileOffset(unsigned int index) const;

	//! Searches for a file or folder within the list, returns the index
	int findFile(const FilePath& filename, bool isFolder=false) const;

	//! Returns the base path of the file list
	const FilePath& getPath() const;

	void setIgnoreCase( bool ignore );

	FileList& operator=( const FileList& other );

protected:

	class Impl;
	Impl* _d;
};

}//end namespace io

#endif

