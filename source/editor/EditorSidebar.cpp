/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "EditorSidebar.h"

////////////////////////////////////////////
//   PREVIEW
////////////////////////////////////////////

void pGL_Screen::OnDraw()
{
	Set2D();

	if( nodePtr )
	{
		nodePtr->DrawThumb(this);
	}
	else if( resPtr )
	{
		resPtr->DrawThumb(this);
	}
}

int pGL_Screen::Update(void *data)
{
	MListCtrl *list = UIManager.GetItem<MListCtrl>("resource list");
	
	assert(list);

	resPtr.reset();
	nodePtr.reset();
	
	if(MapEditor.GetCurrentTool() == TOOL_CREATION)
	{
		int idx = list->GetSelectionMark();

		if(idx != -1)
		{
			resPtr = *(PQResourcePtr*)list->GetItemData(idx);
		}
	}
	else if(MapEditor.GetCurrentTool() == TOOL_SELECTION)
	{
		if(MapEditor.GetCurrentLayer() == LAYER_SPRITES)
		{
			if(MapEditor.map->sceneSelection.size() == 1)
			{
				nodePtr = MapEditor.map->sceneSelection.front();
			}
		}
	}
	
	return RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
}

////////////////////////////////////////////
//   RESOURCE LIST
////////////////////////////////////////////

IMPLEMENT_DYNAMIC(MListCtrl, CListCtrl)
BEGIN_MESSAGE_MAP(MListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &MListCtrl::OnNMDblclk)
	ON_NOTIFY(HDN_ITEMCLICKA, 0, &MListCtrl::OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, &MListCtrl::OnHdnItemclick)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(NM_SETFOCUS, &MListCtrl::OnNMSetfocus)
	ON_NOTIFY_REFLECT(NM_CLICK, &MListCtrl::OnNMClick)
END_MESSAGE_MAP()

int MListCtrl::Update(void *data)
{
	int nItemCount = GetItemCount();
	
	if(MapEditor.map->resources.size() == nItemCount)
	{
		bool changed = false;

		for(int i = 0; i < nItemCount; i++)
		{
			PQResourcePtr &ptr = *(PQResourcePtr*)GetItemData(i);
			CString name = GetItemText(i, 0);
			CString classId = GetItemText(i, 1);

			if(MapEditor.map->resources[i] != ptr
			|| name != ptr->resource_name
			|| classId != ptr->class_id)
			{
				changed = true;
				break;
			}
		}

		if(!changed)
			return 0;
	}

	DeleteAllItems();

	SetItemCount(MapEditor.map->resources.size());
	SetRedraw(FALSE);

	int idx;

	for(Uint16 i = 0; i < MapEditor.map->resources.size(); i++)
	{
		idx = InsertItem(0xFFFF, MapEditor.map->resources[i]->resource_name, 0);
		SetItemText(idx, 1, MapEditor.map->resources[i]->class_id);
		SetItemData(idx, (DWORD_PTR)&MapEditor.map->resources[i]);
	}

	SetRedraw(TRUE);

	return 0;
}

void MListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if(pNMLV->uChanged & LVIF_STATE)
	{
		if(pNMLV->uNewState & LVIS_SELECTED | LVIS_FOCUSED)
		{
			PQResourcePtr res = *((PQResourcePtr*)GetItemData(pNMLV->iItem));
			MapEditor.SetCurrentBrush(res);
		}
	}

	*pResult = 0;
}

void MListCtrl::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}

void MListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	UIManager.UpdateItem("preview");
	UIManager.UpdateItem("property grid");

	*pResult = 0;
}

void MListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// open the collision editor for the current resource
	if(pNMItemActivate->iItem != -1)
	{
		PQResourcePtr res = *((PQResourcePtr*)GetItemData(pNMItemActivate->iItem));
		PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(res);

		if(rImg != NULL)
		{
			CCollisionEditor editor(AfxGetApp()->GetMainWnd());
			
			// show the collision editor dialog
			if(editor.ShowDialog(rImg) == 0)
				MapEditor.SetChanged();

			// update the preview window to reflect the new changes in the collision
			UIManager.UpdateItem("preview");

			SetFocus();
		}
	}

	*pResult = 0;
}

bool SortByName(PQResourcePtr &res1, PQResourcePtr &res2)
{
	// return true if items already in order
	if(strcmp(res1->resource_name, res2->resource_name) < 0)
		return true;
	else
		return false;
}

