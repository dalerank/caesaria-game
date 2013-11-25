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


// UpdaterDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tdm_update_mfc.h"
#include "UpdaterDialog.h"
#include "AdvancedOptionsDialog.h"

// TDM Includes
#include "Constants.h"
#include "DownloadProgressHandler.h"
#include "FileProgressHandler.h"

#define WM_DESTROY_WHEN_THREADS_DONE (WM_APP + 1)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace tdm;
using namespace updater;

// UpdaterDialog dialog

UINT UpdaterDialog::WM_TASKBARBTNCREATED = RegisterWindowMessage(_T("TaskbarButtonCreated"));

UpdaterDialog::UpdaterDialog(const fs::path& executableName, 
							 UpdaterOptions& options, 
							 CWnd* pParent)
	: CDialog(UpdaterDialog::IDD, pParent),
	_options(options),
	_controller(new UpdateController(*this, executableName, _options)),
	_shutdown(false),
	_failed(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TDM_ICON);

	// Instruct the controller to pause before updating mirrors
	_controller->PauseAt(UpdateMirrors);

	// Pause before downloading anything
	_controller->PauseAt(DownloadNewUpdater);
	_controller->PauseAt(DownloadDifferentialUpdate);
	_controller->PauseAt(DownloadFullUpdate);
	_controller->PauseAt(RestartUpdater);
}

UpdaterDialog::~UpdaterDialog()
{
	TraceLog::Instance().Unregister(_logViewer);

	_logViewer.reset();
}

void UpdaterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPDATER_TITLE, _title);
	DDX_Control(pDX, IDC_UPDATER_SUBTITLE, _subTitle);
	DDX_Control(pDX, IDC_PROGRESS1, _progressMain);
	DDX_Control(pDX, IDC_STATUS_TEXT, _statusText);
	DDX_Control(pDX, IDC_STEP1_TEXT, _step1Text);
	DDX_Control(pDX, ID_BUTTON_CONTINUE, _continueButton);
	DDX_Control(pDX, IDC_STEP2_TEXT, _step2Text);
	DDX_Control(pDX, IDC_STEP3_TEXT, _step3Text);
	DDX_Control(pDX, IDC_STEP4_TEXT, _step4Text);
	DDX_Control(pDX, IDC_STEP5_TEXT, _step5Text);
	DDX_Control(pDX, ID_BUTTON_ABORT, _abortButton);
	DDX_Control(pDX, IDC_STEP6_TEXT, _step6Text);
	DDX_Control(pDX, IDC_STEP7_TEXT, _step7Text);
	DDX_Control(pDX, IDC_STEP8_TEXT, _step8Text);
	DDX_Control(pDX, IDC_STEP1_STATE, _step1State);
	DDX_Control(pDX, IDC_STEP2_STATE, _step2State);
	DDX_Control(pDX, IDC_STEP3_STATE, _step3State);
	DDX_Control(pDX, IDC_STEP4_STATE, _step4State);
	DDX_Control(pDX, IDC_STEP5_STATE, _step5State);
	DDX_Control(pDX, IDC_STEP6_STATE, _step6State);
	DDX_Control(pDX, IDC_STEP7_STATE, _step7State);
	DDX_Control(pDX, IDC_STEP8_STATE, _step8State);
	DDX_Control(pDX, IDC_PROGRESS_SPEED, _progressSpeedText);
	DDX_Control(pDX, IDC_ADV_OPTIONS_BUTTON, _advOptionsButton);
	DDX_Control(pDX, IDC_SHOW_LOG_BUTTON, _showLogButton);
}

BEGIN_MESSAGE_MAP(UpdaterDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BUTTON_ABORT, &UpdaterDialog::OnBnClickedButtonAbort)
	ON_BN_CLICKED(ID_BUTTON_CONTINUE, &UpdaterDialog::OnBnClickedButtonContinue)
	ON_MESSAGE(WM_DESTROY_WHEN_THREADS_DONE, OnDestroyWhenThreadsDone)
	ON_REGISTERED_MESSAGE(WM_TASKBARBTNCREATED, OnTaskbarBtnCreated)
	ON_BN_CLICKED(IDC_ADV_OPTIONS_BUTTON, &UpdaterDialog::OnBnClickedAdvOptionsButton)
	ON_BN_CLICKED(IDC_SHOW_LOG_BUTTON, &UpdaterDialog::OnBnClickedShowLogButton)
