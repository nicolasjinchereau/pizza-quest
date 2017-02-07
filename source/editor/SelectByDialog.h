/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "resource.h"
#include "PQMap.h"
#include "PQMapEditor.h"

// SelectByDialog dialog

class SelectByDialog : public CDialog
{
	DECLARE_DYNAMIC(SelectByDialog)

public:
	SelectByDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SelectByDialog();

// Dialog Data
	enum { IDD = IDD_SELECTBY };

	void ShowDialog(PQMap *map);

protected:
	PQMap *pMap;

	CEdit searchText;
	CComboBox cmbSearch;

	afx_msg void OnBtnCancel();
	afx_msg void OnBtnSelect();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeSearch();
};
