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

#include "Updater/UpdaterOptions.h"
#include "afxwin.h"

// AdvancedOptionsDialog dialog

class AdvancedOptionsDialog : public CDialog
{
	DECLARE_DYNAMIC(AdvancedOptionsDialog)

private:
	tdm::updater::UpdaterOptions& _options;

public:
	AdvancedOptionsDialog(tdm::updater::UpdaterOptions& options, CWnd* pParent = NULL);   // standard constructor
	virtual ~AdvancedOptionsDialog();

// Dialog Data
	enum { IDD = IDD_ADV_OPTIONS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	HICON m_hIcon;

	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
	CButton _keepMirrors;
	CButton _noSelfUpdate;
	CButton _keepUpdatePackages;
	CEdit _targetFolder;

private:
	void LoadValuesFromOptions();
	void SaveValuesToOptions();
public:
	CEdit _targetDir;
	CEdit _proxy;
	CStatic _labelBehaviour;
	CStatic _labelTargetDir;
	CStatic _labelProxy;
	CFont _boldFont;
};
