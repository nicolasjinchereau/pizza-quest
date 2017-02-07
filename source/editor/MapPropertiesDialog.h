/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "resource.h"
#include "PQMap.h"
#include "PQString.h"

// MapPropertiesDialog dialog

class MapPropertiesDialog : public CDialog
{
	DECLARE_DYNAMIC(MapPropertiesDialog)

public:
	MapPropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~MapPropertiesDialog();

// Dialog Data
	enum { IDD = IDD_MAP_PROPERTIES };

	void ShowDialog(PQMap *map);

	PQMap *pMap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
};
