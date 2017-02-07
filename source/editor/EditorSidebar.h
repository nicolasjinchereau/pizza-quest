/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "resource.h"
#include "afxdialogex.h"
#include <afxext.h>
#include "GL_Screen.h"
#include "PQMapEditor.h"
#include "propertygrid.h"
#include "CollisionEditor.h"
#include "UIManager.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "PQString.h"

#define BTN_ADDRESOURCE 0x5610
#define BTN_REMOVERESOURCE 0x5611

template<class T>
class NumStr
{
public:
	NumStr(){}
	NumStr(T n) {var = n;}

	NumStr &operator=(T n){var = n;	return *this;}
	operator const char*(){return GetString();}
	operator COleVariant(){return COleVariant(GetString());}
	NumStr &in(T n){var = n; return *this;}

private:
	const char *GetString()
	{
		s.str("");
		s.clear();
		if(typeid(T) == typeid(float)
		|| typeid(T) == typeid(double))
			s << std::showpoint << std::setprecision(3) << var;
		else if(typeid(T) == typeid(bool))
			s << std::boolalpha << var;
		else
			s << var;

		s >> str;
		return str.c_str();
	}

	T var;
	std::stringstream s;
	std::string str;
};

////////////////////////////////////////////
//   PREVIEW
////////////////////////////////////////////

class pGL_Screen : public GL_Screen
{
public:
	pGL_Screen(){}

	virtual void OnDraw();
	int Update(void *data);

private:
	PQResourcePtr resPtr;
	PQMapNodePtr nodePtr;
};

////////////////////////////////////////////
//   RESOURCE LIST
////////////////////////////////////////////

class MListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(MListCtrl)

public:

	MListCtrl()
	{
		
	}

	int Update(void *data);

	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CListCtrl::OnCreate(lpCreateStruct) == -1)
			return -1;

		this->DragAcceptFiles(TRUE);

		return 0;
	}

	afx_msg void OnDropFiles(HDROP hDropInfo)
	{
		char buffer[MAX_PATH];
		string extension;
		Filename fn;

		int nFiles = (int)DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		
		if(nFiles > 0)
		{
			for(int f = 0; f < nFiles; f++)
			{
				if(MapEditor.map->resources.size() >= MAX_RESOURCES)
				{
					MessageBox(0, "Can't add any more resources. Please delete some before trying to add more.", MB_OK | MB_ICONERROR);
					break;
				}

				DragQueryFile(hDropInfo, f, buffer, MAX_PATH);

				fn = buffer;
				extension = fn.GetExtension();

				// skip invalid files
				if(extension != "png" && extension != "wav")
					continue;

				ResourceDialog rc(AfxGetApp()->GetMainWnd());
				PQResourcePtr res = rc.ShowDialog(&MapEditor.map->resources, buffer);

				if( !res )
					break; // break out if the user presses cancel
				
				// add resource to map
				MapEditor.map->AddResource(res);

				// update the resource list
				UIManager.UpdateItem("resource list");
			}
		}
		
		DragFinish(hDropInfo);
	}

	afx_msg void OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};

////////////////////////////////////////////
//   PROPERTY GRID
////////////////////////////////////////////

class MPropertyGrid : public CMFCPropertyGridCtrl
{
public:

	int Update(void *data);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
	void Redraw();
private:
	PQNodePtr nodePtr;
};

////////////////////////////////////////////
//   EDITOR SIDEBAR
////////////////////////////////////////////

#define MY_LV_CTRL 0x9456
#define MY_PG_CTRL 0x9457

class EditorSidebar : public CDialogBar
{
	DECLARE_DYNAMIC(EditorSidebar)
public:
	
	EditorSidebar();
	virtual ~EditorSidebar();

	pGL_Screen preview;		// resource preview window
	MListCtrl list;			// resources
	MPropertyGrid propGrid; // properties for the current resource

	CRect GetDlgItemRect(int nID);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	void uiButtonHandler(CCmdUI* pCmdUI);

	void Redraw();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OffsetWindows(int xPos = 0, int yPos = 0);

	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	
	CRect rcClient;

	CRect rcFrame;
	int scrollPos;
	int scrollMax;
};
