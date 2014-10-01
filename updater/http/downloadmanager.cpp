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

#include "downloadmanager.hpp"

#include <iostream>

namespace updater
{

DownloadManager::DownloadManager() :
	_nextAvailableId(1),
	_allDownloadsDone(true)
{}

int DownloadManager::AddDownload(const DownloadPtr& download)
{
	int id = _nextAvailableId++;

	_downloads[id] = download;

	_allDownloadsDone = false;

	return id;
}

DownloadPtr DownloadManager::GetDownload(int id)
{
	Downloads::iterator found = _downloads.find(id);

	return (found != _downloads.end()) ? found->second : DownloadPtr();
}

void DownloadManager::ClearDownloads()
{
	_downloads.clear();
}

bool DownloadManager::DownloadInProgress()
{
  return GetCurrentDownloadId() != -1;
}

bool DownloadManager::HasPendingDownloads()
{
	for (Downloads::const_iterator i = _downloads.begin(); i != _downloads.end(); ++i)
	{
		if (i->second->GetStatus() == Download::IN_PROGRESS ||
			i->second->GetStatus() == Download::NOT_STARTED_YET)
		{
			return true;
		}
	}

	return false;
}

void DownloadManager::RemoveDownload(int id)
{
	Downloads::iterator found = _downloads.find(id);

	if (found != _downloads.end()) 
	{
		_downloads.erase(found);
	}
}

int DownloadManager::GetCurrentDownloadId()
{
	for (Downloads::const_iterator i = _downloads.begin(); i != _downloads.end(); ++i)
	{
		if (i->second->GetStatus() == Download::IN_PROGRESS)
		{
			return i->first;
		}
	}

	return -1;
}

DownloadPtr DownloadManager::GetCurrentDownload()
{
	return GetDownload(GetCurrentDownloadId());
}

void DownloadManager::ProcessDownloads()
{
	if (_allDownloadsDone || _downloads.empty()) 
	{
		return; // nothing to do
	}

	if (DownloadInProgress())
	{
		return; // download still in progress
	}

	// No download in progress, pick a new from the queue
	for (Downloads::const_iterator i = _downloads.begin(); i != _downloads.end(); ++i)
	{
		if (i->second->GetStatus() == Download::NOT_STARTED_YET)
		{
			i->second->Start();
			return;
		}
	}

	// No download left to handle
	_allDownloadsDone = true;
}

bool DownloadManager::HasFailedDownloads()
{
	for (Downloads::const_iterator i = _downloads.begin(); i != _downloads.end(); ++i)
	{
		if (i->second->GetStatus() == Download::FAILED)
		{
			return true;
		}
	}

	return false;
}

void DownloadManager::ForeachDownload(DownloadVisitor& visitor)
{
	for (Downloads::const_iterator i = _downloads.begin(); i != _downloads.end(); ++i)
	{
		visitor.Visit(i->first, i->second);
	}
}

}