bool SortByClassID(PQResourcePtr &res1, PQResourcePtr &res2)
{
	// return true if items already in order
	if (strcmp(res1->class_id, res2->class_id) < 0)
		return true;
	else
		return false;
}

void MListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// sort the resources when a column header is clicked
	switch(phdr->iItem)
	{
	case 0: // by name
		sort(MapEditor.map->resources.begin(), MapEditor.map->resources.end(), SortByName);
		break;
	case 1: // by ClassID
		sort(MapEditor.map->resources.begin(), MapEditor.map->resources.end(), SortByClassID);
		break;
	}

	Update(NULL);
	
	*pResult = 0;
}

////////////////////////////////////////////
//   PROPERTY GRID
////////////////////////////////////////////

#define PV_STR(s) ((_bstr_t)(_variant_t)s)
#define PV_INT(s) (atoi(PV_STR(s)))
#define PV_FLT(s) ((float)atof(PV_STR(s)))
#define PV_BOOL(s) ( (string(PV_STR(s)) == "true") ? true : false )

int MPropertyGrid::Update(void *data)
{
	MListCtrl *list = UIManager.GetItem<MListCtrl>("resource list");

	assert(list);

	nodePtr.reset();

	if(MapEditor.GetCurrentTool() == TOOL_CREATION)
	{
		int idx = list->GetSelectionMark();

		if(idx != -1)
		{
			PQResourcePtr &ptr = *((PQResourcePtr*)list->GetItemData(idx));

			nodePtr = dynamic_pointer_cast<PQNode>(ptr);
		}

	}
	else if(MapEditor.GetCurrentTool() == TOOL_SELECTION)
	{
		if(MapEditor.GetCurrentLayer() == LAYER_SPRITES)
		{
			if(MapEditor.map->sceneSelection.size() == 1)
			{
				nodePtr = dynamic_pointer_cast<PQNode>( MapEditor.map->sceneSelection.front() );
			}
		}
	}

	NumStr<int> IStr;
	NumStr<bool> BStr;
	NumStr<float> FStr;

	RemoveAll();

	if( nodePtr )
	{
		switch(nodePtr->type)
		{
			case N_RES_IMAGE:
			{
				PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(nodePtr);

				static TCHAR BASED_CODE szFilter[] = _T("Image Files(*.png)|*.png||");

				////////////
				CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty("General");
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Name", rImg->resource_name, "unique name for this resource", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridFileProperty("Source File", TRUE, rImg->source_file, "png", 0, szFilter, "source file for this resource", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Class ID", rImg->class_id, "name of game class", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup1);

				////////////
				CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty("Sprite Sheet");
				pGroup2->AddSubItem(new CMFCPropertyGridProperty("Rows", IStr.in(rImg->nRows), "number of spritesheet rows", (DWORD_PTR)&nodePtr));
				pGroup2->AddSubItem(new CMFCPropertyGridProperty("Columns", IStr.in(rImg->nCols), "number of spritesheet columns", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup2);
		
				////////////
				CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty("New Instances");
				pGroup3->AddSubItem(new CMFCPropertyGridProperty("Row", IStr.in(rImg->newRow), "current spritesheet row", (DWORD_PTR)&nodePtr));
				pGroup3->AddSubItem(new CMFCPropertyGridProperty("Column", IStr.in(rImg->newCol), "current spritesheet column", (DWORD_PTR)&nodePtr));
				pGroup3->AddSubItem(new CMFCPropertyGridProperty("Angle", FStr.in(rImg->newAngle), "angle of this sprite", (DWORD_PTR)&nodePtr));
				pGroup3->AddSubItem(new CMFCPropertyGridProperty("Scale", FStr.in(rImg->newScale), "scale of this sprite", (DWORD_PTR)&nodePtr));
				pGroup3->AddSubItem(new CMFCPropertyGridProperty("Value 1", IStr.in(rImg->newValue1), "value passed into game", (DWORD_PTR)&nodePtr));
				pGroup3->AddSubItem(new CMFCPropertyGridProperty("Value 2", IStr.in(rImg->newValue2), "value passed into game", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup3);
				break;
			}
			case N_RES_SOUND:
			{
				PQResSoundPtr rSnd = dynamic_pointer_cast<PQResSound>(nodePtr);

				static TCHAR BASED_CODE szFilter[] = _T("Sound Files(*.wav)|*.wav||");

				////////////
				CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty("General");
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Name", rSnd->resource_name, "unique name for this resource", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridFileProperty("Source File", TRUE, rSnd->source_file, "png", 0, szFilter, "source file for this resource", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Class ID", rSnd->class_id, "name of game class", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup1);

				////////////
				CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty("New Instances");
				pGroup2->AddSubItem(new CMFCPropertyGridProperty("Volume", FStr.in(rSnd->newVolume), "volume of the sound node in the map", (DWORD_PTR)&nodePtr));

				CMFCPropertyGridProperty *pLoopProp = new CMFCPropertyGridProperty("Loop", BStr.in(rSnd->newLoop), "loop infinitely", (DWORD_PTR)&nodePtr);
				pLoopProp->AddOption("true");
				pLoopProp->AddOption("false");
				pGroup2->AddSubItem(pLoopProp);

				pGroup2->AddSubItem(new CMFCPropertyGridProperty("Delay", IStr.in(rSnd->newDelay), "milliseconds between loops", (DWORD_PTR)&nodePtr));
				pGroup2->AddSubItem(new CMFCPropertyGridProperty("Radius", IStr.in(rSnd->newTriggerRadius), "sound is triggered if player is at least this close", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup2);

				break;
			}
			case N_MAP_IMAGE:
			{
				PQMapImagePtr mImg = dynamic_pointer_cast<PQMapImage>(nodePtr);

				////////////
				CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty("Selection");

				CMFCPropertyGridProperty *p = new CMFCPropertyGridProperty("Name", mImg->res->resource_name, "resource name", (DWORD_PTR)&nodePtr);
				p->Enable(FALSE);
				pGroup1->AddSubItem(p);

				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Row", IStr.in(mImg->row), "current spritesheet row", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Column", IStr.in(mImg->col), "current spritesheet column", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Angle", FStr.in(mImg->angle), "angle of this sprite", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Scale", FStr.in(mImg->scale), "scale of this sprite", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Value 1", IStr.in(mImg->value1), "value passed into game", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Value 2", IStr.in(mImg->value2), "value passed into game", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup1);
				break;
			}
			case N_MAP_SOUND:
			{
				PQMapSoundPtr mSnd = dynamic_pointer_cast<PQMapSound>(nodePtr);

				CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty("Selection");

				CMFCPropertyGridProperty *p = new CMFCPropertyGridProperty("Name", mSnd->res->resource_name, "resource name", (DWORD_PTR)&nodePtr);
				p->Enable(FALSE);
				pGroup1->AddSubItem(p);

				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Volume", FStr.in(mSnd->volume), "volume of the sound node in the map", (DWORD_PTR)&nodePtr));

				CMFCPropertyGridProperty *pLoopProp = new CMFCPropertyGridProperty("Loop", BStr.in(mSnd->loop), "loop infinitely", (DWORD_PTR)&nodePtr);
				pLoopProp->AddOption("true");
				pLoopProp->AddOption("false");
				pGroup1->AddSubItem(pLoopProp);

				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Delay", IStr.in(mSnd->delay), "milliseconds between loops", (DWORD_PTR)&nodePtr));
				pGroup1->AddSubItem(new CMFCPropertyGridProperty("Radius", IStr.in(mSnd->triggerRadius), "sound is triggered if player is at least this close", (DWORD_PTR)&nodePtr));
				AddProperty(pGroup1);
				break;
			}
		}
	}

	Redraw();

	return 0;
}

