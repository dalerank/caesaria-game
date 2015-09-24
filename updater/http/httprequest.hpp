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

#ifndef __CAESARIA_HTTPREQUEST_H_INLCUDE__
#define __CAESARIA_HTTPREQUEST_H_INLCUDE__

#include <fstream>
#include <vector>
#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"
#include "vfs/path.hpp"

typedef void CURL;

namespace updater
{

class HttpConnection;

/**
 * greebo: An object representing a single HttpRequest, holding 
 * the result (string) and status information.
 *
 * Use the Perform() method to execute the request.
 */
class HttpRequest : public ReferenceCounted
{
public:

	enum RequestStatus
	{
		NOT_PERFORMED_YET,
		OK,	// successful
		IN_PROGRESS,
		FAILED,
		ABORTED
	};

private:
	// The connection we're working with
	HttpConnection& _conn;

	// The URL we're supposed to query
	std::string _url;

	std::vector<char> _buffer;

	// The curl handle
	CURL* _handle;

	// The current state
	RequestStatus _status;

	vfs::Path _destFilename;

	std::ofstream _destStream;

	// True if we should cancel the download
	bool _cancelFlag;

	double _progress;
	double _downloadSpeed;

	std::size_t _downloadedBytes;

	std::string _errorMessage;
	bool _showDebugInfo;

public:
	HttpRequest(HttpConnection& conn, const std::string& url);

	HttpRequest(HttpConnection& conn, const std::string& url, vfs::Path destFilename); // TODO: Change to fs::path

	// Callbacks for CURL
	static size_t WriteMemoryCallback(void* ptr, size_t size, size_t nmemb, HttpRequest* self);
	static size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, HttpRequest* self);

  RequestStatus status() const;

	// Perform the request
	void execute();

  void cancel();

	// Between 0.0 and 1.0
	double GetProgressFraction();

	// in bytes/second
	double GetDownloadSpeed();

	// Number of bytes received
	std::size_t GetDownloadedBytes();

	// Returns the result string
	std::string GetResultString();

	// If GetStatus == FAILED, this holds the curl error
	std::string GetErrorMessage();

  bool isOk() const;

private:
	void InitRequest();

	void UpdateProgress();
};
typedef SmartPtr<HttpRequest> HttpRequestPtr;

}

#endif //__CAESARIA_HTTPREQUEST_H_INLCUDE__
