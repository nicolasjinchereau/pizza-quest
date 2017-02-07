/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "main.h"

CMapEditor mapEditor;

BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)

	// TOOLBAR
	ON_COMMAND(IDTB_NEW, OnToolbarNew)
	ON_COMMAND(IDTB_SAVE, OnToolbarSave)
	ON_COMMAND(IDTB_OPEN, OnToolbarOpen)
	ON_COMMAND(IDTB_ADDRESOURCE, OnToolbarAddResource)
	ON_COMMAND(IDTB_STATISTICS, OnToolbarStatistics)
	ON_COMMAND(IDTB_RESOURCE_PROPERTIES, OnToolbarNodeProperties)
	ON_COMMAND(IDTB_SELECTION, OnToolbarSelection)
	ON_COMMAND(IDTB_CREATENODE, OnToolbarCreateNode)
	ON_COMMAND(IDTB_SHOWGRID, OnToolbarShowGrid)
	ON_COMMAND(IDTB_SNAPTOGRID, OnToolbarSnapToGrid)

	// FILE MENU
	ON_COMMAND(IDM_NEW, OnMenuNew)
	ON_COMMAND(IDM_OPEN, OnMenuOpen)
	ON_COMMAND(IDM_CLOSE, OnMenuClose)
	ON_COMMAND(IDM_SAVE, OnMenuSave)
	ON_COMMAND(IDM_SAVEAS, OnMenuSaveAs)
	ON_COMMAND(IDM_EXIT, OnMenuExit)

	// EDIT MENU
	ON_COMMAND(IDM_UNDO, OnMenuUndo)
	ON_COMMAND(IDM_REDO, OnMenuRedo)
	ON_COMMAND(IDM_SELECTBY, OnMenuSelectBy)
	ON_COMMAND(IDM_SELECTALL, OnMenuSelectAll)
	ON_COMMAND(IDM_DESELECTALL, OnMenuDeselectAll)
	ON_COMMAND(IDM_COLLISION, OnMenuCollision)

	// MAP
	ON_COMMAND(IDM_MAP_PROPERTIES, OnMenuProperties)
	ON_COMMAND(IDM_ADDRESOURCE, OnMenuAddResource)
	ON_COMMAND(IDM_REMOVE_UNUSED_RESOURCES, OnMenuRemoveUnusedResources)
	

	// VIEW MENU
	ON_COMMAND(IDM_STANDARDTOOLBAR, OnMenuStandardToolbar)
	ON_COMMAND(IDM_OBJECTBROWSER, OnMenuObjectBrowser)
	ON_COMMAND(IDM_SHOWGRID, OnMenuShowGrid)
	ON_COMMAND(IDM_SNAPTOGRID, OnMenuSnapToGrid)
	ON_COMMAND(IDM_STATISTICS, OnMenuStatistics)

	// HELP
	ON_COMMAND(IDM_ABOUT, OnMenuAbout)
	ON_COMMAND(IDM_HOTKEYS, OnMenuHotkeys)
	
	// UI UPDATERS
	ON_UPDATE_COMMAND_UI(IDM_STANDARDTOOLBAR, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDM_OBJECTBROWSER, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDM_SHOWGRID, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDM_SNAPTOGRID, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDM_SELECTBY, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDTB_SELECTION, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDTB_CREATENODE, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDCMB_LAYERS, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDTB_SHOWGRID, uiButtonHandler)
	ON_UPDATE_COMMAND_UI(IDTB_SNAPTOGRID, uiButtonHandler)

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)

	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_HOTKEY()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

CMainWin::CMainWin()
{
	currentNode = NULL;

	CRect desktop, window;
	GetDesktopWindow()->GetWindowRect(&desktop);
	
	window.SetRect(desktop.right / 2 - WND_CX / 2,
				   desktop.bottom / 2 - WND_CY / 2,
				   desktop.right / 2 + WND_CX / 2,
				   desktop.bottom / 2 + WND_CY / 2);

	stringstream title;
	title << "Pizza Quest Map Editor ";
	title.precision(3);
	title << showpoint << ((float)PQ_MAP_FILE_MAX_VERSION / 100.0f);

	Create(NULL, title.str().c_str(), WS_OVERLAPPEDWINDOW, window);
	
	m_bAutoMenuEnable = FALSE;
	
	CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_MAINMENU));
	this->SetMenu(&menu);
	menu.Detach();
}

CMainWin::~CMainWin()
{

}

