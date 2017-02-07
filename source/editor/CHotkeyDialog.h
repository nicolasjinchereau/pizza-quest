/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "definitions.h"
#include "resource.h"

class CHotkeyDialog : public CDialog
{
	DECLARE_DYNAMIC(CHotkeyDialog)
public:
	CHotkeyDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHotkeyDialog();

	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	void AddHotkey(const char *keys, const char *description);

	afx_msg void OnClose();

	enum { IDD = IDD_HOTKEYS };

	CListCtrl list;

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
};