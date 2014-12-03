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

#ifndef __CAESARIA_UPDATER_H_INLCUDE__
#define __CAESARIA_UPDATER_H_INLCUDE__

#include <stdexcept>
#include "updateroptions.hpp"
#include "http/httpconnection.hpp"
#include "http/mirrorlist.hpp"
#include "http/downloadmanager.hpp"
#include "releasefileset.hpp"
#include "releaseversions.hpp"

/**
 * Main updater class containing the application logic. 
 * Use an instance of this class to control the update process 
 * by calling the appropriate methods, like UpdateMirrors().
 */

namespace updater
{

// The progress taking information about the entire download
struct OverallDownloadProgressInfo
{
	enum UpdateType
	{
		Differential,
		Full
	};

	UpdateType updateType;

	// The progress fraction
	double progressFraction;

	// Number of bytes received
	std::size_t downloadedBytes;

	// Number of bytes to download
	std::size_t totalDownloadSize;

	// Number of bytes left
	std::size_t bytesLeftToDownload;

	// Number of files to download
	std::size_t filesToDownload;
};

struct CurDownloadInfo
{
	// The filename we're downloading
	vfs::Path file;

	// The progress fraction
	double progressFraction;

	// In bytes/sec
	double downloadSpeed;

	// Number of bytes received
	std::size_t downloadedBytes;

	// The display name of the currently active mirror
	std::string mirrorDisplayName;
};

struct CurFileInfo
{
	enum Operation
	{
		Check,
		Delete,
		Replace,
		Add,
		RemoveFilesFromArchive
	};

	Operation operation;

	// The file which is being worked on
	vfs::Path file;

	// The progress of the whole operation
	double progressFraction;
};

class Updater : public ReferenceCounted
{
public:
	// Thrown on any errors during method execution
	class FailureException : public std::runtime_error
	{
	public:
		FailureException(const std::string& message) :
			std::runtime_error(message)
		{}
	};

	// An object to get notified in regular intervals about the download progress
	class DownloadProgress : public virtual ReferenceCounted
	{
	public:
		// Entire step progress (differential or full update)
		virtual void OnOverallProgress(const OverallDownloadProgressInfo& info) = 0;

		// Single-file progress
		virtual void OnDownloadProgress(const CurDownloadInfo& info) = 0;

		// called on finishing the single-file
		virtual void OnDownloadFinish() = 0;
	};

	typedef SmartPtr<DownloadProgress> DownloadProgressPtr;

	// An object to get notified in regular intervals about the file operations
	class FileOperationProgress : public virtual ReferenceCounted
	{
	public:
		virtual void OnFileOperationProgress(const CurFileInfo& info) = 0;

		virtual void OnFileOperationFinish() = 0;
	};
	typedef SmartPtr<FileOperationProgress> FileOperationProgressPtr;

private:
	const UpdaterOptions& _options;

	// The HTTP connection object
	HttpConnectionPtr _conn;

	// The mirror information
	MirrorList _mirrors;

	DownloadManagerPtr _downloadManager;

	// The latest release file set on the server
	ReleaseFileSet _latestRelease;

	// The release files which should be downloaded
	ReleaseFileSet _downloadQueue;

	// Some files like DoomConfig.cfg or dmargs.txt are ignored.
	std::set<std::string> _ignoreList;

	DownloadProgressPtr _downloadProgressCallback;

	FileOperationProgressPtr _fileProgressCallback;

	// The name of the updater executable, to detect self-updates
	vfs::Path _executable;

	// True if we've downloaded an update for the tdm_update binary
	bool _updatingUpdater;

	// The name of the update batch/shell script file
	vfs::Path _updateBatchFile;

	// The version information, indexed by version string "1.02" => [ReleaseFileSet]
	ReleaseVersions _releaseVersions;

	//UpdatePackageInfo _updatePackages;

	// The version the local installation has been found to match (empty if no match)
	// For this to be non-empty the local install needs to be "pure", i.e. all files must match
	std::string _pureLocalVersion;

	// A list of determined versions for the local files ("tdm_x.pk4" => "1.02", "tdm_y.pk4" => "1.03") 
	// If just one file matches 1.02, a differential update might be beneficial, so record all
	// versions here. If a file matches two different versions, the most recent one is taken.
	typedef std::map<std::string, std::string> FileVersionMap;
	FileVersionMap _fileVersions;

	struct VersionTotal
	{
		std::size_t numFiles; // the number of files matching this version
		std::size_t filesize; // the total size of those files

		VersionTotal() : numFiles(0), filesize(0)
		{}
	};

