/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

// SelectByDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SelectByDialog.h"
#include "afxdialogex.h"


// SelectByDialog dialog

IMPLEMENT_DYNAMIC(SelectByDialog, CDialog)

SelectByDialog::SelectByDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SelectByDialog::IDD, pParent)
{
	pMap = NULL;
}

SelectByDialog::~SelectByDialog()
{
}

void SelectByDialog::ShowDialog(PQMap *map)
{
	if(!map)
		return;

	pMap = map;

	DoModal();
}

void SelectByDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_SEARCHTEXT, searchText);
	DDX_Control(pDX, IDC_SEARCH, cmbSearch);
}

BEGIN_MESSAGE_MAP(SelectByDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_SEARCH, &SelectByDialog::OnCbnSelchangeSearch)
	ON_COMMAND(IDCANCEL, OnBtnCancel)
	ON_COMMAND(IDOK, OnBtnSelect)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void SelectByDialog::OnBtnCancel()
{
	EndDialog(0);
}

void SelectByDialog::OnBtnSelect()
{
	CString sText;
	searchText.GetWindowTextA(sText);

	int idx = cmbSearch.GetCurSel();

	switch(idx)
	{
	case 0: // by Class ID
		MapEditor.SelectByClassID((const char*)sText);
		break;

	case 1: // By Name
		MapEditor.SelectByName((const char*)sText);
		break;
	}

	EndDialog(0);
}

// SelectByDialog message handlers

BOOL SelectByDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	cmbSearch.AddString("Class ID");
	cmbSearch.AddString("Name");
	cmbSearch.SetCurSel(0);
	
	searchText.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
}


void SelectByDialog::OnCbnSelchangeSearch()
{
	
}
