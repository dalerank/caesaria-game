#include "releasefileset.hpp"

namespace updater
{
void ReleaseFileSet::Visitor::VisitSection(const IniFile& iniFile, const std::string& section)
			{
				if( StringHelper::startsWith( section, "File" ) )
				{
					vfs::Path filename = section.substr(5);

					ReleaseFile rfile(filename);
					if( rfile.isWrongOS() )
						return;

					std::pair<ReleaseFileSet::iterator, bool> result = _set.insert(	ReleaseFileSet::value_type(filename.toString(), rfile));
					
					result.first->second.crc = CRC::ParseFromString(iniFile.GetValue(section, "crc"));
					result.first->second.filesize = StringHelper::toUint( iniFile.GetValue(section, "filesize") );

					if( filename.isMyExtension( ".zip") )
					{
						result.first->second.isArchive = true;
					}
					else
					{
						result.first->second.isArchive = false;
					}
				}
			}	
}
