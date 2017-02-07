/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "CollisionEditor.h"
#include <afxbutton.h>

IMPLEMENT_DYNAMIC(cGL_Screen, GL_Screen)
BEGIN_MESSAGE_MAP(cGL_Screen, GL_Screen)
	ON_WM_SIZE()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CCollisionEditor, CDialog)
BEGIN_MESSAGE_MAP(CCollisionEditor, CDialog)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_COMMAND(IDCTB_MOVE, OnBtnMove)
	ON_COMMAND(IDCTB_CIRCLE, OnBtnCircle)
	ON_COMMAND(IDCTB_BOX, OnBtnBox)
	ON_COMMAND(IDCTB_POLYGON, OnBtnPolygon)
	ON_COMMAND(IDCTB_EDGE, OnBtnEdge)
END_MESSAGE_MAP()

CCollisionEditor::CCollisionEditor(CWnd* pParent) : CDialog(IDD_COLLISION_EDITOR, pParent)
{
	collisionEditor = NULL;
}

CCollisionEditor::~CCollisionEditor()
{
	delete collisionEditor;
}

void CCollisionEditor::OnClose()
{
	for(int i = 0; i < (int)img->collision_shapes.size(); i++)
		img->collision_shapes[i]->selected = false;

	EndDialog(collisionEditor->changed ? 0 : -1);
}

void CCollisionEditor::OnOK()
{
	OnClose();
}

int CCollisionEditor::ShowDialog(PQResImagePtr &image)
{
	img = image;
	
	collisionEditor = new cGL_Screen(image);

	return DoModal();
}

int CCollisionEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	collisionEditor->GL_Create(this, CRect(0, 0, 0, 0), true, true);
	
	if(CreateToolBar() != 0)
		return -1;

	return 0;
}

int CCollisionEditor::CreateToolBar()
{
	toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_BORDER_3D | CBRS_BORDER_BOTTOM);

	toolbar.SetWindowText("Collision");
	toolbar.GetToolBarCtrl().SetButtonWidth(24, 24);

	CImageList img;
	CBitmap bm;
	
	bm.LoadBitmap(IDB_TOOL_COL_COLD);
	img.Create(24, 24, ILC_COLOR24 | ILC_MASK, 5, 5);
	img.Add(&bm, RGB(255, 0, 255));
	bm.Detach();
	toolbar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();

	bm.LoadBitmap(IDB_TOOL_COL_HOT);
	img.Create(24, 24, ILC_COLOR24 | ILC_MASK, 5, 5);
	img.Add(&bm, RGB(255, 0, 255));
	bm.Detach();
	toolbar.GetToolBarCtrl().SetHotImageList(&img);
	img.Detach();
	
	toolbar.SetButtons(NULL, 5);
	toolbar.SetButtonInfo(0, IDCTB_MOVE, BTNS_BUTTON | BTNS_CHECKGROUP, 0);
	toolbar.SetButtonInfo(1, IDCTB_CIRCLE, BTNS_BUTTON | BTNS_CHECKGROUP, 1);
	toolbar.SetButtonInfo(2, IDCTB_BOX, BTNS_BUTTON | BTNS_CHECKGROUP, 2);
	toolbar.SetButtonInfo(3, IDCTB_POLYGON, BTNS_BUTTON | BTNS_CHECKGROUP, 3);
	toolbar.SetButtonInfo(4, IDCTB_EDGE, BTNS_BUTTON | BTNS_CHECKGROUP, 4);

	CRect rc_item;
	toolbar.GetItemRect(0, &rc_item);
	toolbar.SetSizes(rc_item.Size(), CSize(24, 24));
	
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	
	toolbar.GetToolBarCtrl().CheckButton(IDCTB_MOVE);
	
	return 0;
}

LONG CCollisionEditor::OnInitDialog(UINT wParam, LONG lParam)
{
	BOOL ret = TRUE;
	if(!HandleInitDialog(wParam, lParam))
		ret = FALSE;

	CRect rcEditor;
	GetDlgItem(IDC_COLLISION_EDITOR)->GetWindowRect(&rcEditor);
	ScreenToClient(&rcEditor);
	
	collisionEditor->MoveWindow(&rcEditor);
	
	return ret;
}

afx_msg void CCollisionEditor::OnBtnMove()
{
	collisionEditor->SetShapeID(-1);
}

afx_msg void CCollisionEditor::OnBtnCircle()
{
	collisionEditor->SetShapeID(SHAPE_CIRCLE);
}

afx_msg void CCollisionEditor::OnBtnBox()
{
	collisionEditor->SetShapeID(SHAPE_BOX);
}

afx_msg void CCollisionEditor::OnBtnPolygon()
{
	collisionEditor->SetShapeID(SHAPE_POLYGON);
}

afx_msg void CCollisionEditor::OnBtnEdge()
{
	collisionEditor->SetShapeID(SHAPE_EDGE);
}

void CCollisionEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}