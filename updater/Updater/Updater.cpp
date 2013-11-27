/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5870 $ (Revision of last commit) 
 $Date: 2013-08-22 01:18:13 +0400 (Чт, 22 авг 2013) $ (Date of last commit)
 $Author: grayman $ (Author of last commit)
 
******************************************************************************/

#include "Updater.h"

#include "../ReleaseFileset.h"
#include "../Http/MirrorDownload.h"
#include "../Http/HttpRequest.h"
#include "../Constants.h"
#include "../Util.h"
#include "../UpdatePackage.h"

#ifndef CAESARIA_PLATFORM_WIN
  #include <limits.h>
  #include <unistd.h>
  #include <sys/stat.h>
#endif

namespace tdm
{

namespace updater
{

Updater::Updater(const UpdaterOptions& options, vfs::Path executable) :
	_options(options),
	_downloadManager(new DownloadManager),
	_executable( StringHelper::localeLower( executable.toString() ) ), // convert that file to lower to be sure
	_updatingUpdater(false)
{
	// Set up internet connectivity
	HttpConnectionPtr http( new HttpConnection() );
	http->drop();
	_conn = http;

	// Assign the proxy settings to the connection
	_options.CheckProxy(_conn);

	_ignoreList.insert("doomconfig.cfg");
	_ignoreList.insert("darkmod.cfg");

	MirrorDownload::InitRandomizer();

#ifdef CAESARIA_PLATFORM_WIN
	if( _executable.getExtension().empty() )
	{
		Logger::warning( "Adding EXE extension to executable: " + _executable.toString() );
		_executable = _executable.toString() + ".exe";
	}
#endif
}

void Updater::_markFileAsExecutable(vfs::Path path )
{
#ifdef CAESARIA_PLATFORM_LINUX
	Logger::warning( "Marking file as executable: " + path.toString() );

	struct stat mask;
	stat(path.toString().c_str(), &mask);

	mask.st_mode |= S_IXUSR|S_IXGRP|S_IXOTH;

	if (chmod(path.toString().c_str(), mask.st_mode) == -1)
	{
		Logger::warning( "Could not mark file as executable: " + path.toString() );
	}
#endif
}

void Updater::CleanupPreviousSession()
{
	// Remove batch file from previous run
	vfs::NFile::remove( getTargetDir()/TDM_UPDATE_UPDATER_BATCH_FILE );
}

bool Updater::MirrorsNeedUpdate()
{
	vfs::Directory folder = getTargetDir();
	vfs::Path mirrorPath = folder/TDM_MIRRORS_FILE;

	if( !mirrorPath.isExist() )
	{
		// No mirrors file
		Logger::warning( "No mirrors file present on this machine.");
		return true;
	}

	// File exists, check options
	if( _options.isSet("keep-mirrors") )
	{
		Logger::warning( "Skipping mirrors update (--keep-mirrors is set)." );
		return false;
	}

	// Update by default
	return true;
}

void Updater::UpdateMirrors()
{
	std::string mirrorsUrl = TDM_MIRRORS_SERVER;
	mirrorsUrl += TDM_MIRRORS_FILE;

//	TraceLog::Write(LOG_VERBOSE, " Downloading mirror list from %s...", mirrorsUrl.c_str() ); // grayman - NG: too many args
	Logger::warning( StringHelper::format( 0xff, "Downloading mirror list from %s...", mirrorsUrl.c_str() ) ); // grayman - fixed

	vfs::Directory folder = getTargetDir();
	vfs::Path mirrorPath = folder.getFilePath( TDM_MIRRORS_FILE );

	HttpRequestPtr request = _conn->CreateRequest( mirrorsUrl, mirrorPath.toString());

	request->Perform();

	if (request->GetStatus() == HttpRequest::OK)
	{
		Logger::warning( "Done. ");

		// Load the mirrors from the file
		LoadMirrors();
	}
	else
	{
		Logger::warning( "Mirrors download failed: %s", request->GetErrorMessage().c_str() );
	}
}

void Updater::LoadMirrors()
{
	vfs::Directory folder = getTargetDir();
	vfs::Path mirrorPath = folder.getFilePath( TDM_MIRRORS_FILE );

	// Load the tdm_mirrors.txt into an INI file
	IniFilePtr mirrorsIni = IniFile::ConstructFromFile(mirrorPath);

	// Interpret the info and build the mirror list
	_mirrors = MirrorList( mirrorsIni );

	Logger::warning( "Found %d mirrors.", _mirrors.size() );
}

std::size_t Updater::GetNumMirrors()
{
	return _mirrors.size();
}

void Updater::GetStableVersionFromServer()
{
	Logger::warning( " Downloading CRC information..." );

	PerformSingleMirroredDownload(STABLE_VERSION_FILE);

	// Parse this file
	vfs::Directory folder = getTargetDir();
	IniFilePtr releaseIni = IniFile::ConstructFromFile(folder.getFilePath( STABLE_VERSION_FILE ) );

	if (releaseIni == NULL)
	{
		throw FailureException("Could not download CRC info file from server.");
	}

	// Build the release file set
	_latestRelease = ReleaseFileSet::LoadFromIniFile( releaseIni );
}

void Updater::GetVersionInfoFromServer()
{
	Logger::warning( " Downloading version information...");

	PerformSingleMirroredDownload(UPDATE_VERSION_FILE);

	// Parse this downloaded file
	vfs::Directory folder = getTargetDir();
	IniFilePtr versionInfo = IniFile::ConstructFromFile( folder.getFilePath( UPDATE_VERSION_FILE ) );

	if (versionInfo == NULL) 
	{
		Logger::warning( "Cannot find downloaded version info file: %s", folder.getFilePath(UPDATE_VERSION_FILE).toString().c_str() );
		return;
	}

	_releaseVersions.LoadFromIniFile( versionInfo );
	//_updatePackages.LoadFromIniFile( versionInfo );
}

void Updater::NotifyFileProgress(vfs::Path file, CurFileInfo::Operation op, double fraction)
{
	if (_fileProgressCallback != NULL)
	{
		CurFileInfo info;
		info.operation = op;
		info.file = file;
		info.progressFraction = fraction;

		_fileProgressCallback->OnFileOperationProgress(info);
	}
}

void Updater::DetermineLocalVersion()
{
	_pureLocalVersion.clear();
	_fileVersions.clear();
	_localVersions.clear();
	_applicableDifferentialUpdates.clear();

	Logger::warning( " Trying to determine installed CaesarIA version...");

	std::size_t totalItems = 0;

	// Get the total count of version information items, for calculating the progress
	for (ReleaseVersions::const_iterator v = _releaseVersions.begin(); v != _releaseVersions.end(); ++v)
	{
		totalItems += v->second.size();
	}

	std::size_t curItem = 0;

	for (ReleaseVersions::const_iterator v = _releaseVersions.begin(); v != _releaseVersions.end(); ++v)
	{
		Logger::warning( "Trying to match against version: %s", v->first.c_str() );

		const ReleaseFileSet& set = v->second;

		// Will be true on first mismatching file
		bool mismatch = false;

		for (ReleaseFileSet::const_iterator f = set.begin(); f != set.end(); ++f)
		{
			NotifyFileProgress(f->second.file, CurFileInfo::Check, static_cast<double>(curItem) / totalItems);

			curItem++;
			
			vfs::Directory folder = getTargetDir();
			vfs::Path candidate = folder.getFilePath( f->second.file );

			if( StringHelper::localeLower( candidate.getBasename().toString() )
					==  StringHelper::localeLower( _executable.getBasename().toString() ) )
			{
				Logger::warning( "Ignoring updater executable: %s.", candidate.toString().c_str() );
				continue;
			}

			if( !candidate.isExist() )
			{
				Logger::warning( "File %s is missing.", candidate.toString().c_str() );
				mismatch = true;
				continue;
			}

			if (f->second.localChangesAllowed) 
			{
				Logger::warning( "File %s exists, local changes are allowed, skipping.", candidate.toString().c_str() );
				continue;
			}

			std::size_t candidateFilesize = vfs::NFile::getSize( candidate );

			if (candidateFilesize != f->second.filesize)
			{
				Logger::warning( "File %s has mismatching size, expected %d but found %d.",
												 candidate.toString().c_str(),
												 f->second.filesize,
												 candidateFilesize );
				mismatch = true;
				continue;
			}

			// Calculate the CRC of this file
			unsigned int crc = CRC::GetCrcForFile( candidate );

			if (crc != f->second.crc)
			{
				Logger::warning( "File %s has mismatching CRC, expected %x but found %x.",
													candidate.toString().c_str(),
													f->second.crc,
													crc );
				mismatch = true;
				continue;
			}

			// The file is matching - record this version
			Logger::warning( "File %s is matching version %s.", candidate.toString().c_str(), v->first.c_str() );

			_fileVersions[candidate.toString()] = v->first;
		}

		// All files passed the check?
		if (!mismatch) 
		{
			_pureLocalVersion = v->first;
			Logger::warning( " Local installation matches version: " + _pureLocalVersion );
		}
	}

	// Sum up the totals for all files, each file has exactly one version
	for (FileVersionMap::const_iterator i = _fileVersions.begin(); i != _fileVersions.end(); ++i)
	{
		// sum up the totals for this version
		const std::string& version = i->second;
		VersionTotal& total = _localVersions.insert(LocalVersionBreakdown::value_type(version, VersionTotal())).first->second;

		total.numFiles++;
		total.filesize += vfs::NFile::getSize( i->first );
	}

	Logger::warning( "The local files are matching %d different versions.", _localVersions.size() );

	if (_fileProgressCallback != NULL)
	{
		_fileProgressCallback->OnFileOperationFinish();
	}

	if (_pureLocalVersion.empty())
	{
		Logger::warning( " Could not determine local version.");
	}

	if (!_localVersions.empty())
	{
		for (LocalVersionBreakdown::const_iterator i = _localVersions.begin(); i != _localVersions.end(); ++i)
		{
			const std::string& version = i->first;

			Logger::warning( "Files matching version %s: %d (size: %s)",
											version.c_str(),
											i->second.numFiles,
											Util::GetHumanReadableBytes(i->second.filesize).c_str() );

			// Check if this differential update is wise, from an economic point of view
			UpdatePackageInfo::const_iterator package = _updatePackages.find(version);

			if( package != _updatePackages.end())
			{
				Logger::warning( "Some files match version " + version + ", a differential update is available for that version.");

				UpdatePackageInfo::const_iterator package = _updatePackages.find(version);

				assert(package != _updatePackages.end());

				if (package->second.filesize < i->second.filesize)
				{
					Logger::warning( "The differential package size is smaller than the total size of files needing it, this is good.");
					_applicableDifferentialUpdates.insert(version);
				}
				else
				{
					Logger::warning( "The differential package size is larger than the total size of files needing it, will not download that.");
				}
			}
			else
			{
				Logger::warning( "Some files match version %s, but no differential update is available for that version.", version.c_str() );
			}
		}
	}
}

bool Updater::DifferentialUpdateAvailable()
{
	std::string version = "none";

	if (!_pureLocalVersion.empty())
	{
		// Local installation is pure, differential update is possible
		if (DifferentialUpdateAvailableForVersion(_pureLocalVersion))
		{
			Logger::warning( "Local version is exactly determined, differential update is available.");
			return true;
		}

		Logger::warning( "Local version is exactly determined, but no differential update is available.");
	}

	// Check applicable differential updates
	if (!_applicableDifferentialUpdates.empty())
	{
		return true;
	}

	Logger::warning( "No luck, differential updates don't seem to be applicable.");

	return false;
}

bool Updater::DifferentialUpdateAvailableForVersion(const std::string& version)
{
//	UpdatePackageInfo::const_iterator it = _updatePackages.find(version);

	//return it != _updatePackages.end();

	return false;
}

std::string Updater::GetNewestVersion()
{
	for (ReleaseVersions::reverse_iterator i = _releaseVersions.rbegin();
		 i != _releaseVersions.rend(); ++i)
	{
		return i->first;
	}

	return "";
}

std::size_t Updater::GetTotalDifferentialUpdateSize()
{
	std::size_t size = 0;

	if (!_pureLocalVersion.empty())
	{
		UpdatePackageInfo::const_iterator it = _updatePackages.find(_pureLocalVersion);

		while (it != _updatePackages.end())
		{
			size += it->second.filesize;

			it = _updatePackages.find(it->second.toVersion);
		}
	}
	else
	{
		std::set<std::string> visitedPackages;

		// Non-pure local install, check all differential update paths - consider duplicate visits
		for (std::set<std::string>::const_iterator i = _applicableDifferentialUpdates.begin(); 
			 i != _applicableDifferentialUpdates.end(); ++i)
		{
			UpdatePackageInfo::const_iterator p = _updatePackages.find(*i);

			while (p != _updatePackages.end())
			{
				if (visitedPackages.find(p->first) == visitedPackages.end())
				{
					visitedPackages.insert(p->first);

					size += p->second.filesize;	
				}

				// Traverse up the version path
				p = _updatePackages.find(p->second.toVersion);
			}
		}
	}

	return size;
}

DifferentialUpdateInfo Updater::GetDifferentialUpdateInfo()
{
	DifferentialUpdateInfo info;

	UpdatePackageInfo::const_iterator it = _updatePackages.find(_pureLocalVersion);

	if (it != _updatePackages.end())
	{
		info.fromVersion = it->second.fromVersion;
		info.toVersion = it->second.toVersion;
		info.filesize = it->second.filesize;
	}
	// Check applicable differential updates, return the info of the first item
	else if (!_applicableDifferentialUpdates.empty())
	{
		std::string firstApplicableVersion = *_applicableDifferentialUpdates.begin();

		it = _updatePackages.find(firstApplicableVersion);

		if (it != _updatePackages.end())
		{
			info.fromVersion = it->second.fromVersion;
			info.toVersion = it->second.toVersion;
			info.filesize = it->second.filesize;
		}
	}

	return info;
}

void Updater::DownloadDifferentialUpdate()
{
	DifferentialUpdateInfo info = GetDifferentialUpdateInfo();

	Logger::warning( " Downloading differential update package for version " + info.fromVersion);

	// Use the first package available for the local version, even if multiple ones might be registered
	UpdatePackageInfo::const_iterator it = _updatePackages.find(info.fromVersion);

	if (it == _updatePackages.end())
	{
		throw FailureException("Cannot download differential update - nothing found!");
	}

	vfs::Directory packageFilename = it->second.filename;
	vfs::Path packageTargetPath = getTargetDir().getFilePath( packageFilename );

	// Check if the package is already there
	if (VerifyUpdatePackageAt(it->second, packageTargetPath))
	{
		// Skip the download
		Logger::warning( " Found intact PK4 at target location, skipping download: " + packageFilename.toString());
	}
	else
	{
		// Download the file from one of our mirrors, with CRC check
		PerformSingleMirroredDownload( packageFilename.toString(), it->second.filesize, it->second.crc );

		if (!VerifyUpdatePackageAt(it->second, packageTargetPath))
		{
			throw FailureException("Failed to download update package: " + packageTargetPath.toString());
		}
	}
}

bool Updater::VerifyUpdatePackageAt(const UpdatePackage& info, vfs::Path package)
{
	if (!package.isExist())
	{
		Logger::warning( "VerifyUpdatePackageAt: File %s does not exist.", package.toString().c_str() );
		return false;
	}

	if( vfs::NFile::getSize( package ) != info.filesize)
	{
		Logger::warning( "File %s has mismatching size, expected %d but found %d.",
										package.toString().c_str(),
										info.filesize,
										vfs::NFile::getSize( package) );
		return false;
	}

	// Calculate the CRC of this file
	unsigned int  crc = CRC::GetCrcForFile(package);

	if (crc != info.crc)
	{
		Logger::warning( "File %s has mismatching CRC, expected %x but found %x.",
										 package.toString().c_str(),
										 info.crc, crc );
		return false;
	}

	Logger::warning( "File %s is intact with checksum %x.",package.toString().c_str(), crc );
	return true; // all checks passed, file is ok
}

void Updater::PerformDifferentialUpdateStep()
{
	DifferentialUpdateInfo updateInfo = GetDifferentialUpdateInfo();

	Logger::warning( " Applying differential update package for version " + updateInfo.fromVersion);

	// Use the first package available for the local version, even if multiple ones might be registered
	UpdatePackageInfo::iterator it = _updatePackages.find(updateInfo.fromVersion);

	if (it == _updatePackages.end())
	{
		throw FailureException("Cannot apply differential update - nothing found!");
	}

	vfs::Path packageFilename = it->second.filename;
	vfs::Directory targetdir = getTargetDir();

	vfs::Path packageTargetPath = targetdir.getFilePath( packageFilename );

	if (!VerifyUpdatePackageAt(it->second, packageTargetPath))
	{
		throw FailureException("Update package not found at the expected location: " + packageTargetPath.toString() );
	}

	UpdatePackage& info = it->second;

	ZipFileReadPtr package = Zip::OpenFileRead(packageTargetPath);

	if (package == NULL)
	{
		throw FailureException("Update package cannot be opened: " + packageTargetPath.toString());
	}

	{
		std::string updateInfoStr = package->LoadTextFile(TDM_UDPATE_INFO_FILE);

		IniFilePtr iniFile = IniFile::ConstructFromString(updateInfoStr);

		if (iniFile->IsEmpty())
		{
			throw FailureException("Cannot load update info file from that package.");
		}

		// Load the data from the INI file into the UpdatePackage structure
		info.LoadFromIniFile( iniFile );
	}

	// Some math for the progress meter
	std::size_t totalFileOperations = 0;

	totalFileOperations += info.pk4sToBeRemoved.size();
	totalFileOperations += info.pk4sToBeAdded.size();

	for (UpdatePackage::Pk4DifferenceMap::const_iterator pk4Diff = info.pk4Differences.begin(); 
		 pk4Diff != info.pk4Differences.end(); ++pk4Diff)
	{
		const UpdatePackage::PK4Difference& diff = pk4Diff->second;

		totalFileOperations += diff.membersToBeRemoved.size();
		totalFileOperations += diff.membersToBeReplaced.size();
		totalFileOperations += diff.membersToBeAdded.size();
	}

	totalFileOperations += info.nonArchiveFiles.toBeAdded.size();
	totalFileOperations += info.nonArchiveFiles.toBeRemoved.size();
	totalFileOperations += info.nonArchiveFiles.toBeReplaced.size();

	std::size_t curOperation = 0;

	// Start working
	
	// Remove PK4s as requested
	/*for (std::set<ReleaseFile>::const_iterator pk4 = info.pk4sToBeRemoved.begin();
		 pk4 != info.pk4sToBeRemoved.end(); ++pk4)
	{
		Logger::warning( " Removing PK4: %s", pk4->file.toString().c_str() );

		NotifyFileProgress(pk4->file, CurFileInfo::Delete, static_cast<double>(curOperation++) / totalFileOperations);
		
		vfs::Path( targetdir.getFilePath( pk4->file ) );
	}*/

	// Add PK4s as requested
	/*for (std::set<ReleaseFile>::const_iterator pk4 = info.pk4sToBeAdded.begin();
		 pk4 != info.pk4sToBeAdded.end(); ++pk4)
	{
		Logger::warning( " Adding PK4: " + pk4->file.toString() );

		NotifyFileProgress(pk4->file, CurFileInfo::Add, static_cast<double>(curOperation++) / totalFileOperations);

		vfs::Path targetPk4Path = targetdir.getFilePath( pk4->file );

		package->ExtractFileTo(pk4->file.toString(), targetPk4Path);

		if( targetPk4Path.isExtension( ".zip" ) )
		{
			Logger::warning( " Extracting file after adding package: %s" + pk4->file.toString() );

			// Extract this ZIP archive after adding it to the local inventory
			ExtractAndRemoveZip(targetPk4Path);
		}
	}*/

	// Perform in-depth PK4 changes
	/*for (UpdatePackage::Pk4DifferenceMap::const_iterator pk4Diff = info.pk4Differences.begin();
		 pk4Diff != info.pk4Differences.end(); ++pk4Diff)
	{
		Logger::warning( " Changing PK4: %s...", pk4Diff->first.c_str() );

		vfs::Path targetPk4Path = targetdir.getFilePath( pk4Diff->first );

		const UpdatePackage::PK4Difference& diff = pk4Diff->second;

		NotifyFileProgress(pk4Diff->first, CurFileInfo::Check, static_cast<double>(curOperation) / totalFileOperations);
		
		bool fileIsMatching = false;

		// Double-check the PK4 checksum before doing the merge
		try
		{
			unsigned int crc = CRC::GetCrcForFile(targetPk4Path);

			if (crc == diff.checksumBefore)
			{
				fileIsMatching = true;
			}
			else
			{
				Logger::warning( "Cannot apply change, PK4 checksum is different." );
			}
		}
		catch (std::runtime_error&)
		{} // leave fileIsMatching at false

		if (!fileIsMatching)
		{
			curOperation += diff.membersToBeRemoved.size();
			curOperation += diff.membersToBeReplaced.size();
			curOperation += diff.membersToBeAdded.size();

			continue;
		}

		std::set<std::string> removeList;

		// Remove members to be removed as first measure
		for (std::set<ReleaseFile>::const_iterator m = diff.membersToBeRemoved.begin();
			 m != diff.membersToBeRemoved.end(); ++m)
		{
			removeList.insert(m->file.toString());

			curOperation++;
		}

		// Remove all changed files too
		for (std::set<ReleaseFile>::const_iterator m = diff.membersToBeReplaced.begin();
			 m != diff.membersToBeReplaced.end(); ++m)
		{
			removeList.insert(m->file.toString());
		}

		NotifyFileProgress(pk4Diff->first, CurFileInfo::RemoveFilesFromArchive, static_cast<double>(curOperation++) / totalFileOperations);

		// Perform the removal step here
		Zip::RemoveFilesFromArchive(targetPk4Path, removeList);

		// Open the archive for writing (append mode)
		ZipFileWritePtr targetPk4 = Zip::OpenFileWrite(targetPk4Path, Zip::APPEND);

		// Add new members
		for (std::set<ReleaseFile>::const_iterator m = diff.membersToBeAdded.begin();
			 m != diff.membersToBeAdded.end(); ++m)
		{
			targetPk4->CopyFileFromZip(package, m->file.toString(), m->file.toString());

			NotifyFileProgress(m->file, CurFileInfo::Add, static_cast<double>(curOperation++) / totalFileOperations);
		}
			 
		// Re-add changed members
		for (std::set<ReleaseFile>::const_iterator m = diff.membersToBeReplaced.begin();
			 m != diff.membersToBeReplaced.end(); ++m)
		{
			targetPk4->CopyFileFromZip(package, m->file.toString(), m->file.toString());

			NotifyFileProgress(m->file, CurFileInfo::Add, static_cast<double>(curOperation++) / totalFileOperations);
		}

		// Close the file
		targetPk4 = ZipFileWritePtr();

		// Calculate CRC after patching
		unsigned int crcAfter = CRC::GetCrcForFile(targetPk4Path);

		if (crcAfter != diff.checksumAfter)
		{
			Logger::warning( "  Failed applying changes, PK4 checksum is different after patching." );
		}
		else
		{
			Logger::warning( " OK - Files added: %d, removed: %d, changed: %d",
												diff.membersToBeAdded.size(),
												diff.membersToBeRemoved.size(),
												diff.membersToBeReplaced.size() );
		}
	}*/

	// Perform non-archive file changes

	// Added files
	for (std::set<ReleaseFile>::const_iterator f = info.nonArchiveFiles.toBeAdded.begin();
		 f != info.nonArchiveFiles.toBeAdded.end(); ++f)
	{
		NotifyFileProgress(f->file, CurFileInfo::Add, static_cast<double>(curOperation++) / totalFileOperations);

		if( _ignoreList.find( StringHelper::localeLower( f->file.toString() ) ) != _ignoreList.end())
		{
			Logger::warning( " Ignoring non-archive file: " + f->file.toString() );
			continue;
		}

		Logger::warning( " Adding non-archive file: " + f->file.toString() );

		package->ExtractFileTo(f->file.toString(), targetdir.getFilePath( f->file ) );
	}

	// Removed files
	for (std::set<ReleaseFile>::const_iterator f = info.nonArchiveFiles.toBeRemoved.begin();
		 f != info.nonArchiveFiles.toBeRemoved.end(); ++f)
	{
		NotifyFileProgress(f->file, CurFileInfo::Delete, static_cast<double>(curOperation++) / totalFileOperations);

		if( _ignoreList.find( StringHelper::localeLower( f->file.toString() ) ) != _ignoreList.end())
		{
			Logger::warning( " Ignoring non-archive file: " + f->file.toString() );
			continue;
		}

		Logger::warning( " Removing non-archive file: " + f->file.toString() );

		vfs::Path path = targetdir.getFilePath( f->file );
		vfs::NFile::remove( path );
	}

	// Changed files
	for (std::set<ReleaseFile>::const_iterator f = info.nonArchiveFiles.toBeReplaced.begin();
		 f != info.nonArchiveFiles.toBeReplaced.end(); ++f)
	{
		NotifyFileProgress(f->file, CurFileInfo::Replace, static_cast<double>(curOperation++) / totalFileOperations);

		if (_ignoreList.find( StringHelper::localeLower( f->file.toString() ) ) != _ignoreList.end())
		{
			Logger::warning( " Ignoring non-archive file: " + f->file.toString() );
			continue;
		}

		vfs::Path path = targetdir.getFilePath( f->file );
		vfs::NFile::remove( path );

		Logger::warning( " Replacing non-archive file: " + f->file.toString() );

		package->ExtractFileTo( f->file.toString(), targetdir.getFilePath( f->file ) );
	}

#ifdef CAESARIA_PLATFORM_WIN
	std::string tdmExecutableName = "caesaria.exe";
#else 
	std::string tdmExecutableName = "caesaria.x86";
#endif

	vfs::Path path2exe = targetdir.getFilePath( tdmExecutableName );
	if( path2exe.isExist() )
	{
		// Set the executable bit on the TDM binary
		_markFileAsExecutable( targetdir.getFilePath( tdmExecutableName ) );
	}

	// Close the ZIP file before removing it
	package = ZipFileReadPtr();

	if (_fileProgressCallback != NULL)
	{
		_fileProgressCallback->OnFileOperationFinish();
	}

	// Remove the update package after completion
	if (!_options.isSet("keep-update-packages"))
	{
		Logger::warning( " Removing package after differential update completion: " + packageTargetPath.toString() );
		vfs::NFile::remove( packageTargetPath );
	}
	else
	{
		Logger::warning( " Keeping package after differential update completion: " + packageTargetPath.toString() );
	}
}

std::string Updater::GetDeterminedLocalVersion()
{
	return _pureLocalVersion;
}

DownloadPtr Updater::PrepareMirroredDownload(const std::string& remoteFile)
{
	AssertMirrorsNotEmpty();

	vfs::Directory dir = getTargetDir();
	vfs::Path targetPath =  dir.getFilePath( remoteFile );

	// Remove target path first
	vfs::NFile::remove( targetPath );

	// Create a mirrored download
	DownloadPtr download(new MirrorDownload(_conn, _mirrors, remoteFile, targetPath));

	vfs::Path rpath( remoteFile );
	/*if( rpath.isExtension( ".zip" ) )
	{
		download->EnableValidPK4Check(true);
	}*/

	return download;
}

void Updater::PerformSingleMirroredDownload(const std::string& remoteFile)
{
	// Create a mirrored download
	DownloadPtr download = PrepareMirroredDownload(remoteFile);

	// Perform and wait for completion
	PerformSingleMirroredDownload(download);
}

void Updater::PerformSingleMirroredDownload(const std::string& remoteFile, std::size_t requiredSize, unsigned int requiredCrc)
{
	DownloadPtr download = PrepareMirroredDownload(remoteFile);

	download->EnableCrcCheck(true);
	download->EnableFilesizeCheck(true);
	download->SetRequiredCrc(requiredCrc);
	download->SetRequiredFilesize(requiredSize);

	// Perform and wait for completion
	PerformSingleMirroredDownload(download);
}

void Updater::PerformSingleMirroredDownload(const DownloadPtr& download)
{
	int downloadId = _downloadManager->AddDownload(download);

	while (_downloadManager->HasPendingDownloads())
	{
		//boost::this_thread::interruption_point();

		_downloadManager->ProcessDownloads();

		NotifyDownloadProgress();

		for (int i = 0; i < 50; ++i)
		{
			//boost::this_thread::interruption_point();
			Util::Wait(10);
		}
	}

	if( _downloadProgressCallback.isValid() )
	{
		_downloadProgressCallback->OnDownloadFinish();
	}

	_downloadManager->RemoveDownload(downloadId);
}

vfs::Directory Updater::getTargetDir()
{
	// Use the target directory 
	if (_options.isSet("targetdir") && !_options.Get("targetdir").empty())
	{
		return vfs::Path( _options.Get("targetdir") );
	}

	// Get the current path
	vfs::Path targetPath = vfs::Directory::getCurrent();

	// If the current path is the actual engine path, switch folders to "darkmod"
	// We don't want to download the PK4s into the Doom3.exe location
	/* grayman - no longer necessary
	if (Util::PathIsTDMEnginePath(targetPath))
	{
		TraceLog::WriteLine(LOG_VERBOSE, "Doom3 found in current path, switching directories.");

		targetPath /= TDM_STANDARD_MOD_FOLDER;

		if (!fs::exists(targetPath))
		{
			TraceLog::WriteLine(LOG_VERBOSE, "darkmod/ path not found, creating folder: " + targetPath.string());

			fs::create_directory(targetPath);
		}

		TraceLog::WriteLine(LOG_VERBOSE, " Changed working directory to darkmod/, continuing update process.");
	}
	*/

	return targetPath;
}

void Updater::CheckLocalFiles()
{
	_downloadQueue.clear();

	// Get the current path
	vfs::Directory targetDir = getTargetDir();

	Logger::warning( "Checking target folder: " + targetDir.toString() );

	std::size_t count = 0;
	for (ReleaseFileSet::const_iterator i = _latestRelease.begin(); i != _latestRelease.end(); ++i)
	{
		if (_fileProgressCallback != NULL)
		{
			CurFileInfo info;
			info.operation = CurFileInfo::Check;
			info.file = i->second.file;
			info.progressFraction = static_cast<double>(count) / _latestRelease.size();

			_fileProgressCallback->OnFileOperationProgress(info);
		}

		Logger::warning( "Checking for file: " + i->second.file.toString() + "...");
		if (!CheckLocalFile(targetDir, i->second))
		{
			// A member is missing or out of date, mark the archive for download
			_downloadQueue.insert(*i);
		}

		count++;
	}

	if (_fileProgressCallback != NULL)
	{
		_fileProgressCallback->OnFileOperationFinish();
	}

	if (NewUpdaterAvailable())
	{
		// Remove all download packages from the queue, except the one containing the updater
		RemoveAllPackagesExceptUpdater();
	}
}

bool Updater::CheckLocalFile(vfs::Path installPath, const ReleaseFile& releaseFile)
{
	//boost::this_thread::interruption_point();

	vfs::Path localFile = vfs::Directory( installPath ).getFilePath( releaseFile.file );

	Logger::warning( " Checking for file " + releaseFile.file.toString() + ": ");

	if( localFile.isExist() )
	{
		// File exists, check ignore list
		if (_ignoreList.find( StringHelper::localeLower( releaseFile.file.toString()) ) != _ignoreList.end())
		{
			Logger::warning( "OK, file will not be updated. ");
			return true; // ignore this file
		}

		// Compare file size
		std::size_t fileSize = vfs::NFile::getSize( localFile );

		if (fileSize != releaseFile.filesize)
		{
			Logger::warning( "SIZE MISMATCH" );
			return false;
		}

		// Size is matching, check CRC
		unsigned int existingCrc = CRC::GetCrcForFile(localFile);

		if (existingCrc == releaseFile.crc)
		{
			Logger::warning( "OK");
			return true;
		}
		else
		{
			Logger::warning( "CRC MISMATCH");
			return false;
		}
	}
	else
	{
		Logger::warning( "MISSING");
		return false;
	}
}

bool Updater::LocalFilesNeedUpdate()
{
	return !_downloadQueue.empty();
}

void Updater::PrepareUpdateStep()
{
	vfs::Directory targetdir = getTargetDir();

	// Create a download for each of the files
	for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
	{
		//boost::this_thread::interruption_point();

		// Create a mirrored download
		DownloadPtr download(new MirrorDownload(_conn, _mirrors, i->second.file.toString(), targetdir.getFilePath(i->second.file) )) ;

		// Check archives after download, pass crc and filesize to download
		/*if( i->second.file.isExtension( ".zip" ) )
		{
			download->EnableValidPK4Check(true);
			download->EnableCrcCheck(true);
			download->EnableFilesizeCheck(true);

			download->SetRequiredCrc(i->second.crc);
			download->SetRequiredFilesize(i->second.filesize);
		}*/

		i->second.downloadId = _downloadManager->AddDownload(download);
	}
}

void Updater::PerformUpdateStep()
{
	// Wait until the download is done
	while (_downloadManager->HasPendingDownloads())
	{
		// For catching terminations
		//boost::this_thread::interruption_point();

		_downloadManager->ProcessDownloads();

		if (_downloadManager->HasFailedDownloads())
		{
			throw FailureException("Could not download from any mirror.");
		}

		NotifyDownloadProgress();

		NotifyFullUpdateProgress();

		Util::Wait(100);
	}

	if (_downloadManager->HasFailedDownloads())
	{
		throw FailureException("Could not download from any mirror.");
	}

	if (_downloadProgressCallback.isValid())
	{
		_downloadProgressCallback->OnDownloadFinish();
	}

	// Check if any ZIP files have been downloaded, these need to be extracted
	for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
	{
		//boost::this_thread::interruption_point();

		DownloadPtr download = _downloadManager->GetDownload(i->second.downloadId);

		if (download != NULL && download->GetStatus() == Download::SUCCESS)
		{
			/*if( download->GetDestFilename().isExtension( ".zip" ) )
			{
				// Extract this ZIP archive after download
				ExtractAndRemoveZip(download->GetDestFilename());
			}*/
		}
	}
}

void Updater::NotifyFullUpdateProgress()
{
	if (_downloadProgressCallback == NULL)
	{
		return;
	}

	std::size_t totalDownloadSize = GetTotalDownloadSize();
	std::size_t totalBytesDownloaded = 0;

	for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
	{
		//boost::this_thread::interruption_point();

		if (i->second.downloadId == -1)
		{
			continue;
		}

		DownloadPtr download = _downloadManager->GetDownload(i->second.downloadId);

		if (download == NULL) continue;

		if (download->GetStatus() == Download::SUCCESS)
		{
			totalBytesDownloaded += i->second.filesize;
		}
		else if (download->GetStatus() == Download::IN_PROGRESS)
		{
			totalBytesDownloaded += download->GetDownloadedBytes();
		}
	}

	OverallDownloadProgressInfo info;

	if (totalBytesDownloaded > totalDownloadSize)
	{
		totalBytesDownloaded = totalDownloadSize;
	}

	info.updateType = OverallDownloadProgressInfo::Full;
	info.totalDownloadSize = totalDownloadSize;
	info.bytesLeftToDownload = totalDownloadSize - totalBytesDownloaded;
	info.downloadedBytes = totalBytesDownloaded;
	info.progressFraction = static_cast<double>(totalBytesDownloaded) / totalDownloadSize;
	info.filesToDownload = _downloadQueue.size();
	
	_downloadProgressCallback->OnOverallProgress(info);
}

void Updater::NotifyDownloadProgress()
{
	int curDownloadId = _downloadManager->GetCurrentDownloadId();
	
	if(curDownloadId != -1 && _downloadProgressCallback.isValid())
	{
		DownloadPtr curDownload = _downloadManager->GetCurrentDownload();

		CurDownloadInfo info;

		info.file = curDownload->GetFilename();
		info.progressFraction = curDownload->GetProgressFraction();
		info.downloadSpeed = curDownload->GetDownloadSpeed();
		info.downloadedBytes = curDownload->GetDownloadedBytes();

		MirrorDownloadPtr mirrorDownload = curDownload.as<MirrorDownload>();

		if (mirrorDownload != NULL)
		{
			info.mirrorDisplayName = mirrorDownload->GetCurrentMirrorName();
		}

		_downloadProgressCallback->OnDownloadProgress(info);
	}
}

void Updater::ExtractAndRemoveZip(vfs::Path zipFilePath)
{
	ZipFileReadPtr zipFile = Zip::OpenFileRead(zipFilePath);

	if (zipFile == NULL)
	{
		Logger::warning( "Archive is not a valid ZIP.");
		return;
	}

	vfs::Directory destPath = getTargetDir();

	// tdm_update exists in its own PK4, so we can assume tdm_update and the TDM binaries will never be found in the same PK4.

	bool TDMbinaryPresent = false; // grayman - true if this zip contains "TheDarkMod.exe" or "thedarkmod.x86"

#ifdef CAESARIA_PLATFORM_WIN
	const std::string TDM_BINARY_NAME("TheDarkMod.exe");
#else
	const std::string TDM_BINARY_NAME("thedarkmod.x86");
#endif

	try
	{
		Logger::warning( "Extracting files from " + zipFilePath.toString() );

		// Check if the archive contains the updater binary
		if( zipFile->ContainsFile(_executable.toString()))
		{
			// Set the flag for later use
			_updatingUpdater = true;
		}

		// Check if the archive contains the TDM binary
		if (zipFile->ContainsFile(TDM_BINARY_NAME))
		{
			// Set the flag for later use
			TDMbinaryPresent = true;
		}

		std::list<vfs::Path> extractedFiles;

		if (_updatingUpdater)
		{
			// Update all files, but save the updater binary; this will be handled separately
			std::set<std::string> hardIgnoreList;
			hardIgnoreList.insert(_executable.toString());

			// Extract all but the updater
			// Ignore DoomConfig.cfg, etc. if already existing
			extractedFiles = zipFile->ExtractAllFilesTo(destPath, _ignoreList, hardIgnoreList); 

			// Extract the updater to a temporary filename
			vfs::Path tempUpdater = destPath.getFilePath( "_" + _executable.toString() );

			zipFile->ExtractFileTo(_executable.toString(), tempUpdater);

			// Set the executable bit on the temporary updater
			_markFileAsExecutable( tempUpdater );

			// Prepare the update batch file
			PrepareUpdateBatchFile(tempUpdater);
		}
		else if (TDMbinaryPresent)
		{
			// Update all files, but save the TDM binary; this will be handled separately
			std::set<std::string> hardIgnoreList;
			hardIgnoreList.insert(TDM_BINARY_NAME);

			// Extract all but the TDM binary
			// Ignore DoomConfig.cfg, etc. if already existing
			extractedFiles = zipFile->ExtractAllFilesTo(destPath, _ignoreList, hardIgnoreList); 

			// Extract the TDM binary
			vfs::Path binaryFileName = destPath.getFilePath( TDM_BINARY_NAME );
			zipFile->ExtractFileTo(TDM_BINARY_NAME, binaryFileName);

			// Set the executable bit on the TDM binary
			_markFileAsExecutable( binaryFileName );
		}
		else
		{
			// Regular archive (without updater or TDM binary), extract all files, ignore existing DoomConfig.cfg
			extractedFiles = zipFile->ExtractAllFilesTo(destPath, _ignoreList);
		}

		Logger::warning( "All files successfully extracted from " + zipFilePath.toString() );

#ifndef WIN32
		// In Linux or Mac, mark *.linux files as executable after extraction
		for (std::list<vfs::Path>::const_iterator i = extractedFiles.begin(); i != extractedFiles.end(); ++i)
		{
			std::string extension = StringHelper::localeLower( (*i).getExtension() );

			if (extension == ".linux" || extension == ".macosx")
			{
				_markFileAsExecutable( *i );
			}
		}
#endif
		
		// Close the zip file before removal
		zipFile = ZipFileReadPtr();

		// Remove the Zip
		vfs::Path removedFilePath = zipFilePath;
		vfs::NFile::remove( removedFilePath );
	}
	catch (std::runtime_error& ex)
	{
		Logger::warning("Failed to extract files from " + zipFilePath.toString() + ": " + ex.what() );
	}
}

void Updater::PrepareUpdateBatchFile(vfs::Path temporaryUpdater)
{
	// Create a new batch file in the target location
	vfs::Directory targetdir = getTargetDir();
	_updateBatchFile =  targetdir.getFilePath( TDM_UPDATE_UPDATER_BATCH_FILE );

	Logger::warning( "Preparing TDM update batch file in " + _updateBatchFile.toString() );

	std::ofstream batch(_updateBatchFile.toString().c_str());

	vfs::Path tempUpdater = temporaryUpdater.getBasename();
	vfs::Path updater = _executable.getBasename();

	// Append the current set of command line arguments to the new instance
	std::string arguments;

	for (std::vector<std::string>::const_iterator i = _options.GetRawCmdLineArgs().begin();
		 i != _options.GetRawCmdLineArgs().end(); ++i)
	{
		arguments += " " + *i;
	}

#ifdef CAESARIA_PLATFORM_WIN
	batch << "@ping 127.0.0.1 -n 2 -w 1000 > nul" << std::endl; // # hack equivalent to Wait 2
	batch << "@copy " << tempUpdater.toString() << " " << updater.toString() << " >nul" << std::endl;
	batch << "@del " << tempUpdater.toString() << std::endl;
	batch << "@echo TDM Updater executable has been updated." << std::endl;

	batch << "@echo Re-launching TDM Updater executable." << std::endl << std::endl;

	batch << "@start " << updater.toString() << " " << arguments;
#else // POSIX
	// grayman - accomodate spaces in pathnames
	tempUpdater = targetdir.getFilePath( tempUpdater );
	updater = targetdir.getFilePath( updater );

	batch << "#!/bin/bash" << std::endl;
	batch << "echo \"Upgrading TDM Updater executable...\"" << std::endl;
	batch << "cd \"" << getTargetDir().toString() << "\"" << std::endl;
	batch << "sleep 2s" << std::endl;
	batch << "mv -f \"" << tempUpdater.toString() << "\" \"" << updater.toString() << "\"" << std::endl;
	batch << "chmod +x \"" << updater.toString() << "\"" << std::endl;
	batch << "echo \"TDM Updater executable has been updated.\"" << std::endl;
	batch << "echo \"Re-launching TDM Updater executable.\"" << std::endl;

	batch << "\"" << updater.toString() << "\" " << arguments;
#endif

	batch.close();

#ifdef CAESARIA_PLATFORM_UNIX
	// Mark the shell script as executable in *nix
	_markFileAsExecutable(_updateBatchFile);
#endif
}

void Updater::CleanupUpdateStep()
{
	for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
	{
		_downloadManager->RemoveDownload(i->second.downloadId);
	}

	_downloadQueue.clear();
}

void Updater::AssertMirrorsNotEmpty()
{
	if (_mirrors.empty())
	{
		throw FailureException("No mirror information, cannot continue.");
	}
}

std::size_t Updater::GetNumFilesToBeUpdated()
{
	return _downloadQueue.size();
}

std::size_t Updater::GetTotalDownloadSize()
{
	std::size_t totalSize = 0;

	for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
	{
		totalSize += i->second.filesize;
	}

	return totalSize;
}

std::size_t Updater::GetTotalBytesDownloaded()
{
	return _conn->GetBytesDownloaded();
}

void Updater::SetDownloadProgressCallback(DownloadProgressPtr callback)
{
	_downloadProgressCallback = callback;
}

void Updater::ClearDownloadProgressCallback()
{
	_downloadProgressCallback = DownloadProgressPtr();
}

void Updater::SetFileOperationProgressCallback(FileOperationProgressPtr callback)
{
	_fileProgressCallback = callback;
}

void Updater::ClearFileOperationProgressCallback()
{
	_fileProgressCallback = FileOperationProgressPtr();
}

bool Updater::NewUpdaterAvailable()
{
	if(_options.isSet("noselfupdate"))
	{
		return false; // no self-update overrides everything
	}

	Logger::warning( "Looking for executable " + _executable.toString() + " in download queue.");
	
	// Is this the updater?
	for (ReleaseFileSet::const_iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
	{
		if (i->second.ContainsUpdater(_executable.toString()))
		{
			Logger::warning( "The tdm_update binary needs to be updated.");
			return true;
		}
	}

	Logger::warning( "Didn't find executable name " + _executable.toString() + " in download queue.");

	return false;
}

void Updater::RemoveAllPackagesExceptUpdater()
{
	Logger::warning("Removing all packages, except the one containing the updater");

	for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); /* in-loop */)
	{
		if (i->second.ContainsUpdater(_executable.toString()))
		{
			// This package contains the updater, keep it
			++i;
		}
		else
		{
			// The inner loop didn't find the executable, remove that package
			_downloadQueue.erase(i++);
		}
	}
}

bool Updater::RestartRequired()
{
	return _updatingUpdater;
}

void Updater::RestartUpdater()
{
	Logger::warning( "Preparing restart...");

#ifdef CAESARIA_PLATFORM_WIN
	if (!_updateBatchFile.toString().empty())
	{
		Logger::warning( "Update batch file pending, launching process.");
		
		// Spawn a new process

		// Create a tdmlauncher process, setting the working directory to the target directory
		STARTUPINFOA siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;

		memset(&siStartupInfo, 0, sizeof(siStartupInfo));
		memset(&piProcessInfo, 0, sizeof(piProcessInfo));

		siStartupInfo.cb = sizeof(siStartupInfo);

		io::FileDir parentPath = _updateBatchFile.getFileDir();

		Logger::warning( "Starting batch file " + _updateBatchFile.toString() + " in " + parentPath.toString() );

		BOOL success = CreateProcessA( NULL, (LPSTR) _updateBatchFile.toString().c_str(), NULL, NULL,  false, 0, NULL,
																	 parentPath.toString().c_str(), &siStartupInfo, &piProcessInfo);

		if (!success)
		{
			LPVOID lpMsgBuf;

			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL,
						  GetLastError(),
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  (LPTSTR) &lpMsgBuf,
						  0,
						  NULL);

			throw FailureException( "Could not start new process: " + std::string((LPCSTR)lpMsgBuf));
			
			LocalFree(lpMsgBuf);
		}
		else
		{
			Logger::warning( "Process started");
		}
	}
#else

