/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5841 $ (Revision of last commit) 
 $Date: 2013-08-07 06:44:10 +0400 (Ср, 07 авг 2013) $ (Date of last commit)
 $Author: grayman $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include "core/stringhelper.hpp"
#include "vfs/filepath.hpp"

// Platform-specific Sleep(int msec) definition
#ifdef CAESARIA_PLATFORM_WIN
	#include <windows.h>
#else
	// Linux doesn't know Sleep(), add a substitute def
	#include <unistd.h>
	#define Sleep(x) usleep(static_cast<int>(1000 * (x)))
#endif 

namespace tdm
{

/**
 * Generic helper classes
 */
class Util
{
public:
	// Formats the given number in bytes/kB/MB/GB
	static std::string GetHumanReadableBytes(std::size_t size)
	{
		if (size > 1024*1024*1024)
		{
			return StringHelper::format( 0xff, "%0.2f GB", size / (1024*1024*1024.f) );
		}
		else if (size > 1024*1024)
		{
			return  StringHelper::format( 0xff, "%0.1f MB", size / (1024*1024.f) );
		}
		else if (size > 1024)
		{
			return  StringHelper::format( 0xff, "%0.0f kB", size / 1024.f );
		}
		else
		{
			return  StringHelper::format( 0xff, "%d bytes", size);
		}
	}

	/**
	 * greebo: Checks if the current path is the one TheDarkMod.exe is located in.
	 * This is used to determine whether this is a clean installation attempt
	 * and the user downloaded the tdm_update.exe into the wrong folder.

	 * grayman - check for The Dark Mod, not D3
	 * grayman - no longer necessary for 2.00.
	 */
/*
	static bool PathIsTDMEnginePath(const fs::path& path)
	{

#if WIN32
		std::string tdmExecutableName = "TheDarkMod.exe";
#else 
		std::string tdmExecutableName = "thedarkmod.x86";
#endif

		if (fs::exists(path / tdmExecutableName))
		{
			return true;
		}

		return false;
	}
	*/

	static void Wait(int millisecs)
	{
		Sleep(millisecs);
	}

	// Platform-dependent process check routine (grayman - searches for TheDarkMod executable among active processes)
	static bool TDMIsRunning();

	// Platform-dependent process check routine (searches for the DarkRadiant executable among active processes)
	static bool DarkRadiantIsRunning();
};

} // namespace
