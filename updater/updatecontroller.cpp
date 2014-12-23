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

#include "updatecontroller.hpp"
#include "constants.hpp"

#include "util.hpp"

namespace updater
{

UpdateController::UpdateController(IUpdateView& view, vfs::Path executableName, UpdaterOptions& options) :
	_view(view),
	_curStep(Init),
	_updater(options, executableName.baseName()),
	_abortFlag(false),
	_differentialUpdatePerformed(false)
{
	_progress = ProgressHandlerPtr( new ProgressHandler(_view) );
	_progress->drop();

	_updater.SetDownloadProgressCallback( ptr_cast<Updater::DownloadProgress>( _progress ));
	_updater.SetFileOperationProgressCallback( ptr_cast<Updater::FileOperationProgress>( _progress ));
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
	_updater.postUpdateCleanup();
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

		_updater.cancelDownloads();
	}
}

bool UpdateController::AllThreadsDone() {	return _synchronizer == NULL;}
std::size_t UpdateController::GetNumMirrors(){	return _updater.GetNumMirrors(); }
bool UpdateController::NewUpdaterAvailable(){	return _updater.NewUpdaterAvailable();}
bool UpdateController::LocalFilesNeedUpdate(){	return _updater.LocalFilesNeedUpdate();}
std::size_t UpdateController::GetTotalDownloadSize(){	return _updater.GetTotalDownloadSize();}
std::size_t UpdateController::GetTotalBytesDownloaded(){	return _updater.GetTotalBytesDownloaded();}
std::size_t UpdateController::GetNumFilesToBeUpdated(){	return _updater.GetNumFilesToBeUpdated();}
std::string UpdateController::GetLocalVersion(){	return _updater.getDeterminedLocalVersion();}
std::string UpdateController::GetNewestVersion(){	return _updater.getNewestVersion();}

void UpdateController::removeDownload(std::string itemname)
{
	_updater.removeDownload( itemname );
}

void UpdateController::run()
{
	while( _curStep != Done )
	{
		// Launch the thread and setup the callbacks
		performStep( _curStep );
		finalizeStep( _curStep );
	}
}

void UpdateController::performStep(int step)
{
	//Logger::warning( "Step thread started: %d", step);

	_view.onStartStep((UpdateStep)step);

	// Dispatch the calls to the updater, any exceptions will be caught by the ExceptionSafeThread class

	switch (step)
	{
	case Init:
		// Check if TDM is active
		if( Util::caesariaIsRunning() )
		{
			// grayman - change "Doom3" to "The Dark Mod"
			_view.OnWarning("The CaesarIA was found to be active.\nThe updater will not be able to update any files.\nPlease exit CaesarIA before continuing.");
		}

		break;

	case CleanupPreviousSession:
		// Pass the call to the updater
		_updater.CleanupPreviousSession();
		break;

	case UpdateMirrors:
		// Pass the call to the updater
		if( _updater.isMirrorsNeedUpdate() )
		{
			_updater.updateMirrors();
		}
		else
		{
			// Load Mirrors
			_updater.loadMirrors();
		}
		break;

	case DownloadStableVersion:
		_updater.GetStableVersionFromServer();
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
		_updater.PrepareUpdateStep(TEMP_FILE_PREFIX);
		_updater.PerformUpdateStep();
		_updater.cleanupUpdateStep();
		break;
	
	case DownloadDifferentialUpdate:
	break;

	case PerformDifferentialUpdate:
	break;

	case DownloadFullUpdate:
		_updater.PrepareUpdateStep("");
		_updater.PerformUpdateStep();
		_updater.cleanupUpdateStep();
		break;

	case PostUpdateCleanup:
		_updater.postUpdateCleanup();
		_updater.setBinaryAsExecutable();
		break;

	case RestartUpdater:		
		_updater.RestartUpdater();
		break;

	case Done:
		break;
	};
}

void UpdateController::finalizeStep(int step)
{
	//Logger::warning( "Step thread finished: %d", step );

	// Notify the view
	_view.onFinishStep( (UpdateStep)step );

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
		TryToProceedTo(DownloadStableVersion);
		break;

	case DownloadStableVersion:
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
			_updater.RestartUpdater();
			TryToProceedTo(RestartUpdater);
		}
		break;

	case DownloadDifferentialUpdate:
		{
			TryToProceedTo(PerformDifferentialUpdate);
		}
		break;

	case PerformDifferentialUpdate:
		{
			/*_differentialUpdatePerformed = true;

			// After applying a differential update, check our progress
			DifferentialUpdateInfo info = _updater.GetDifferentialUpdateInfo();

			_view.OnFinishDifferentialUpdate(info);

			// Go back to determine our local version */
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
