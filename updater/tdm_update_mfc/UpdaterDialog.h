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

#pragma once

#include "afxwin.h"
#include "afxcmn.h"

#include "Win7Decls.h"

#include "Updater/UpdateController.h"
#include "Updater/UpdateStep.h"
#include "ExceptionSafeThread.h"

#include "LogViewer.h"

// UpdaterDialog dialog
class UpdaterDialog : 
	public CDialog,
	public tdm::updater::IUpdateView
{
private:
	tdm::updater::UpdaterOptions& _options;
	tdm::updater::UpdateControllerPtr _controller;

	bool _shutdown;

	bool _failed;

	LogViewerPtr _logViewer;

	enum TaskbarProgressType
	{
		TBP_NoProgress,
		TBP_Indeterminate,
		TBP_Normal,
		TBP_Paused,
		TBP_Error,
	};

public:
	UpdaterDialog(const fs::path& executableName, 
				  tdm::updater::UpdaterOptions& options, 
				  CWnd* pParent = NULL);	// standard constructor

	~UpdaterDialog();

	// Dialog Data
	enum { IDD = IDD_TDM_UPDATE_MFC_DIALOG };

	void SetProgressText(const std::string& text);
	void SetProgressSpeedText(const std::string& text);
	void SetProgress(double progressFraction);
	void SetFullDownloadProgress(const tdm::updater::ProgressInfo& info);

	void SetTaskbarProgress(TaskbarProgressType type, double progressFraction);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	// IUpdateView implementation
	void OnStartStep(tdm::updater::UpdateStep step);
	void OnFinishStep(tdm::updater::UpdateStep step);
	void OnFailure(tdm::updater::UpdateStep step, const std::string& errorMessage);
	void OnProgressChange(const tdm::updater::ProgressInfo& info);
	void OnMessage(const std::string& message);
	void OnWarning(const std::string& message);
	void OnStartDifferentialUpdate(const tdm::updater::DifferentialUpdateInfo& info);
	void OnPerformDifferentialUpdate(const tdm::updater::DifferentialUpdateInfo& info);
	void OnFinishDifferentialUpdate(const tdm::updater::DifferentialUpdateInfo& info);

private:
	void ClearSteps();

	void OnFail();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnDestroyWhenThreadsDone(UINT wParam, LONG lParam);
	afx_msg LRESULT OnTaskbarBtnCreated(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	CComPtr<ITaskbarList3>  _taskbarList;
	static UINT WM_TASKBARBTNCREATED;

public:
	CStatic _title;
	CStatic _subTitle;
	CFont _titleFont;
	CProgressCtrl _progressMain;
	afx_msg void OnBnClickedButtonAbort();
	CStatic _statusText;
	CStatic _step1Text;
	afx_msg void OnBnClickedButtonContinue();
	CButton _continueButton;
	CStatic _step2Text;
	CStatic _step3Text;
	CStatic _step4Text;
	CStatic _step5Text;
	CButton _abortButton;
	CStatic _step6Text;
	CStatic _step7Text;
	CStatic _step8Text;
	CStatic _step1State;
	CStatic _step2State;
	CStatic _step3State;
	CStatic _step4State;
	CStatic _step5State;
	CStatic _step6State;
	CStatic _step7State;
	CStatic _step8State;
	CStatic _progressSpeedText;
	afx_msg void OnBnClickedAdvOptionsButton();
	CButton _advOptionsButton;
	CButton _showLogButton;
	afx_msg void OnBnClickedShowLogButton();
	afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
};
