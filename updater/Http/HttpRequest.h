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

#include <fstream>
#include <vector>
#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"

typedef void CURL;

namespace tdm
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
		ABORTED,
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

	std::string _destFilename;

	std::ofstream _destStream;

	// True if we should cancel the download
	bool _cancelFlag;

	double _progress;
	double _downloadSpeed;

	std::size_t _downloadedBytes;

	std::string _errorMessage;

public:
	HttpRequest(HttpConnection& conn, const std::string& url);

	HttpRequest(HttpConnection& conn, const std::string& url, const std::string& destFilename); // TODO: Change to fs::path

	// Callbacks for CURL
	static size_t WriteMemoryCallback(void* ptr, size_t size, size_t nmemb, HttpRequest* self);
	static size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, HttpRequest* self);

	RequestStatus GetStatus();

	// Perform the request
	void Perform();

	void Cancel();

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

private:
	void InitRequest();

	void UpdateProgress();
};
typedef SmartPtr<HttpRequest> HttpRequestPtr;

}
