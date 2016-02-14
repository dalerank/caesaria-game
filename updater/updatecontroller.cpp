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
#include "core/osystem.hpp"
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
  _progress = ProgressHandler::create(_view);

  _updater.SetDownloadProgressCallback( _progress.object() );
  _updater.SetFileOperationProgressCallback( _progress.object() );
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

void UpdateController::start()
{
  assert(_synchronizer == NULL); // debug builds take this seriously

  if( _synchronizer.isNull() )
  {
     auto p = threading::SafeThread::create( threading::SafeThread::WorkFunction( this, &UpdateController::run ) );
     _synchronizer = p;
  }
}

void UpdateController::doPostUpdateCleanup()
{
	_updater.postUpdateCleanup();
}

void UpdateController::abort()
{
  _abortFlag = true;

  if(_synchronizer != NULL)
  {
    try
    {
      _synchronizer->abort();
    }
    catch (std::runtime_error& ex)
    {
      Logger::warning( "Controller thread aborted. {}", ex.what() );
    }

    _updater.cancelDownloads();
  }
}

bool UpdateController::allThreadsDone() {	return _synchronizer == NULL;}
std::size_t UpdateController::mirrors_n(){	return _updater.mirrors_n(); }
bool UpdateController::haveNewUpdater(){	return _updater.NewUpdaterAvailable();}
bool UpdateController::haveNewFiles(){	return _updater.isDownloadQueueFull();}
std::size_t UpdateController::totalDownloadSize(){	return _updater.GetTotalDownloadSize();}
std::size_t UpdateController::totalDownloadedBytes(){	return _updater.GetTotalBytesDownloaded();}
std::size_t UpdateController::flesToBeUpdated_n(){	return _updater.GetNumFilesToBeUpdated();}
std::string UpdateController::localVersion(){	return _updater.getDeterminedLocalVersion();}
std::string UpdateController::farVersion(){	return _updater.getNewestVersion();}

void UpdateController::removeDownload(const std::string& itemname)
{
	_updater.removeDownload( itemname );
}

void UpdateController::run(bool& continues)
{
	while( _curStep != Done )
	{
		// Launch the thread and setup the callbacks
    doStep( _curStep );
		finalizeStep( _curStep );
	}

  continues = false;
}

void UpdateController::doStep(int step)
{
  Logger::warning( "Step thread started: {}", step);

	_view.onStartStep((UpdateStep)step);

	// Dispatch the calls to the updater, any exceptions will be caught by the ExceptionSafeThread class

	switch (step)
	{
	case Init:
    // Check if CaesarIA is active
		if( Util::caesariaIsRunning() )
		{
      _view.onWarning("The CaesarIA was found to be active.\nThe updater will not be able to update any files.\nPlease exit CaesarIA before continuing.");
		}
  break;

	case CleanupPreviousSession:
		// Pass the call to the updater
    _updater.cleanupPreviousSession();
  break;

	case UpdateMirrors:
		// Pass the call to the updater
    if( _updater.isNeedLoadMirrorsFromServer() )
		{
      _updater.downloadNewMirrors();
		}
		else
		{
			_updater.loadMirrors();
		}
  break;

	case DownloadStableVersion:
    _updater.downloadStableVersion();
  break;

	case CompareLocalFilesToNewest:
    _updater.checkLocalFiles();
  break;

	case DownloadVersionInfo:
    _updater.downloadCurrentVersion();
		break;

	case DetermineLocalVersion:
		_updater.DetermineLocalVersion();
		break;

	case DownloadNewUpdater:
		// Prepare, Download, Apply
    _updater.prepareUpdateStep(TEMP_FILE_PREFIX);
		_updater.PerformUpdateStep();
		_updater.cleanupUpdateStep();
		break;
	
	case DownloadDifferentialUpdate:
	break;

	case PerformDifferentialUpdate:
	break;

	case DownloadFullUpdate:
    _updater.prepareUpdateStep("");
		_updater.PerformUpdateStep();
		_updater.cleanupUpdateStep();
		break;

	case PostUpdateCleanup:
		_updater.postUpdateCleanup();
		_updater.setBinaryAsExecutable();
		break;

  case RestartUpdater:
    _updater.restartUpdater();
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
            else if (_updater.isDownloadQueueFull())
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
            _view.onMessage("A differential update has been applied to your installation,\nthough some files still need to be updated.");
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
      _updater.restartUpdater();
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

bool UpdateController::isDone()
{
	return _curStep == Done;
}

} // namespace
