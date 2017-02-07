/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_DLGBUTTONPROPS_H
#define INC_DLGBUTTONPROPS_H

#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <string>
#include <sstream>
#include "resource.h"
#include "mapnode.h"

class DlgButtonProps : public CDialog
{
	DECLARE_DYNAMIC(DlgButtonProps)

public:
	DlgButtonProps();
	~DlgButtonProps();

	int ShowModal(CWnd *parent, MenuButton *btn);

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnBrowseImgCold();
	afx_msg void OnBrowseImgHot();
	afx_msg void OnBrowseImgDown();
	afx_msg void OnBrowseSndHot();
	afx_msg void OnBrowseSndDown();
	afx_msg void OnBrowseSndUp();

	CString Browse(CString fileType, CString extension);

	DECLARE_MESSAGE_MAP()

private:

	MenuButton *button;
};

#endif /* INC_DLGBUTTONPROPS_H */