	if (!_updateBatchFile.toString().empty())
	{
		Logger::warning( "Relaunching tdm_update via shell script " + _updateBatchFile.toString() );

		// Perform the system command in a fork
		if (fork() == 0)
		{
			// Don't wait for the subprocess to finish
			system((_updateBatchFile.toString() + " &").c_str());
			exit(EXIT_SUCCESS);
			return;
		}

		Logger::warning( "Process spawned.");

		// Done here too
		return;
	}
#endif
}

void Updater::PostUpdateCleanup()
{
	vfs::Directory pdir =  getTargetDir();
	vfs::Entries dir = pdir.getEntries();
	for( vfs::Entries::ConstItemIt i = dir.begin(); i != dir.end(); i++)
	{
		if( StringHelper::startsWith( i->name.toString(), TMP_FILE_PREFIX) )
		{
			vfs::Path p = i->fullName;
			vfs::NFile::remove( p );
		}		
	}

	// grayman #3514 - Remove DLL file in case the user is updating an existing installation.
	// Also remove leftover updater file.

#if WIN32
	vfs::Path tdmDLLName = "gamex86.dll";
	vfs::Path tdmUpdateName = "_tdm_update.exe";
#else 
	vfs::Path tdmDLLName = "gamex86.so";
	vfs::Path tdmUpdateName = "_tdm_update.linux";
#endif


	vfs::NFile::remove( pdir/tdmDLLName );
	vfs::NFile::remove( pdir/tdmUpdateName );
}

void Updater::CancelDownloads()
{
	_downloadManager->ClearDownloads();
}

} // namespace

} // namespace