END_MESSAGE_MAP()

// UpdaterDialog message handlers

BOOL UpdaterDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	CString titleStr;
	_title.GetWindowText(titleStr);
	titleStr += " v";
	titleStr += tdm::LIBTDM_UPDATE_VERSION; 

	_title.SetWindowText(titleStr);

	_titleFont.CreateFont(18, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
						  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);

	_title.SetFont(&_titleFont);

	// Perform the initial cleanup right off at start
	_controller->StartOrContinue();

	_logViewer.reset(new LogViewer);
	_logViewer->Create(LogViewer::IDD);

	TraceLog::Instance().Register(_logViewer);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT UpdaterDialog::OnDestroyWhenThreadsDone(UINT wParam, LONG lParam)
{
	_progressSpeedText.SetWindowText(CString("Waiting for threads to terminate..."));

	if (_controller->AllThreadsDone())
	{
		if (!_controller->RestartRequired())
		{
			_controller->PerformPostUpdateCleanup();
		}

		_taskbarList.Release();
		DestroyWindow();
	}
	else
	{
		Sleep(20);
		PostMessage(WM_DESTROY_WHEN_THREADS_DONE);
	}

	return 0;
}

LRESULT UpdaterDialog::OnTaskbarBtnCreated(WPARAM, LPARAM)
{
	DWORD dwMajor = LOBYTE(LOWORD(GetVersion()));
	DWORD dwMinor = HIBYTE(LOWORD(GetVersion()));

	// Check at runtime that the OS is Win 7 or later (Win 7 is v6.1),
	// otherwise ignore this signal
	if ( dwMajor > 6 || ( dwMajor == 6 && dwMinor > 0 ) )
	{
		_taskbarList.Release();

		_taskbarList.CoCreateInstance ( CLSID_TaskbarList );
	}
	
	return 0;
}

void UpdaterDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR UpdaterDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void UpdaterDialog::OnBnClickedButtonAbort()
{
	if (_controller->RestartRequired())
	{
		_controller->StartOrContinue();
		
		PostMessage(WM_DESTROY_WHEN_THREADS_DONE);
	}
	else if (_controller->IsDone())
	{
		_taskbarList.Release();
		DestroyWindow();
	}
	else if (_failed)
	{
		// Updater has failed, don't ask for quit
		PostMessage(WM_DESTROY_WHEN_THREADS_DONE);
		_shutdown = true;
	}
	else
	{
		// Display a confirmation popup
		int result = MessageBox(CString("This will abort the update process. Are you sure?"), 
			CString("Cancel update process?"), MB_YESNO);

		if (result == IDYES)
		{
			// Unregister the logwriters when aborting, the download thread call log messages and 
			// and end up in a deadlock in the LogViewer class
			TraceLog::Instance().Unregister(_logViewer);
			_logViewer.reset();

			_showLogButton.ShowWindow(FALSE);
			
			_controller->Abort();

			PostMessage(WM_DESTROY_WHEN_THREADS_DONE);

			_continueButton.ShowWindow(FALSE);
			_shutdown = true;
		}
	}
}

void UpdaterDialog::OnFail()
{
	_continueButton.ShowWindow(FALSE);
	_failed = true;

	SetTaskbarProgress(TBP_Error, 0);

	std::string totalBytesStr = (boost::format("Total bytes downloaded: %s") % Util::GetHumanReadableBytes(_controller->GetTotalBytesDownloaded())).str();
	_progressSpeedText.SetWindowText(CString(totalBytesStr.c_str()));
}

void UpdaterDialog::OnBnClickedButtonContinue()
{
	// Disable the button to avoid double-calls
	_continueButton.EnableWindow(FALSE);

	// Just dispatch the call to the controller
	_controller->StartOrContinue();
}

