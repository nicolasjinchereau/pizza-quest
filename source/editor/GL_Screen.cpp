/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "GL_Screen.h"

IMPLEMENT_DYNAMIC(GL_Screen, CWnd)
BEGIN_MESSAGE_MAP(GL_Screen, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

HGLRC GL_Screen::hGLRC = NULL;
int GL_Screen::count = 0;

GL_Screen::GL_Screen()
{
	hDC = NULL;
	hWndParent = NULL;

	rect.Set(0, 0, 0, 0);
	clientRect = false;
	canFocus = false;
}

GL_Screen::~GL_Screen()
{

}

void GL_Screen::GL_Create(CWnd *parent, CRect rect, bool drawClientRect, bool canGetFocus)
{
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);
	CreateEx(0, className, "OpenGL", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP, rect, parent, 0);

	hWndParent = parent;
	clientRect = drawClientRect;
	canFocus = canGetFocus;
}

int GL_Screen::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	GLInit(lpCreateStruct->cx, lpCreateStruct->cy);

	count++;

	return 0;
}

void GL_Screen::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0 || nType == SIZE_MINIMIZED)
		return;

	rect.w = cx;
	rect.h = cy;

	MakeCurrent();
}

void GL_Screen::OnPaint()
{
	Redraw();
	ValidateRect(NULL);
}

void GL_Screen::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	OnLMouseDown(point.x, point.y);

	CWnd::OnLButtonDown(nFlags, point);
}

void GL_Screen::OnLButtonUp(UINT nFlags, CPoint point)
{
	OnLMouseUp(point.x, point.y);
	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void GL_Screen::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	OnRMouseDown(point.x, point.y);
	
	CWnd::OnRButtonDown(nFlags, point);
}

void GL_Screen::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnRMouseUp(point.x, point.y);
	ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}


void GL_Screen::OnMouseMove(UINT nFlags, CPoint point)
{
	OnMouseMove(point.x, point.y);

	CWnd::OnMouseMove(nFlags, point);
}

void GL_Screen::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLMouseDblClick(point.x, point.y);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void GL_Screen::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	OnRMouseDblClick(point.x, point.y);

	CWnd::OnRButtonDblClk(nFlags, point);
}

void GL_Screen::OnMove(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

void GL_Screen::GLInit(int cx, int cy)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32, // bit depth
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, // z-buffer depth
		1, // stencil-buffer depth
		0, 0, 0, 0, 0, 0,
	};
	
	CDC *cDC = GetDC();
	hDC = cDC->GetSafeHdc();
	ReleaseDC(cDC);

	int fmt = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, fmt, &pfd);
	
	rect.w = cx;
	rect.h = cy;

	if(hGLRC == NULL)
	{
		hGLRC = wglCreateContext(hDC);
		
		wglMakeCurrent(hDC, hGLRC);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0,  0, cx, cy);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
		glClearStencil(0); // stencil clears to 0
	}
}

inline int GL_Screen::MakeCurrent()
{
	if(hDC == NULL || hGLRC == NULL)
		return -1;

	wglMakeCurrent(hDC, hGLRC);

	glViewport(0,  0, rect.w, rect.h);

	return 0;
}

void GL_Screen::Set2D()
{
	MakeCurrent();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (double)rect.w, (double)rect.h, 0, 0.0f, 1.0);
}

void GL_Screen::SetProjection(const Mat4<float> *matrix)
{
	MakeCurrent();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)matrix);
}

void GL_Screen::LoadTransform(Mat4<float> &transform)
{
	transform.SetIdentity();
}

void GL_Screen::Redraw()
{
	MakeCurrent();

	Clear();

	LoadTransform(transform);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)&transform);

	OnDraw();
	
	MakeCurrent();
	Flip();
}

void GL_Screen::Clear()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT |  GL_STENCIL_BUFFER_BIT);
}

void GL_Screen::Flip()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, (double)rect.w, (double)rect.h, 0, 0.0f, 1.0);

	if(clientRect)
		DrawClientRect();

	if(GetFocus() == this)
		DrawFocusRect();

	glPopMatrix();

	SwapBuffers(hDC);
}

void GL_Screen::DrawClientRect()
{
	glDisable(GL_TEXTURE_2D);
	
	Rect<float> rc(0, 0, (float)rect.w - 1.0f, (float)rect.h - 1.0f);
	rc.Offset(0.5f, 0.5f);
	
	glLineWidth(1);

	glBegin(GL_LINES);
		
		glColor3ub(160, 160, 160);
		glVertex2f(rc.x, rc.y);
		glVertex2f(rc.x + rc.w, rc.y);

		glColor3ub(255, 255, 255);
		glVertex2f(rc.x + rc.w, rc.y);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x, rc.y + rc.h);

		glColor3ub(160, 160, 160);
		glVertex2f(rc.x, rc.y + rc.h);
		glVertex2f(rc.x, rc.y);

		rc.Deflate(1.0f, 1.0f);

		glColor3ub(105, 105, 105);
		glVertex2f(rc.x, rc.y);
		glVertex2f(rc.x + rc.w, rc.y);

		glColor3ub(227, 227, 227);
		glVertex2f(rc.x + rc.w, rc.y);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x, rc.y + rc.h);

		glColor3ub(105, 105, 105);
		glVertex2f(rc.x, rc.y + rc.h);
		glVertex2f(rc.x, rc.y);
	glEnd();

}

void GL_Screen::DrawFocusRect()
{
	Rect<float> rc(0, 0, (float)rect.w - 1.0f, (float)rect.h - 1.0f);
	rc.Deflate(3.0f, 3.0f);
	rc.Offset(0.5f, 0.5f);

	glLineWidth(1);

	glBegin(GL_LINES);

		glColor3ub(255, 255, 255);

		glVertex2f(rc.x, rc.y);
		glVertex2f(rc.x + rc.w, rc.y);

		glVertex2f(rc.x + rc.w, rc.y);
		glVertex2f(rc.x + rc.w, rc.y + rc.h);

		glVertex2f(rc.x + rc.w, rc.y + rc.h);
		glVertex2f(rc.x, rc.y + rc.h);

		glVertex2f(rc.x, rc.y + rc.h);
		glVertex2f(rc.x, rc.y);
	glEnd();

}

int GL_Screen::GetWidth()
{
	return rect.w;
}

int GL_Screen::GetHeight()
{
	return rect.h;
}

Rect<int> GL_Screen::GetRect()
{
	return rect;
}

void GL_Screen::OnDestroy()
{
	// if this is the last instance of GL_Screen, destroy the rendering context
	if(count == 1)
	{
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hGLRC);
		hGLRC = NULL;
	}

	count--;

	CWnd::OnDestroy();
}

BOOL GL_Screen::OnEraseBkgnd(CDC* pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void GL_Screen::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt == 1)
		OnKeyDown(nChar);
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void GL_Screen::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt == 1)
		OnKeyUp(nChar);
	
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void GL_Screen::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt == 1)
		OnSysKeyDown(nChar);

	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void GL_Screen::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt == 1)
		OnSysKeyUp(nChar);

	CWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

int GL_Screen::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int ret = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);

	if(canFocus && GetFocus() != this)
		SetFocus();
	
	return ret;
}

void GL_Screen::OnKillFocus(CWnd* pNewWnd)
{
	Redraw();
	CWnd::OnKillFocus(pNewWnd);
}

void GL_Screen::EnableClientRect(bool enabled)
{
	clientRect = enabled;
}

void GL_Screen::EnableFocus(bool enabled)
{
	canFocus = enabled;
}

UINT GL_Screen::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}
