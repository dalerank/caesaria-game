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
	// The status
	ConsoleUpdater::Outcome outcome;
};

namespace
{
	const std::size_t PROGRESS_METER_WIDTH = 25;

	std::string GetShortenedString(const std::string& input, std::size_t maxLength)
	{
		if (input.length() > maxLength)
		{
			if (maxLength == 0)
			{
				return "";
			}
			else if (maxLength < 3)
			{
				return std::string(maxLength, '.');
			}

			std::size_t diff = input.length() - maxLength + 3; // 3 chars for the ellipsis
			std::size_t curLength = input.length();

			return input.substr(0, (curLength - diff) / 2) + "..." +
				input.substr((curLength + diff) / 2);
		}

		return input;
	}
}

ConsoleUpdater::ConsoleUpdater(int argc, char* argv[]) :
	_options(argc, argv),
	_controller(*this, vfs::Path( argv[0] ), _options),
	_done(false),
	_d( new Impl )
{
	_abortSignalHandler = makeDelegate( this, &ConsoleUpdater::onAbort);

	signal(SIGABRT, resolveAbortSignal);
	signal(SIGTERM, resolveAbortSignal);
	signal(SIGINT, resolveAbortSignal);

	_d->outcome = ConsoleUpdater::None;
}

ConsoleUpdater::~ConsoleUpdater() {}

ConsoleUpdater::Outcome ConsoleUpdater::GetOutcome()
{
	return _d->outcome;
}

void ConsoleUpdater::run()
{
	// Parse the command line
	if( _options.isSet("help") )
	{
		_options.PrintHelp();
		_d->outcome = ConsoleUpdater::Ok;
		return;
	}

	//VariantMap saveSettings = SaveAdapter::load( "/resources/settings.model" );
	_controller.StartOrContinue();

	// Main loop, just keep the controller object going
	while( /*!_done*/ !_controller.IsDone() )
	{
		Sleep(50);
	}

	if (!_controller.AllThreadsDone())
	{
		// Termination seems to be abnormal, attempt to exit gracefully 

		Logger::warning( " Waiting 5 seconds for threads to finish their work..." );
		std::size_t count = 0;

		while (count++ < 100 && !_controller.AllThreadsDone())
		{
			Sleep(50);
		}

		// Exit anyway after 5000 milliseconds

		_controller.PerformPostUpdateCleanup();
	}
}

void ConsoleUpdater::resolveAbortSignal(int signal)
{
	if (_abortSignalHandler)
	{
		_abortSignalHandler(signal);
	}
}

void ConsoleUpdater::onAbort(int)
{
	Logger::warning( "\nAbort signal received, trying to exit gracefully.");

	_controller.Abort();

	_done = true; // exit main loop
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
		bool keepMirrors = _options.isSet("keep-mirrors");

		Logger::warning( keepMirrors ? "\n Skipped downloading mirrors." : "\n Done downloading mirrors.");

		std::size_t numMirrors = _controller.GetNumMirrors();

		Logger::warning( "   Found %d mirror%s.", numMirrors, (numMirrors == 1 ? "" : "s") );

		if( numMirrors == 0 )
		{
			Logger::warning( " No mirror information available - cannot continue.");

			// Stop right here
			_controller.Abort();
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

		if (!_controller.GetNewestVersion().empty())
		{
			Logger::warning( "Newest version is " + _controller.GetNewestVersion() );
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

		if (_controller.GetLocalVersion().empty())
		{
			Logger::warning( "no luck, zip files do not match.");
		}
		else
		{
			std::string versionFound = utils::format( 0xff, "local version is %s.", _controller.GetLocalVersion().c_str() );
			Logger::warning( versionFound );
		}
	}
	break;

	case CompareLocalFilesToNewest:
	{
		Logger::warning( " Done comparing local files to server definitions.");

		std::string sizeStr = Util::getHumanReadableBytes(_controller.GetTotalDownloadSize());
		std::size_t numFiles = _controller.GetNumFilesToBeUpdated();

		std::string totalSize = utils::format( 0xff, "%d %s to be downloaded (size: %s).",
																									numFiles,
																									(numFiles == 1 ? "file needs" : "files need"),
																									sizeStr.c_str() );

		vfs::Path settingsPath( "resources/settings.model" );
		if( settingsPath.exist() )
		{
			Logger::warning( "User also have own settings, remove it from downloading list" );
			_controller.removeDownload( settingsPath.toString() );
		}        

        if( _options.isSet( "no-exec" ) )
        {
            Logger::warning( "Remove executable files" );
            StringArray extensions;
            extensions << "linux" << "macos" << "exe" << "haiku";

            std::string bin="caesaria.", upd="updater.";
            foreach( it, extensions )
            {
              _controller.removeDownload( bin + *it );
              _controller.removeDownload( upd + *it );
            }
        }

		// Print a summary
		if( _controller.NewUpdaterAvailable() )
		{
			Logger::warning( " A new updater is available: " + totalSize );
		}
		else if (_controller.LocalFilesNeedUpdate())
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
																											Util::getHumanReadableBytes(_controller.GetTotalBytesDownloaded()).c_str() );
		Logger::warning( totalBytesStr);

		if (!_controller.LocalFilesNeedUpdate())
		{
			Logger::warning( "----------------------------------------------------------------------------");
			Logger::warning( " Your CaesarIA installation is up to date.");
		}
	}
	break;

	case PostUpdateCleanup:
		break;

	case Done:
		_done = true; // break main loop
		break;

	case RestartUpdater:
		_done = true; // break main loop
		break;
	};
}