void MPropertyGrid::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	CMFCPropertyGridCtrl::OnPropertyChanged(pProp);

	string propName = pProp->GetName();

	PQNodePtr *node = ((PQNodePtr*)pProp->GetData());

	switch((*node)->type)
	{
		case N_RES_IMAGE:
		{
			PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(*node);

			// basic resource properties
			if(propName == "Name")
			{
				rImg->SetResourceName(PV_STR(pProp->GetValue()));
				UIManager.UpdateItem("resource list");
			}
			else if(propName == "Source File")
			{
				rImg->SetSourceFile(PV_STR(pProp->GetValue()));
				UIManager.UpdateItem("property grid");
			}
			else if(propName == "Class ID")
			{
				rImg->SetClassID(PV_STR(pProp->GetValue()));
				UIManager.UpdateItem("resource list");
			}
			else if(propName == "Rows")
			{
				rImg->nRows = PV_INT(pProp->GetValue());
				rImg->img->SetNumRows( rImg->nRows );
				UIManager.UpdateItem("viewport");
			}
			else if(propName == "Columns")
			{
				rImg->nCols = PV_INT(pProp->GetValue());
				rImg->img->SetNumCols( rImg->nCols );
				UIManager.UpdateItem("viewport");
			}
			// image instance properties
			else if(propName == "Row")
			{
				rImg->newRow = PV_INT(pProp->GetValue());
			}
			else if(propName == "Column")
			{
				rImg->newCol = PV_INT(pProp->GetValue());
			}
			else if(propName == "Angle")
			{
				rImg->newAngle = PV_FLT(pProp->GetValue());
			}
			else if(propName == "Scale")
			{
				rImg->newScale = PV_FLT(pProp->GetValue());
			}
			else if(propName == "Value 1")
			{
				rImg->newValue1 = PV_INT(pProp->GetValue());
			}
			else if(propName == "Value 2")
			{
				rImg->newValue2 = PV_INT(pProp->GetValue());
			}

			volatile int a = 5;
			UIManager.UpdateItem("preview");

			break;
		}
		case N_RES_SOUND:
		{
			// basic resource properties
			PQResSoundPtr rSnd = dynamic_pointer_cast<PQResSound>(*node);
			
			if(propName == "Name")
			{
				rSnd->SetResourceName(PV_STR(pProp->GetValue()));
				UIManager.UpdateItem("resource list");
			}
			else if(propName == "Source File")
			{
				rSnd->SetSourceFile(PV_STR(pProp->GetValue()));
				UIManager.UpdateItem("property grid");
			}
			else if(propName == "Class ID")
			{
				rSnd->SetClassID(PV_STR(pProp->GetValue()));
				UIManager.UpdateItem("resource list");
			}

			// sound instance properties
			if(propName == "Volume")
			{
				rSnd->newVolume = PV_FLT(pProp->GetValue());
			}
			else if(propName == "Loop")
			{
				rSnd->newLoop = PV_BOOL(pProp->GetValue());
			}
			else if(propName == "Delay")
			{
				rSnd->newDelay = PV_INT(pProp->GetValue());
			}
			else if(propName == "Radius")
			{
				rSnd->newTriggerRadius = PV_INT(pProp->GetValue());
			}
			
			break;
		}
		case N_MAP_IMAGE:
		{
			PQMapImagePtr mImg = dynamic_pointer_cast<PQMapImage>(*node);

			// instance image properties
			if(propName == "Row")
			{
				mImg->row = PV_INT(pProp->GetValue());
			}
			else if(propName == "Column")
			{
				mImg->col = PV_INT(pProp->GetValue());
			}
			else if(propName == "Angle")
			{
				mImg->angle = PV_FLT(pProp->GetValue());
				mImg->CalcRect();
			}
			else if(propName == "Scale")
			{
				mImg->scale = PV_FLT(pProp->GetValue());
				mImg->CalcRect();
			}
			else if(propName == "Value 1")
			{
				mImg->value1 = PV_INT(pProp->GetValue());
			}
			else if(propName == "Value 2")
			{
				mImg->value2 = PV_INT(pProp->GetValue());
			}

			UIManager.UpdateItem("preview");
			UIManager.UpdateItem("viewport");

			break;
		}
		case N_MAP_SOUND:
		{
			PQMapSoundPtr mSnd = dynamic_pointer_cast<PQMapSound>(*node);

			// sound instance properties
			if(propName == "Volume")
			{
				mSnd->volume = PV_FLT(pProp->GetValue());
			}
			else if(propName == "Loop")
			{
				mSnd->loop = PV_BOOL(pProp->GetValue());
			}
			else if(propName == "Delay")
			{
				mSnd->delay = PV_INT(pProp->GetValue());
			}
			else if(propName == "Radius")
			{
				mSnd->triggerRadius = PV_INT(pProp->GetValue());
			}
			
			UIManager.UpdateItem("viewport");

			break;
		}
	}
}