void UpdaterDialog::SetProgressText(const std::string& text)
{
	// To avoid flickering, check if the text has actually changed
	CString prevText;
	_statusText.GetWindowText(prevText);

	CString newText(text.c_str());

	if (prevText != newText)
	{
		_statusText.SetWindowText(newText);
	}
}

void UpdaterDialog::SetProgressSpeedText(const std::string& text)
{
	// To avoid flickering, check if the text has actually changed
	CString prevText;
	_progressSpeedText.GetWindowText(prevText);

	CString newText(text.c_str());

	if (prevText != newText)
	{
		_progressSpeedText.SetWindowText(newText);
	}
}

void UpdaterDialog::SetProgress(double progressFraction)
{
	_progressMain.SetRange(0, 100);
	_progressMain.SetPos(static_cast<int>(100*progressFraction));

	SetTaskbarProgress(TBP_Normal, progressFraction);
}

void UpdaterDialog::SetFullDownloadProgress(const ProgressInfo& info)
{
	std::string totalBytesStr = Util::GetHumanReadableBytes(info.bytesToDownload);

	std::string str = (boost::format("Downloading updates... %0.2f%% of %s (%d %s)") % 
		(info.progressFraction*100) % 
		totalBytesStr %
		info.filesToDownload %
		(info.filesToDownload == 1 ? "file" : "files")).str();
	_step5Text.SetWindowText(CString(str.c_str()));
}

void UpdaterDialog::SetTaskbarProgress(TaskbarProgressType type, double progressFraction)
{
	if (_taskbarList)
	{
		switch (type)
		{
		case TBP_NoProgress:
			_taskbarList->SetProgressState(m_hWnd, TBPF_NOPROGRESS);
			break;
		case TBP_Indeterminate:
			_taskbarList->SetProgressState(m_hWnd, TBPF_INDETERMINATE);
			break;
		case TBP_Normal:
			_taskbarList->SetProgressState(m_hWnd, TBPF_NORMAL);
			_taskbarList->SetProgressValue(m_hWnd, static_cast<ULONGLONG>(100*progressFraction), 100);
			break;
		case TBP_Paused:
			_taskbarList->SetProgressState(m_hWnd, TBPF_PAUSED);
			break;
		case TBP_Error:
			_taskbarList->SetProgressState(m_hWnd, TBPF_ERROR);
			break;
		}
	}
}

void UpdaterDialog::ClearSteps()
{
	SetTaskbarProgress(TBP_NoProgress, 0);

	_step1Text.SetWindowText(CString());
	_step2Text.SetWindowText(CString());
	_step3Text.SetWindowText(CString());
	_step4Text.SetWindowText(CString());
	_step5Text.SetWindowText(CString());
	_step6Text.SetWindowText(CString());
	_step7Text.SetWindowText(CString());
	_step8Text.SetWindowText(CString());

	_step1State.SetWindowText(CString());
	_step2State.SetWindowText(CString());
	_step3State.SetWindowText(CString());
	_step4State.SetWindowText(CString());
	_step5State.SetWindowText(CString());
	_step6State.SetWindowText(CString());
	_step7State.SetWindowText(CString());
	_step8State.SetWindowText(CString());
}

