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

#ifndef __CAESARIA_UPDATER_OPTIONS_H_INLCUDE__
#define __CAESARIA_UPDATER_OPTIONS_H_INLCUDE__

#include "../programoptions.hpp"

namespace updater
{

class UpdaterOptions :
	public ProgramOptions
{
public:
	UpdaterOptions()
	{
		SetupDescription();
	}

	// Construct options from command line arguments
	UpdaterOptions(int argc, char* argv[])
	{
		SetupDescription();
		ParseFromCommandLine(argc, argv);

		for (int i = 1; i < argc; ++i)
		{
			_cmdLineArgs.push_back(argv[i]);
		}
	}

	void CheckProxy(const HttpConnectionPtr& conn) const
	{
		std::string proxyStr = Get( "--proxy" );

		if( proxyStr.empty() )
		{
			Logger::warning( "No proxy configured.");
			return; // nothing to do
		}

		std::string proxyUser = Get( "--proxyuser" );
		std::string proxyPassword = Get( "--proxypass" );
		if( proxyUser.empty() && proxyPassword.empty() )
		{
			// Non-authenticated proxy
			Logger::warning( "Using proxy: %s", proxyStr.c_str() );
			conn->SetProxyHost( proxyStr );
		}
		else
		{
			// Proxy with authentication
			Logger::warning( "Using proxy with authentication: %s", proxyStr.c_str() );
			conn->SetProxyHost( proxyStr );
			conn->SetProxyUsername( proxyUser );
			conn->SetProxyPassword( proxyPassword );
		}
	}

private:
	// Implement base class method
	void SetupDescription()
	{
		// Get options from command line
		_desc[ "--proxy" ] = "Use a proxy to connect to the internet, example --proxy http://proxy:port";
		_desc[ "--proxyuser" ] = "Use a proxy to connect to the internet, example --proxyuser user";
		_desc[ "--proxypass" ] = "Use a proxy to connect to the internet, example --proxypass pass";
		_desc[ "--targetdir" ] = "The folder which should be updated.--targetdir c:\\games\\caesaria";
		_desc[ "--help" ] = "Display this help page";
		_desc[ "--keep-mirrors" ] = "Don't download updated mirrors list from the server, use local one.";
		_desc[ "--keep-update-packages" ] = "Don't delete downloaded update packages after applying them.";
		_desc[ "--noselfupdate" ] = "Don't perform any special 'update the updater' routines.";
		_desc[ "--dry-run" ] = "Don't do any updates, just perform checks.";
		_desc[ "--release" ] = "Create stable_info.txt for this configuration";
		_desc[ "--directory" ] = "Use only in release/update mode, path to working directory";
		_desc[ "--version" ] = "Use only in releases/udate mode, current version";
	}
};

}

#endif //__CAESARIA_UPDATER_OPTIONS_H_INLCUDE__
