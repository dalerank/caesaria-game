/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5598 $ (Revision of last commit) 
 $Date: 2012-10-19 19:46:11 +0400 (Пт, 19 окт 2012) $ (Date of last commit)
 $Author: greebo $ (Author of last commit)
 
******************************************************************************/

#include "ConsoleUpdater.h"

#include "Util.h"

namespace tdm
{

namespace updater
{

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
	_outcome(None),
	_options(argc, argv),
	_controller(*this, io::FilePath( argv[0] ), _options),
	_done(false)
{
	_abortSignalHandler = makeDelegate( this, &ConsoleUpdater::OnAbort);

	signal(SIGABRT, AbortSignalHandler);
	signal(SIGTERM, AbortSignalHandler);
	signal(SIGINT, AbortSignalHandler);
}

ConsoleUpdater::~ConsoleUpdater()
{
}

void ConsoleUpdater::Run()
{
	// Parse the command line
	if( _options.isSet("help") )
	{
		_options.PrintHelp();
		_outcome = Ok;
		return;
	}

	_controller.StartOrContinue();

	// Main loop, just keep the controller object going
	while (!_done)
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

void ConsoleUpdater::AbortSignalHandler(int signal)
{
	if (_abortSignalHandler)
	{
		_abortSignalHandler(signal);
	}
}

void ConsoleUpdater::OnAbort(int)
{
	Logger::warning( "\nAbort signal received, trying to exit gracefully.");

	_controller.Abort();

	_done = true; // exit main loop
}

void ConsoleUpdater::OnStartStep(UpdateStep step)
{
	switch (step)
	{
	case Init:
		Logger::warning("----------------------------------------------------------------------------");
		Logger::warning(" Initialising...");
		break;

	case CleanupPreviousSession:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Cleaning up previous update session...");
		break;

	case UpdateMirrors:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading mirror information...");
		break;

	case DownloadCrcs:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading CRC file...");
		break;

	case DownloadVersionInfo:
		Logger::warning( "----------------------------------------------------------------------------");
		Logger::warning( " Downloading version info file...");
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
		Logger::warning( " Downloading TDM Update application...");
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

void ConsoleUpdater::OnFinishStep(UpdateStep step)
{
	switch (step)
	{
	case Init:
	{
		Logger::warning(" Done.");
	}
	break;

	case CleanupPreviousSession:
	{
		Logger::warning(" Done.");
	}
	break;

	case UpdateMirrors:
	{
		// Mirrors
		bool keepMirrors = _options.isSet("keep-mirrors");

		if (keepMirrors)
		{
			Logger::warning( " Skipped downloading mirrors.");
		}
		else
		{
			Logger::warning( " Done downloading mirrors.");
		}

		std::size_t numMirrors = _controller.GetNumMirrors();

		Logger::warning( "   Found %d mirror%s.", numMirrors, (numMirrors == 1 ? "" : "s") );

		if (numMirrors == 0)
		{
			Logger::warning( " No mirror information available - cannot continue.");

			// Stop right here
			_controller.Abort();
		}
	}
	break;

	case DownloadCrcs:
	{
		//TraceLog::WriteLine(LOG_STANDARD, " Done downloading checksums.");
	}
	break;

	case DownloadVersionInfo:
	{
		Logger::warning( " Done downloading versions.");

		if (!_controller.GetNewestVersion().empty())
		{
			Logger::warning( "Newest version is %s.", _controller.GetNewestVersion().c_str() );
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
			Logger::warning( "no luck, PK4 files do not match.");
		}
		else
		{
			std::string versionFound = StringHelper::format( 0xff, "local version is %s.", _controller.GetLocalVersion().c_str() );
			Logger::warning( versionFound );
		}
	}
	break;

	case CompareLocalFilesToNewest:
	{
		Logger::warning( " Done comparing local files to server definitions.");

		std::string sizeStr = Util::GetHumanReadableBytes(_controller.GetTotalDownloadSize());
		std::size_t numFiles = _controller.GetNumFilesToBeUpdated();

		std::string totalSize = StringHelper::format( 0xff, "%d %s to be downloaded (size: %s).",
																									numFiles,
																									(numFiles == 1 ? "file needs" : "files need"),
																									sizeStr.c_str() );

		// Print a summary
		if (_controller.NewUpdaterAvailable())
		{
			Logger::warning( " A new updater is available: %s", totalSize.c_str());
		}
		else if (_controller.LocalFilesNeedUpdate())
		{
			if (_controller.DifferentialUpdateAvailable())
			{
				Logger::warning( " A differential update is available.");

				sizeStr = Util::GetHumanReadableBytes(_controller.GetTotalDifferentialUpdateSize());
				totalSize = StringHelper::format( 0xff, " Download size: %s", sizeStr.c_str() );
			}
			else
			{
				Logger::warning( " Updates are available.");
			}

			Logger::warning( totalSize );
		}
		else
		{
			Logger::warning( " Your TDM installation is up to date" );
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
		Logger::warning( " Done applying the differential update.");
	}
	break;

	case DownloadFullUpdate:
	{
		Logger::warning( " Done downloading updates.");

		std::string totalBytesStr = StringHelper::format( 0xff, " Total bytes downloaded: %s",
																											Util::GetHumanReadableBytes(_controller.GetTotalBytesDownloaded()).c_str() );
		Logger::warning( totalBytesStr);

		if (!_controller.LocalFilesNeedUpdate())
		{
			Logger::warning( "----------------------------------------------------------------------------");
			Logger::warning( " Your TDM installation is up to date.");
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
	Logger::warning( "");
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

void ConsoleUpdater::OnProgressChange(const ProgressInfo& info)
{
	switch (info.type)
	{
	case ProgressInfo::FileDownload:
		// Download progress
		if (!_info.file.toString().empty() && info.file.toString() != _info.file.toString() )
		{
			// New file, finish the current download
			_info.progressFraction = 1.0f;
			PrintProgress();

			// Add a line break when a new file starts
			Logger::warning( "" );

			Logger::warning( StringHelper::format( 0xff, " Downloading from Mirror %s: %s", info.mirrorDisplayName.c_str(), info.file.toString().c_str() ) );
		}
		else if (_info.file.toString().empty())
		{
			// First file
			Logger::warning( StringHelper::format( 0xff, " Downloading from Mirror %s: %s", info.mirrorDisplayName.c_str(), info.file.toString().c_str() ) );
		}

		_info = info;

		// Print the new info
		PrintProgress();

		// Add a new line if we're done here
		if (info.progressFraction >= 1)
		{
			Logger::warning( "");
		}
		break;

	case ProgressInfo::FileOperation:

		_info = info;

		// Print the new info
		PrintProgress();

		// Add a new line if we're done here
		if (info.progressFraction >= 1)
		{
			Logger::warning( "");
		}
		break;
	};
}

void ConsoleUpdater::PrintProgress()
{
	// Progress bar
	std::size_t numTicks = static_cast<std::size_t>(floor(_info.progressFraction * PROGRESS_METER_WIDTH));
	std::string progressBar(numTicks, '=');
	std::string progressSpace(PROGRESS_METER_WIDTH - numTicks, ' ');

	std::string line = " [" + progressBar + progressSpace + "]";
	
	// Percent
	line += StringHelper::format( 0xff, " %2.1f%%", _info.progressFraction*100 );

	switch (_info.type)
	{
	case ProgressInfo::FileDownload:	
	{
		line += " at " + Util::GetHumanReadableBytes(static_cast<std::size_t>(_info.downloadSpeed)) + "/sec ";
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
		line += GetShortenedString(_info.file.getBasename().toString(), remainingLength);
	}
	break;
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

void ConsoleUpdater::OnStartDifferentialUpdate(const DifferentialUpdateInfo& )
{
}

void ConsoleUpdater::OnPerformDifferentialUpdate(const DifferentialUpdateInfo& )
{
}

void ConsoleUpdater::OnFinishDifferentialUpdate(const DifferentialUpdateInfo& )
{
}

Delegate1<int> ConsoleUpdater::_abortSignalHandler;

} // namespace

} // namespace
