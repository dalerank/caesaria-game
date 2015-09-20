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

#include "consoleupdater.hpp"
#include "core/saveadapter.hpp"

#include "util.hpp"

namespace updater
{

class ConsoleUpdater::Impl
{
public:
  // Most recent progress info
  ProgressInfo info;

  static Delegate1<int> abortSignalHandler;

  UpdaterOptions options;

	// The status
	ConsoleUpdater::Outcome outcome;

  // The update controller manages the logic
  ScopedPtr<UpdateController> controller;

  // Exit flag
  volatile bool done;

  bool progressDone;
};

const std::size_t PROGRESS_METER_WIDTH = 25;

void ConsoleUpdater::_initAbortSignalHandler()
{
  signal(SIGABRT, resolveAbortSignal);
  signal(SIGTERM, resolveAbortSignal);
  signal(SIGINT, resolveAbortSignal);
}

ConsoleUpdater::ConsoleUpdater(int argc, char* argv[]) :
	_d( new Impl )
{
  _d->options.reset( argc, argv );
  _d->controller.reset( new UpdateController( *this, vfs::Path( argv[0] ), _d->options ) );
  _d->done = false;
  _d->abortSignalHandler = makeDelegate( this, &ConsoleUpdater::onAbort);
  _d->outcome = ConsoleUpdater::None;

  _initAbortSignalHandler();
}

ConsoleUpdater::~ConsoleUpdater() {}

ConsoleUpdater::Outcome ConsoleUpdater::GetOutcome()
{
	return _d->outcome;
}

void ConsoleUpdater::run()
{
	// Parse the command line
  if( _d->options.isSet("help") )
	{
    _d->options.printHelp();
		_d->outcome = ConsoleUpdater::Ok;
		return;
	}

  _d->controller->start();

	// Main loop, just keep the controller object going
  while( !_d->controller->isDone() )
	{
		Sleep(50);
	}

  if (!_d->controller->allThreadsDone())
	{
		// Termination seems to be abnormal, attempt to exit gracefully 

		Logger::warning( " Waiting 5 seconds for threads to finish their work..." );
		std::size_t count = 0;

    while (count++ < 100 && !_d->controller->allThreadsDone())
		{
			Sleep(50);
		}

		// Exit anyway after 5000 milliseconds

    _d->controller->doPostUpdateCleanup();
	}
}

void ConsoleUpdater::resolveAbortSignal(int signal)
{
  if( !Impl::abortSignalHandler.empty() )
	{
    Impl::abortSignalHandler( signal );
	}
}

void ConsoleUpdater::onAbort(int)
{
	Logger::warning( "\nAbort signal received, trying to exit gracefully.");

  _d->controller->abort();

  _d->done = true; // exit main loop
}

void ConsoleUpdater::onStartStep(UpdateStep step)
{
	switch (step)
	{
	case Init:
		Logger::warning("----------------------------------------------------------------------------");
		Logger::update(" Initialising...");
		break;

	case CleanupPreviousSession:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::update( " Cleaning up previous update session...");
		break;

	case UpdateMirrors:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::update( " Update mirrors information...", true );
		break;

	case DownloadStableVersion:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::update( " Downloading stable version info...", true);
		break;

	case DownloadVersionInfo:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading available version info...");
		break;

	case DetermineLocalVersion:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Trying to match local files to version definitions...");
		break;

	case CompareLocalFilesToNewest:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Comparing local files to server definitions...");
		break;

	case DownloadNewUpdater:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading CaesarIA updater...");
		break;

	case DownloadDifferentialUpdate:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading differential update package...");
		break;

	case PerformDifferentialUpdate:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Applying differential update package...");
		break;

	case DownloadFullUpdate:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading updates...");
		break;

	default:
		break;
	};
}

void ConsoleUpdater::onFinishStep(UpdateStep step)
{
	switch (step)
	{
	case Init:
	{
		Logger::warning(" OK");
	}
	break;

	case CleanupPreviousSession:
	{
		Logger::warning(" OK");
	}
	break;

	case UpdateMirrors:
	{
		// Mirrors
    bool keepMirrors = _d->options.isSet("keep-mirrors");

		Logger::warning( keepMirrors ? "\n Skipped downloading mirrors." : "\n Done downloading mirrors.");

    std::size_t numMirrors = _d->controller->mirrors_n();

		Logger::warning( "   Found %d mirror%s.", numMirrors, (numMirrors == 1 ? "" : "s") );

		if( numMirrors == 0 )
		{
			Logger::warning( " No mirror information available - cannot continue.");

			// Stop right here
      _d->controller->abort();
		}
	}
	break;

	case DownloadStableVersion:
	{
		Logger::warning( " Done downloading stable version info.");
	}
	break;

	case DownloadVersionInfo:
	{
		Logger::warning( " Done downloading versions.");

    if (!_d->controller->farVersion().empty())
		{
      Logger::warning( "Newest version is " + _d->controller->farVersion() );
		}
		else
		{
			Logger::warning("");
		}
	}
	break;

	case DetermineLocalVersion:
	{
		Logger::warning( " Done comparing local files: ");

    if (_d->controller->localVersion().empty())
		{
			Logger::warning( "no luck, zip files do not match.");
		}
		else
		{
      std::string versionFound = utils::format( 0xff, "local version is %s.", _d->controller->localVersion().c_str() );
			Logger::warning( versionFound );
		}
	}
	break;

	case CompareLocalFilesToNewest:
	{
		Logger::warning( " Done comparing local files to server definitions.");

    std::string sizeStr = Util::getHumanReadableBytes(_d->controller->totalDownloadSize());
    std::size_t numFiles = _d->controller->flesToBeUpdated_n();

		std::string totalSize = utils::format( 0xff, "%d %s to be downloaded (size: %s).",
																									numFiles,
																									(numFiles == 1 ? "file needs" : "files need"),
																									sizeStr.c_str() );

		vfs::Path settingsPath( "resources/settings.model" );
		if( settingsPath.exist() )
		{
			Logger::warning( "User also have own settings, remove it from downloading list" );
      _d->controller->removeDownload( settingsPath.toString() );
		}        

    if( _d->options.isSet( "no-exec" ) )
    {
        Logger::warning( "Remove executable files" );
        StringArray extensions;
        extensions << "linux" << "macos" << "exe" << "haiku";

        std::string bin="caesaria.", upd="updater.";
        for( auto ext : extensions )
        {
          _d->controller->removeDownload( bin + ext );
          _d->controller->removeDownload( upd + ext );
        }
    }

		// Print a summary
    if( _d->controller->haveNewUpdater() )
		{
			Logger::warning( " A new updater is available: " + totalSize );
		}
    else if (_d->controller->haveNewFiles())
		{
			Logger::warning( " Updates are available.");
			Logger::warning( totalSize );
		}
		else
		{
			Logger::warning( " Your CaesarIA installation is up to date" );
		}
	}
	break;

	case DownloadNewUpdater:
	{
		Logger::warning( " Done downloading updater - will restart the application.");
	}
	break;

	case DownloadDifferentialUpdate:
	{
		Logger::warning(" Done downloading the differential update.");
	}
	break;

	case PerformDifferentialUpdate:
	{
		Logger::warning( "Done applying the differential update." );
	}
	break;

	case DownloadFullUpdate:
	{
		Logger::warning( " Done downloading updates.");

		std::string totalBytesStr = utils::format( 0xff, " Total bytes downloaded: %s",
                                                      Util::getHumanReadableBytes(_d->controller->totalDownloadedBytes()).c_str() );
		Logger::warning( totalBytesStr);

    if (!_d->controller->haveNewFiles())
		{
			Logger::warning( "----------------------------------------------------------------------------");
			Logger::warning( " Your CaesarIA installation is up to date.");
		}
	}
	break;

	case PostUpdateCleanup:
  break;

  case Done: _d->done = true; // break main loop
  break;

  case RestartUpdater: _d->done = true; // break main loop
  break;
	};
}

void ConsoleUpdater::OnFailure(UpdateStep step, const std::string& errorMessage)
{
	Logger::warning( "\n");
	Logger::warning( errorMessage );

  _d->done = true; // break main loop
}

void ConsoleUpdater::onMessage(const std::string& message)
{
	Logger::warning( "=======================================");
	Logger::warning( message);
}

void ConsoleUpdater::onWarning(const std::string& message)
{
	Logger::warning( "============== WARNING ================");
	Logger::warning( message);
	Logger::warning( "=======================================");

	Logger::warning( "Waiting 10 seconds before continuing automatically...");

	Util::Wait(10000);
}

void ConsoleUpdater::onProgressChange(const ProgressInfo& info)
{
	bool progressPrinted = false;
	switch (info.type)
	{
	case ProgressInfo::FileDownload:
		if( info.mirrorDisplayName.empty() )
			break;

		// Download progress
    if (!_d->info.file.toString().empty()
        && info.file.toString() != _d->info.file.toString() )
		{
			// New file, finish the current download
      _d->info.progressFraction = 1.0f;
      _d->progressDone = false;
			progressPrinted = true;
      printProgress();

			// Add a line break when a new file starts
      Logger::warning( utils::format( 0xff, "\nDownloading from Mirror %s: %s", info.mirrorDisplayName.c_str(), info.file.toCString() ) );
		}
    else if (_d->info.file.toString().empty())
		{
			// First file
      Logger::warning( utils::format( 0xff, " Downloading from Mirror %s: %s", info.mirrorDisplayName.c_str(), info.file.toCString() ) );
		}

    _d->info = info;

		// Print the new info
		if( !progressPrinted )
      printProgress();

		// Add a new line if we're done here
    if( info.progressFraction >= 1 && !_d->progressDone)
		{
      _d->progressDone = true;
			Logger::warning( "\n" );
		}
		break;

	case ProgressInfo::FileOperation:

    _d->info = info;

		// Print the new info
    printProgress();

		// Add a new line if we're done here
		if (info.progressFraction >= 1)
		{
			Logger::warning( "\n");
		}
	break;

	default: break;
	};
}

void ConsoleUpdater::printProgress()
{
	// Progress bar
	Logger::update( "\r" );

  std::size_t numTicks = static_cast<std::size_t>(floor(_d->info.progressFraction * PROGRESS_METER_WIDTH));
	std::string progressBar(numTicks, '=');
	std::string progressSpace(PROGRESS_METER_WIDTH - numTicks, ' ');

	std::string line = " [" + progressBar + progressSpace + "]";
	
	// Percent
  line += utils::format( 0xff, " %2.1f%%", _d->info.progressFraction*100 );

  switch (_d->info.type)
	{
	case ProgressInfo::FileDownload:	
	{
    line += " at " + Util::getHumanReadableBytes( _d->info.connection.speed ) + "/sec ";
	}
	break;

	case ProgressInfo::FileOperation:
	{
		std::string verb;

    switch (_d->info.operationType)
		{
		case ProgressInfo::Check: 			verb = "Checking: "; 			break;
		case ProgressInfo::Remove: 			verb = "Removing: ";			break;
    case ProgressInfo::Replace: 			verb = "Replacing: ";		break;
    case ProgressInfo::Add: 			verb = "Adding: ";    			break;
		case ProgressInfo::RemoveFilesFromPK4: 			verb = "Preparing PK4: ";			break;
    default: 			verb = "File: ";
    };

		line += " " + verb;

		std::size_t remainingLength = line.length() > 79 ? 0 : 79 - line.length();
    line += utils::toShortString( _d->info.file.baseName().toString(), remainingLength);
	}
	break;

	default: break;
	};

	// Expand the line length to 79 characters
	if (line.length() < 79)
	{
		line += std::string(79 - line.length(), ' ');
	}
	else if (line.length() > 79)
	{
		line = line.substr(0, 79);
	}

	Logger::update( line );
}

//create static signal
Delegate1<int> ConsoleUpdater::Impl::abortSignalHandler;

} // namespace
