/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5122 $ (Revision of last commit) 
 $Date: 2011-12-11 23:47:31 +0400 (Вс, 11 дек 2011) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include <string>
//#include <boost/regex.hpp>
#include "ReleaseFileset.h"
#include "IniFile.h"

namespace tdm
{

class ReleaseVersions :
	public std::map<std::string, ReleaseFileSet>,
	public IniFile::SectionVisitor
{
public:
	void LoadFromIniFile(IniFilePtr iniFile)
	{
		iniFile->ForeachSection(*this);
	}

	void VisitSection(const IniFile& iniFile, const std::string& sectionName)
	{
		// Get the info from the section header
		if(StringHelper::startsWith( sectionName, "Version"))
		{
			std::string version = sectionName.substr( 0, sectionName.find("#"));
			std::string filename = sectionName.substr( sectionName.find("#")+1 );

			ReleaseFileSet& set = FindOrInsertVersion(version);

			ReleaseFile file(filename);

			file.crc = CRC::ParseFromString(iniFile.GetValue(sectionName, "crc"));
			file.filesize = StringHelper::toUint( iniFile.GetValue(sectionName, "filesize") );
			file.localChangesAllowed = iniFile.GetValue(sectionName, "allow_local_modifications") == "1";

			Logger::warning( "Found version %s file: %s with checksum %x", version.c_str(), filename.c_str(), file.crc );

			set.insert(ReleaseFileSet::value_type(filename, file));
		}
	}

private:
	ReleaseFileSet& FindOrInsertVersion(const std::string& version)
	{
		iterator found = find(version);

		if (found != end())
		{
			return found->second;
		}

		return insert(value_type(version, ReleaseFileSet())).first->second;
	}
};

} // namespace
