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

#include "Download.h"
#include "../Http/HttpConnection.h"
#include "../Zip/Zip.h"
#include "../TraceLog.h"
#include "../File.h"
#include "../CRC.h"
#include "../Util.h"
#include "../Constants.h"

#include <boost/bind.hpp>

namespace fs = boost::filesystem;

namespace tdm
{

Download::Download(const HttpConnectionPtr& conn, const std::string& url, const fs::path& destFilename) :
	_curUrl(0),
	_destFilename(destFilename),
	_conn(conn),
	_status(NOT_STARTED_YET),
	_pk4CheckEnabled(false),
	_crcCheckEnabled(false),
	_filesizeCheckEnabled(false),
	_requiredFilesize(0),
	_requiredCrc(0)
{
	_urls.push_back(url);
}

Download::Download(const HttpConnectionPtr& conn, const std::vector<std::string>& urls, const fs::path& destFilename) :
	_urls(urls),
	_curUrl(0),
	_destFilename(destFilename),
	_conn(conn),
	_status(NOT_STARTED_YET),
	_pk4CheckEnabled(false),
	_crcCheckEnabled(false),
	_filesizeCheckEnabled(false),
	_requiredFilesize(0),
	_requiredCrc(0)
{}

Download::~Download()
{
	Stop();
}

Download::DownloadStatus Download::GetStatus()
{
	return _status;
}

bool Download::IsFinished()
{
	return _status == FAILED || _status == SUCCESS;
}

void Download::Start()
{
	if (_status != NOT_STARTED_YET)
	{
		return; // don't allow double starts
	}

	// Construct the temporary filename
	fs::path filename = fs::path(_destFilename).leaf();
	fs::path folder = fs::path(_destFilename).branch_path();

	if (!fs::exists(folder))
	{
		// Make sure the folder exists
		if (!fs::create_directories(folder))
		{
			throw std::runtime_error("Could not create directory: " + folder.string());
		}
	}

	// /path/to/fms/TMP_FILE_PREFIXfilename.pk4 (TMP_FILE_PREFIX is usually an underscore)
	_tempFilename = folder / (TMP_FILE_PREFIX + filename.string());
	TraceLog::WriteLine(LOG_VERBOSE, "Downloading to temporary file " + _tempFilename.string());

	_status = IN_PROGRESS;
	_thread = ThreadPtr(new boost::thread(boost::bind(&Download::Perform, this)));
}

void Download::Stop()
{
	if (_thread != NULL && _request != NULL)
	{
		// Set the URL index beyond the list size to prevent 
		// the worker thread from proceeding to the next URL
		_curUrl = _urls.size();

		// Cancel the request
		_request->Cancel();

		// Wait for the thread to finish
		_thread->join();

		_thread.reset();
		_request.reset();

		// Don't reset successful stati
		if (_status != SUCCESS)
		{
			_status = FAILED;
		}

		// Remove temporary file
		File::Remove(_tempFilename);
	}
}

double Download::GetProgressFraction()
{
	return _request != NULL ? _request->GetProgressFraction() : 0.0;
}

double Download::GetDownloadSpeed()
{
	return _request != NULL ? _request->GetDownloadSpeed() : 0.0;
}

std::size_t Download::GetDownloadedBytes()
{
	return _request != NULL ? _request->GetDownloadedBytes() : 0;
}

void Download::EnableValidPK4Check(bool enable)
{
	_pk4CheckEnabled = enable;
}

void Download::EnableCrcCheck(bool enable)
{
	_crcCheckEnabled = enable;
}

void Download::EnableFilesizeCheck(bool enable)
{
	_filesizeCheckEnabled = enable;
}

void Download::SetRequiredCrc(boost::uint32_t requiredCrc)
{
	_requiredCrc = requiredCrc;
}

void Download::SetRequiredFilesize(std::size_t requiredSize)
{
	_requiredFilesize = requiredSize;
}

void Download::Perform()
{
	while (_curUrl < _urls.size())
	{
		// Remove any previous temporary file
		File::Remove(_tempFilename);

		const std::string& url = _urls[_curUrl];

		// Create a new request
		_request = _conn->CreateRequest(url, _tempFilename.string());
	
		// Start the download, blocks until finished or aborted
		_request->Perform();

		if (_request->GetStatus() == HttpRequest::OK)
		{
			// Check the downloaded file
			bool valid = CheckIntegrity();

			if (!valid)
			{
				_curUrl++;
				continue;
			}
			else
			{
				TraceLog::WriteLine(LOG_VERBOSE, "Downloaded file passed the integrity checks.");
			}

			// Remove the destination filename before moving the temporary file over
			File::Remove(_destFilename);

			// Move temporary file to the real one
			if (File::Move(_tempFilename, _destFilename))
			{
				_status = SUCCESS;
			}
			else
			{
				// Move failed
				_status = FAILED;
			}

			// Download succeeded, exit the loop
			break;
		}
		else 
		{
			// Download error
			if (_request->GetStatus() == HttpRequest::ABORTED)
			{
				TraceLog::WriteLine(LOG_VERBOSE, "Download aborted.");
			}
			else
			{
				TraceLog::WriteLine(LOG_VERBOSE, "Connection Error.");
			}

			// Proceed to the next URL
			_curUrl++;
		}
	} // while

	// Have we run out of URLs
	if (_curUrl >= _urls.size())
	{
		// This was our last URL, set the status to FAILED
		_status = FAILED;
	}
}

const fs::path& Download::GetDestFilename() const
{
	return _destFilename;
}

std::string Download::GetFilename() const
{
	return _destFilename.leaf().string();
}

bool Download::CheckIntegrity()
{
	if (_filesizeCheckEnabled)
	{
		TraceLog::WriteLine(LOG_VERBOSE, (boost::format("Checking filesize of downloaded file, expecting %d") %
				_requiredFilesize).str());

		if (fs::file_size(_tempFilename) != _requiredFilesize)
		{
			TraceLog::WriteLine(LOG_VERBOSE, (boost::format("Downloaded file has the wrong size, expected %d but found %d") %
				_requiredFilesize % fs::file_size(_tempFilename)).str());
			return false; // failed the file size check
		}
	}

	if (_pk4CheckEnabled)
	{
		TraceLog::WriteLine(LOG_VERBOSE, (boost::format("Checking download for 'is-a-zipfile'.")).str());

		ZipFileReadPtr zipFile = Zip::OpenFileRead(_tempFilename);

		if (zipFile == NULL) 
		{
			TraceLog::WriteLine(LOG_VERBOSE, (boost::format("Downloaded file failed the zip check: %s") %
				_tempFilename.string()).str());
			return false; // failed the ZIP check
		}
	}

	if (_crcCheckEnabled)
	{
		TraceLog::WriteLine(LOG_VERBOSE, (boost::format("Checking CRC of downloaded file, expecting %x") %
				_requiredCrc).str());

		boost::uint32_t crc = CRC::GetCrcForFile(_tempFilename);

		if (crc != _requiredCrc)
		{
			TraceLog::WriteLine(LOG_VERBOSE, (boost::format("Downloaded file has the wrong size, expected %x but found %x") %
				_requiredCrc % crc).str());
			return false; // failed the crc check
		}
	}

	return true; // no failed checks, assume OK
}

}