void UpdaterDialog::OnStartStep(UpdateStep step)
{
	if (_shutdown) return;

	// Always reset the task bar progress when starting a step
	SetTaskbarProgress(TBP_NoProgress, 0);

	switch (step)
	{
	case Init:
		ClearSteps();
		_statusText.SetWindowText(CString("Initialising..."));
		_progressMain.SetPos(0);
		break;

	case CleanupPreviousSession:
		_statusText.SetWindowText(CString("Cleaning up previous update session..."));
		_progressMain.SetPos(0);
		break;

	case UpdateMirrors:
		// Clear step texts
		ClearSteps();

		// Hide the options button once we've started
		_advOptionsButton.ShowWindow(FALSE);

		// Update header title in this step
		_subTitle.SetWindowText(CString("Updating Mirrors and checking installation"));

		_statusText.SetWindowText(CString("Downloading mirror information from thedarkmod.com..."));
		_step1Text.SetWindowText(CString("Downloading mirror information..."));
		_step1State.SetWindowText(CString("--"));
		_progressMain.SetPos(0);
		break;

	case DownloadCrcs:
		_statusText.SetWindowText(CString("Download CRC information..."));
		_progressMain.SetPos(0);

		_step2Text.SetWindowText(CString("Downloading CRC file..."));
		_step2State.SetWindowText(CString("--"));
		break;

	case DownloadVersionInfo:
		_statusText.SetWindowText(CString("Download version information..."));
		_progressMain.SetPos(0);

		_step2Text.SetWindowText(CString("Downloading version info file..."));
		_step2State.SetWindowText(CString("--"));
		break;

	case DetermineLocalVersion:
		_statusText.SetWindowText(CString("Determining local version..."));
		_progressMain.SetPos(0);

		_step3Text.SetWindowText(CString("Trying to match local files to version definitions..."));
		_step3State.SetWindowText(CString("--"));

		_step4Text.SetWindowText(CString(""));
		_step5Text.SetWindowText(CString(""));
		_step6Text.SetWindowText(CString(""));
		_step7Text.SetWindowText(CString(""));
		_step8Text.SetWindowText(CString(""));

		_step4State.SetWindowText(CString(""));
		_step5State.SetWindowText(CString(""));
		_step6State.SetWindowText(CString(""));
		_step7State.SetWindowText(CString(""));
		_step8State.SetWindowText(CString(""));
		break;

	case CompareLocalFilesToNewest:
		_statusText.SetWindowText(CString("Comparing local files to server definitions..."));
		_progressMain.SetPos(0);

		_step4Text.SetWindowText(CString("Comparing local files to server definitions..."));
		_step4State.SetWindowText(CString("--"));

		_step5Text.SetWindowText(CString(""));
		_step6Text.SetWindowText(CString(""));
		_step7Text.SetWindowText(CString(""));
		_step8Text.SetWindowText(CString(""));

		_step5State.SetWindowText(CString(""));
		_step6State.SetWindowText(CString(""));
		_step7State.SetWindowText(CString(""));
		_step8State.SetWindowText(CString(""));
		break;

	case DownloadNewUpdater:
		// Update header title in this step
		_subTitle.SetWindowText(CString("Downloading new updater application"));

		_statusText.SetWindowText(CString("Downloading TDM Update..."));
		_progressMain.SetPos(0);

		_step5Text.SetWindowText(CString("Downloading TDM Update..."));
		_step5State.SetWindowText(CString("--"));
		break;

	case DownloadDifferentialUpdate:
		// Update header title in this step
		_subTitle.SetWindowText(CString("Downloading differential update from servers"));

		_step5Text.SetWindowText(CString("Downloading TDM Update..."));
		_step5State.SetWindowText(CString("--"));

		_step6Text.SetWindowText(CString(""));
		_step7Text.SetWindowText(CString(""));
		_step8Text.SetWindowText(CString(""));

		_step6State.SetWindowText(CString(""));
		_step7State.SetWindowText(CString(""));
		_step8State.SetWindowText(CString(""));
		break;

	case PerformDifferentialUpdate:
		_statusText.SetWindowText(CString("Applying Differential Update Package..."));
		_progressMain.SetPos(0);

		_step6Text.SetWindowText(CString("Applying Differential Update Package..."));
		_step6State.SetWindowText(CString("--"));

		_step7Text.SetWindowText(CString(""));
		_step8Text.SetWindowText(CString(""));

		_step7State.SetWindowText(CString(""));
		_step8State.SetWindowText(CString(""));
		break;

	case DownloadFullUpdate:
		// Update header title in this step
		_subTitle.SetWindowText(CString("Downloading updates from server"));

		_step5Text.SetWindowText(CString("Downloading updates..."));
		_step5State.SetWindowText(CString("--"));

		_step6Text.SetWindowText(CString(""));
		_step7Text.SetWindowText(CString(""));
		_step8Text.SetWindowText(CString(""));

		_step6State.SetWindowText(CString(""));
		_step7State.SetWindowText(CString(""));
		_step8State.SetWindowText(CString(""));
		break;

	};
}

