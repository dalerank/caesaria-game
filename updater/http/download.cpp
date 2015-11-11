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

#include "download.hpp"
#include "../http/httpconnection.hpp"
#include "vfs/directory.hpp"
#include "../CRC.h"
#include "../util.hpp"
#include "../constants.hpp"

#include "core/logger.hpp"
#include "core/delegate.hpp"
#include "vfs/file.hpp"

namespace updater
{

Download::Download(const HttpConnectionPtr& conn, const std::string& url, vfs::Path destFilename) :
	_curUrl(0),
	_destFilename(destFilename),
	_conn(conn),
	_status(NOT_STARTED_YET),
	_crcCheckEnabled(false),
	_filesizeCheckEnabled(false),
	_requiredFilesize(0),
	_requiredCrc(0)
{
	_urls.push_back(url);
}

Download::Download(const HttpConnectionPtr& conn, const std::vector<std::string>& urls, vfs::Path destFilename) :
	_urls(urls),
	_curUrl(0),
	_destFilename(destFilename),
	_conn(conn),
	_status(NOT_STARTED_YET),
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
	vfs::Path filename = _destFilename.baseName();
	vfs::Directory folder = _destFilename.directory();

	if ( !folder.exist() )
	{
		// Make sure the folder exists
		if( !vfs::Directory::createByPath( folder ) )
		{
			throw std::runtime_error("Could not create directory: " + folder.toString());
		}
	}

	// /path/to/fms/TMP_FILE_PREFIXfilename.pk4 (TMP_FILE_PREFIX is usually an underscore)
	_tempFilename = folder.getFilePath( TEMP_FILE_PREFIX + filename.toString() );
	//Logger::warning(  "Downloading to temporary file " + _tempFilename.toString() );

	_status = IN_PROGRESS;
  auto p = threading::SafeThread::create( threading::SafeThread::WorkFunction( this, &Download::perform ) );
	_thread = p;
}

void Download::Stop()
{
	if (_thread != NULL && _request != NULL)
	{
		// Set the URL index beyond the list size to prevent 
		// the worker thread from proceeding to the next URL
		_curUrl = _urls.size();

		// Cancel the request
    _request->cancel();

    _thread = threading::SafeThreadPtr();
		_request = HttpRequestPtr();

		// Don't reset successful stati
		if (_status != SUCCESS)
		{
			_status = FAILED;
		}

		// Remove temporary file
		vfs::NFile::remove( _tempFilename );
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

void Download::enableCrcCheck(bool enable)
{
	_crcCheckEnabled = enable;
}

void Download::enableFilesizeCheck(bool enable)
{
	_filesizeCheckEnabled = enable;
}

void Download::setRequiredCrc(unsigned int requiredCrc)
{
	_requiredCrc = requiredCrc;
}

void Download::setRequiredFilesize(std::size_t requiredSize)
{
	_requiredFilesize = requiredSize;
}

void Download::perform( bool& continues )
{
	while (_curUrl < _urls.size())
	{
		// Remove any previous temporary file
		vfs::NFile::remove( _tempFilename );

		const std::string& url = _urls[_curUrl];

		// Create a new request
    _request = _conn->request(url, _tempFilename.toString());
	
		// Start the download, blocks until finished or aborted
    _request->execute();

    if (_request->status() == HttpRequest::OK)
		{
			// Check the downloaded file
			bool valid = checkIntegrity();

			if (!valid)
			{
				_curUrl++;
				continue;
			}
			else
			{
				Logger::update( " CRC");
			}

			// Remove the destination filename before moving the temporary file over
			vfs::NFile::remove( _destFilename );

			// Move temporary file to the real one
			if( vfs::NFile::rename( _tempFilename, _destFilename ) )
			{
				Logger::update( " REPLACE" );
				_status = SUCCESS;
			}
			else
			{
				// Move failed
        Logger::warning( "\nFailed renamed {} to {} ", _tempFilename.toCString(),
                                                     _destFilename.toCString() );
				_status = FAILED;
			}

			// Download succeeded, exit the loop
			break;
		}
		else 
		{
			// Download error
      if (_request->status() == HttpRequest::ABORTED)
			{
				Logger::warning(  "Download aborted.");
			}
			else
			{
				Logger::warning(  "Connection Error.");
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

  continues = false;
}

vfs::Path Download::GetDestFilename() const
{
	return _destFilename;
}

std::string Download::GetFilename() const
{
	return _destFilename.baseName().toString();
}

bool Download::checkIntegrity()
{
	if (_filesizeCheckEnabled)
	{
		//Logger::warning( "Checking filesize of downloaded file, expecting %d", _requiredFilesize);

		if( vfs::NFile::size(_tempFilename) != _requiredFilesize)
		{
      Logger::warning( "Downloaded file has the wrong size, expected {} but found {}",
											 _requiredFilesize,
											 vfs::NFile::size(_tempFilename) );
			return false; // failed the file size check
		}
		else
		{
			Logger::update( " SIZEOK" );
		}
	}

	if (_crcCheckEnabled)
	{
		//Logger::warning( "Checking CRC of downloaded file, expecting %x", _requiredCrc );

		unsigned int crc = CRC::GetCrcForFile(_tempFilename);

		if (crc != _requiredCrc)
		{
      Logger::warning( "Downloaded file has the wrong crc, expected {} but found {}", _requiredCrc, crc );
			return false; // failed the crc check
		}
		else
		{
			Logger::update( " CRCOK" );
		}
	}

	return true; // no failed checks, assume OK
}

}
