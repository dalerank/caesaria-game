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

#ifndef __CAESARIA_UPDATER_UTIL_H_INCLUDE__
#define __CAESARIA_UPDATER_UTIL_H_INCLUDE__

#include "core/utils.hpp"
#include "vfs/path.hpp"

// Platform-specific Sleep(int msec) definition
#ifdef CAESARIA_PLATFORM_WIN
	#include <windows.h>
#else
	// Linux doesn't know Sleep(), add a substitute def
	#include <unistd.h>
	#define Sleep(x) usleep(static_cast<int>(1000 * (x)))
#endif 

namespace updater
{

/**
 * Generic helper classes
 */
class Util
{
  static const std::size_t KbBts = 1024;
  static const std::size_t MbBts = 1024 * 1024;
  static const std::size_t GbBts = 1024 * 1024 * 1024;
public:
	// Formats the given number in bytes/kB/MB/GB
  static std::string getHumanReadableBytes(std::size_t size);

  static void Wait(int millisecs)
  {
    Sleep(millisecs);
  }

  // Platform-dependent process check routine (grayman - searches for caesaria executable among active processes)
  static bool caesariaIsRunning();
};

} // end namespace updater


#endif //__CAESARIA_UPDATER_UTIL_H_INCLUDE__