int CMainWin::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	mainIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(mainIcon, false);

	MapEditor.SetCurrentTool(TOOL_CREATION);

	screen.RefreshCursor();

	if(CreateStatusBar() != 0)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);

	if(CreateToolBar() != 0)
		return -1;

	if(CreateSideBar() != 0)
		return -1;

	CRect rc;
	GetClientRect(&rc);

	screen.GL_Create(this, rc, true, true);
	UIManager.AddItem<nGL_Screen, &nGL_Screen::Refresh>("viewport", &screen);

	return 0;
}

void CMainWin::OnDestroy()
{
	screen.DestroyWindow();
	toolbar.DestroyWindow();
	status_bar.DestroyWindow();
	sideBar.DestroyWindow();
	DeleteObject(mainIcon);
}

void CMainWin::uiButtonHandler(CCmdUI* pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case IDM_STANDARDTOOLBAR:
		pCmdUI->SetCheck(toolbar.IsWindowVisible());
		break;
	case IDM_OBJECTBROWSER:
		pCmdUI->SetCheck(sideBar.IsWindowVisible());
		break;
	case IDM_SHOWGRID:
		pCmdUI->SetCheck(MapEditor.IsGridVisible());
		break;
	case IDM_SNAPTOGRID:
		pCmdUI->SetCheck(MapEditor.IsSnappingToGrid());
		break;
	case IDM_SELECTBY:
		pCmdUI->Enable( MapEditor.GetCurrentTool() == TOOL_SELECTION
					 && MapEditor.map->GetCurrentLayer() == LAYER_SPRITES);
		return;
	case IDTB_SELECTION:
		pCmdUI->SetCheck(MapEditor.GetCurrentTool() == TOOL_SELECTION);
		break;
	case IDTB_CREATENODE:
		pCmdUI->SetCheck(MapEditor.GetCurrentTool() == TOOL_CREATION);
		break;
	case IDTB_SHOWGRID:
		pCmdUI->SetCheck(MapEditor.IsGridVisible());
		break;
	case IDTB_SNAPTOGRID:
		pCmdUI->SetCheck(MapEditor.IsSnappingToGrid());
		break;
	}

    pCmdUI->Enable(TRUE);
}

int CMainWin::CreateToolBar()
{
	if(!toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
		return -1;
	
	toolbar.SetWindowText("Standard");
	
	toolbar.GetToolBarCtrl().SetButtonWidth(32, 32);

	toolbar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	DockControlBar(&toolbar);

	CImageList img;
	CBitmap bm;
	
	bm.LoadBitmap(IDB_TOOL_COLD);
	img.Create(32, 32, ILC_COLOR24 | ILC_MASK, 16, 5);
	img.Add(&bm, RGB(255, 0, 255));
	bm.Detach();
	toolbar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();

	bm.LoadBitmap(IDB_TOOL_HOT);
	img.Create(32, 32, ILC_COLOR24 | ILC_MASK, 16, 5);
	img.Add(&bm, RGB(255, 0, 255));
	bm.Detach();
	toolbar.GetToolBarCtrl().SetHotImageList(&img);
	img.Detach();

	toolbar.SetButtons(NULL, 18);
	toolbar.SetButtonInfo(0, IDTB_SEP, BTNS_SEP, 12);
	toolbar.SetButtonInfo(1, IDTB_NEW, BTNS_BUTTON, 0);
	toolbar.SetButtonInfo(2, IDTB_SAVE, BTNS_BUTTON, 1);
	toolbar.SetButtonInfo(3, IDTB_OPEN, BTNS_BUTTON, 2);
	toolbar.SetButtonInfo(4, IDTB_SEP, BTNS_SEP, 12);
	toolbar.SetButtonInfo(5, IDTB_ADDRESOURCE, BTNS_BUTTON, 3);
	toolbar.SetButtonInfo(6, IDTB_STATISTICS, BTNS_BUTTON, 4);
	toolbar.SetButtonInfo(7, IDTB_SEP, BTNS_SEP, 12);
	toolbar.SetButtonInfo(8, IDTB_RESOURCE_PROPERTIES, BTNS_BUTTON, 5);
	toolbar.SetButtonInfo(9, IDTB_SEP, BTNS_SEP, 12);
	toolbar.SetButtonInfo(10, IDTB_SELECTION, BTNS_BUTTON | BTNS_CHECKGROUP, 6);
	toolbar.SetButtonInfo(11, IDTB_CREATENODE, BTNS_BUTTON | BTNS_CHECKGROUP, 7);
	toolbar.SetButtonInfo(12, IDTB_SEP, BTNS_SEP, 12);
	toolbar.SetButtonInfo(13, IDCMB_LAYERSCONTAINER, BTNS_SEP, 200); // LAYERS
	toolbar.SetButtonInfo(14, IDTB_SEP, BTNS_SEP, 12);
	toolbar.SetButtonInfo(15, IDTB_SHOWGRID, BTNS_CHECK, 8);
	toolbar.SetButtonInfo(16, IDTB_SNAPTOGRID, BTNS_CHECK, 9);
	toolbar.SetButtonInfo(17, IDTB_SEP, BTNS_SEP, 12);
	
	CRect rc_item;
	toolbar.GetItemRect(1, &rc_item);
	
	toolbar.SetSizes(rc_item.Size(), CSize(32, 32));

	toolbar.GetItemRect(13, &rc_item);
	if(stLayers.Create(" Layer", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, rc_item, &toolbar, 0) == FALSE)
		return -1;

	rc_item.left += 56;
	rc_item.DeflateRect(0, 9);
	rc_item.bottom += 400;
	
	if(cmbLayers.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, rc_item, &toolbar, IDCMB_LAYERS) == FALSE)
		return -1;
	
	cmbLayers.AddString("Sprites");
	cmbLayers.AddString("Pedestian Graph");
    cmbLayers.AddString("Vehicle Graph");
	cmbLayers.AddString("Rulers");
	cmbLayers.SetCurSel(0);
	MapEditor.SetCurrentLayer(0);

	return 0;
}

