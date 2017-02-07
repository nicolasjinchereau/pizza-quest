/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_GL_SCREEN_H
#define INC_GL_SCREEN_H

#include <afxwin.h>
//#define GL3_PROTOTYPES 1
#include <gl/GL.h>
#include <gl/GLU.h>
#include "definitions.h"

class GL_Screen : public CWnd
{
	DECLARE_DYNAMIC(GL_Screen)
public:

	GL_Screen();
	virtual ~GL_Screen();

	virtual void GL_Create(CWnd *parent, CRect rect = CRect(0, 0, 0, 0), bool drawClientRect = false, bool canGetFocus = false);
	
	int MakeCurrent();
	void Set2D();
	void SetProjection(const Mat4<float> *matrix);

	int GetWidth();
	int GetHeight();

	Rect<int> GetRect();

	void EnableClientRect(bool enabled = false);
	void EnableFocus(bool enabled = false);

	void Redraw();

private:

	HDC hDC;
	
	static HGLRC hGLRC;
	static int count;

	CWnd *hWndParent;

	Rect<int> rect;
	Mat4<float> transform;
  
	bool clientRect;
	bool canFocus;

	void GLInit(int cx, int cy);
	
	afx_msg void OnPaint();
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

public:

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void OnLMouseDown(int x, int y){}
	virtual void OnLMouseUp(int x, int y){}
	virtual void OnRMouseDown(int x, int y){}
	virtual void OnRMouseUp(int x, int y){}
	virtual void OnLMouseDblClick(int x, int y){}
	virtual void OnRMouseDblClick(int x, int y){}
	virtual void OnMouseMove(int x, int y){}
	
	virtual void OnKeyDown(UINT nChar){}
	virtual void OnKeyUp(UINT nChar){}
	virtual void OnSysKeyDown(UINT nChar){}
	virtual void OnSysKeyUp(UINT nChar){}

	virtual void OnDraw(){}
	virtual void LoadTransform(Mat4<float> &transform);

	void DrawClientRect();
	void DrawFocusRect();
	void Clear();
	void Flip();

	DECLARE_MESSAGE_MAP()
};



#endif /* INC_GL_SCREEN_H */