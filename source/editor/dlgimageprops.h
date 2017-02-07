/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_DLGIMAGEPROPS_H
#define INC_DLGIMAGEPROPS_H
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include <string>
#include <sstream>
#include "resource.h"
#include "mapnode.h"

class DlgImageProps : public CDialog
{
	DECLARE_DYNAMIC(DlgImageProps)

public:
	DlgImageProps();
	~DlgImageProps();

	int ShowModal(CWnd *parent, MenuImage *img);

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg virtual void OnOK();

	BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnClose();
	afx_msg void OnCancel();
	afx_msg void OnBrowseImage();
	afx_msg void OnCheckAutosize();

	CString Browse(CString fileType, CString extension);

	DECLARE_MESSAGE_MAP()

private:

	MenuImage *image;
};

#endif /* INC_DLGIMAGEPROPS_H */