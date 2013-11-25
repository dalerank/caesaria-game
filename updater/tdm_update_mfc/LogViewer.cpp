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

// LogViewer.cpp : implementation file
//

#include "stdafx.h"
#include "tdm_update_mfc.h"
#include "LogViewer.h"
#include <boost/algorithm/string/replace.hpp>

// LogViewer dialog

IMPLEMENT_DYNAMIC(LogViewer, CDialog)

LogViewer::LogViewer(CWnd* pParent /*=NULL*/)
	: CDialog(LogViewer::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TDM_ICON);
}

LogViewer::~LogViewer()
{
}

BOOL LogViewer::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	// Remove the default text limit from edit controls
	_logView.SetLimitText(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void LogViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGVIEW, _logView);
}

void LogViewer::WriteLog(LogClass lc, const std::string& str)
{
	if (m_hWnd == NULL) return;

	// get the initial text length
	int nLength = _logView.GetWindowTextLength();

	// put the selection at the end of text
	_logView.SetSel(nLength, nLength);

	// replace the \n character with \r\n for multiline edits
	std::string tweaked = boost::algorithm::replace_all_copy(str, "\n", "\r\n");

	_logView.ReplaceSel(CString(tweaked.c_str()));
}

BEGIN_MESSAGE_MAP(LogViewer, CDialog)
	ON_BN_CLICKED(IDOK, &LogViewer::OnBnClickedOk)
END_MESSAGE_MAP()


// LogViewer message handlers


void LogViewer::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
}
