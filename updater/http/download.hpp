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

#ifndef __CAESARIA_UPDATER_DOWNLOAD_H_INLCUDE__
#define __CAESARIA_UPDATER_DOWNLOAD_H_INLCUDE__

#include "httpconnection.hpp"
#include "httprequest.hpp"
#include "vfs/path.hpp"
#include "core/smartptr.hpp"
#include "thread/safethread.hpp"

namespace updater
{

/**
 * An object representing a single download.
 * The actual work will be performed in a separate thread,
 * hence the Start() method will not block execution.
 *
 * Check the GetStatus() and GetProgressFraction() methods
 * to get some information about the actual status.
 * 
 * Once the Download object is destroyed any ongoing 
 * worker thread will be terminated.
 *
 * Download data is stored in a temporary file until the whole
 * file is finished - then the temporary will be renamed to
 * the destination filename. If the destination filename is already
 * in use, the move will fail and the download will remain
 * in the temporary file. The temporary file is named the same
 * as the destination filename, but with a prefixed underscore character:
 * e.g. target/directory/_download.pk4
 */
class Download : public ReferenceCounted
{
protected:
	// The target URLs we're trying to download from
	std::vector<std::string> _urls;

	// The URL index
	std::size_t _curUrl;

	// destination file
	vfs::Path _destFilename;

	// Temporary filename for partial download data
	vfs::Path _tempFilename;

public:
	enum DownloadStatus
	{
		NOT_STARTED_YET,
		IN_PROGRESS,
		FAILED,
		SUCCESS
	};

protected:
	HttpConnectionPtr _conn;

	DownloadStatus _status;

	// The corresponding HTTP request
	HttpRequestPtr _request;

  threading::SafeThreadPtr _thread;

	bool _crcCheckEnabled;
	bool _filesizeCheckEnabled;

	std::size_t _requiredFilesize;
	std::size_t _requiredCrc;

public:
	/** 
	 * greebo: Construct a new Download using the given URL.
	 * The download data will be saved to the given destFilename;
	 */
	Download(const HttpConnectionPtr& conn, const std::string& url, vfs::Path destFilename);

	/**
	 * greebo: Construct a new Download using the given list of 
	 * alternative URLs. If downloading from the first URL fails
	 * the next one will be tried until no more alternatives are left.
	 * The result will be saved to destFilename.
	 */
	Download(const HttpConnectionPtr& conn, const std::vector<std::string>& urls, vfs::Path destFilename);

	virtual ~Download();

	// Start the download. This will spawn a new thread and execution
	// will return to the calling code.
	void Start();

	// Cancel the download. If the download has already finished, this does nothing.
	void Stop();

	// Enable or disable CRC validation after download
  void enableCrcCheck(bool enable);

	// Enable or disable the filesize check after download
  void enableFilesizeCheck(bool enable);

	// Set the required CRC for this download
  void setRequiredCrc(unsigned int requiredCrc);

	// Set the required filesize for this download
  void setRequiredFilesize(std::size_t requiredSize);
	
	// The current status of this download
	DownloadStatus GetStatus();

	// Returns true if the download is done (successful or not)
	bool IsFinished();

	// Get the progress fraction [0..1]
	double GetProgressFraction();

	// Get the download speed (bytes/sec)
	double GetDownloadSpeed();

	// Get the downloaded bytes
	std::size_t GetDownloadedBytes();

	// Return the destination filename of this download
	vfs::Path GetDestFilename() const;

	// Get the destination filename (leaf) of the current download (remote filename)
	std::string GetFilename() const;

protected:
	// Thread entry point
  void perform(bool& continues);

	// Check method
	bool checkIntegrity();
};
typedef SmartPtr<Download> DownloadPtr;

}

#endif //__CAESARIA_UPDATER_DOWNLOAD_H_INLCUDE__