int CMainWin::CreateSideBar()
{
	if(!sideBar.Create(this, IDD_DIALOGBAR, CBRS_LEFT | CBRS_SIZE_DYNAMIC /*| CBRS_GRIPPER */| CBRS_FLYBY, IDD_DIALOGBAR))
		return -1;

	sideBar.SetWindowText("Object Browser");

	sideBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&sideBar);
	return 0;
}

int CMainWin::CreateStatusBar()
{
	if(!status_bar.CreateEx(this))
		return -1;

	UINT indicators[4];
	indicators[0] = 0x5501;
	indicators[1] = 0x5502;
	indicators[2] = 0x5503;
	indicators[3] = 0x5504;
	status_bar.SetIndicators(indicators, 4);

	return 0;
}

void CMainWin::RecalcLayout(BOOL bNotify)
{
	CFrameWnd::RecalcLayout(bNotify);

	if(!this->IsWindowVisible())
		return;

/*** adjust all child windows ***/
	DWORD style;

	// get the client rectangle for the main window
	CRect rcClient;
	GetClientRect(&rcClient);
	
	if(toolbar.IsWindowVisible() && !toolbar.IsFloating())
	{
		CRect rcToolbar;
		toolbar.GetWindowRect(&rcToolbar);
		ScreenToClient(&rcToolbar);

		style = toolbar.GetBarStyle();

		if(style & CBRS_ALIGN_TOP)
			rcClient.top += rcToolbar.Height() - 2;
		else if(style & CBRS_ALIGN_BOTTOM)
			rcClient.bottom -= rcToolbar.Height() - 2;
	}

	CRect rcStatus;
	status_bar.GetWindowRect(&rcStatus);
	ScreenToClient(&rcStatus);

	rcClient.bottom -= (rcStatus.Height());
	
	if(sideBar.IsWindowVisible() && !sideBar.IsFloating())
	{
		CRect rcSideBar;
		sideBar.GetWindowRect(&rcSideBar);
		ScreenToClient(&rcSideBar);

		style = sideBar.GetBarStyle();

		if(style & CBRS_ALIGN_LEFT)
		{
			rcClient.left += rcSideBar.Width() - 4;
			sideBar.SetWindowPos(NULL, 0, 0, rcSideBar.Width(), rcClient.Height() - (rcSideBar.top - rcClient.top), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOREDRAW);
		}
		else if(style & CBRS_ALIGN_RIGHT)
		{
			rcClient.right -= rcSideBar.Width() - 4;
			sideBar.SetWindowPos(NULL, 0, 0, rcSideBar.Width(), rcClient.Height() - (rcSideBar.top - rcClient.top), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOREDRAW);
		}
	}

	screen.MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());
}

void CMainWin::Redraw()
{
	RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
}

void CMainWin::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 200;
	lpMMI->ptMinTrackSize.y = 150;

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

