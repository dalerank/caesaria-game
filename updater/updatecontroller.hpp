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

#ifndef __CAESARIA_UPDATECONTROLLER_H_INLCUDE__
#define __CAESARIA_UPDATECONTROLLER_H_INLCUDE__

#include <set>
#include "updater.hpp"
#include "updateview.hpp"
#include "updateroptions.hpp"
#include "thread/safethread.hpp"
#include "core/logger.hpp"
#include "progresshandler.hpp"

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
  threading::SafeThreadPtr _synchronizer;

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
	UpdateController(IUpdateView& view, vfs::Path executableName, UpdaterOptions& options);

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
  void start();

	// Calling abort will try to terminate the update process in a controlled fashion.
	// Does not block execution, you'll need to query AllThreadsDone() to check whether
	// all worker threads have been terminated
  void abort();

	// Returns true if the controller has no more running threads
  bool allThreadsDone();

	// Used by the console updater on Ctrl-C, should not be called by regular GUIs
  void doPostUpdateCleanup();

	// Status query
  std::size_t mirrors_n();

	// Returns true if a new updater is available for download
  bool haveNewUpdater();

	// Returns true if any files need to be updated
  bool haveNewFiles();

  std::size_t totalDownloadSize();
  std::size_t totalDownloadedBytes();
  std::size_t flesToBeUpdated_n();

	// True if the update process has reached the end of the sequence
  bool isDone();

	bool DifferentialUpdateAvailable();

  std::string localVersion();
  std::string farVersion();

  void removeDownload(const std::string& itemname );

private:
  void run(bool& b);

	// The entry point for the worker thread
  void doStep(int step);

	// Invoked by the ExceptionSafeThread when a step is done, do some error handling here
	void finalizeStep(int step);

	void TryToProceedTo(UpdateStep step);

	// Returns true if we are allowed to proceed to the given step
	bool IsAllowedToContinueTo(UpdateStep step);
};
typedef SmartPtr<UpdateController> UpdateControllerPtr;

} // end namespace updater

#endif //__CAESARIA_UPDATECONTROLLER_H_INLCUDE__
