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

#ifndef __CAESARIA_UPDATERCONSTANTS_H_INLCUDE__
#define __CAESARIA_UPDATERCONSTANTS_H_INLCUDE__

#include "core/platform.hpp"

namespace updater
{

const char* const LIBTDM_UPDATE_VERSION = "0.61";

const char* const LIBTDM_UPDATE_PLATFORM = 
#ifdef CAESARIA_PLATFORM_WIN
"Win32"
#elif defined(CAESARIA_PLATFORM_LINUX)
"Linux"
#elif defined(CAESARIA_PLATFORM_MACOSX)
"MacOSX"
#else
"Unknown"
#endif
;

const char* const TDM_MIRRORS_FILE = "mirrors.txt";
const char* const TDM_MIRRORS_SERVER = "http://sourceforge.net/projects/opencaesar3/files/update/";

// The file containing the version information of the most up-to-date package
const char* const STABLE_VERSION_FILE = "stable_info.txt";
const char* const TDM_UDPATE_INFO_FILE = "tdm_update_info.txt";

// The file containing the version information of all released packages since 1.02
const char* const UPDATE_VERSION_FILE = "version_info.txt";

#ifdef WIN32
const char* const TDM_UPDATE_UPDATER_BATCH_FILE = "tdm_update_updater.cmd";
#else
const char* const TDM_UPDATE_UPDATER_BATCH_FILE = "tdm_update_updater.sh";
#endif

// The standard mod path below D3
const char* const TDM_STANDARD_MOD_FOLDER = "darkmod";

// The standard manifest folder, relative to darkmod
const char* const TDM_MANIFEST_PATH = "devel/manifests/";
const char* const TDM_MANIFEST_EXTENSION = ".txt";

const char* const TMP_FILE_PREFIX = "__";

} // namespace

#endif //__CAESARIA_UPDATERCONSTANTS_H_INLCUDE__
