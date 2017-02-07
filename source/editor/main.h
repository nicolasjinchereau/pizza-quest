/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "definitions.h"
#include "GL_Screen.h"
#include "GL_Sprite.h"
#include "resource.h"
#include "vecmath.h"
#include "GL_Draw.h"
#include "PQMapEditor.h"
#include "EditorSidebar.h"
#include "keycodes.h"
#include <sstream>
#include "CHotkeyDialog.h"
#include "UIManager.h"
#include "History.h"
#include "SelectByDialog.h"
#include "MapPropertiesDialog.h"

#define WND_CX 800
#define WND_CY 650

#define IDTB_SEP					0x1100
#define IDTB_NEW					0x1101
#define IDTB_SAVE					0x1102
#define IDTB_OPEN					0x1103
#define IDTB_ADDRESOURCE			0x1104
#define IDTB_STATISTICS				0x1105
#define IDTB_RESOURCE_PROPERTIES	0x1106
#define IDTB_SELECTION				0x1107
#define IDTB_CREATENODE				0x1108
#define IDCMB_LAYERSCONTAINER		0x1109
#define IDCMB_LAYERS				0x1110
#define IDTB_SHOWGRID				0x1111
#define IDTB_SNAPTOGRID				0x1112

class nGL_Screen : public GL_Screen
{
public:
	nGL_Screen()
	{
		hCurCreate = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
		hCurSelect = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
		hCurGrabOpen = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_GRABOPEN));
		hCurGrabClosed = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_GRABCLOSED));
	}
	
	~nGL_Screen()
	{
		DeleteObject(hCurCreate);
		// don't delete system cursor hCurSelect
	}

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		if(pWnd == this)
		{
			if(MapEditor.IsAltDown() && !MapEditor.IsScrolling())
			{
				SetCursor(hCurGrabOpen);
			}
			else if(MapEditor.IsScrolling())
			{
				SetCursor(hCurGrabClosed);
			}
			else
			{
				if(MapEditor.GetCurrentTool() == TOOL_CREATION)
					SetCursor(hCurCreate);
				else
					SetCursor(hCurSelect);
			}
		}
		else
		{
			SetCursor(hCurSelect);
		}
		
		return TRUE;

		//return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}

	void RefreshCursor()
	{
		OnSetCursor(this, 0, 0);
	}

	virtual void OnLMouseDown(int x, int y)
	{
		// if changes where already made by clicking, do a snapshot
		MapEditor.MouseDown(x, y);

		if(!MapEditor.IsAltDown() && !MapEditor.IsControlDown())
		{
			UIManager.UpdateItem("property grid");
			UIManager.UpdateItem("preview");
		}

		RefreshCursor();
		Redraw();
	}
	virtual void OnLMouseUp(int x, int y)
	{
		MapEditor.MouseUp(x, y, true);
		RefreshCursor();
		UIManager.UpdateItem("property grid");
		UIManager.UpdateItem("preview");
		Redraw();
	}
	virtual void OnRMouseDown(int x, int y)
	{
		MapEditor.MouseUp(x, y, false);
		RefreshCursor();
		Redraw();
	}
	virtual void OnRMouseUp(int x, int y)
	{

	}
	virtual void OnRMouseDblClick(int x, int y)
	{
		
	}
	virtual void OnLMouseDblClick(int x, int y)
	{

	}
	virtual void OnMouseMove(int x, int y)
	{
		if(MapEditor.MouseMove(x, y))
			Redraw();
	}
	
	virtual void LoadTransform(Mat4<float> &transform)
	{
		transform.SetIdentity();
	}

	void OnDraw()
	{
		MapEditor.Draw(this);
	}

	int Refresh(void *n)
	{
		return RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_ALLCHILDREN);
	}

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		ScreenToClient(&pt);
		const float zoomIncrement = 0.1f;

		float delta = ((float)zDelta / (float)WHEEL_DELTA) * zoomIncrement;

		MapEditor.Zoom(pt.x, pt.y, delta);
		UIManager.UpdateItem("viewport");
		
		return TRUE;
	}

	void OnSize(UINT nType, int cx, int cy)
	{
		GL_Screen::OnSize(nType, cx, cy);
		MapEditor.SetViewport(cx, cy);
	}

	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (GL_Screen::OnCreate(lpCreateStruct) == -1)
			return -1;

		this->DragAcceptFiles(TRUE);

		return 0;
	}

