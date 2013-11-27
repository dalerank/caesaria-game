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

#ifndef __CAESARIA_MIRRORDOWNLOAD_H_INLCUDE__
#define __CAESARIA_MIRRORDOWNLOAD_H_INLCUDE__

#include <algorithm>
#include <list>
#include "download.hpp"
#include "mirrorlist.hpp"

namespace updater
{

/**
 * This extends the threaded Download class by adding
 * support for a weighted list of mirrors.
 */
class MirrorDownload : public Download
{
private:
	// The ordered list of mirrors, same order as the protected _url;
	std::vector<Mirror> _mirrors;

public:
	/**
	 * Construct a MirrorDownload by passing the connection to use,
	 * the list of available mirrors as well as the filename which is 
	 * appended to the respective mirror URL, e.g. "tdm_ai_animals01.pk4".
	 * 
	 * After successful download, the file will be moved to the given
	 * destination file.
	 * 
	 * If no more mirrors are availabe, the status will be FAILED.
	 */
	MirrorDownload(const HttpConnectionPtr& conn, 
				   const MirrorList& mirrors, 
				   const std::string& srcFilename,
					 vfs::Path destFilename);
	
	// Get the display name of the currently chosen mirror
	std::string GetCurrentMirrorName();

	// Called by the Updater
	static void InitRandomizer();
};
typedef SmartPtr<MirrorDownload> MirrorDownloadPtr;

} // namespace

#endif //__CAESARIA_MIRRORDOWNLOAD_H_INLCUDE__
