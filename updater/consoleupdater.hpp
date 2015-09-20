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

#ifndef __CAESARIA_CONSOLEUPDATER_H_INLCUDE__
#define __CAESARIA_CONSOLEUPDATER_H_INLCUDE__

#include "constants.hpp"
#include "updater.hpp"
#include "updatecontroller.hpp"
#include "updateview.hpp"
#include "core/logger.hpp"
#include "thread/safethread.hpp"
#include <signal.h>

namespace updater
{

class ConsoleUpdater : public IUpdateView
{
public:
	enum Outcome
	{
		None,
		Failed,
		Ok,
		OkNeedRestart
	};

public:
	ConsoleUpdater(int argc, char* argv[]);

	virtual ~ConsoleUpdater();

	Outcome GetOutcome();

	// Main entry point, algorithm starts here, does not leak exceptions
	void run();

	void onStartStep(UpdateStep step);
	void onFinishStep(UpdateStep step);
	void OnFailure(UpdateStep step, const std::string& errorMessage);
	void onProgressChange(const ProgressInfo& info);
  void onMessage(const std::string& message);
  void onWarning(const std::string& message);

private:
  void _initAbortSignalHandler();
	static void resolveAbortSignal(int signal);

	void onAbort(int);

  void printProgress();

	class Impl;
	ScopedPtr< Impl > _d;
};

} // namespace

#endif //__CAESARIA_CONSOLEUPDATER_H_INLCUDE__
