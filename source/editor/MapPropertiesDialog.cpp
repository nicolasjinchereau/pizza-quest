/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

// MapPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MapPropertiesDialog.h"
#include "afxdialogex.h"


// MapPropertiesDialog dialog

IMPLEMENT_DYNAMIC(MapPropertiesDialog, CDialog)

MapPropertiesDialog::MapPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MapPropertiesDialog::IDD, pParent)
{

}

MapPropertiesDialog::~MapPropertiesDialog()
{
}

void MapPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void MapPropertiesDialog::ShowDialog(PQMap *map)
{
	if(!map)
		return;

	pMap = map;

	DoModal();
}

BEGIN_MESSAGE_MAP(MapPropertiesDialog, CDialog)

END_MESSAGE_MAP()

BOOL MapPropertiesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	PQString timeLimit(pMap->timeLimit);
	PQString numDeliveries(pMap->numDeliveries);

	SetDlgItemText(IDC_TIME_LIMIT, timeLimit);
	SetDlgItemText(IDC_NUMBER_DELIVERIES, numDeliveries);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void MapPropertiesDialog::OnOK()
{
	CString timeLimit;
	CString numDeliveries;

	GetDlgItemText(IDC_TIME_LIMIT, timeLimit);
	GetDlgItemText(IDC_NUMBER_DELIVERIES, numDeliveries);

	PQString pqTimeLimit = timeLimit;
	PQString pqNumDeliveries = numDeliveries;

	pMap->timeLimit = pqTimeLimit;
	pMap->numDeliveries = pqNumDeliveries;

	EndDialog(0);
}


void MapPropertiesDialog::OnCancel()
{
	EndDialog(0);
}
