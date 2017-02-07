/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "dlgbuttonprops.h"

//using namespace std;

IMPLEMENT_DYNAMIC(DlgButtonProps, CDialog)

BEGIN_MESSAGE_MAP(DlgButtonProps, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDC_BROWSE_IMG_COLD, OnBrowseImgCold)
	ON_COMMAND(IDC_BROWSE_IMG_HOT, OnBrowseImgHot)
	ON_COMMAND(IDC_BROWSE_IMG_DOWN, OnBrowseImgDown)
	ON_COMMAND(IDC_BROWSE_SND_HOT, OnBrowseSndHot)
	ON_COMMAND(IDC_BROWSE_SND_DOWN, OnBrowseSndDown)
	ON_COMMAND(IDC_BROWSE_SND_UP, OnBrowseSndUp)
	ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

DlgButtonProps::DlgButtonProps() : CDialog(IDD_BUTTON_PROP, GetActiveWindow())
{
	button = NULL;
}

DlgButtonProps::~DlgButtonProps()
{

}

int DlgButtonProps::ShowModal(CWnd *parent, MenuButton *btn)
{
	button = btn;
	return DoModal();
}

BOOL DlgButtonProps::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	std::stringstream strm;

	// set ID field
	strm << button->GetID();
	SetDlgItemText(IDC_ID, strm.str().c_str());
	strm.str("");

	// set X Position field
	strm << button->GetPosition().x;
	SetDlgItemText(IDC_XPOS, strm.str().c_str());
	strm.str("");

	// set Y Position field
	strm << button->GetPosition().y;
	SetDlgItemText(IDC_YPOS, strm.str().c_str());
	strm.str("");

	// set image filesnames
	SetDlgItemText(IDC_IMG_COLD, button->GetColdImage().c_str());
	SetDlgItemText(IDC_IMG_HOT, button->GetHotImage().c_str());
	SetDlgItemText(IDC_IMG_DOWN, button->GetDownImage().c_str());
	
	// set sound filesnames
	SetDlgItemText(IDC_SND_HOT, button->GetHotSound().c_str());
	SetDlgItemText(IDC_SND_DOWN, button->GetDownSound().c_str());
	SetDlgItemText(IDC_SND_UP, button->GetUpSound().c_str());

	return TRUE;
}

BOOL DlgButtonProps::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void DlgButtonProps::OnClose()
{
	EndDialog(0);
}

void DlgButtonProps::OnOK()
{
	std::stringstream strm;
	CString txt;

// button position
	GetDlgItemText(IDC_XPOS, txt);
	int x = atoi(txt);

	GetDlgItemText(IDC_YPOS, txt);
	int y = atoi(txt);

	button->SetPosition(x, y);

//button id
	GetDlgItemText(IDC_ID, txt);
	int id = atoi(txt);

	button->SetID(id);

// button images
	GetDlgItemText(IDC_IMG_COLD, txt);
	button->SetColdImage((const char*)txt);

	GetDlgItemText(IDC_IMG_HOT, txt);
	button->SetHotImage((const char*)txt);

	GetDlgItemText(IDC_IMG_DOWN, txt);
	button->SetDownImage((const char*)txt);

// button sounds
	GetDlgItemText(IDC_SND_HOT, txt);
	button->SetHotSound((const char*)txt);

	GetDlgItemText(IDC_SND_DOWN, txt);
	button->SetDownSound((const char*)txt);

	GetDlgItemText(IDC_SND_UP, txt);
	button->SetUpSound((const char*)txt);
	
	EndDialog(IDOK);
}

void DlgButtonProps::OnCancel()
{
	EndDialog(0);
}

void DlgButtonProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void DlgButtonProps::OnBrowseImgCold()
{
	SetDlgItemText(IDC_IMG_COLD, Browse("PNG Image Files", "png"));
}

void DlgButtonProps::OnBrowseImgHot()
{
	SetDlgItemText(IDC_IMG_HOT, Browse("PNG Image Files", "png"));
}

void DlgButtonProps::OnBrowseImgDown()
{
	SetDlgItemText(IDC_IMG_DOWN, Browse("PNG Image Files", "png"));
}

void DlgButtonProps::OnBrowseSndHot()
{
	SetDlgItemText(IDC_SND_HOT, Browse("Sound Files", "wav"));
}

void DlgButtonProps::OnBrowseSndDown()
{
	SetDlgItemText(IDC_SND_DOWN, Browse("Sound Files", "wav"));
}

void DlgButtonProps::OnBrowseSndUp()
{
	SetDlgItemText(IDC_SND_UP, Browse("Sound Files", "wav"));
}

CString DlgButtonProps::Browse(CString fileType, CString extension)
{
	CFileDialog dlg_open(TRUE, NULL, NULL,
					OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_LONGNAMES |
					OFN_DONTADDTORECENT | OFN_ENABLESIZING,
					fileType + " (*." + extension + ")|*." + extension + "||", NULL, 0);

	if(dlg_open.DoModal() == IDOK) return dlg_open.GetPathName();

	return "";
}