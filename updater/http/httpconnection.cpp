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

#include "httpconnection.hpp"
#include "httprequest.hpp"

#ifdef CAESARIA_PLATFORM_WIN
  #include <winsock2.h> // greebo: need to include winsock2 before curl/curl.h
  #include <ws2tcpip.h>
  #include <wspiapi.h>
#endif

#include <curl/curl.h>
#include "thread/thread.hpp"
#include "vfs/path.hpp"

namespace updater
{

class HttpConnection::Impl
{
public:
	// The mutex for managing access to the counter above
  std::mutex bytesDownloadedMutex;
};

HttpConnection::HttpConnection() : _d( new Impl ),
	_bytesDownloaded(0)
{
	curl_global_init(CURL_GLOBAL_ALL);
}

HttpConnection::~HttpConnection()
{
	// Clean up cURL
	curl_global_cleanup();
}

bool HttpConnection::HasProxy()
{
	return !_proxyHost.empty();
}

std::string HttpConnection::GetProxyHost()
{
	return _proxyHost;
}

std::string HttpConnection::GetProxyUsername()
{
	return _proxyUser;
}

std::string HttpConnection::GetProxyPassword()
{
	return _proxyPass;
}

void HttpConnection::SetProxyHost(const std::string& host)
{
	_proxyHost = host;
}

void HttpConnection::SetProxyUsername(const std::string& user)
{
	_proxyUser = user;
}

void HttpConnection::SetProxyPassword(const std::string& pass)
{
	_proxyPass = pass;
}

HttpRequestPtr HttpConnection::createRequest(const std::string& url)
{
	return HttpRequestPtr(new HttpRequest(*this, url));
}

HttpRequestPtr HttpConnection::createRequest(const std::string& url, vfs::Path destFilename)
{
	return HttpRequestPtr(new HttpRequest(*this, url, destFilename));
}

void HttpConnection::AddBytesDownloaded(std::size_t bytes)
{
	// Make sure only one thread is accessing the counter at a time
  std::unique_lock<std::mutex> locker(_d->bytesDownloadedMutex);
  locker;

	_bytesDownloaded += bytes;
}

std::size_t HttpConnection::GetBytesDownloaded() const
{
	return _bytesDownloaded;
}

}
