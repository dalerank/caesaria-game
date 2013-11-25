/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod Updater (http://www.thedarkmod.com/)
 
 $Revision: 5793 $ (Revision of last commit) 
 $Date: 2013-05-15 15:45:36 +0400 (Ср, 15 май 2013) $ (Date of last commit)
 $Author: tels $ (Author of last commit)
 
******************************************************************************/


// tdm_update_mfc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tdm_update_mfc.h"
#include "UpdaterDialog.h"
#include "LogWriters.h"
#include "CommandLineInfo.h"

#include "Constants.h"
#include <boost/format.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// UpdaterApplication

BEGIN_MESSAGE_MAP(UpdaterApplication, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// UpdaterApplication construction

UpdaterApplication::UpdaterApplication()
{}

// The one and only UpdaterApplication object

UpdaterApplication theApp;

using namespace tdm;

// UpdaterApplication initialization
BOOL UpdaterApplication::InitInstance()
{
	// Start logging
	RegisterLogWriters();

	TraceLog::WriteLine(LOG_STANDARD, 
		(boost::format("TDM Updater v%s (c) 2009-2013 by tels & greebo. Part of The Dark Mod (http://www.thedarkmod.com).") % LIBTDM_UPDATE_VERSION).str());
	TraceLog::WriteLine(LOG_STANDARD, "");

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need

	WCHAR szFileName[MAX_PATH];
    HINSTANCE hInstance = GetModuleHandle(NULL);

    GetModuleFileName(hInstance, szFileName, MAX_PATH);

	std::wstring ws(szFileName);
	std::string str;
	str.assign(ws.begin(), ws.end()); 

	fs::path executableName = fs::path(str).leaf();

	// Parse the command line into our program options
	tdm::updater::CommandLineInfo commandLine;
	ParseCommandLine(commandLine);

	UpdaterDialog dlg(executableName, commandLine.options);
	m_pMainWnd = &dlg;

	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