void UpdaterDialog::OnFinishStep(UpdateStep step)
{
	if (_shutdown) return;

	_progressSpeedText.SetWindowText(CString(""));

	switch (step)
	{
	case Init:
	{
		_statusText.SetWindowText(CString("Initialisation done."));
	}
	break;

	case CleanupPreviousSession:
	{
		_statusText.SetWindowText(CString("Ready - press continue to start the update process"));
	}
	break;

	case UpdateMirrors:
	{
		// Mirrors
		bool keepMirrors = _options.IsSet("keep-mirrors");

		if (keepMirrors)
		{
			_statusText.SetWindowText(CString("Skipped downloading mirrors."));
		}
		else
		{
			_statusText.SetWindowText(CString("Done downloading mirrors."));
		}

		CString prevText;
		_step1Text.GetWindowText(prevText);

		std::size_t numMirrors = _controller->GetNumMirrors();

		if (numMirrors > 0)
		{
			prevText += (keepMirrors ? " skipped." : " done.");
			prevText += (boost::format(" Found %d mirror%s.") % numMirrors % (numMirrors == 1 ? "" : "s")).str().c_str();

			_step1Text.SetWindowText(prevText);
			_statusText.SetWindowText(CString("Done downloading mirrors."));
		}
		else
		{
			_step1Text.SetWindowText(prevText + " failed, cannot continue.");
			_statusText.SetWindowText(CString("No mirror information available - cannot continue."));

			// Stop right here
			_controller->Abort();
			OnFail();
		}
	}
	break;

	case DownloadCrcs:
	{
		CString prevText;
		_step2Text.GetWindowText(prevText);

		_step2Text.SetWindowText(prevText + " done.");
		_statusText.SetWindowText(CString("Done downloading checksums."));
	}
	break;

	case DownloadVersionInfo:
	{
		CString prevText;
		_step2Text.GetWindowText(prevText);

		std::string newest;
		
		if (!_controller->GetNewestVersion().empty())
		{
			newest = (boost::format(" done. Newest version is %s.") % _controller->GetNewestVersion()).str();
		}
		else
		{
			newest = " done.";
		}

		_step2Text.SetWindowText(prevText + newest.c_str());
		_statusText.SetWindowText(CString("Done downloading version info."));
	}
	break;

	case DetermineLocalVersion:
	{
		CString prevText;
		_step3Text.GetWindowText(prevText);

		std::string versionFound;

		if (_controller->GetLocalVersion().empty())
		{
			versionFound = " no exact match.";
		}
		else
		{
			versionFound = (boost::format(" done. Local version is %s.") % _controller->GetLocalVersion()).str();
		}

		_step3Text.SetWindowText(prevText + versionFound.c_str());
		_statusText.SetWindowText(CString("Done matching local version."));
	}
	break;

	case CompareLocalFilesToNewest:
	{
		CString prevText;
		_step4Text.GetWindowText(prevText);

		_step4Text.SetWindowText(prevText + " done.");
		_statusText.SetWindowText(CString("Done comparing local files to server definitions."));

		std::string sizeStr = Util::GetHumanReadableBytes(_controller->GetTotalDownloadSize());
		std::size_t numFiles = _controller->GetNumFilesToBeUpdated();

		std::string totalSize = (boost::format("%d %s to be downloaded (total size: %s).") % 
								 numFiles % (numFiles == 1 ? "file needs" : "files need") % sizeStr).str();

		// Print a summary
		if (_controller->NewUpdaterAvailable())
		{
			_step5Text.SetWindowText(CString("New TDM Updater available: ") + totalSize.c_str());
			_statusText.SetWindowText(CString("New TDM update application available - click continue to update"));

			// Enable the continue button
			_continueButton.EnableWindow(TRUE);
		}
		else if (_controller->LocalFilesNeedUpdate())
		{
			if (_controller->DifferentialUpdateAvailable())
			{
				sizeStr = Util::GetHumanReadableBytes(_controller->GetTotalDifferentialUpdateSize());
				totalSize = (boost::format("Total download size for differential update: %s") % sizeStr).str();	

				_statusText.SetWindowText(CString("Differential updates available - click continue to start download"));

				_step5Text.SetWindowText(CString(totalSize.c_str()));
			}
			else
			{
				_statusText.SetWindowText(CString("Updates available - click continue to start download"));
				_step5Text.SetWindowText(CString(totalSize.c_str()));
			}

			// Enable the continue button
			_continueButton.EnableWindow(TRUE);
			_progressMain.SetPos(0);
		}
		else
		{
			// No update necessary
			_subTitle.SetWindowText(CString("Finished"));

			_step5Text.SetWindowText(CString("All files are up to date"));
			_statusText.SetWindowText(CString("Your TDM installation is up to date"));
		}
	}
	break;

	case DownloadNewUpdater:
	{
		CString prevText;
		_step5Text.GetWindowText(prevText);

		_step5Text.SetWindowText(prevText + " done.");
		_statusText.SetWindowText(CString("Done downloading updater - click restart to continue."));

		_continueButton.ShowWindow(FALSE);
		_abortButton.SetWindowText(CString("Restart"));
	}
	break;

	case DownloadDifferentialUpdate:
	{
		CString prevText;
		_step5Text.GetWindowText(prevText);

		_step5Text.SetWindowText(prevText + " done.");
		_statusText.SetWindowText(CString("Done downloading differential update."));
	}
	break;

	case PerformDifferentialUpdate:
	{
		CString prevText;
		_step6Text.GetWindowText(prevText);

		_step6Text.SetWindowText(prevText + " done.");
		_statusText.SetWindowText(CString("Done applying differential update."));
	}
	break;

	case DownloadFullUpdate:
	{
		_step5Text.SetWindowText(CString("Downloading updates... done."));
		_statusText.SetWindowText(CString("Done downloading update."));

		if (!_controller->LocalFilesNeedUpdate())
		{
			_statusText.SetWindowText(CString("Your TDM installation is up to date."));
		}
	}
	break;

	case Done:
	{
		// Hide the continue button
		_continueButton.ShowWindow(FALSE);
		_abortButton.SetWindowText(CString("Close"));

		std::string totalBytesStr = (boost::format("Total bytes downloaded: %s") % Util::GetHumanReadableBytes(_controller->GetTotalBytesDownloaded())).str();
		_progressSpeedText.SetWindowText(CString(totalBytesStr.c_str()));
	}
	break;

	};
}

