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

#include "DifferentialUpdateInfo.h"
#include "UpdateStep.h"
//#include <boost/filesystem.hpp>

namespace tdm
{

namespace updater
{

struct ProgressInfo
{
	enum Type
	{
		FileDownload,	// File download progress (single file)
		FileOperation,	// File operation
		DifferentialPackageDownload,	// differential update download
		FullUpdateDownload				// full update download
	};

	// The type of progress
	Type type;

	// File operation types
	enum FileOperationType
	{
		Check,
		Remove,
		Replace,
		Add,
		RemoveFilesFromPK4,
		Unspecified,
	};

	FileOperationType operationType;

	// The file we're busy with
	vfs::Path file;

	// The mirror display name
	std::string mirrorDisplayName;

	// The progress fraction of the entire step in [0..1]
	double progressFraction;

	// The current download speed, if applicable, in bytes/sec
	double downloadSpeed;

	// Number of bytes received
	std::size_t downloadedBytes;

	// Number of bytes to download
	std::size_t bytesToDownload;

	// Number of files to download
	std::size_t filesToDownload;
};

/**
 * Abstract base class of the GUI layer in TDM Update.
 * 
 * I'm trying to follow the Model-View-Controller pattern here,
 * so this is the "View" part of the TDM Updater application
 * which should be implemented by any updater frontend, be it 
 * a console app or an actual GUI application.
 * 
 * The UpdateController class accepts an IUpdateView& reference 
 * which can receive events from the lower level classes.
 */
class IUpdateView
{
public:
	/**
	 * Gets called when the given update step is about to start,
	 * such that the GUI elements can be adjusted.
	 */
	virtual void OnStartStep(UpdateStep step) = 0;

	/**
	 * Gets called when the given update step is about to end,
	 * such that the GUI elements can be adjusted. When an error
	 * occurs in the step OnFinishStep will not called, see OnFailure.
	 */
	virtual void OnFinishStep(UpdateStep step) = 0;

	/**
	 * The failure signal, indicating that an error occurred
	 * in the given update step.
	 */
	virtual void OnFailure(UpdateStep step, const std::string& errorMessage) = 0;

	/**
	 * During longer operations like downloads or large file changes,
	 * this method will be called with some progress updates.
	 * The passed struct will hold further information.
	 */
	virtual void OnProgressChange(const ProgressInfo& info) = 0;

	/** 
	 * Use this method to send a message to the view - this might result in
	 * a popup being displayed, or a line printed to the console, this depends
	 * on the implementation.
	 */
	virtual void OnMessage(const std::string& message) = 0;

	/** 
	 * Use this method to send a warning to the view - this might result in
	 * a popup being displayed, or a line printed to the console, this depends
	 * on the implementation.
	 */
	virtual void OnWarning(const std::string& message) = 0;

	/**
	 * Called when the controller starts to download a differential update package.
	 */
	virtual void OnStartDifferentialUpdate(const DifferentialUpdateInfo& info) = 0;

	/**
	 * Called when the controller starts to apply a differential update package.
	 */
	virtual void OnPerformDifferentialUpdate(const DifferentialUpdateInfo& info) = 0;

	/**
	 * Called when the controller is done applying a single differential update package.
	 */
	virtual void OnFinishDifferentialUpdate(const DifferentialUpdateInfo& info) = 0;
};

} // namespace

} // namespace