private:

	HCURSOR hCurCreate;
	HCURSOR hCurSelect;
	HCURSOR hCurGrabOpen;
	HCURSOR hCurGrabClosed;

public:

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(nGL_Screen, GL_Screen)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

class MToolBar : public CToolBar
{
public:
	afx_msg void OnClose()
	{
		ShowWindow(SW_HIDE);
	}
	
	bool IsButtonChecked(int btnID)
	{
		return (SendMessage(TB_ISBUTTONCHECKED, btnID, 0) != 0);
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(MToolBar, CToolBar)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

#define IDHKEY_NEW 0x870
#define IDHKEY_SAVE 0x871
#define IDHKEY_OPEN 0x872

class MComboBox : public CComboBox
{
public:
	afx_msg void OnCbnSelchange()
	{
		int sel = GetCurSel();
		MapEditor.SetCurrentLayer( sel );
		UIManager.UpdateItem("viewport");
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(MComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &MComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

class CMainWin : public CFrameWnd
{
public:
	CMainWin();
	~CMainWin();
	
	// window controls
	MToolBar toolbar;
	CStatic stLayers;
	MComboBox cmbLayers;
	CStatusBar status_bar;
	nGL_Screen screen;
	EditorSidebar sideBar;

	HICON mainIcon;

	// window
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// TOOLBAR
	afx_msg void OnToolbarNew();
	afx_msg void OnToolbarSave();
	afx_msg void OnToolbarOpen();
	afx_msg void OnToolbarAddResource();
	afx_msg void OnToolbarStatistics();
	afx_msg void OnToolbarNodeProperties();
	afx_msg void OnToolbarSelection();
	afx_msg void OnToolbarCreateNode();
	afx_msg void OnToolbarShowGrid();
	afx_msg void OnToolbarSnapToGrid();

	// FILE MENU
	afx_msg void OnMenuNew();
	afx_msg void OnMenuOpen();
	afx_msg void OnMenuClose();
	afx_msg void OnMenuSave();
	afx_msg void OnMenuSaveAs();
	afx_msg void OnMenuExit();

	// EDIT MENU
	afx_msg void OnMenuUndo();
	afx_msg void OnMenuRedo();
	afx_msg void OnMenuSelectBy();
	afx_msg void OnMenuSelectAll();
	afx_msg void OnMenuDeselectAll();
	afx_msg void OnMenuCollision();

	// MAP MENU
	afx_msg void OnMenuProperties();
	afx_msg void OnMenuAddResource();
	afx_msg void OnMenuRemoveUnusedResources();

	// VIEW MENU
	afx_msg void OnMenuStandardToolbar();
	afx_msg void OnMenuObjectBrowser();
	afx_msg void OnMenuShowGrid();
	afx_msg void OnMenuSnapToGrid();
	afx_msg void OnMenuStatistics();

	// HELP
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuHotkeys();

	// toolbar tooltips
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	void uiButtonHandler(CCmdUI* pCmdUI);

	// class
	int CreateToolBar();
	int CreateSideBar();
	int CreateStatusBar();

	void Redraw();
	bool AskSave();
	int SaveFileAs();

	virtual void RecalcLayout(BOOL bNotify = TRUE);

private:
	
	PQMapNode *currentNode;

public:

	DECLARE_MESSAGE_MAP()
};

class CMapEditor : public CWinApp
{
public:
	CMainWin *pMainWindow;

	bool shiftDown;
	bool altDown;
	bool controlDown;

	BOOL InitInstance()
	{
		shiftDown = false;
		altDown = false;
		controlDown = false;

		InitCommonControls();
		
		pMainWindow = new CMainWin;

		m_pMainWnd = pMainWindow;
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
		
		return TRUE;
	}
	
	virtual int CMapEditor::ExitInstance()
	{
		return CWinApp::ExitInstance();
	}
	
	// HANDLE HOTKEYS
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
		{
			
			MListCtrl *list = UIManager.GetItem<MListCtrl>("resource list");
			GL_Screen *scrn = UIManager.GetItem<GL_Screen>("viewport");
			MPropertyGrid *pgrid = UIManager.GetItem<MPropertyGrid>("property grid");
			CWnd *focus = pMainWindow->GetFocus();

			unsigned char key = (unsigned char)pMsg->wParam;

			// GLOBAL KEYS
			switch(key)
			{
			case VK_SHIFT:
				shiftDown = true;
				MapEditor.ShiftKey(true);
				pMainWindow->screen.RefreshCursor();
				break;
			case VK_CONTROL:
				controlDown = true;
				MapEditor.ControlKey(true);
				pMainWindow->screen.RefreshCursor();
				break;
			case VK_MENU: // ALT
				altDown = true;
				MapEditor.AltKey(true);
				pMainWindow->screen.RefreshCursor();
				break;
			}

			// VIEWPORT KEYS
			if(focus == scrn)
			{
				if(controlDown)
				{
					switch(key)
					{
					case VK_OEM_PLUS:
					case VK_ADD:
						MapEditor.ScaleSelectionUp();
						UIManager.UpdateItem("viewport");
						UIManager.UpdateItem("property grid");
						return TRUE;

					case VK_OEM_MINUS:
					case VK_SUBTRACT:
						MapEditor.ScaleSelectionDown();
						UIManager.UpdateItem("viewport");
						UIManager.UpdateItem("property grid");
						return TRUE;

					case VK_Z:
						if( !shiftDown ) // UNDO
						{
							pMainWindow->OnMenuUndo();
							UIManager.UpdateItem("viewport");
							UIManager.UpdateItem("resource list");
							UIManager.UpdateItem("preview");
							UIManager.UpdateItem("property grid");
							return TRUE;
						}
						else // REDO
						{
							pMainWindow->OnMenuRedo();
							UIManager.UpdateItem("viewport");
							UIManager.UpdateItem("resource list");
							UIManager.UpdateItem("preview");
							UIManager.UpdateItem("property grid");
							return TRUE;
						}
						break;

					case VK_Y: // REDO
						pMainWindow->OnMenuRedo();
						UIManager.UpdateItem("viewport");
						UIManager.UpdateItem("resource list");
						UIManager.UpdateItem("preview");
						UIManager.UpdateItem("property grid");
						return TRUE;
					case VK_C:
						MapEditor.ConnectSelectedNodes();
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_UP:
						if(shiftDown == true)
							MapEditor.SelBringToFront();
						else
							MapEditor.SelBringForward();
						
						pMainWindow->screen.Redraw();
						return TRUE;
						
					case VK_DOWN:
						if(shiftDown == true)
							MapEditor.SelSendToBack();
						else
							MapEditor.SelSendBack();

						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_A:
						MapEditor.SelectAll();
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_D:
						MapEditor.DeselectAll();
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_G:
						MapEditor.ShowGrid(!MapEditor.IsGridVisible());
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_BACK:
						MapEditor.BreakAllConnections();
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_P:
						MapEditor.ToggleDestination();
						pMainWindow->screen.Redraw();
						return TRUE;
					}
				}
				else
				{
					switch(key)
					{
					case VK_UP:
						MapEditor.MoveSelection(0, -1);
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_DOWN:
						MapEditor.MoveSelection(0, 1);
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_G:
						MapEditor.SnapToGrid(!MapEditor.IsSnappingToGrid());
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_DELETE:
						MapEditor.DeleteSelection();
						UIManager.UpdateItem("preview");
						UIManager.UpdateItem("resource list");
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_LEFT:
						MapEditor.MoveSelection(-1, 0);
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_RIGHT:
						MapEditor.MoveSelection(1, 0);
						pMainWindow->screen.Redraw();
						return TRUE;

					case VK_S:
						MapEditor.SetCurrentTool(TOOL_CREATION);
						pMainWindow->screen.RefreshCursor();
						return TRUE;

					case VK_V:
						MapEditor.SetCurrentTool(TOOL_SELECTION);
						pMainWindow->screen.RefreshCursor();
						return TRUE;

					case VK_BACK:
						MapEditor.BreakSelectedPairs();
						pMainWindow->screen.Redraw();
						return TRUE;
					}
				}
			}
			else if(focus == list) // LIST KEYS
			{
				switch(key)
				{
				case VK_DELETE:
					int idx = list->GetSelectionMark();
					if(idx != -1)
					{
						PQResourcePtr res = *((PQResourcePtr*)list->GetItemData(idx));
						
						if(MapEditor.RemoveResource(res) == true)
						{
							UIManager.UpdateItem("resource list");
							UIManager.UpdateItem("preview");
							UIManager.UpdateItem("viewport");
						}
					}
					return TRUE;
				}
			}

			return CWinApp::PreTranslateMessage(pMsg);
		}
		else if(pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
		{
			MListCtrl *list = UIManager.GetItem<MListCtrl>("resource list");
			GL_Screen *scrn = UIManager.GetItem<GL_Screen>("viewport");
			MPropertyGrid *pgrid = UIManager.GetItem<MPropertyGrid>("property grid");
			CWnd *focus = pMainWindow->GetFocus();

			unsigned char key = (unsigned char)pMsg->wParam;

			// GLOBAL KEYS
			switch(key)
			{
			case VK_SHIFT:
				shiftDown = false;
				MapEditor.ShiftKey(false);
				pMainWindow->screen.RefreshCursor();
				break;
			case VK_CONTROL:
				controlDown = false;
				MapEditor.ControlKey(false);
				pMainWindow->screen.RefreshCursor();
				break;
			case VK_MENU:
				altDown = false;
				MapEditor.AltKey(false);
				pMainWindow->screen.RefreshCursor();
				break;
			}

			
			if(controlDown)
			{
				switch(key)
				{
				case VK_N: // NEW
					pMainWindow->OnMenuNew();
					return TRUE;

				case VK_O: // OPEN
					pMainWindow->OnMenuOpen();
					return TRUE;

				case VK_S: // SAVE
					pMainWindow->OnMenuSave();
					return TRUE;
				}
			}

		} // if(keyup)

		return CWinApp::PreTranslateMessage(pMsg);
	} // PreTranslateMessage
};

void nGL_Screen::OnDropFiles(HDROP hDropInfo)
{
	CMapEditor *cApp = (CMapEditor*)AfxGetApp();
	
	assert(cApp);

	if(cApp->pMainWindow->AskSave())
	{
		char buffer[MAX_PATH];
		string extension;
		Filename fn;

		int nFiles = (int)DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		
		if(nFiles > 0)
		{
			DragQueryFile(hDropInfo, 0, buffer, MAX_PATH);

			fn = buffer;
			extension = fn.GetExtension();

			// skip invalid files
			if(extension == "pqm")
			{
				MapEditor.CloseMap();
			
				if(MapEditor.LoadMap(fn) != 0)
					MessageBox("Failed to open map file!", "Error", MB_OK | MB_ICONERROR);

				UIManager.UpdateItem("resource list");
				UIManager.UpdateItem("preview");
				UIManager.UpdateItem("property grid");
				UIManager.UpdateItem("viewport");

				Redraw();
			}
		}
	}

	DragFinish(hDropInfo);
}