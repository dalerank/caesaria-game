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

#include "updater.hpp"

#include "releasefileset.hpp"
#include "http/mirrordownload.hpp"
#include "http/httprequest.hpp"
#include "constants.hpp"
#include "core/foreach.hpp"
#include "core/osystem.hpp"
#include "vfs/filesystem.hpp"
#include "util.hpp"

namespace updater
{

class Updater::Impl
{
public:
};

Updater::Updater(const UpdaterOptions& options, vfs::Path executable) :
  _d( new Impl ),
	_options(options),
	_downloadManager(new DownloadManager),
	_executable( executable ), // convert that file to lower to be sure
	_updatingUpdater(false)
{
  // Set up internet connectivity
  HttpConnectionPtr http( new HttpConnection() );
  http->drop();
  _conn = http;

  // Assign the proxy settings to the connection
  _options.CheckProxy(_conn);

  _ignoreList.insert(STABLE_VERSION_FILE);
  _ignoreList.insert(UPDATE_VERSION_FILE);
  _ignoreList.insert(CAESARIA_MIRRORS_INFO);

  MirrorDownload::InitRandomizer();

#ifdef GAME_PLATFORM_WIN
  if( !_executable.haveExtension() )
  {
    Logger::warning( "Adding EXE extension to executable: " + _executable.toString() );
    _executable.changeExtension(".exe");
  }
#endif
}

Updater::~Updater()
{

}

void Updater::setBinaryAsExecutable()
{
  std::vector<vfs::Path> executableNames;
#ifdef GAME_PLATFORM_WIN
  executableNames.push_back( "caesaria.exe" );
#elif defined(GAME_PLATFORM_LINUX)
  executableNames.push_back( "caesaria.linux" );
  executableNames.push_back( "caesaria.linux64" );
#elif defined(GAME_PLATFORM_MACOSX)
  executableNames.push_back( "caesaria.macos" );
#elif defined(GAME_PLATFORM_HAIKU)
  executableNames.push_back( "caesaria.haiku" );
#endif

  for( auto& name : executableNames )
  {
    vfs::Path path2exe = getTargetDir()/name;
    if( path2exe.exist() )
    {
      // set the executable bit on binary
      OSystem::markFileAsExecutable( path2exe.toString() );
    }
  }
}

void Updater::removeDownload(std::string itemname)
{
	_downloadQueue.removeItem( itemname );
}

void Updater::cleanupPreviousSession()
{
  // Remove batch file from previous run
  vfs::NFile::remove( getTargetDir()/UPDATE_UPDATER_BATCH_FILE );
}

bool Updater::isNeedLoadMirrorsFromServer()
{
  vfs::Directory folder = getTargetDir();
  vfs::Path mirrorPath = folder/CAESARIA_MIRRORS_INFO;

  if( !mirrorPath.exist() )
  {
    // No mirrors file
    Logger::update( "No mirrors file present on this machine.");
    return true;
  }

  // File exists, check options
  if( _options.isSet("keep-mirrors") )
  {
    Logger::update( "Skipping mirrors update (--keep-mirrors is set)." );
    return false;
  }

  // Update by default
  return true;
}

void Updater::downloadNewMirrors()
{
  std::string mirrorsUrl = CAESARIA_MAIN_SERVER;
  mirrorsUrl += CAESARIA_MIRRORS_INFO;

  Logger::warning( "Downloading mirror list from {}...", mirrorsUrl  ); // grayman - fixed

  vfs::Path mirrorPath = getTargetDir()/CAESARIA_MIRRORS_INFO;

  auto request = _conn->request( mirrorsUrl, mirrorPath.toString());
  request->execute();

  if( request->isOk() )
  {
    // Load the mirrors from the file
    loadMirrors();
  }
  else
  {
    Logger::warning( " Mirrors download failed: {}", request->GetErrorMessage() );
  }
}

void Updater::loadMirrors()
{
  vfs::Directory folder = getTargetDir();
  vfs::Path mirrorPath = folder.getFilePath( CAESARIA_MIRRORS_INFO );

  // Load the mirrors.txt into an INI file
  IniFilePtr mirrorsIni = IniFile::fromFile(mirrorPath);

  // Interpret the info and build the mirror list
  _mirrors = MirrorList( mirrorsIni );
}

std::size_t Updater::mirrors_n()
{
  return _mirrors.size();
}

void Updater::downloadStableVersion()
{
  downloadSingleFile(STABLE_VERSION_FILE);

  // Parse this file
  vfs::Directory folder = getTargetDir();
  vfs::Path filename = folder.getFilePath( STABLE_VERSION_FILE );
  auto inifile = IniFile::fromFile( filename );

  if (inifile == NULL)
  {
    throw FailureException("Could not download current version info file from server.");
  }

  // Build the release file set
  _latestRelease = ReleaseFileSet::LoadFromIniFile( inifile );
}

void Updater::downloadCurrentVersion()
{
  Logger::warning( " Downloading version information...");

  downloadSingleFile(UPDATE_VERSION_FILE);

  // Parse this downloaded file
  vfs::Directory folder = getTargetDir();
  auto inifile = IniFile::fromFile( folder.getFilePath( UPDATE_VERSION_FILE ) );

  if (inifile == NULL)
  {
    Logger::warning( "Cannot find downloaded version info file: {}", folder.getFilePath(UPDATE_VERSION_FILE).toCString() );
    return;
  }

  _releaseVersions.LoadFromIniFile( inifile );
}

void Updater::NotifyFileProgress(vfs::Path file, CurFileInfo::Operation op, double fraction)
{
  if (_fileProgressCallback != NULL)
  {
    CurFileInfo info;
    info.operation = op;
    info.file = file;
    info.progressFraction = fraction;

    _fileProgressCallback->onFileOperationProgress(info);
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
  for( auto& v : _releaseVersions )
    totalItems += v.second.size();

  std::size_t curItem = 0;

  for( auto& version : _releaseVersions )
  {
    Logger::warning( "Trying to match against version: " + version.first );

    const ReleaseFileSet& set = version.second;

    // Will be true on first mismatching file
    bool mismatch = false;

    for( auto& item : set )
    {
      NotifyFileProgress(item.second.file, CurFileInfo::Check, static_cast<double>(curItem) / totalItems);

      curItem++;

      vfs::Directory folder = getTargetDir();
      vfs::Path candidate = folder.getFilePath( item.second.file );

      bool filesEquale = utils::isEquale( candidate.baseName().toString(), _executable.baseName().toString(), utils::equaleIgnoreCase );
      if( filesEquale )
      {
        Logger::warning( "IGNORE" );
        continue;
      }

      if( !candidate.exist() )
      {
        Logger::warning( "MISSING" );
        mismatch = true;
        continue;
      }

      if (item.second.localChangesAllowed)
      {
        Logger::warning( "File {} exists, local changes are allowed, skipping.", candidate.toCString() );
        continue;
      }

      std::size_t candidateFilesize = vfs::NFile::size( candidate );

      if (candidateFilesize != item.second.filesize)
      {
        Logger::warning( "WRONG SIZE[need={}  have={}]", item.second.filesize, candidateFilesize );
        mismatch = true;
        continue;
      }

      // Calculate the CRC of this file
      if (!_options.isSet("no-crc"))
      {
        unsigned int crc = CRC::GetCrcForFile(candidate);

        if (crc != item.second.crc)
        {
          Logger::warning("WRONG CRC[need=%x have=%x]", item.second.crc, crc);
          mismatch = true;
          continue;
        }
      }

      // The file is matching - record this version
      Logger::warning( item.second.isWrongOS() ? "SKIP [WRONG OS]" : "NOT NEED UPDATE" );

      _fileVersions[candidate.toString()] = version.first;
    }

    // All files passed the check?
    if (!mismatch)
    {
      _pureLocalVersion = version.first;
      Logger::warning( " Local installation matches version: " + _pureLocalVersion );
    }
  }

  // Sum up the totals for all files, each file has exactly one version
  for( auto& i : _fileVersions )
  {
    // sum up the totals for this version
    const std::string& version = i.second;
    VersionTotal& total = _localVersions.insert( LocalVersionBreakdown::value_type(version, VersionTotal()) ).first->second;

    total.numFiles++;
    total.filesize += vfs::NFile::size( i.first );
  }

  Logger::warning( "The local files are matching {} different versions.", _localVersions.size() );

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
    foreach( i, _localVersions )
    {
      const std::string& version = i->first;

      Logger::warning( "Files matching version {}: {} (size: {})",
                       version.c_str(),
                       i->second.numFiles,
                       Util::getHumanReadableBytes(i->second.filesize).c_str() );
    }
  }
}

bool Updater::DifferentialUpdateAvailable()
{
  // Check applicable differential updates
  if (!_applicableDifferentialUpdates.empty())
  {
    return true;
  }

  Logger::warning( "No luck, differential updates don't seem to be applicable.");

  return false;
}

std::string Updater::getNewestVersion()
{
  for( auto i = _releaseVersions.rbegin();
       i != _releaseVersions.rend(); ++i)
  {
   return i->first;
  }

  return "";
}

std::string Updater::getDeterminedLocalVersion()
{
  return _pureLocalVersion;
}

DownloadPtr Updater::prepareMirroredDownload(const std::string& remoteFile)
{
  if (_mirrors.empty())
    throw FailureException("No mirror information, cannot continue.");

  vfs::Directory dir = getTargetDir();
  vfs::Path targetPath =  dir.getFilePath( remoteFile );

  // Remove target path first
  vfs::NFile::remove( targetPath );

  // Create a mirrored download
  return MirrorDownload::create(_conn, _mirrors, remoteFile, targetPath);
}

void Updater::downloadSingleFile(const std::string& remoteFile)
{
  // Create a mirrored download
  DownloadPtr download = prepareMirroredDownload(remoteFile);

  // Perform and wait for completion
  downloadSingleFile(download);
}

void Updater::downloadSingleFile(const std::string& remoteFile, std::size_t requiredSize, unsigned int requiredCrc)
{
  DownloadPtr download = prepareMirroredDownload(remoteFile);

  download->enableCrcCheck(!_options.isSet("no-crc"));
  download->enableFilesizeCheck(true);
  download->setRequiredCrc(requiredCrc);
  download->setRequiredFilesize(requiredSize);

  // Perform and wait for completion
  downloadSingleFile(download);
}

void Updater::downloadSingleFile(const DownloadPtr& download)
{
  int downloadId = _downloadManager->add(download);

  while( _downloadManager->hasPendingDownloads() )
  {
    _downloadManager->process();

    notifyDownloadProgress();

    for (int i = 0; i < 50; ++i)
        Util::Wait(10);
  }

  if( _downloadProgressCallback.isValid() )
  {
    _downloadProgressCallback->onDownloadFinish();
  }

  _downloadManager->remove(downloadId);
}

vfs::Directory Updater::getTargetDir()
{
  // Use the target directory
  if (_options.isSet("targetdir") && !_options.get("targetdir").empty())
  {
    return vfs::Path( _options.get("targetdir") );
  }

  // Get the current path
  vfs::Path targetPath = vfs::Directory::current();

  return targetPath;
}

void Updater::checkLocalFiles()
{
  _downloadQueue.clear();

  // Get the current path
  vfs::Directory targetDir = getTargetDir();

  Logger::warning( "Checking target folder: " + targetDir.toString() );

  std::size_t count = 0;
  for( auto& item : _latestRelease )
  {
    if (_fileProgressCallback != NULL)
    {
      CurFileInfo info;
      info.operation = CurFileInfo::Check;
      info.file = item.second.file;
      info.progressFraction = static_cast<double>(count) / _latestRelease.size();

      _fileProgressCallback->onFileOperationProgress(info);
    }

    //Logger::warning( "Checking for file: " + i->second.file.toString() + "...");
    if( !checkLocalFile(targetDir, item.second) )
    {
      // A member is missing or out of date, mark the archive for download
      _downloadQueue.insert(item);
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
    removeAllPackagesExceptUpdater();
  }
}

bool Updater::isIgnored(std::string name)
{
  return _ignoreList.find(name) != _ignoreList.end();
}

bool Updater::checkLocalFile(vfs::Path installPath, const ReleaseFile& releaseFile)
{
  vfs::Path localFile = vfs::Directory(installPath).getFilePath(releaseFile.file);

  //Logger::warning( " Checking for file " + releaseFile.file.toString() + ": ");

  // check ignore list
  if( isIgnored( releaseFile.file.canonical().toString() ) )
  {
    Logger::warning("IGNORED");
    return true; // ignore this file
  }

  if (!localFile.exist())
  {
    Logger::warning("MISSING");
    return false;
  }
  // File exists
  // Compare file size
  std::size_t fileSize = vfs::NFile::size(localFile);

  if (fileSize != releaseFile.filesize)
  {
    Logger::warning("SIZE MISMATCH");
    return false;
  }
  // Size is matching

  // Check CRC if not disabled
  if (!_options.isSet("no-crc"))
  {
    unsigned int existingCrc = CRC::GetCrcForFile(localFile);

    if (existingCrc != releaseFile.crc)
    {
      Logger::warning("CRC MISMATCH");
      return false;
    }
  }

  Logger::warning("EQUALE");
  return true;
}

bool Updater::isDownloadQueueFull()
{
  return !_downloadQueue.empty();
}

void Updater::prepareUpdateStep(const std::string& prefix)
{
  vfs::Directory targetdir = getTargetDir();

  // Create a download for each of the files
  bool checkCrc = !_options.isSet("no-crc");
  for( auto& item : _downloadQueue )
  {
    auto download = MirrorDownload::create(_conn, _mirrors,
                                           item.second.file.toString(),
                                           targetdir.getFilePath(prefix+item.second.file.toString() ) );

    download->enableCrcCheck( checkCrc );
    download->enableFilesizeCheck( true );
    download->setRequiredCrc( item.second.crc );
    download->setRequiredFilesize( item.second.filesize );

    item.second.downloadId = _downloadManager->add(download);
  }
}

void Updater::PerformUpdateStep()
{
	// Wait until the download is done
  while (_downloadManager->hasPendingDownloads())
  {
    // For catching terminations
    _downloadManager->process();

    if (_downloadManager->HasFailedDownloads())
    {
      // Lets continue downloading. Fetch as much as we can.
    }

    notifyDownloadProgress();
    NotifyFullUpdateProgress();
    Util::Wait(100);
  }

  Logger::warning("Downloading finished");

  if (_downloadManager->HasFailedDownloads())
  {
    Logger::warning("Some downloads failed. Check log above for details.");
  }

  if (_downloadProgressCallback.isValid())
  {
    _downloadProgressCallback->onDownloadFinish();
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

  for( auto& i : _downloadQueue )
  {
    if (i.second.downloadId == -1)
    {
        continue;
    }

    DownloadPtr download = _downloadManager->GetDownload(i.second.downloadId);

    if (download == NULL) continue;

    if (download->GetStatus() == Download::SUCCESS)
    {
        totalBytesDownloaded += i.second.filesize;
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

void Updater::notifyDownloadProgress()
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

    MirrorDownloadPtr mirrorDownload = ptr_cast<MirrorDownload>( curDownload );

    if (mirrorDownload != NULL)
    {
      info.mirrorDisplayName = mirrorDownload->GetCurrentMirrorName();
    }

    _downloadProgressCallback->OnDownloadProgress(info);
  }
}

void Updater::cleanupUpdateStep()
{
  for (ReleaseFileSet::iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
  {
    _downloadManager->remove(i->second.downloadId);
  }

  _downloadQueue.clear();
}


std::size_t Updater::GetNumFilesToBeUpdated()
{
  return _downloadQueue.size();
}

std::size_t Updater::GetTotalDownloadSize()
{
  std::size_t totalSize = 0;

  foreach( i, _downloadQueue )
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
  if(_options.isSet("noselfupdate") || _options.isSet("no-exec") )
  {
    return false; // no self-update overrides everything
  }

  Logger::warning( "Looking for executable " + _executable.toString() + " in download queue.");

  vfs::Path myPath = vfs::Directory::applicationDir()/_executable;
  ByteArray crcData = vfs::NFile::open( myPath ).readAll();
  unsigned int fileSize = vfs::NFile::size( myPath );

  unsigned int crc = crcData.crc32( 0 );

  // Is this the updater?
  for (ReleaseFileSet::const_iterator i = _downloadQueue.begin(); i != _downloadQueue.end(); ++i)
  {
    if( i->second.isUpdater(_executable.toString() ) )
    {
      if( i->second.crc != crc && i->second.filesize != fileSize  )
      {
        Logger::warning( "The updater binary needs to be updated.");
        return true;
      }

      return false;
    }
  }

  Logger::warning( "Didn't find executable name " + _executable.toString() + " in download queue.");

  return false;
}

void Updater::removeAllPackagesExceptUpdater()
{
  Logger::warning("Removing all packages, except the one containing the updater");

  for( auto i = _downloadQueue.begin(); i != _downloadQueue.end(); /* in-loop */ )
  {
    if (i->second.isUpdater(_executable.toString()))
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

void Updater::restartUpdater()
{
  vfs::Path temporaryUpdater = TEMP_FILE_PREFIX + _executable.toString();
  // Create a new batch file in the target location
  vfs::Directory targetdir = getTargetDir();
  OSystem::restartProcess( temporaryUpdater.toString(), targetdir.toString(), _options.GetRawCmdLineArgs() );
}

void Updater::postUpdateCleanup()
{
  vfs::Directory pdir =  getTargetDir();
  vfs::Entries entries = pdir.entries();
  for( auto& entry : entries )
  {
    if( utils::startsWith( entry.name.toString(), TEMP_FILE_PREFIX) )
    {
      vfs::NFile::remove( entry.fullpath );
    }
  }
}

void Updater::cancelDownloads()
{
  _downloadManager->clearDownloads();
}

void SteamHelper::checkDepsAndStart()
{
#ifdef GAME_PLATFORM_MACOSX
  vfs::Path sdl2relpath = "Library/Frameworks/SDL2.framework";
  vfs::Path sdl2abspath = vfs::Directory::userDir()/sdl2relpath;

  if( !sdl2abspath.exist() )
  {
    vfs::Path tmp = "__install_frameworks.sh";
    Logger::warning( "Preparing updater for install frameworks " + tmp.toString() );

    std::ofstream batch(tmp.toCString());

    // grayman - accomodate spaces in pathnames
    tmp = vfs::Directory::applicationDir()/tmp;

    batch << "#!/usr/bin/env bash" << std::endl;
    batch << "SDLFR=~/Library/Frameworks/SDL2.framework" << std::endl;
    batch << "SAVEDIR=saves" << std::endl;
    batch << "LIBDIR=~/Library/Frameworks/" << std::endl;
    batch << "# if the file doesn't exist, try to create folder" << std::endl;
    batch << "if [ ! -d $SDLFR ]" << std::endl;
    batch << "then" << std::endl;
    batch << "hdiutil mount SDL2_mixer-2.0.0.dmg" << std::endl;
    batch << "hdiutil mount SDL2-2.0.3.dmg" << std::endl;
    batch << "mkdir -p $LIBDIR" << std::endl;
    batch << "cp -r /Volumes/SDL2/SDL2.framework $LIBDIR" << std::endl;
    batch << "cp -r /Volumes/SDL2_mixer/SDL2_mixer.framework $LIBDIR" << std::endl;
    batch << "hdiutil unmount /Volumes/SDL2" << std::endl;
    batch << "hdiutil unmount /Volumes/SDL2_mixer" << std::endl;
    batch << "fi" << std::endl;
    batch << "if [ ! -d $SAVEDIR ]" << std::endl;
    batch << "then" << std::endl;
    batch << "rm -f $SAVEDIR" << std::endl;
    batch << "mkdir $SAVEDIR" << std::endl;
    batch << "fi" << std::endl;

    batch.close();

    // Mark the shell script as executable in *nix
    OSystem::markFileAsExecutable(tmp.toCString());
    system( "./__install_frameworks.sh" );
  }

  system( "./caesaria.macos &" );
  exit(EXIT_SUCCESS);
#elif defined(GAME_PLATFORM_WIN)
  STARTUPINFOA siStartupInfo;
  PROCESS_INFORMATION piProcessInfo;

  memset(&siStartupInfo, 0, sizeof(siStartupInfo));
  memset(&piProcessInfo, 0, sizeof(piProcessInfo));

  siStartupInfo.cb = sizeof(siStartupInfo);

  vfs::Directory parentPath = vfs::Directory::applicationDir();

  Logger::warning( "Starting game in " + parentPath.toString() );

  BOOL success = CreateProcessA( NULL, "caesaria.exe", NULL, NULL,  false, 0, NULL,
                                 parentPath.toCString(), &siStartupInfo, &piProcessInfo);

#elif defined(GAME_PLATFORM_LINUX)
  system( "./caesaria.linux &" );
  exit(EXIT_SUCCESS);
#endif
}

} // namespace
