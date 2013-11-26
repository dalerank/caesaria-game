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

#ifndef _TDM_CRC_H_
#define _TDM_CRC_H_

#include <string>
#include <stdexcept>
#include <sstream>
#include <cstdio>
#include "core/bytearray.hpp"

//#include <boost/algorithm/string/case_conv.hpp>
//#include <boost/algorithm/string/predicate.hpp>
//#include <boost/format.hpp>
//#include <boost/crc.hpp>
//#include <boost/filesystem.hpp>

#include "vfs/filepath.hpp"
#include "azip/azip.h"
#include "core/logger.hpp"
#include "core/stringhelper.hpp"

namespace tdm
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

		if (!StringHelper::startsWith(hexaStr, "0x"))
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
		return StringHelper::format( 0xff, "%x", crc );
	}

	/**
	 * greebo: Returns the CRC for the given file. ZIP-archives (PK4s too) will 
	 * be opened and a cumulative CRC over the archive members will be returned.
	 *
	 * @throws: std::runtime_error if something goes wrong.
	 */
	static unsigned int GetCrcForFile(const io::FilePath& file)
	{
		try
		{
			if( file.isExtension( ".zip" ) )
			{
				return GetCrcForZip(file);
			}
			else
			{
				return GetCrcForNonZipFile(file);
			}
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

	static unsigned int GetCrcForNonZipFile(const io::FilePath& file)
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


		Logger::warning( "CRC calculated for file %s=%x", file.toString().c_str(), crc );

		fclose(fh);

		return crc;
	}

	static unsigned int GetCrcForZip(const io::FilePath& file)
	{
		// Open the file for reading
		ZipFileReadPtr zipFile = Zip::OpenFileRead(file);

		if (zipFile == NULL) throw std::runtime_error("Could not open ZIP file: " + file.toString());

		unsigned int crc = zipFile->GetCumulativeCrc();

		Logger::warning( "CRC calculated for zip file %s=%x", file.toString().c_str(), crc );

		return crc;
	}
};

} // namespace

#endif /* _TDM_CRC_H_ */
