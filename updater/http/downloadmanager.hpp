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

#ifndef __CAESARIA_DOWNLOADMANAGER_H_INLCUDE__
#define __CAESARIA_DOWNLOADMANAGER_H_INLCUDE__

#include <map>
#include "download.hpp"

namespace updater
{

/**
 * The class handling the actual mission downloads.
 */
class DownloadManager : public ReferenceCounted
{
public:
	class DownloadVisitor
	{
	public:
		// Called per download object, with the assigned ID
		// The visitor should not change any downloads of the manager class
		virtual void Visit(int id, const DownloadPtr& download) = 0;
	};

private:
	// Ongoing downloads
	typedef std::map<int, DownloadPtr> Downloads;
	Downloads _downloads;

	int _nextAvailableId;

	bool _allDownloadsDone;

public:
	DownloadManager();

	void ProcessDownloads();

	void ClearDownloads();

	int AddDownload(const DownloadPtr& download);
	void RemoveDownload(int id);

	DownloadPtr GetDownload(int id);

	// Return the current in-progress download
	DownloadPtr GetCurrentDownload();

	// Return the ID of the current in-progress download
	int GetCurrentDownloadId();

	// Returns true if there is a download already in progress
	bool DownloadInProgress();

	// Returns true if there is a download in progress or waiting for start
	bool HasPendingDownloads();

	// Returns true if one or more downloads have failed status
	bool HasFailedDownloads();

	// Iterate over all registered downloads
	void ForeachDownload(DownloadVisitor& visitor);
};
typedef SmartPtr<DownloadManager> DownloadManagerPtr;

}

#endif //__CAESARIA_DOWNLOADMANAGER_H_INLCUDE__
