/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5598 $ (Revision of last commit) 
 $Date: 2012-10-19 19:46:11 +0400 (Пт, 19 окт 2012) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include "IniFile.h"
#include "CRC.h"
#include "vfs/path.hpp"
#include "core/stringhelper.hpp"
#include "vfs/entries.hpp"
#include "vfs/file.hpp"
#include "core/logger.hpp"

namespace tdm
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

	// Members of this archive
	std::set<ReleaseFile> members;

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
				|| isArchive != isArchive)
		{
			return false;
		}

		if (members.size() != other.members.size()) 
		{
			return false; // member size mismatch
		}

		// Equal member size, compare all 
		for (std::set<ReleaseFile>::const_iterator i = members.begin(), j = other.members.begin();
			 i != members.end(); ++i, ++j)
		{
			if (*i != *j) return false;
		}

		return true; // all checks passed
	}

	bool operator!=(const ReleaseFile& other) const
	{
		return !(this->operator==(other));
	}

	bool ContainsUpdater(const std::string& executable) const
	{
		for (std::set<ReleaseFile>::const_iterator m = members.begin(); m != members.end(); ++m)
		{
			if( StringHelper::localeLower( m->file.toString() ) == StringHelper::localeLower( executable ) )
			{
				return true;
			}
		}

		return false;
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

	// Construct a set by specifying the INI file it is described in
	// The INI file is usually the crc_info.txt file on the servers
	static ReleaseFileSet LoadFromIniFile(IniFilePtr iniFile)
	{
		ReleaseFileSet set;

		class Visitor :
			public IniFile::SectionVisitor
		{
		private:
			ReleaseFileSet& _set;

		public:
			Visitor(ReleaseFileSet& set) :
				_set(set)
			{}
				
			void VisitSection(const IniFile& iniFile, const std::string& section)
			{
				if( StringHelper::startsWith( section, "File" ) )
				{
					vfs::Path filename = section.substr(5);

					std::pair<ReleaseFileSet::iterator, bool> result = _set.insert(	ReleaseFileSet::value_type(filename.toString(), ReleaseFile(filename)));
					
					result.first->second.crc = CRC::ParseFromString(iniFile.GetValue(section, "crc"));
					result.first->second.filesize = StringHelper::toUint( iniFile.GetValue(section, "size") );

					if( filename.isExtension( "zip") )
					{
						result.first->second.isArchive = true;
					}
					else
					{
						result.first->second.isArchive = false;
					}
				}
				else if( StringHelper::startsWith( section, "Member" ) )
				{
					std::string combo = section.substr(7);

					// bar.zip:foo.pk4
					std::string archiveName = combo.substr(0, combo.rfind(':') );
					std::string memberName = combo.substr(combo.rfind(':') + 1 );

					// Find or insert the archive
					std::pair<ReleaseFileSet::iterator, bool> result = _set.insert(
						ReleaseFileSet::value_type(archiveName, ReleaseFile(archiveName)));

					// Find or insert the member
					ReleaseFile member(memberName);

					member.isArchive = false;
					member.crc = CRC::ParseFromString(iniFile.GetValue(section, "crc"));
					member.filesize = StringHelper::toUint( iniFile.GetValue(section, "size") );

					result.first->second.members.insert(member);
				}
			}
		} _visitor(set);

		// Traverse the settings using the ReleaseFileSet as visitor
		iniFile->ForeachSection(_visitor);

		return set;
	}

	// Construct a release file set from the given folder
	static ReleaseFileSet LoadFromFolder( vfs::Directory folder,
																				const std::set<std::string> ignoreList = std::set<std::string>())
	{
		ReleaseFileSet set;

		vfs::Entries files = folder.getEntries();
		for( vfs::Entries::ConstItemIt i = files.begin(); i != files.end(); ++i)
		{
			vfs::Path entry = i->name;

			if( entry.isFolder() )
			{
				continue; // skip directories
			}

			std::string filename = entry.getBasename().toString();

			vfs::Path relativePath = entry.getRelativePathTo( folder );

			if( ignoreList.find( StringHelper::localeLower( filename ) ) != ignoreList.end() )
			{
				Logger::warning( "Ignoring file: %s", relativePath.toString().c_str() );
				continue;
			}
		
			Logger::warning( "Found file: %s", relativePath.toString().c_str() );

			if( entry.isExtension( ".zip" ) )
			{
				ReleaseFile archive(relativePath);

				archive.isArchive = true;
				archive.crc = CRC::GetCrcForFile(entry);

				archive.filesize = static_cast<std::size_t>( vfs::NFile::getSize( entry ) );


				// Add all members of this archive to the ReleaseFile
				class ZipFileVisitor : 
					public ZipFileRead::Visitor
				{
				private:
					ReleaseFile& _archive;

				public:
					ZipFileVisitor(ReleaseFile& archive) :
						_archive(archive)
					{}

					void VisitFile(const ZipFileRead::MemberInfo& info)
					{
						ReleaseFile file(info.filename);

						file.crc = info.crc;
						file.filesize = info.uncompressedSize;

						Logger::warning( "Adding archive member %s ith CRC %x", info.filename.c_str(), file.crc );

						_archive.members.insert(file);
					}

				} _visitor(archive);

				Logger::warning( " This is an archive, checksum is: %x", archive.crc );

				// Open this archive
				ZipFileReadPtr zipFile = Zip::OpenFileRead( entry );

				if (zipFile == NULL)
				{
					Logger::warning( "  Failed to open archive:  %s", entry.toString().c_str() );
					continue;
				}

				zipFile->ForeachFile(_visitor);

				Logger::warning( "  Archive has %d members", archive.members.size() );
				
				set[relativePath.toString()] = archive;
			}
		}

		return set;
	}
};

} // namespace
