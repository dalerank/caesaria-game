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

#include "afxwin.h"
#include "TraceLog.h"
#include <boost/shared_ptr.hpp>

// LogViewer dialog

using namespace tdm;

class LogViewer : 
	public CDialog,
	public ILogWriter
{
	DECLARE_DYNAMIC(LogViewer)

	HICON m_hIcon;

public:
	LogViewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~LogViewer();

	// Dialog Data
	enum { IDD = IDD_LOGVIEWER_DIALOG };

	// ILogWriter impl.
	void WriteLog(LogClass lc, const std::string& str);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit _logView;
	afx_msg void OnBnClickedOk();
};
typedef boost::shared_ptr<LogViewer> LogViewerPtr;
