/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5122 $ (Revision of last commit) 
 $Date: 2011-12-11 23:47:31 +0400 (Вс, 11 дек 2011) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include <map>
#include "Download.h"

namespace tdm
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
