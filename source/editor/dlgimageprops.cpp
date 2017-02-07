/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "dlgimageprops.h"

IMPLEMENT_DYNAMIC(DlgImageProps, CDialog)

BEGIN_MESSAGE_MAP(DlgImageProps, CDialog)
	ON_WM_CLOSE()
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(IDC_BROWSE_IMAGE, OnBrowseImage)
	ON_COMMAND(IDC_AUTOSIZE, OnCheckAutosize)
END_MESSAGE_MAP()

DlgImageProps::DlgImageProps() : CDialog(IDD_IMAGE_PROP, GetActiveWindow())
{
	image = NULL;
}

DlgImageProps::~DlgImageProps()
{

}

int DlgImageProps::ShowModal(CWnd *parent, MenuImage *img)
{
	image = img;
	return DoModal();
}

BOOL DlgImageProps::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd *Item;
	std::stringstream strm;
	
	// x position
	strm << image->GetPosition().x;
	GetDlgItem(IDC_XPOS)->SetWindowTextA(strm.str().c_str());
	strm.str("");

	// y position
	strm << image->GetPosition().y;
	GetDlgItem(IDC_YPOS)->SetWindowTextA(strm.str().c_str());
	strm.str("");

	// width
	strm << image->GetSize().w;
	GetDlgItem(IDC_WID)->SetWindowTextA(strm.str().c_str());
	strm.str("");

	// height
	strm << image->GetSize().h;
	GetDlgItem(IDC_HGT)->SetWindowTextA(strm.str().c_str());
	strm.str("");

	// filename
	Item = GetDlgItem(IDC_IMAGE);
	if(Item) Item->SetWindowTextA(image->GetImage().c_str());

	// autosize
	//int checked = ( == true) ? BST_CHECKED : BST_UNCHECKED;
	CheckDlgButton(IDC_AUTOSIZE, (int)image->Autosize());
	OnCheckAutosize();

	return TRUE;
}

BOOL DlgImageProps::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void DlgImageProps::OnClose()
{
	EndDialog(0);
}
void DlgImageProps::OnOK()
{
	std::stringstream strm;
	CString txt;

	// image position
	GetDlgItem(IDC_XPOS)->GetWindowTextA(txt);
	int x = atoi(txt);

	GetDlgItem(IDC_YPOS)->GetWindowTextA(txt);
	int y = atoi(txt);
	
	image->SetPosition(x, y);

	// set image size (this is ignored if autosize is on and the image is valid)
	GetDlgItem(IDC_WID)->GetWindowTextA(txt);
	int w = atoi(txt);

	GetDlgItem(IDC_HGT)->GetWindowTextA(txt);
	int h = atoi(txt);

	image->SetSize(w, h);

	// image filename
	GetDlgItem(IDC_IMAGE)->GetWindowTextA(txt);
	
	// load the image
	if(image->SetImage((const char*)txt) != 0)
	{
		MessageBox("Failed to open the image", "Error opening file", MB_OK | MB_ICONERROR);
		image->SetImage("");
	}
	
	// autosize
	image->SetAutosize(IsDlgButtonChecked(IDC_AUTOSIZE) == BST_CHECKED);

	EndDialog(IDOK);
}

void DlgImageProps::OnCancel()
{
	EndDialog(0);
}

void DlgImageProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void DlgImageProps::OnBrowseImage()
{
	((CEdit*)GetDlgItem(IDC_IMAGE))->SetWindowTextA(Browse("PNG Image Files", "png"));
}

void DlgImageProps::OnCheckAutosize()
{
	if(IsDlgButtonChecked(IDC_AUTOSIZE) == BST_CHECKED)
	{
		GetDlgItem(IDC_WID)->EnableWindow(FALSE);
		GetDlgItem(IDC_HGT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_WID)->EnableWindow(TRUE);
		GetDlgItem(IDC_HGT)->EnableWindow(TRUE);
	}
}

CString DlgImageProps::Browse(CString fileType, CString extension)
{
	CFileDialog dlg_open(TRUE, NULL, NULL,
					OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_LONGNAMES |
					OFN_DONTADDTORECENT | OFN_ENABLESIZING,
					fileType + " (*." + extension + ")|*." + extension + "||", NULL, 0);

	if(dlg_open.DoModal() == IDOK) return dlg_open.GetPathName();

	return "";
}