void UpdaterDialog::OnFailure(UpdateStep step, const std::string& errorMessage)
{
	_statusText.SetWindowText(CString("Error: ") + errorMessage.c_str());
	_progressSpeedText.SetWindowText(CString(""));

	switch (step)
	{
	case DownloadCrcs:
	{
		CString prevText;
		_step2Text.GetWindowText(prevText);

		_step2Text.SetWindowText(prevText + " failed.");
		_progressMain.SetPos(0);
		_controller->Abort();
		OnFail();
	}
	break;

	case DownloadVersionInfo:
	{
		CString prevText;
		_step2Text.GetWindowText(prevText);

		_step2Text.SetWindowText(prevText + " failed.");
		_progressMain.SetPos(0);
		_controller->Abort();
		OnFail();
	}
	break;

	case DownloadNewUpdater:
	{
		CString prevText;
		_step5Text.GetWindowText(prevText);

		_step5Text.SetWindowText(prevText + " failed.");
		_progressMain.SetPos(0);
		_controller->Abort();
		OnFail();
	}
	break;

	case DownloadDifferentialUpdate:
	{
		CString prevText;
		_step5Text.GetWindowText(prevText);

		_step5Text.SetWindowText(prevText + " failed.");
		_progressMain.SetPos(0);
		_controller->Abort();
		OnFail();
	}
	break;

	case PerformDifferentialUpdate:
	{
		CString prevText;
		_step6Text.GetWindowText(prevText);

		_step6Text.SetWindowText(prevText + " failed.");
		_progressMain.SetPos(0);

		_controller->Abort();
		OnFail();
	}
	break;

	case DownloadFullUpdate:
	{
		CString prevText;
		_step5Text.GetWindowText(prevText);

		_step5Text.SetWindowText(prevText + " failed.");
		_progressMain.SetPos(0);
		_controller->Abort();
		OnFail();
	}
	break;

	};
}

