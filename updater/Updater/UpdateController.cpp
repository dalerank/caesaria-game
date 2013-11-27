/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5508 $ (Revision of last commit) 
 $Date: 2012-07-31 23:13:19 +0400 (Вт, 31 июл 2012) $ (Date of last commit)
 $Author: grayman $ (Author of last commit)
 
******************************************************************************/

#include "UpdateController.h"

#include "../Util.h"

namespace tdm
{

namespace updater
{

UpdateController::UpdateController(IUpdateView& view, vfs::Path executableName, UpdaterOptions& options) :
	_view(view),
	_curStep(Init),
	_updater(options, executableName.getBasename()),
	_abortFlag(false),
	_differentialUpdatePerformed(false)
{
	_progress = ProgressHandlerPtr( new ProgressHandler(_view) );
	_progress->drop();

	_updater.SetDownloadProgressCallback(_progress.as<updater::Updater::DownloadProgress>());
	_updater.SetFileOperationProgressCallback(_progress.as<updater::Updater::FileOperationProgress>());
}

UpdateController::~UpdateController()
{
	_updater.ClearFileOperationProgressCallback();
	_updater.ClearDownloadProgressCallback();
}

void UpdateController::PauseAt(UpdateStep step)
{
	_interruptionPoints.insert(step);
}

void UpdateController::DontPauseAt(UpdateStep step)
{
	_interruptionPoints.erase(step);
}

void UpdateController::StartOrContinue()
{
	assert(_synchronizer == NULL); // debug builds take this seriously

	if( _synchronizer.isNull() )
	{
		ExceptionSafeThreadPtr p( new ExceptionSafeThread( makeDelegate( this, &UpdateController::run )) );
		p->SetThreadType( ThreadTypeIntervalDriven );
		p->drop();
		_synchronizer = p;
	}
}

void UpdateController::PerformPostUpdateCleanup()
{
	_updater.PostUpdateCleanup();
}

void UpdateController::Abort()
{
	_abortFlag = true;

	if(_synchronizer != NULL)
	{
		try
		{
			_synchronizer->Stop();
		}
		catch (std::runtime_error& ex)
		{
			Logger::warning( "Controller thread aborted. %s", ex.what() );
		}

		_updater.CancelDownloads();
	}
}

bool UpdateController::AllThreadsDone()
{
	return _synchronizer == NULL;
}

std::size_t UpdateController::GetNumMirrors()
{
	return _updater.GetNumMirrors();
}

bool UpdateController::NewUpdaterAvailable()
{
	return _updater.NewUpdaterAvailable();
}

bool UpdateController::LocalFilesNeedUpdate()
{
	return _updater.LocalFilesNeedUpdate();
}

std::size_t UpdateController::GetTotalDownloadSize()
{
	return _updater.GetTotalDownloadSize();
}

std::size_t UpdateController::GetTotalBytesDownloaded()
{
	return _updater.GetTotalBytesDownloaded();
}

std::size_t UpdateController::GetNumFilesToBeUpdated()
{
	return _updater.GetNumFilesToBeUpdated();
}

bool UpdateController::RestartRequired()
{
	return _updater.RestartRequired();
}

bool UpdateController::DifferentialUpdateAvailable()
{
	return _updater.DifferentialUpdateAvailable();
}

std::string UpdateController::GetLocalVersion()
{
	return _updater.GetDeterminedLocalVersion();
}

std::string UpdateController::GetNewestVersion()
{
	return _updater.GetNewestVersion();
}

std::size_t UpdateController::GetTotalDifferentialUpdateSize()
{
	return _updater.GetTotalDifferentialUpdateSize();
}

DifferentialUpdateInfo UpdateController::GetDifferentialUpdateInfo()
{
	return _updater.GetDifferentialUpdateInfo();
}

void UpdateController::run()
{
	while( true )
	{
		// Launch the thread and setup the callbacks
		PerformStep( _curStep );
		OnFinishStep( _curStep );
	}
}

void UpdateController::PerformStep(int step)
{
	Logger::warning( "Step thread started: %d", step);

	_view.OnStartStep((UpdateStep)step);

	// Dispatch the calls to the updater, any exceptions will be caught by the ExceptionSafeThread class

	switch (step)
	{
	case Init:
		// Check if TDM is active
		if (Util::TDMIsRunning())
		{
			// grayman - change "Doom3" to "The Dark Mod"
			_view.OnWarning("The Dark Mod was found to be active.\nThe updater will not be able to update any Dark Mod PK4s.\nPlease exit The Dark Mod before continuing.");
		}

		if (Util::DarkRadiantIsRunning())
		{
			_view.OnWarning("DarkRadiant was found to be active.\nThe updater will not be able to update any Dark Mod PK4s.\nPlease exit DarkRadiant before continuing.");
		}

		break;

	case CleanupPreviousSession:
		// Pass the call to the updater
		_updater.CleanupPreviousSession();
		break;

	case UpdateMirrors:
		// Pass the call to the updater
		if (_updater.MirrorsNeedUpdate())
		{
			_updater.UpdateMirrors();
		}
		else
		{
			// Load Mirrors
			_updater.LoadMirrors();
		}
		break;

	case DownloadCrcs:
		_updater.GetCrcFromServer();
		break;

	case CompareLocalFilesToNewest:
		_updater.CheckLocalFiles();
		break;

	case DownloadVersionInfo:
		_updater.GetVersionInfoFromServer();
		break;

	case DetermineLocalVersion:
		_updater.DetermineLocalVersion();
		break;

	case DownloadNewUpdater:
		// Prepare, Download, Apply
		_updater.PrepareUpdateStep();
		_updater.PerformUpdateStep();
		_updater.CleanupUpdateStep();
		break;
	
	case DownloadDifferentialUpdate:
		{
			DifferentialUpdateInfo info = _updater.GetDifferentialUpdateInfo();
			_view.OnStartDifferentialUpdate(info);

			// Download the update package, integrate it
			_updater.DownloadDifferentialUpdate();
		}
		break;

	case PerformDifferentialUpdate:
		{
			DifferentialUpdateInfo info = _updater.GetDifferentialUpdateInfo();

			_view.OnPerformDifferentialUpdate(info);

			_updater.PerformDifferentialUpdateStep();
		}
		break;

	case DownloadFullUpdate:
		_updater.PrepareUpdateStep();
		_updater.PerformUpdateStep();
		_updater.CleanupUpdateStep();
		break;

	case PostUpdateCleanup:
		_updater.PostUpdateCleanup();
		break;

	case RestartUpdater:
		_updater.RestartUpdater();
		break;

	case Done:
		break;
	};
}

void UpdateController::OnFinishStep(int step)
{
	Logger::warning( "Step thread finished: %d", step );

	// Notify the view
	_view.OnFinishStep( (UpdateStep)step );

	switch (_curStep)
	{
	case Init:
		// Startup, there's nothing to do, proceed to cleanup if allowed
		TryToProceedTo(CleanupPreviousSession);
		break;

	case CleanupPreviousSession:
		TryToProceedTo(UpdateMirrors);
		break;

	case UpdateMirrors:
		TryToProceedTo(DownloadCrcs);
		break;

	case DownloadCrcs:
		TryToProceedTo(DownloadVersionInfo);
		break;

	case DownloadVersionInfo:
		// Compare local version to see if it matches
		TryToProceedTo(DetermineLocalVersion);
		break;

	case DetermineLocalVersion:
		TryToProceedTo(CompareLocalFilesToNewest);
		break;

	case CompareLocalFilesToNewest:
		{
			if (_updater.NewUpdaterAvailable())
			{
				// Update necessary, new updater available
				TryToProceedTo(DownloadNewUpdater);
			}
			else if (_updater.LocalFilesNeedUpdate())
			{
				// Update necessary, updater is ok
				if (_updater.DifferentialUpdateAvailable())
				{
					TryToProceedTo(DownloadDifferentialUpdate);
				}
				else
				{
					// Did we already perform a differential update?
					if (_differentialUpdatePerformed)
					{
						_view.OnMessage("A differential update has been applied to your installation,\nthough some files still need to be updated.");
					}

					TryToProceedTo(DownloadFullUpdate);
				}
			}
			else
			{
				// No update necessary
				TryToProceedTo(PostUpdateCleanup);
			}
		}
		break;

	case DownloadNewUpdater:
		{
			if (_updater.RestartRequired())
			{
				TryToProceedTo(RestartUpdater);
			}
			else
			{
				TryToProceedTo(Done);
			}
		}
		break;

	case DownloadDifferentialUpdate:
		{
			TryToProceedTo(PerformDifferentialUpdate);
		}
		break;

	case PerformDifferentialUpdate:
		{
			_differentialUpdatePerformed = true;

			// After applying a differential update, check our progress
			DifferentialUpdateInfo info = _updater.GetDifferentialUpdateInfo();

			_view.OnFinishDifferentialUpdate(info);

			// Go back to determine our local version
			TryToProceedTo(DetermineLocalVersion);
		}
		break;

	case DownloadFullUpdate:
		TryToProceedTo(PostUpdateCleanup);
		break;

	case PostUpdateCleanup:
		TryToProceedTo(Done);
		break;

	case RestartUpdater:
		TryToProceedTo(Done);
		break;

	case Done:
		// Nothing to do
		break;
	};
}

void UpdateController::TryToProceedTo(UpdateStep step)
{
	if (_abortFlag) return;	

	if (IsAllowedToContinueTo(step))
	{
		_curStep = step;
	}
}

// Returns true if we are allowed to proceed to the given step
bool UpdateController::IsAllowedToContinueTo(UpdateStep step)
{
	return _interruptionPoints.find(step) == _interruptionPoints.end();
}

bool UpdateController::IsDone()
{
	return _curStep == Done;
}

} // namespace

} // namespace
