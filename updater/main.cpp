/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5793 $ (Revision of last commit) 
 $Date: 2013-05-15 15:45:36 +0400 (Ср, 15 май 2013) $ (Date of last commit)
 $Author: tels $ (Author of last commit)
 
******************************************************************************/

#include <cstdlib>
#include <iostream>

#include "core/logger.hpp"

#include "Updater/UpdaterOptions.h"
#include "Updater/Updater.h"
#include "Packager/Packager.h"

#include "Util.h"

#include "ConsoleUpdater.h"

using namespace tdm;
using namespace updater;

int main(int argc, char* argv[])
{
	// Start logging
	Logger::registerWriter( Logger::consolelog );

	Logger::warning( "Updater v%s (c) 2012-2013 by dalerank is"
									 "part of CaesarIA (http://github.com/gecube/opencaesar3).",
									 "0.0.1");
	Logger::warning( "" );

	UpdaterOptions localOptions( argc, argv );

	if( localOptions.isSet( "update" ) || localOptions.isSet( "release" ))
	{
		std::string basedir = localOptions.Get( "directory" );
		std::string version = localOptions.Get( "version" );
		Packager p( basedir, version );

		p.createUpdate( localOptions.isSet( "release" ) );

		return 0;
	}
	else
	{
		ConsoleUpdater updater(argc, argv);

		updater.Run();

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
