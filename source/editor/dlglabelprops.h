/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_DLGLABELPROPS_H
#define INC_DLGLABELPROPS_H
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>
#include "definitions.h"
#include <string>
#include <sstream>
#include "resource.h"
#include "mapnode.h"

#define IDC_COLORSTATIC 0x1201

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:

	CColorStatic() : clr(0){}

protected:
	
	BOOL OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
	}

	void OnPaint()
	{
		CPaintDC dc(this);
		GetClientRect(&clientRect);
		CBrush brush(clr);
		dc.FillRect(clientRect, &brush);
		dc.DrawEdge(&clientRect, EDGE_SUNKEN, BF_RECT);
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(pMsg->message == WM_LBUTTONUP)
		{
			PickColor();
		}

		return CStatic::PreTranslateMessage(pMsg);
	}
public:

	void PickColor()
	{
		CColorDialog dlg_color(clr, CC_ANYCOLOR | CC_RGBINIT | CC_SOLIDCOLOR, this);

		if(dlg_color.DoModal() == IDOK)
			clr = dlg_color.GetColor();

		RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_UPDATENOW | RDW_INVALIDATE);
	}

	void SetColor(COLORREF color)
	{
		clr = color;
		RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_UPDATENOW | RDW_INVALIDATE);
	}

	COLORREF GetColor(){return clr;}

protected:

	COLORREF clr;
	CRect clientRect;

	DECLARE_MESSAGE_MAP()
public:

};

/************************/
/*   LABEL PROPERTIES   */
/************************/

class DlgLabelProps : public CDialog
{
	DECLARE_DYNAMIC(DlgLabelProps)

public:
	DlgLabelProps();
	~DlgLabelProps();

	int ShowModal(CWnd *parent, MenuLabel *lbl);

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

private:
	
	CColorStatic *textSample;

	MenuLabel *label;
};

#endif /* INC_DLGLABELPROPS_H */