	// A map of [version] => [number of files matching that version]
	typedef std::map<std::string, VersionTotal> LocalVersionBreakdown;
	LocalVersionBreakdown _localVersions;

	// The local versions a differential update is applicable to
	std::set<std::string> _applicableDifferentialUpdates;

public:
	// Pass the program options to this class
	Updater(const UpdaterOptions& options, vfs::Path executable);

	// Removes leftovers from previous run
	void CleanupPreviousSession();

	// Returns TRUE if new mirrors should be downloaded
	bool isMirrorsNeedUpdate();

	// Returns the number of registered mirrors
	std::size_t GetNumMirrors();

	// Update the local tdm_mirrors.txt file from the main servers.
	void updateMirrors();

	// Load information from the tdm_mirrors.txt file
	void loadMirrors();

	// Download the checksum crc_info.txt file from the mirrors.
	void GetStableVersionFromServer();

	// Download the tdm_version_info.txt file from a mirror.
	void GetVersionInfoFromServer();

	// Return the version string of the newest available version
	std::string getNewestVersion();

	// Attempt to determine the local version using the version info downloaded earlier
	void DetermineLocalVersion();

	// Returns true if a differential update is available (based on this version)
	bool DifferentialUpdateAvailable();

	// Downloads the package applicable to the local version
	void DownloadDifferentialUpdate();

	// Apply changes to the local installation
	void PerformDifferentialUpdateStep();

	// Return the version as determined in DetermineLocalVersion()
	std::string getDeterminedLocalVersion();

	// Get Information about the next differential update
	//DifferentialUpdateInfo GetDifferentialUpdateInfo();

	// Compare locally installed files to the "ideal" state defined in crc_info.txt 
	void CheckLocalFiles();

	// True if the local files are not up to date and downloads need to be started
	bool LocalFilesNeedUpdate();

	// Generates an internal TODO list needed to perform the update step
	void PrepareUpdateStep(std::string prefix);

	// Performs the update step, downloads stuff and integrates the package
	void PerformUpdateStep();

	// Performs any post-update step actions, if any
	void cleanupUpdateStep();

	std::size_t GetNumFilesToBeUpdated();

	// Returns the number of bytes which need to be downloaded
	std::size_t GetTotalDownloadSize();

	// Returns the number of bytes which have been downloaded (including broken downloads, etc.)
	std::size_t GetTotalBytesDownloaded();

	// Calls with information about the current download status (periodically)
	void SetDownloadProgressCallback(DownloadProgressPtr callback);
	void ClearDownloadProgressCallback();

	// Calls with information about the current file operation status (periodically)
	void SetFileOperationProgressCallback(FileOperationProgressPtr callback);
	void ClearFileOperationProgressCallback();

	// Returns TRUE if the tdm_update binary needs an update
	bool NewUpdaterAvailable();

	// Removes all packages, except the one containing the tdm_update binary
	void RemoveAllPackagesExceptUpdater();

	// Re-launches the updater (starts update batch file in Win32 builds)
	void RestartUpdater();

	// Cleanup (both after regular exit and user terminations)
	void postUpdateCleanup();

	// Interrupts ongoing downloads
	void cancelDownloads();

	// Prepare the update batch/script file
	void PrepareUpdateBatchFile();

	void setBinaryAsExecutable();

	void removeDownload( std::string itemname );

private:
	// Throws if mirrors are empty
	void AssertMirrorsNotEmpty();

	void NotifyDownloadProgress();
	void NotifyFullUpdateProgress();

	// Notifier shortcut
	void NotifyFileProgress(vfs::Path file, CurFileInfo::Operation op, double fraction);

	// Returns false if the local files is missing or needs an update
	bool CheckLocalFile(vfs::Path installPath, const ReleaseFile& releaseFile);

	// Get the target path (defaults to current path)
	vfs::Directory getTargetDir();

	// Creates a mirrored download
	DownloadPtr prepareMirroredDownload(const std::string& remoteFile);

	// Downloads a file from a random mirror to the target folder
	void PerformSingleMirroredDownload(const std::string& remoteFile);

	// Downloads a file from a random mirror to the target folder, checking required size and CRC after download
	void PerformSingleMirroredDownload(const std::string& remoteFile, std::size_t requiredSize, unsigned int requiredCrc);

	// Starts the download and waits for completion
	void PerformSingleMirroredDownload(const DownloadPtr& download);

	void _markFileAsExecutable(vfs::Path path);

  bool isIgnored(std::string name);
};

} // namespace

#endif //__CAESARIA_UPDATER_H_INLCUDE__

