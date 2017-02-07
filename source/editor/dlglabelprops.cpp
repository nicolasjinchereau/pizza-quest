/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "dlglabelprops.h"

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


IMPLEMENT_DYNAMIC(DlgLabelProps, CDialog)

BEGIN_MESSAGE_MAP(DlgLabelProps, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()


DlgLabelProps::DlgLabelProps() : CDialog(IDD_LABEL_PROP, GetActiveWindow())
{
	label = NULL;
}

DlgLabelProps::~DlgLabelProps()
{
	textSample->DestroyWindow();
	delete textSample;
}

int DlgLabelProps::ShowModal(CWnd *parent, MenuLabel *lbl)
{
	label = lbl;
	return DoModal();
}

BOOL DlgLabelProps::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::stringstream strm;

	strm << label->GetPosition().x;
	SetDlgItemText(IDC_XPOS, strm.str().c_str());
	strm.str("");

	strm << label->GetPosition().y;
	SetDlgItemText(IDC_YPOS, strm.str().c_str());
	strm.str("");

	strm << label->GetSize().w;
	SetDlgItemText(IDC_WID, strm.str().c_str());
	strm.str("");

	strm << label->GetSize().h;
	SetDlgItemText(IDC_HGT, strm.str().c_str());
	strm.str("");

	SetDlgItemText(IDC_TEXT, label->GetText().c_str());

	// get as pointer to the original static control
	CStatic *tmp = (CStatic*)GetDlgItem(IDC_SAMPLE);

	// get the rectangle and style of the original static control
	CRect staticRect;
	tmp->GetWindowRect(&staticRect);
	ScreenToClient(&staticRect);
	DWORD style = tmp->GetStyle();
	
	// create the custom colorbox
	textSample = new CColorStatic;
	textSample->Create("Sample", WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTERIMAGE | SS_CENTER | SS_OWNERDRAW, staticRect, this, IDC_COLORSTATIC);

	textSample->SetColor(Color(label->GetColor()));

	return TRUE;
}

BOOL DlgLabelProps::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void DlgLabelProps::OnClose()
{
	EndDialog(0);
}

void DlgLabelProps::OnOK()
{
	std::stringstream strm;
	CString txt;

	// label position
	GetDlgItemText(IDC_XPOS, txt);
	int x = atoi(txt);

	GetDlgItemText(IDC_YPOS, txt);
	int y = atoi(txt);

	label->SetPosition(x, y);
				
	// label size
	GetDlgItemText(IDC_WID, txt);
	int w = atoi(txt);

	GetDlgItemText(IDC_HGT, txt);
	int h = atoi(txt);

	label->SetSize(w, h);

	// label text
	GetDlgItemText(IDC_TEXT, txt);
	label->SetText((const char*)txt);

	label->SetColor(Color(textSample->GetColor()));

	EndDialog(IDOK);
}

void DlgLabelProps::OnCancel()
{
	EndDialog(0);
}

void DlgLabelProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}