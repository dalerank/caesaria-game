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

#include <cstdlib>
#include <iostream>

#include "core/logger.hpp"

#include "updateroptions.hpp"
#include "updater.hpp"
#include "packager.hpp"
#include "core/utils.hpp"
#include "util.hpp"

#include "consoleupdater.hpp"

using namespace updater;

int main(int argc, char* argv[])
{
	// Start logging
  Logger::registerWriter( Logger::consolelog, "" );

	Logger::warning( "Updater v%s (c) 2012-2014 by dalerank is"
									 " part of CaesarIA (http://bitbucket.org/dalerank/caesaria).",
									 LIB_UPDATE_VERSION );
	Logger::warning( "\n" );

	UpdaterOptions localOptions( argc, argv );

	if( localOptions.isSet( "verbose") )
	{
		utils::useStackTrace( true );
	}

	if( localOptions.isSet( "update" ) || localOptions.isSet( "release" ))
	{
        std::string basedir = localOptions.get( "directory" );
        std::string version = localOptions.get( "version" );
		Packager p( basedir, version );

		p.createUpdate( localOptions.isSet( "release" ) );

		return 0;
	}
	else
	{
		ConsoleUpdater updater(argc, argv);

		updater.run();

		int exitCode = EXIT_FAILURE;

		switch (updater.GetOutcome())
		{
			case ConsoleUpdater::None:
				// should not happen?
				break;
			case ConsoleUpdater::Failed:
				exitCode = EXIT_FAILURE;
				break;
			case ConsoleUpdater::Ok:
				exitCode = EXIT_SUCCESS;
				break;
			case ConsoleUpdater::OkNeedRestart:
				exitCode = EXIT_SUCCESS;
				break;
		};

		return exitCode;
	}
}