int CMainWin::SaveFileAs()
{
	int ret = 0;

	CFileDialog dlg_save(FALSE, "*.pqm", MapEditor.GetFilename().c_str(),
							OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_LONGNAMES |
							OFN_DONTADDTORECENT | OFN_ENABLESIZING,
							"Pizza Quest Maps (*.pqm)|*.pqm||", NULL, 0);

	if(dlg_save.DoModal() == IDOK)
	{
		if(MapEditor.SaveMap((LPCTSTR)dlg_save.GetPathName()) != 0)
			ret = -1;
	}

	return ret;
}

BOOL CMainWin::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    TOOLTIPTEXT* pTipText = (TOOLTIPTEXT*)pNMHDR;
    
    UINT nID = pNMHDR->idFrom;
	
	std::string tipText;

	if(nID != 0 && ((pTipText->uFlags & TTF_IDISHWND) == 0))
	{
		switch(nID)
		{
		case IDTB_NEW:
			tipText = "New Map";
			break;
		case IDTB_SAVE:
			tipText = "Save Map";
			break;
		case IDTB_OPEN:
			tipText = "Open Map";
			break;
		case IDTB_ADDRESOURCE:
			tipText = "Add Resource";
			break;
		case IDTB_STATISTICS:
			tipText = "Statistics";
			break;
		case IDTB_RESOURCE_PROPERTIES:
			tipText = "Collision Editor";
			break;
		case IDTB_SELECTION:
			tipText = "Selection Tool";
			break;
		case IDTB_CREATENODE:
			tipText = "Creation Tool";
			break;
		case IDTB_SHOWGRID:
			tipText = "Show Grid";
			break;
		case IDTB_SNAPTOGRID:
			tipText = "Snap To Grid";
			break;
		}

		strcpy_s(pTipText->szText, sizeof(pTipText->szText), tipText.c_str());
		*pResult = 0;
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
		return TRUE;
	}

    return FALSE;
}

/*** MENU FUNCTIONS ***/

void CMainWin::OnMenuNew()
{
	if(AskSave() == true)
	{
		MapEditor.CloseMap();
		UIManager.UpdateItem("resource list");
		UIManager.UpdateItem("preview");
		UIManager.UpdateItem("property grid");

		Redraw();
	}
}

void CMainWin::OnMenuOpen()
{
	if(AskSave())
	{
		CFileDialog dlg_open(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_LONGNAMES |
					OFN_DONTADDTORECENT | OFN_ENABLESIZING,	"Pizza Quest Maps (*.pqm)|*.pqm||", this, 0);

		if(dlg_open.DoModal() == IDOK)
		{
			MapEditor.CloseMap();

			std::string fn = dlg_open.GetPathName();
			
			if(MapEditor.LoadMap(fn) != 0)
			{
				MessageBox("Failed to open map file!", "Error", MB_OK | MB_ICONERROR);
			}

			UIManager.UpdateItem("resource list");
			UIManager.UpdateItem("preview");
			UIManager.UpdateItem("property grid");
			Redraw();
		}
	}
}

void CMainWin::OnMenuClose()
{
	if(AskSave() == true)
	{
		MapEditor.CloseMap();
		UIManager.UpdateItem("resource list");
		UIManager.UpdateItem("preview");
		UIManager.UpdateItem("property grid");
		Redraw();
	}
}

void CMainWin::OnMenuSave()
{
	if(MapEditor.SaveMap("") != 0)
	{
		OnMenuSaveAs();
	}
}

void CMainWin::OnMenuSaveAs()
{
	if(SaveFileAs() != 0)
	{
		MessageBox("Could not save map to file", "Error", MB_OK | MB_ICONERROR);
	}
}

bool CMainWin::AskSave()
{
	int ret = IDOK;

	if(MapEditor.HasChanged() == true)
	{
		std::string sSave = "Do you want to save changes to ";
		std::string mapName = MapEditor.GetFilename();
	
		if(mapName == "") mapName = "Untitled";
	
		sSave += mapName;
		sSave += "?";

		ret = MessageBox(sSave.c_str(), "Map Editor", MB_YESNOCANCEL | MB_ICONQUESTION);

		if(ret == IDYES)
		{
			if(MapEditor.SaveMap("") != 0)
			{
				if(SaveFileAs() != 0)
				{
					MessageBox("Could not save map to file", "Error", MB_OK | MB_ICONERROR);
				}
			}
		}
	}

	return (ret != IDCANCEL);
}

void CMainWin::OnMenuExit()
{
	if(AskSave() == true)
	{
		MapEditor.CloseMap();
		DestroyWindow();
	}
}

void CMainWin::OnMenuUndo()
{
	MapEditor.Undo();
}

void CMainWin::OnMenuRedo()
{
	MapEditor.Redo();
}

