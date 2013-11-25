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

// AdvancedOptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tdm_update_mfc.h"
#include "AdvancedOptionsDialog.h"

using namespace tdm;
using namespace updater;

// AdvancedOptionsDialog dialog

IMPLEMENT_DYNAMIC(AdvancedOptionsDialog, CDialog)

AdvancedOptionsDialog::AdvancedOptionsDialog(UpdaterOptions& options, CWnd* pParent /*=NULL*/)
	: CDialog(AdvancedOptionsDialog::IDD, pParent),
	_options(options)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TDM_ICON);
}

AdvancedOptionsDialog::~AdvancedOptionsDialog()
{
}

void AdvancedOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEEP_MIRRORS, _keepMirrors);
	DDX_Control(pDX, IDC_NOSELFUPDATE, _noSelfUpdate);
	DDX_Control(pDX, IDC_KEEP_UPDATE_PACKAGES, _keepUpdatePackages);
	DDX_Control(pDX, IDC_TARGET_DIR, _targetDir);
	DDX_Control(pDX, IDC_PROXY, _proxy);
	DDX_Control(pDX, IDC_LABEL_BEHAVIOUR, _labelBehaviour);
	DDX_Control(pDX, IDC_LABEL_TARGETDIR, _labelTargetDir);
	DDX_Control(pDX, IDC_LABEL_PROXY, _labelProxy);
}

BEGIN_MESSAGE_MAP(AdvancedOptionsDialog, CDialog)
	ON_BN_CLICKED(IDOK, &AdvancedOptionsDialog::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL AdvancedOptionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	_boldFont.CreateFont(14, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
						  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);

	_labelBehaviour.SetFont(&_boldFont);
	_labelProxy.SetFont(&_boldFont);
	_labelTargetDir.SetFont(&_boldFont);

	// Load values from the settings
	LoadValuesFromOptions();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// AdvancedOptionsDialog message handlers

void AdvancedOptionsDialog::OnBnClickedOk()
{
	// Save values from widgets into options class
	SaveValuesToOptions();

	OnOK();
}

void AdvancedOptionsDialog::LoadValuesFromOptions()
{
	_keepMirrors.SetCheck(_options.IsSet("keep-mirrors") ? BST_CHECKED : BST_UNCHECKED);
	_noSelfUpdate.SetCheck(_options.IsSet("noselfupdate") ? BST_CHECKED : BST_UNCHECKED);
	_keepUpdatePackages.SetCheck(_options.IsSet("keep-update-packages") ? BST_CHECKED : BST_UNCHECKED);

	_targetDir.SetWindowText(CString(_options.Get("targetdir").c_str()));
	_proxy.SetWindowText(CString(_options.Get("proxy").c_str()));
}

void AdvancedOptionsDialog::SaveValuesToOptions()
{
	if (_keepMirrors.GetCheck() == BST_CHECKED)
	{
		_options.Set("keep-mirrors");
	}
	else
	{
		_options.Unset("keep-mirrors");
	}

	if (_noSelfUpdate.GetCheck() == BST_CHECKED)
	{
		_options.Set("noselfupdate");
	}
	else
	{
		_options.Unset("noselfupdate");
	}

	if (_keepUpdatePackages.GetCheck() == BST_CHECKED)
	{
		_options.Set("keep-update-packages");
	}
	else
	{
		_options.Unset("keep-update-packages");
	}

	CString value;
	std::wstring wValueStr;
	std::string valueStr;

	_targetDir.GetWindowText(value);
	wValueStr = (LPCTSTR)value;
	valueStr.assign(wValueStr.begin(), wValueStr.end());

	if (!valueStr.empty())
	{
		_options.Set("targetdir", valueStr);
	}
	else
	{
		_options.Unset("targetdir");
	}

	_proxy.GetWindowText(value);
	wValueStr = (LPCTSTR)value;
	valueStr.assign(wValueStr.begin(), wValueStr.end());

	if (!valueStr.empty())
	{
		_options.Set("proxy", valueStr);
	}
	else
	{
		_options.Unset("proxy");
	}
}
