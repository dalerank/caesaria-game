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

#include "util.hpp"

#include "vfs/path.hpp"
#include "core/logger.hpp"
#include "vfs/entries.hpp"
#include "vfs/directory.hpp"
#include "core/foreach.hpp"

#ifdef CAESARIA_PLATFORM_WIN

#include <string>
#include <windows.h>
#include <psapi.h>

namespace updater
{

bool Util::caesariaIsRunning()
{
	DWORD processes[1024];
	DWORD num;

	if (!EnumProcesses(processes, sizeof(processes), &num))
	{
		return false;
	}

	// Iterate over the processes
	for (int i = 0; i < int(num/sizeof(DWORD)); i++)
	{
		char szProcessName[MAX_PATH] = "unknown";

		// Get the handle for this process
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, processes[i]);

		if (hProcess)
		{
			HMODULE hMod;
			DWORD countBytes;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &countBytes))
			{
				GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName));

				std::string processName(szProcessName);

				// grayman - This was checking for "Doom3.exe". Starting with 1.08, the D3
				// executable is no longer needed to run TDM, so we'll check for TheDarkMod.exe
				// instead.

				if (processName == "TheDarkMod.exe")
				{
					// At this point, we know we need to quit. There's no longer a need
					// to check for "gamex86.dll".

/*					HMODULE hModules[1024];
					DWORD cbNeeded;

					if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded))
					{
						for (unsigned int m = 0; m < (cbNeeded / sizeof(HMODULE)); m++)
						{
							TCHAR szModName[MAX_PATH];

							// Get the full path to the module's file.
							if (GetModuleBaseName(hProcess, hModules[m], szModName, sizeof(szModName)/sizeof(TCHAR)))
							{
								// Print the module name and handle value.
								if (std::string(szModName) == "gamex86.dll")
								{
									CloseHandle(hProcess); // close the handle, we're terminating

									return true;
								}
							}
						}
					}
 */
					// instead, quit

					CloseHandle(hProcess); // close the handle, we're terminating

					return true;
				}
			}
		}

		CloseHandle(hProcess);
	}

	return false;
}
    
} // namespace

#elif defined(CAESARIA_PLATFORM_LINUX) || defined(CAESARIA_PLATFORM_HAIKU)
// Linux implementation

#include <iostream>
#include <fstream>
#include <string>

namespace updater
{

namespace
{
	const std::string systemProcFolder("/proc/");
	const std::string caesariaProcessName("caesaria.linux"); // grayman - looking for tdm now instead of doom3

	bool CheckProcessFile(const std::string& name, const std::string& processName)
	{
		// Try to cast the filename to an integer number (=PID)
		try
		{
			unsigned long pid = utils::toUint( name );
		
			// Was the PID read correctly?
			if (pid == 0)
			{
				return false;
			}
			
			const std::string cmdLineFileName = systemProcFolder + name + "/cmdline";
			
			std::ifstream cmdLineFile(cmdLineFileName.c_str());

			if (cmdLineFile.is_open())
			{
				// Read the command line from the process file
				std::string cmdLine;
				getline(cmdLineFile, cmdLine);
				
				if (cmdLine.find(processName) != std::string::npos)
				{
					// Process found, return success
					return true;
				}
			}
			
			// Close the file
			cmdLineFile.close();
		}
		catch( ... )
		{
			// Cast to int failed, no PID
		}

		return false;
	}

} // namespace

bool Util::caesariaIsRunning()
{
  // Traverse the /proc folder, this sets the flag to TRUE if the process was found
  vfs::Entries procs = vfs::Directory(systemProcFolder).getEntries();

  foreach( i, procs )
  {
    if( CheckProcessFile( i->name.toString(), caesariaProcessName) ) // grayman - looking for caesaria
    {
      return true;
    }
  }
	
  return false;
}

} // namespace

#elif defined(CAESARIA_PLATFORM_MACOSX)
// Mac OS X
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/logger.hpp"
#include <sys/sysctl.h>

namespace updater
{

// greebo: Checks for a named process, modeled loosely after
// http://developer.apple.com/library/mac/#qa/qa2001/qa1123.html
bool FindProcessByName(const char* processName)
{
    int name[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t length = 0;
	
	// Call sysctl with a NULL buffer.
	int err = sysctl(name, 4, NULL, &length, NULL, 0);
	
	if (err == -1)
	{
        Logger::warning("Failed to receive buffer size for process list.");
		return false;
	}
	
    kinfo_proc* procList = static_cast<kinfo_proc*>(malloc(length));
	
	if (procList == NULL)
	{
        Logger::warning( "Out of Memory trying to allocate process buffer");
		return false;
	}
	
	// Load process info
	sysctl(name, 4, procList, &length, NULL, 0);
	
	size_t procCount = length / sizeof(kinfo_proc);
	bool result = false;
	
	for (size_t i = 0; i < procCount; ++i)
	{
		//TraceLog::WriteLine(LOG_STANDARD, procList[i].kp_proc.p_comm);
		
		if (strcmp(procList[i].kp_proc.p_comm, processName) == 0)
		{
			result = true;
			break;
		}
	}
	
	free(procList);
	
	return result;
}

bool Util::caesariaIsRunning()
{
	return FindProcessByName("caesaria.macosx"); // grayman - look for caesaria
}

} // namespace

#else
#error Unsupported Platform
#endif
