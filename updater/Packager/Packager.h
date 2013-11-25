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

#include "PackagerOptions.h"
#include "../ReleaseFileset.h"
#include "../UpdatePackage.h"
#include "../ReleaseManifest.h"
#include "../Pk4Mappings.h"
#include "../PackageInstructions.h"

namespace tdm
{

namespace packager
{

/**
 * A class containing logic and algorithms for creating
 * TDM release packages. It can be used to compare PK4 sets
 * and generate update PK4s as well as crc_info.txt files.
 */
class Packager
{
public:
	// Thrown on any errors during method execution
	class FailureException :
		public std::runtime_error
	{
	public:
		FailureException(const std::string& message) :
			std::runtime_error(message)
		{}
	};

private:
	// Program parameters
	const PackagerOptions& _options;

	// The base set ("old")
	ReleaseFileSet _baseSet;

	// The head set ("new")
	ReleaseFileSet _headSet;

	// The PK4 representing the difference between base and head
	UpdatePackage _difference;

	// ---- Manifest creation ---

	// The pre-existing manifest before we're starting to create a new one
	ReleaseManifest _oldManifest;

	// The manifest as defined by "base.txt"
	ReleaseManifest _baseManifest;

	// The file containing the include/exclude statements
	PackageInstructions _instructionFile;

	// ---- Manifest creation End ----

	// ---- Package creation ----

	// The manifest of a specific release
	ReleaseManifest _manifest;
	Pk4Mappings _pk4Mappings;

	// Manifest files distributed into Pk4s
	typedef std::list<ManifestFile> ManifestFiles;
	typedef std::map<std::string, ManifestFiles> Package;
	Package _package;

	// ---- Package creation End ----

public:
	// Pass the program options to this class
	Packager(const PackagerOptions& options);

	// Collects information about the base set (given by basedir parameter)
	void GatherBaseSet();

	// Collects information about the head set (given by headdir parameter)
	void GatherHeadSet();

	// Searches for changes between base and head, and stores information locally.
	void CalculateSetDifference();

	// Creates the update PK4 in the output folder
	void CreateUpdatePackage();

	// Creates or update the version info file in the location given by the "version-info-file" parameter
	void CreateVersionInformation();

	// Adds the package information to the given ini file
	void RegisterUpdatePackage(const fs::path& path);

	// Loads the manifest information from the options - needs darkmoddir set to something
	void LoadManifest();

	// Copies the loaded manifest as "old" manifest for later comparison
	void SaveManifestAsOldManifest();

	// Loads the base manifest ("base.txt") from the darkmoddir specified in the options
	void LoadBaseManifest();

	// Load the file containing the INCLUDE/EXCLUDE/FM statements
	void LoadInstructionFile();

	// Traverse the repository to collect all files that should go into the manifest
	void CollectFilesForManifest();

	// Sorts the manifest and removes any duplicates
	void CleanupAndSortManifest();

	// Compares the "old" manifest with the new one and prints a summary
	void ShowManifestComparison();

	// Saves the manifest to devel/manifests/<name>.txt, including the base manifest
	void SaveManifest();

	// Checks if all files in the manifest are existing
	void CheckRepository();

	// Loads the darkmod_pk4s.txt file from the devel folder.
	void LoadPk4Mapping();

	// Sorts files into Pk4s, resulting in a ReleaseFileset
	void SortFilesIntoPk4s();

	// Creates the package at the given output folder
	void CreatePackage();

	// Create the crc_info.txt in the basedir (call GatherBaseSet() beforehand)
	void CreateCrcInfoFile();

private:
	// Worker thread for creating a release archive
	void ProcessPackageElement(Package::const_iterator p);
};

} // namespace

} // namespace
