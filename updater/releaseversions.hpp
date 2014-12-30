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

#ifndef __CAESARIA_RELEASEVERSION_H_INLCUDE__
#define __CAESARIA_RELEASEVERSION_H_INLCUDE__

#include <string>
#include "releasefileset.hpp"
#include "inifile.hpp"

namespace updater
{

class ReleaseVersions :
	public std::map<std::string, ReleaseFileSet>,
	public IniFile::SectionVisitor
{
public:
	virtual ~ReleaseVersions() {}
	void LoadFromIniFile(IniFilePtr iniFile)
	{
		iniFile->ForeachSection(*this);
	}

	void VisitSection(const IniFile& iniFile, const std::string& sectionName)
	{
		// Get the info from the section header
		if(utils::startsWith( sectionName, "Version"))
		{
			StringArray tokens = utils::split( sectionName, " " );

			if( tokens.size() == 3 && tokens[ 1 ] == "File")
			{
				std::string version = tokens[ 0 ].substr( 7 );
				std::string filename = tokens[ 2 ];

				ReleaseFileSet& set = FindOrInsertVersion(version);

				ReleaseFile file(filename);

				file.crc = CRC::ParseFromString(iniFile.GetValue(sectionName, "crc"));
				file.filesize = utils::toUint( iniFile.GetValue(sectionName, "filesize") );
				file.localChangesAllowed = iniFile.GetValue(sectionName, "allow_local_modifications") == "1";

				Logger::warning( "Found version %s file: %s with checksum %x", version.c_str(), filename.c_str(), file.crc );

				set.insert(ReleaseFileSet::value_type(filename, file));
			}
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

#endif //__CAESARIA_RELEASEVERSION_H_INLCUDE__