void MPropertyGrid::Redraw()
{
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
}

////////////////////////////////////////////
//   EDITOR SIDEBAR
////////////////////////////////////////////

IMPLEMENT_DYNAMIC(EditorSidebar, CDialogBar)
BEGIN_MESSAGE_MAP(EditorSidebar, CDialogBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(BTN_ADDRESOURCE, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(BTN_REMOVERESOURCE, uiButtonHandler)
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

EditorSidebar::EditorSidebar()
{

}

EditorSidebar::~EditorSidebar()
{

}

int EditorSidebar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	DWORD listFlags = WS_CHILD | WS_VISIBLE | LVS_REPORT;
	DWORD listExFlags = LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | WS_EX_CLIENTEDGE;

	// CREATE THE LIST FOR ACTIVE RESOURCES
	list.CreateEx(listExFlags, listFlags, CRect(0, 0, 0, 0), this, MY_LV_CTRL);
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	list.InsertColumn(0, "Name", 0, 90);
	list.InsertColumn(1, "Class ID", 0, 90);

	preview.GL_Create(this, CRect(0, 0, 0, 0), true, false);

	propGrid.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, CRect(0, 0, 0, 0), this, MY_PG_CTRL);

	UIManager.AddItem<MListCtrl, &MListCtrl::Update>("resource list", &list);
	UIManager.AddItem<MPropertyGrid, &MPropertyGrid::Update>("property grid", &propGrid);
	UIManager.AddItem<pGL_Screen, &pGL_Screen::Update>("preview", &preview);
	
	return 0;
}

