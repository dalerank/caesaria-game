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

#include "httprequest.hpp"
#include "httpconnection.hpp"

#include <cstring>
#include "core/logger.hpp"
#include "core/utils.hpp"
#include "../constants.hpp"

#ifdef CAESARIA_PLATFORM_WIN
#include <winsock2.h> // greebo: need to include winsock2 before curl/curl.h
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif

#include <curl/curl.h>

namespace updater
{

HttpRequest::HttpRequest(HttpConnection& conn, const std::string& url) :
	_conn(conn),
	_url(url),
	_handle(NULL),
	_status(NOT_PERFORMED_YET),
	_cancelFlag(false),
	_progress(0),
	_downloadedBytes(0),
	_showDebugInfo( false )
{}

HttpRequest::HttpRequest(HttpConnection& conn, const std::string& url, vfs::Path destFilename) :
	_conn(conn),
	_url(url),
	_handle(NULL),
	_status(NOT_PERFORMED_YET),
	_destFilename(destFilename),
	_cancelFlag(false),
	_progress(0),
	_downloadedBytes(0),
	_showDebugInfo( false )
{}

void HttpRequest::InitRequest()
{
	// Init the curl session
	_handle = curl_easy_init();

	// specify URL to get
	curl_easy_setopt(_handle, CURLOPT_URL, _url.c_str());

	// Connect the callback
	if( !_destFilename.toString().empty() )
	{
		curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, HttpRequest::WriteFileCallback);
	}
	else
	{
		curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, HttpRequest::WriteMemoryCallback);
	}

	// We pass ourselves as user data pointer to the callback function
	curl_easy_setopt(_handle, CURLOPT_WRITEDATA, this);

	// Set agent
	std::string agent = utils::format( 0xff, "Caesaria Updater v%s/%s",
																									LIB_UPDATE_VERSION,
																									LIB_UPDATE_PLATFORM );
	curl_easy_setopt(_handle, CURLOPT_USERAGENT, agent.c_str());

	// Tels: #3261: only allow FTP, FTPS, HTTP and HTTPS (HTTPS and FTPS need SSL support compiled in)
	curl_easy_setopt(_handle, CURLOPT_PROTOCOLS, CURLPROTO_FTP + CURLPROTO_FTPS + CURLPROTO_HTTP + CURLPROTO_HTTPS);

	// Tels: #3261: allow redirects on the server, with a limit of 10 redirects, and limit
	// 	 the protocols to FTP, FTPS, HTTP, HTTPS to avoid rogue servers giving us random
	//	 things like Telnet or POP3 on random targets.
	curl_easy_setopt(_handle, CURLOPT_FOLLOWLOCATION,			       true);
		curl_easy_setopt(_handle, CURLOPT_MAXREDIRS,					 30 );
	curl_easy_setopt(_handle, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_FTP + CURLPROTO_FTPS + CURLPROTO_HTTP + CURLPROTO_HTTPS);

	// Get the proxy from the HttpConnection class
	if (_conn.HasProxy())
	{
		curl_easy_setopt(_handle, CURLOPT_PROXY, _conn.GetProxyHost().c_str());
		curl_easy_setopt(_handle, CURLOPT_PROXYUSERPWD, (_conn.GetProxyUsername() + ":" + _conn.GetProxyPassword()).c_str());
	}
}

void HttpRequest::Perform()
{
	_errorMessage.clear();

	if( _showDebugInfo )
	{
		Logger::update( "Download from " + _url, true );
	}

	InitRequest();

	_progress = 0;
	_status = IN_PROGRESS;

	// Check target file
	if (!_destFilename.toString().empty())
	{
		_destStream.open(_destFilename.toString().c_str(), std::ofstream::out|std::ofstream::binary);
	}

	CURLcode result = curl_easy_perform(_handle);

	if (!_destFilename.toString().empty())
	{
		_destStream.flush();
		_destStream.close();
	}

	if (_cancelFlag)
	{
		_status = ABORTED;
	}
	else
	{
		switch (result)
		{
		case CURLE_OK:
			_status = OK;
			_progress = 1.0;
			Logger::update( "  OK" );
			break;
		default:
			_status = FAILED;
			_errorMessage = curl_easy_strerror(result);
			Logger::warning( "  FAILED" );
			Logger::warning( "ERROR " + _errorMessage );
		};
	}

	curl_easy_cleanup(_handle);

	_handle = NULL;
}

void HttpRequest::Cancel()
{
	// The memory callback will catch this flag
	_cancelFlag = true;
}

HttpRequest::RequestStatus HttpRequest::GetStatus()
{
	return _status;
}

std::string HttpRequest::GetErrorMessage()
{
	return _errorMessage;
}

double HttpRequest::GetProgressFraction()
{
	return _progress;
}

double HttpRequest::GetDownloadSpeed()
{
	return _downloadSpeed;
}

std::size_t HttpRequest::GetDownloadedBytes()
{
	return _downloadedBytes;
}

std::string HttpRequest::GetResultString()
{
	return _buffer.empty() ? "" : std::string(&_buffer.front());
}

void HttpRequest::UpdateProgress()
{
	double size;
	double downloaded;
	CURLcode result = curl_easy_getinfo(_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);

	if (result != CURLE_OK) 
	{
		_progress = 0;
		return;
	}

	result = curl_easy_getinfo(_handle, CURLINFO_SIZE_DOWNLOAD, &downloaded);

	if (result != CURLE_OK) 
	{
		_progress = 0;
		return;
	}

	_downloadedBytes = static_cast<std::size_t>(downloaded);

	_progress = downloaded / size;

	if (_progress > 1.0)
	{
		_progress = 1.0;
	}

	result = curl_easy_getinfo(_handle, CURLINFO_SPEED_DOWNLOAD, &_downloadSpeed);

	if (result != CURLE_OK) 
	{
		_downloadSpeed = 0;
		return;
	}
}

size_t HttpRequest::WriteMemoryCallback(void* ptr, size_t size, size_t nmemb, HttpRequest* self)
{
	// Needed size
	std::size_t bytesToCopy = size * nmemb;

	// Add to the "total bytes downloaded" counter, regardless of whether we're saving it or not
	self->_conn.AddBytesDownloaded(bytesToCopy);

	if (self->_cancelFlag)
	{
		return 0; // cancel the process
	}

	std::vector<char>& buf = self->_buffer; // shortcut 

	std::size_t appendPosition = buf.size() > 0 ? buf.size() - 2 : 0;

	// The first allocation should request one extra byte for the trailing \0
	self->_buffer.resize(buf.size() > 0 ? buf.size() + bytesToCopy : bytesToCopy + 1);

	// Push the bytes
	std::memcpy(&(buf[appendPosition]), ptr, bytesToCopy);

	// Append trailing \0 if possible
	if (buf.size() > 0)
	{
		buf[buf.size() - 1] = 0;
	}

	self->UpdateProgress();

	return static_cast<size_t>(bytesToCopy);
}

size_t HttpRequest::WriteFileCallback(void* ptr, size_t size, size_t nmemb, HttpRequest* self)
{
	// Needed size
	std::size_t bytesToCopy = size * nmemb;

	// Add to the "total bytes downloaded" counter, regardless of whether we're saving it or not
	self->_conn.AddBytesDownloaded(bytesToCopy);

	if (self->_cancelFlag)
	{
		return 0; // cancel the process
	}

	self->_destStream.write( (const char*)ptr, bytesToCopy);

	self->UpdateProgress();

	return static_cast<size_t>(bytesToCopy);
}

}
