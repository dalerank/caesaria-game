#ifndef _NRP_FILELIST_ITEM_INCLUDE_H_
#define _NRP_FILELIST_ITEM_INCLUDE_H_

#include "core/stringhelper.hpp"

namespace io
{

//! An entry in a list of files, can be a folder or a file.
class FileListItem
{
public:
	//! The name of the file
	/** If this is a file or folder in the virtual filesystem and the archive
	was created with the ignoreCase flag then the file name will be lower case. */
	FilePath name;

	//! The name of the file including the path
	/** If this is a file or folder in the virtual filesystem and the archive was
	created with the ignoreDirs flag then it will be the same as name. */
	FilePath fullName;

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

	//! The == operator is provided so that CFileList can slowly search the list!
	bool operator == (const FileListItem& other) const
	{
		if (isDirectory != other.isDirectory)
			return false;

		return StringHelper::isEquale( fullName.toString(), other.fullName.toString(), StringHelper::equaleIgnoreCase );
	}

	//! The < operator is provided so that CFileList can sort and quickly search the list.
	bool operator <(const FileListItem& other) const
	{
		if (isDirectory != other.isDirectory)
			return isDirectory;

    return StringHelper::isEquale( fullName.toString(), other.fullName.toString(), StringHelper::equaleIgnoreCase );
	}
};

} // end namspace io

#endif