void UpdaterDialog::OnMessage(const std::string& message)
{
	MessageBox(CString(message.c_str()), CString("TDM Updater Message"), MB_OK);
}

void UpdaterDialog::OnWarning(const std::string& message)
{
	MessageBox(CString(message.c_str()), CString("TDM Updater Warning"), MB_OK);
}

void UpdaterDialog::OnProgressChange(const ProgressInfo& info)
{
	if (info.type == ProgressInfo::FullUpdateDownload)
	{
		SetFullDownloadProgress(info);
		return;
	}

	_progressSpeedText.SetWindowText(CString(""));
	SetProgress(info.progressFraction);

	if (!info.file.empty())
	{
		switch (info.type)
		{
		case ProgressInfo::FileDownload:
		{
			_progressSpeedText.SetWindowText(CString(""));
			SetProgress(info.progressFraction);

			std::string text = (boost::format("Downloading from mirror %s: %s") % 
								info.mirrorDisplayName % info.file.string()).str();
			SetProgressText(text);

			if (info.progressFraction < 1.0f)
			{
				std::string speed = (boost::format("Downloaded: %s - Current download speed: %s/sec.") % 
									Util::GetHumanReadableBytes(static_cast<std::size_t>(info.downloadedBytes)) % 
									Util::GetHumanReadableBytes(static_cast<std::size_t>(info.downloadSpeed))).str();
				SetProgressSpeedText(speed);
			}
		}
		break;

		case ProgressInfo::FileOperation:
		{
			_progressSpeedText.SetWindowText(CString(""));
			SetProgress(info.progressFraction);

			std::string verb;

			switch (info.operationType)
			{
			case ProgressInfo::Check: 
				verb = "Checking file: "; 
				break;
			case ProgressInfo::Remove: 
				verb = "Removing file: ";
				break;
			case ProgressInfo::Replace: 
				verb = "Replacing file: ";
				break;
			case ProgressInfo::Add: 
				verb = "Adding file: ";
				break;
			case ProgressInfo::RemoveFilesFromPK4: 
				verb = "Removing files from PK4: ";
				break;
			default: 
				verb = "Working on file: ";
			};

			std::string text = (boost::format("%s%s...") % verb % info.file.string()).str();
			SetProgressText(text);
		}
		break;
		};
	}
}

void UpdaterDialog::OnStartDifferentialUpdate(const DifferentialUpdateInfo& info)
{
	_controller->DontPauseAt(DownloadDifferentialUpdate);

	std::string sizeStr = Util::GetHumanReadableBytes(info.filesize);
	std::string text = (boost::format("Downloading update package for version %s to %s (size: %s)...") % 
		info.fromVersion % info.toVersion % sizeStr).str();

	_step5Text.SetWindowText(CString(text.c_str()));
}

void UpdaterDialog::OnPerformDifferentialUpdate(const DifferentialUpdateInfo& info)
{
	std::string sizeStr = Util::GetHumanReadableBytes(info.filesize);
	std::string text = (boost::format("Applying update package for version %s to %s...") % 
		info.fromVersion % info.toVersion).str();

	_step6Text.SetWindowText(CString(text.c_str()));
}

void UpdaterDialog::OnFinishDifferentialUpdate(const DifferentialUpdateInfo& info)
{
	CString prevText;
	_step6Text.GetWindowText(prevText);

	_step6Text.SetWindowText(prevText + " done.");
}

void UpdaterDialog::OnBnClickedAdvOptionsButton()
{
	AdvancedOptionsDialog dialog(_options, this);

	dialog.DoModal();
}

void UpdaterDialog::OnBnClickedShowLogButton()
{
	if (_logViewer && !_logViewer->IsWindowVisible())
	{
		_logViewer->ShowWindow(SW_SHOW);
	}
}
