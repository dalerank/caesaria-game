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

#include <set>
#include "Updater.h"
#include "UpdateView.h"
#include "UpdaterOptions.h"
#include "ExceptionSafeThread.h"
#include "core/logger.hpp"
#include "ProgressHandler.h"

namespace tdm
{

namespace updater
{

/** 
 * The high-level application logic is contained in this object.
 * The controller will dispatch calls to the underlying Updater object,
 * maintain threads, etc. It accepts an IUpdateView implementation
 * which will be receiving some events about the current update state.
 *
 * Upon entering the StartOrContinue() method, a controller thread 
 * will be constructed running the logic. The thread can be configured
 * to automatically stop before entering certain update steps.
 */
class UpdateController : public ReferenceCounted
{
private:
	IUpdateView& _view;

	// The update step we're currently in
	UpdateStep _curStep;

	// If a certain step is mentioned here, the controller thread will
	// pause before entering it.
	std::set<UpdateStep> _interruptionPoints;

	// The main worker thread. A new one is created each time 
	// StartOrContinue is called.
	ExceptionSafeThreadPtr _synchronizer;

	// The main updater object (containing the non-threaded methods)
	Updater _updater;

	// Aborted flag, once called this won't go back to false
	bool _abortFlag;

	// The download progress callback
	ProgressHandlerPtr _progress;

	// True if the differential update step has been completed at least once
	bool _differentialUpdatePerformed;

public:
	// Options will be passed to the Updater object
	UpdateController(IUpdateView& view, const io::FilePath& executableName, UpdaterOptions& options);

	~UpdateController();

	// Instruct the controller to wait before entering the next step
	void PauseAt(UpdateStep step);

	// Instruct the controller not to wait before entering the given step
	void DontPauseAt(UpdateStep step);

	// Main trigger - call this to let the controller continue to the next
	// step. The thread will drive the update process until it's done or
	// the next interruption point is reached. Calling StartOrContinue()
	// after reaching an interruption point will resume the update process.
	// Calling this method while the Controller is still active will do nothing.
	void StartOrContinue();

	// Calling abort will try to terminate the update process in a controlled fashion.
	// Does not block execution, you'll need to query AllThreadsDone() to check whether
	// all worker threads have been terminated
	void Abort();

	// Returns true if the controller has no more running threads
	bool AllThreadsDone();

	// Used by the console updater on Ctrl-C, should not be called by regular GUIs
	void PerformPostUpdateCleanup();

	// Status query
	std::size_t GetNumMirrors();

	// Returns true if a new updater is available for download
	bool NewUpdaterAvailable();

	// Returns true if any files need to be updated
	bool LocalFilesNeedUpdate();

	std::size_t GetTotalDownloadSize();
	std::size_t GetTotalBytesDownloaded();
	std::size_t GetNumFilesToBeUpdated();

	bool RestartRequired();

	// True if the update process has reached the end of the sequence
	bool IsDone();

	bool DifferentialUpdateAvailable();

	std::string GetLocalVersion();
	std::string GetNewestVersion();
	
	std::size_t GetTotalDifferentialUpdateSize();

	DifferentialUpdateInfo GetDifferentialUpdateInfo();

private:
	void run();

	// The entry point for the worker thread
	void PerformStep(int step);

	// Invoked by the ExceptionSafeThread when a step is done, do some error handling here
	void OnFinishStep(int step);

	void TryToProceedTo(UpdateStep step);

	// Returns true if we are allowed to proceed to the given step
	bool IsAllowedToContinueTo(UpdateStep step);
};
typedef SmartPtr<UpdateController> UpdateControllerPtr;

} // namespace

} // namespace
