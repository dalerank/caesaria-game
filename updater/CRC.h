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

#ifndef __CAESARIA_CRC_H_INLCUDE__
#define __CAESARIA_CRC_H_INLCUDE__

#include <string>
#include <stdexcept>
#include <sstream>
#include <cstdio>
#include "core/bytearray.hpp"

#include "vfs/path.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"

namespace updater
{

/**
 * Service class for CRC handling.
 */
class CRC
{
public:
	static unsigned int ParseFromString(const std::string& hexaStr)
	{
		unsigned int out;

		std::stringstream ss;

		if (!utils::startsWith(hexaStr, "0x"))
		{
			ss << ("0x" + hexaStr);
		}
		else
		{
			ss << hexaStr;
		}

		ss >> std::hex >> out;
		
		return out;
	}

	static std::string ToString(unsigned int crc)
	{
		return utils::format( 0xff, "%x", crc );
	}

	/**
	 * greebo: Returns the CRC for the given file. ZIP-archives (PK4s too) will 
	 * be opened and a cumulative CRC over the archive members will be returned.
	 *
	 * @throws: std::runtime_error if something goes wrong.
	 */
	static unsigned int GetCrcForFile(vfs::Path file)
	{
		try
		{
			return GetCrcForNonZipFile(file);
		}
		catch (std::runtime_error& ex)
		{
			Logger::warning( ex.what() );
			throw ex;
		}
	}
	
	// This is the algorithm as used in the TDM 1.02 tdm_update.pl version 0.46
	// It fails to produce the same CRC as the one found in the ZIP archives
	// See http://modetwo.net/darkmod/index.php?/topic/11473-problem-with-crcs-and-the-updater/

	static unsigned int GetCrcForNonZipFile(vfs::Path file)
	{
		// Open the file for reading
		FILE* fh = fopen(file.toString().c_str(), "rb");

		if (fh == NULL) throw std::runtime_error("Could not open file: " + file.toString());

		unsigned int crc = 0;
		
		while (true)
		{
			// Read the file in 32kb chunks
			ByteArray buf;
			buf.resize(32*1024);

			size_t bytesRead = fread(buf.data(), 1, sizeof(buf), fh);

			if( bytesRead > 0 )
			{
				buf.resize( bytesRead );
				crc = buf.crc32( crc );
				continue;
			}
			
			break;
		}

		//Logger::warning( "CRC calculated for file %s=%x", file.toString().c_str(), crc );

		fclose(fh);

		return crc;
	}
};

} // namespace

#endif //__CAESARIA_CRC_H_INLCUDE__