void ConsoleUpdater::OnFailure(UpdateStep step, const std::string& errorMessage)
{
	Logger::warning( "\n");
	Logger::warning( errorMessage );

	_done = true; // break main loop
}

void ConsoleUpdater::OnMessage(const std::string& message)
{
	Logger::warning( "=======================================");
	Logger::warning( message);
}

void ConsoleUpdater::OnWarning(const std::string& message)
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
		if (!_info.file.toString().empty()
				&& info.file.toString() != _info.file.toString() )
		{
			// New file, finish the current download
			_info.progressFraction = 1.0f;
			_progressDone = false;
			progressPrinted = true;
			PrintProgress();

			// Add a line break when a new file starts
			Logger::warning( utils::format( 0xff, "\nDownloading from Mirror %s: %s", info.mirrorDisplayName.c_str(), info.file.toString().c_str() ) );
		}
		else if (_info.file.toString().empty())
		{
			// First file
			Logger::warning( utils::format( 0xff, " Downloading from Mirror %s: %s", info.mirrorDisplayName.c_str(), info.file.toString().c_str() ) );
		}

		_info = info;

		// Print the new info
		if( !progressPrinted )
			PrintProgress();

		// Add a new line if we're done here
		if( info.progressFraction >= 1 && !_progressDone)
		{
			_progressDone = true;
			Logger::warning( "\n" );
		}
		break;

	case ProgressInfo::FileOperation:

		_info = info;

		// Print the new info
		PrintProgress();

		// Add a new line if we're done here
		if (info.progressFraction >= 1)
		{
			Logger::warning( "\n");
		}
	break;

	default: break;
	};
}

void ConsoleUpdater::PrintProgress()
{
	// Progress bar
	Logger::update( "\r" );

	std::size_t numTicks = static_cast<std::size_t>(floor(_info.progressFraction * PROGRESS_METER_WIDTH));
	std::string progressBar(numTicks, '=');
	std::string progressSpace(PROGRESS_METER_WIDTH - numTicks, ' ');

	std::string line = " [" + progressBar + progressSpace + "]";
	
	// Percent
	line += utils::format( 0xff, " %2.1f%%", _info.progressFraction*100 );

	switch (_info.type)
	{
	case ProgressInfo::FileDownload:	
	{
		line += " at " + Util::getHumanReadableBytes( _info.downloadSpeed ) + "/sec ";
	}
	break;

	case ProgressInfo::FileOperation:
	{
		std::string verb;

		switch (_info.operationType)
		{
		case ProgressInfo::Check: 
			verb = "Checking: "; 
			break;
		case ProgressInfo::Remove: 
			verb = "Removing: ";
			break;
		case ProgressInfo::Replace: 
			verb = "Replacing: ";
			break;
		case ProgressInfo::Add: 
			verb = "Adding: ";
			break;
		case ProgressInfo::RemoveFilesFromPK4: 
			verb = "Preparing PK4: ";
			break;
		default: 
			verb = "File: ";
		};

		line += " " + verb;

		std::size_t remainingLength = line.length() > 79 ? 0 : 79 - line.length();
		line += GetShortenedString(_info.file.baseName().toString(), remainingLength);
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

Delegate1<int> ConsoleUpdater::_abortSignalHandler;

} // namespace