void CMainWin::OnMenuSelectBy()
{
	SelectByDialog sb(AfxGetApp()->GetMainWnd());
	sb.ShowDialog(MapEditor.map);

	UIManager.UpdateItem("preview");
	UIManager.UpdateItem("viewport");
	UIManager.UpdateItem("property grid");
}

void CMainWin::OnMenuSelectAll()
{
	MapEditor.SelectAll();
	screen.Redraw();
}

void CMainWin::OnMenuDeselectAll()
{
	MapEditor.DeselectAll();
	screen.Redraw();
}

void CMainWin::OnMenuAddResource()
{
	MapEditor.AddResource();
	UIManager.UpdateItem("resource list");
}

void CMainWin::OnMenuRemoveUnusedResources()
{
	MapEditor.RemoveUnusedResources();
	UIManager.UpdateItem("resource list");
}

void CMainWin::OnMenuProperties()
{
	MapPropertiesDialog mp;
	mp.ShowDialog(MapEditor.map);
}

void CMainWin::OnMenuStandardToolbar()
{
	ShowControlBar(&toolbar, !toolbar.IsWindowVisible(), false);
}

void CMainWin::OnMenuObjectBrowser()
{
	ShowControlBar(&sideBar, !sideBar.IsWindowVisible(), false);
}

void CMainWin::OnMenuShowGrid()
{
	MapEditor.ShowGrid(!MapEditor.IsGridVisible());
	screen.Redraw();
}

void CMainWin::OnMenuSnapToGrid()
{
	MapEditor.SnapToGrid(!MapEditor.IsSnappingToGrid());
	screen.Redraw();
}

void CMainWin::OnMenuCollision()
{
	int idx = sideBar.list.GetSelectionMark();

	if(idx != -1)
	{
		PQResourcePtr res = *((PQResourcePtr*)sideBar.list.GetItemData(idx));
		PQResImagePtr rImg = dynamic_pointer_cast<PQResImage>(res);

		if(rImg)
		{
			CCollisionEditor editor(AfxGetApp()->GetMainWnd());
			
			if(editor.ShowDialog(rImg) == 0)
				MapEditor.SetChanged();

			UIManager.UpdateItem("preview");
			sideBar.list.SetFocus();
		}
	}
}

void CMainWin::OnMenuStatistics()
{
	MapEditor.ShowStats();
}

void CMainWin::OnMenuAbout()
{
	stringstream str;
	float version = (float)MapEditor.GetVersion() / 100.0f;

	str << "Pizza Quest Map Editor v" << std::showpoint << std::setprecision(3) << version << endl;
	str << "Created By: Nicolas Jinchereau";

	MessageBox(str.str().c_str(), "About");
}

void CMainWin::OnMenuHotkeys()
{
	CHotkeyDialog dlg;

	dlg.DoModal();
}

/*** TOOLBAR FUNCTIONS ***/
void CMainWin::OnToolbarNew()
{
	OnMenuNew();
}

void CMainWin::OnToolbarSave()
{
	OnMenuSave();
}

void CMainWin::OnToolbarOpen()
{
	OnMenuOpen();
}

void CMainWin::OnToolbarAddResource()
{
	OnMenuAddResource();
}

void CMainWin::OnToolbarStatistics()
{
	OnMenuStatistics();
}

void CMainWin::OnToolbarNodeProperties()
{
	OnMenuCollision();
}

void CMainWin::OnToolbarSelection()
{
	MapEditor.SetCurrentTool(TOOL_SELECTION);
	sideBar.list.EnableWindow(FALSE);
	UIManager.UpdateItem("preview");
	UIManager.UpdateItem("property grid");
	UIManager.UpdateItem("viewport");
	screen.RefreshCursor();
}
void CMainWin::OnToolbarCreateNode()
{
	MapEditor.SetCurrentTool(TOOL_CREATION);
	sideBar.list.EnableWindow(TRUE);
	UIManager.UpdateItem("preview");
	UIManager.UpdateItem("property grid");
	UIManager.UpdateItem("viewport");
	screen.RefreshCursor();
}

void CMainWin::OnToolbarShowGrid()
{
	OnMenuShowGrid();
}

void CMainWin::OnToolbarSnapToGrid()
{
	OnMenuSnapToGrid();
}

/*** STANDARD WINDOW MESSAGES ***/

void CMainWin::OnPaint()
{
	screen.Redraw();
	CFrameWnd::OnPaint();
}

void CMainWin::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
}

BOOL CMainWin::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void CMainWin::OnClose()
{
	OnMenuExit();
}
