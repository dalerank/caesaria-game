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

#ifndef __CAESARIA_RELEASEFILESET_H_INLCUDE__
#define __CAESARIA_RELEASEFILESET_H_INLCUDE__

#include "inifile.hpp"
#include "CRC.h"
#include "vfs/path.hpp"
#include "core/utils.hpp"
#include "vfs/entries.hpp"
#include "vfs/file.hpp"
#include "vfs/directory.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"

namespace updater
{

struct ReleaseFile
{
	// True if this file is a PK4 / ZIP package
	bool isArchive;

	// Filename including path
	vfs::Path file;

	// CRC32 checksum
	unsigned int crc;

	// The file size in bytes
	std::size_t	filesize;

	// This is TRUE for DoomConfig.cfg, for example. 
	// Is only used when comparing release versions in the updater code
	bool localChangesAllowed;

	// The download ID of this file (-1 == no download ID)
	int downloadId;

	ReleaseFile() :
		isArchive(false),
		localChangesAllowed(false),
		downloadId(-1)
	{}

	ReleaseFile(vfs::Path pathToFile) :
		isArchive(false),
		file(pathToFile),
		localChangesAllowed(false),
		downloadId(-1)
	{}

	ReleaseFile(vfs::Path pathToFile, unsigned int crc_) :
		isArchive(false),
		file(pathToFile),
		crc(crc_),
		localChangesAllowed(false),
		downloadId(-1)
	{}

	// Implement less operator for use in std::set or std::map
	bool operator<(const ReleaseFile& other) const
	{
		return file.toString() < other.file.toString();
	}

	// A ReleaseFile is equal if filename, size, archive flag, CRC and all members are equal
	bool operator==(const ReleaseFile& other) const
	{
		if (file.toString() != other.file.toString()
				|| crc != other.crc
				|| filesize != other.filesize
				|| isArchive != other.isArchive)
		{
			return false;
		}

		return true; // all checks passed
	}

	bool operator!=(const ReleaseFile& other) const
	{
		return !(this->operator==(other));
	}

	bool isWrongOS() const
	{
#ifdef CAESARIA_PLATFORM_LINUX
       if( file.isMyExtension(".exe") || file.isMyExtension(".dll") || file.isMyExtension(".macos") || file.isMyExtension(".haiku") )
           return true;
#elif defined(CAESARIA_PLATFORM_HAIKU)
        if( file.isMyExtension(".linux") || file.isMyExtension(".exe") || file.isMyExtension(".dll") || file.isMyExtension(".macos"))
           return true;
#elif defined(CAESARIA_PLATFORM_WIN)
        if( file.isMyExtension(".linux") || file.isMyExtension(".macos") || file.isMyExtension(".haiku") )
           return true;
#elif defined(CAESARIA_PLATFORM_MACOSX)
        if( file.isMyExtension(".linux") || file.isMyExtension(".exe") || file.isMyExtension(".dll") || file.isMyExtension(".haiku"))
            return true;
#endif
        return false;
    }

	bool isUpdater(const std::string& executable) const
	{
		return utils::isEquale( file.toString(), executable );
	}
};

/** 
 * A FileSet represents a full release package, including
 * PK4 files and extracted files like INI, tdmlauncher.exe, etc.
 * Each file contains a checksum so that it can be compared to 
 * an available package on the update servers.
 */
class ReleaseFileSet :
	public std::map<std::string, ReleaseFile>
{
public:
	ReleaseFileSet()
	{}
	
	class Visitor :	public IniFile::SectionVisitor
	{
	private:
		ReleaseFileSet& _set;

	public:
		Visitor(ReleaseFileSet& set) :
			_set(set)
		{}

			void VisitSection(const IniFile& iniFile, const std::string& section);
	};

	void removeItem( const std::string& itemName )
	{
		foreach( i, *this )
		{
			if( i->first == itemName )
			{
				erase( i );
				break;
			}
		}
	}

	// Construct a set by specifying the INI file it is described in
	// The INI file is usually the crc_info.txt file on the servers
	static ReleaseFileSet LoadFromIniFile(IniFilePtr iniFile)
	{
		ReleaseFileSet set;	
		
		Visitor _visitor( set );
		
		// Traverse the settings using the ReleaseFileSet as visitor
		iniFile->ForeachSection(_visitor);

		return set;
	}
};

} // namespace

#endif //__CAESARIA_RELEASEFILESET_H_INLCUDE__
