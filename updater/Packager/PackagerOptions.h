/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5767 $ (Revision of last commit) 
 $Date: 2013-05-13 13:53:23 +0400 (Пн, 13 май 2013) $ (Date of last commit)
 $Author: tels $ (Author of last commit)
 
******************************************************************************/

#pragma once

#include "../ProgramOptions.h"

namespace tdm
{

namespace packager
{

class PackagerOptions :
	public ProgramOptions
{
public:
	// Construct options from command line arguments
	PackagerOptions(int argc, char* argv[])
	{
		SetupDescription();
		ParseFromCommandLine(argc, argv);

		for (int i = 1; i < argc; ++i)
		{
			_cmdLineArgs.push_back(argv[i]);
		}
	}

	void PrintHelp()
	{
		ProgramOptions::PrintHelp();

		// Add some custom examples
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "Command: --create-manifest --darkmoddir=c:/games/tdm/darkmod [--release-name=darkmod]");
		TraceLog::WriteLine(LOG_STANDARD, " The parameter 'darkmoddir' is mandatory, the optional --release-name parameter specifies the name of the package (defaults to 'darkmod').");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "Command: --create-update-package");
		TraceLog::WriteLine(LOG_STANDARD, " The parameters 'basedir', 'headdir', 'baseversion', 'headversion' and 'outputdir' are mandatory.");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "Command: --update-version-info-file");
		TraceLog::WriteLine(LOG_STANDARD, " The parameters 'basedir', 'baseversion' and 'outputdir' are mandatory.");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "Examples:");
		TraceLog::WriteLine(LOG_STANDARD, "-------------------------------------------------------------------------------------");
		TraceLog::WriteLine(LOG_STANDARD, " tdm_package --create-package [--name=darkmod] --darkmoddir=c:/games/tdm/darkmod --outputdir=d:/temp/package");
		TraceLog::WriteLine(LOG_STANDARD, " This will create a full release PK4 set in the specified output directory. The name argument is optional and will default to 'darkmod'. The darkmoddir parameter specifies the darkmod SVN repository. The manifest will be taken from <darkmoddir>/devel/manifests/<releasename>.txt");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "-------------------------------------------------------------------------------------");
		TraceLog::WriteLine(LOG_STANDARD, " tdm_package --create-update-package --basedir=d:/temp/darkmod1.02 --baseversion=1.02 --headdir=d:/temp/darkmod1.03 --headversion=1.03 --outputdir=d:/temp/darkmod_update");
		TraceLog::WriteLine(LOG_STANDARD, " This will compare the PK4 set in 'basedir' against the newer version in 'headdir' and create an update package in the folder 'outputdir'.");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "-------------------------------------------------------------------------------------");
		TraceLog::WriteLine(LOG_STANDARD, " tdm_package --update-version-info-file --basedir=d:/temp/darkmod1.03 --baseversion=1.03 --outputdir=d:/temp/darkmod_update");
		TraceLog::WriteLine(LOG_STANDARD, " This will analyse the PK4 set in 'basedir' and store/merge the version information into the version info txt file in 'outputdir'.");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "-------------------------------------------------------------------------------------");
		TraceLog::WriteLine(LOG_STANDARD, " tdm_package --register-update-package --package=update_1.02_to_1.03.pk4 --outputdir=d:/temp/darkmod_update");
		TraceLog::WriteLine(LOG_STANDARD, " This will register the given PK4 in the tdm_version_info.txt file located in the specified output directory. If the version info file doesn't exist yet, it will be created.");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, "-------------------------------------------------------------------------------------");
		TraceLog::WriteLine(LOG_STANDARD, " tdm_package --create-crc-info --basedir=d:/temp/darkmod1.02");
		TraceLog::WriteLine(LOG_STANDARD, " This will calculate the CRCs for the release set in the given base dir and write/overwrite the crc_info.txt into the same folder.");
		TraceLog::WriteLine(LOG_STANDARD, "");
		TraceLog::WriteLine(LOG_STANDARD, " tdm_package --check-repository --darkmoddir=c:/games/tdm/darkmod");
		TraceLog::WriteLine(LOG_STANDARD, " This will check your repository for completeness.");
		TraceLog::WriteLine(LOG_STANDARD, "");
	}

private:
	void SetupDescription()
	{
		// Get options from command line
		_desc.add_options()
			("create-manifest", "Create a list of files to be released, needs the darkmod SVN folder as argument.\n")
			("create-package", "Create a full release package, needs outputdir and darkmoddir as argument.\n")
			("create-update-package", "Create an update package, containing the difference between a defined base and head PK4 set.\n")
			("create-crc-info", "Create a crc_info.txt file for the release set in the given basedir.\n")
			("check-repository", "Checks the repository for completeness.\n")
			("update-version-info-file", "Updates the INI/TXT containing the version information. The information gathered from 'basedir' will be added to the file in 'outputdir'.\n")
			("register-update-package", "Registers the specified update package in the specified version info file.\n")
			("outputdir", bpo::value<std::string>(), "The folder the update package PK4 should be saved to.\n")
			("darkmoddir", bpo::value<std::string>(), "The folder the darkmod SVN repository is checked out to.\n")
			("name", bpo::value<std::string>()->default_value("darkmod"), "The name of the release/manifest, as used by the --create-package option. Defaults to 'darkmod'.\n")
			("package", bpo::value<std::string>(), "The package which should be registered in a version info file (applicable for --register-update-package).\n")
			("basedir", bpo::value<std::string>(), "The folder containing the 'old' or base PK4 set.\ne.g. --basedir=c:\\temp\\darkmod1.02\n")
			("baseversion", bpo::value<std::string>(), "The version number of the base PK4 set, e.g. '1.02'\n")
			("headdir", bpo::value<std::string>(), "The folder containing the 'new' or head PK4 set.\ne.g. --headdir=c:\\temp\\darkmod1.03\n")
			("release-name", bpo::value<std::string>()->default_value("darkmod"), "The name of the release package to generate the manifest for, e.g. --release-name=saintlucia \n")
			("headversion", bpo::value<std::string>(), "The version number of the head PK4 set, e.g. '1.03'\n")
			("allow-unversioned-files", "Skips the 'is under SVN version control' check when creating the manifest (use this only if you actually exported your SVN working copy for packaging.)\n")
			("use-singlethread-compression", "Processes one file after the other during packaging, to avoid threading issues on the TDM server.\n")
			("help", "Display this help page")
			;
	}
};

}

}