LONG EditorSidebar::OnInitDialog(UINT wParam, LONG lParam)
{
	BOOL ret = TRUE;
	if(!HandleInitDialog(wParam, lParam))
		ret = FALSE;
	
	UpdateData(FALSE);
	
	OffsetWindows(0, 0);
	GetClientRect(rcFrame);

	propGrid.SetVSDotNetLook();
	propGrid.EnableHeaderCtrl();
	propGrid.EnableDescriptionArea();
	propGrid.SetDescriptionRows(3);
	propGrid.MarkModifiedProperties();
	
	scrollPos = 0;

	return ret;
}

void EditorSidebar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize(nType, cx, cy);

	if(!this->IsWindowVisible() || cx <= 0 || cy <= 0)
		return;

	if(cy <= rcFrame.Height())
	{
		scrollMax = rcFrame.Height() - cy;
	}
	else
	{
		scrollMax = 0;
		scrollPos = 0;
		SetScrollPos(SB_VERT, 0, FALSE);
		OffsetWindows(0, 0);
	}

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = scrollMax;
	si.nPage = scrollMax / 10;
	si.nPos = scrollPos;
	SetScrollInfo(SB_VERT, &si, TRUE);
}

void EditorSidebar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//int oldPos = GetScrollPos(SB_VERT);
	//int scrPos = scrollPos;

	switch (nSBCode)
	{
		case SB_LINEUP:
			if(scrollPos > 0)
				scrollPos--;
			break;

		case SB_LINEDOWN:
			if(scrollPos < scrollMax - 1)
				scrollPos++;
			break;

		case SB_PAGEUP:
			if(scrollPos > 2)
				scrollPos -= 2;
			break;

		case SB_PAGEDOWN:
			if(scrollPos < scrollMax - 1 - 2)
				scrollPos += 2;
			break;

		case SB_THUMBPOSITION:
			scrollPos = nPos;
			break;

		case SB_THUMBTRACK:
			scrollPos = nPos;
			break;

		default:
			return;
	}

	SetScrollPos(SB_VERT, scrollPos);
	OffsetWindows(0, -scrollPos);

	CDialogBar::OnVScroll(nSBCode, nPos, pScrollBar);
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
}

void EditorSidebar::OffsetWindows(int xPos, int yPos)
{
	HDWP hDwp = BeginDeferWindowPos(3);

	CRect rcList = GetDlgItemRect(IDC_LIST1);
	CRect rcPrev = GetDlgItemRect(IDC_PREVIEW);
	CRect rcGrid = GetDlgItemRect(IDC_PROPGRID);

	DeferWindowPos(hDwp, list.m_hWnd, 0, rcList.left + xPos, rcList.top + yPos, rcList.Width(), rcList.Height(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(hDwp, preview.m_hWnd, 0, rcPrev.left + xPos, rcPrev.top + yPos, rcPrev.Width(), rcPrev.Height(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	DeferWindowPos(hDwp, propGrid.m_hWnd, 0, rcGrid.left + xPos, rcGrid.top + yPos, rcGrid.Width(), rcGrid.Height(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	EndDeferWindowPos(hDwp);

	list.GetClientRect(&rcClient);
	list.SetColumnWidth(0, rcClient.right / 2);
	list.SetColumnWidth(1, rcClient.right - (rcClient.right / 2));
}

void EditorSidebar::uiButtonHandler(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

CRect EditorSidebar::GetDlgItemRect(int nID)
{
	CRect rc;
	GetDlgItem(nID)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	return rc;
}

void EditorSidebar::OnDestroy()
{
	list.DestroyWindow();
	preview.DestroyWindow();
	propGrid.DestroyWindow();
	CDialogBar::OnDestroy();
}

void EditorSidebar::Redraw()
{
	RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
}