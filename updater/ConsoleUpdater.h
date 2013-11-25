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

#include "Constants.h"
#include "Updater/Updater.h"
#include "Updater/UpdateController.h"
#include "Updater/UpdateView.h"
#include "core/logger.hpp"
#include "ExceptionSafeThread.h"
#include <signal.h>

namespace tdm
{

namespace updater
{

class ConsoleUpdater :
	public IUpdateView
{
public:
	enum Outcome
	{
		None,
		Failed,
		Ok,
		OkNeedRestart
	};

private:
	// The status
	Outcome _outcome;

	UpdaterOptions _options;

	// The update controller manages the logic
	UpdateController _controller;

	// Exit flag
	volatile bool _done;

	static Delegate1<int> _abortSignalHandler;

	// Most recent progress info
	ProgressInfo _info;

public:
	ConsoleUpdater(int argc, char* argv[]);

	~ConsoleUpdater();

	Outcome GetOutcome() 
	{
		return _outcome;
	}

	// Main entry point, algorithm starts here, does not leak exceptions
	void Run();

	// IUpdateView implementation
	void OnStartStep(UpdateStep step);
	void OnFinishStep(UpdateStep step);
	void OnFailure(UpdateStep step, const std::string& errorMessage);
	void OnProgressChange(const ProgressInfo& info);
	void OnMessage(const std::string& message);
	void OnWarning(const std::string& message);
	void OnStartDifferentialUpdate(const DifferentialUpdateInfo& info);
	void OnPerformDifferentialUpdate(const DifferentialUpdateInfo& info);
	void OnFinishDifferentialUpdate(const DifferentialUpdateInfo& info);

private:
	static void AbortSignalHandler(int signal);

	void OnAbort(int);

	void PrintProgress();
};

} // namespace

} // namespace
