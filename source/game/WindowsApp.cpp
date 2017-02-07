/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "WindowsApp.h"
#include "Keycodes.h"

WindowsApp::WindowsApp(const string &windowTitle, int width, int height)
{
	this->windowTitle = windowTitle;
	this->windowClassName = windowTitle + "_WindowClass";
	this->screenWidth = width;
	this->screenHeight = height;

	fullScreen = false;

	if(fullScreen == true)
		windowStyle = WS_POPUP | WS_MAXIMIZE;
	else
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	hInst = GetModuleHandle(NULL);
}

WindowsApp::~WindowsApp()
{

}

int WindowsApp::CreateAppWindow()
{
	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wcex.lpfnWndProc    = &WindowsApp::WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = sizeof(WindowsApp*);
	wcex.hInstance      = hInst;
	wcex.hIcon          = LoadIcon(hInst, IDI_APPLICATION);
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = windowClassName.c_str();
	wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	
	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Failed to register window class", "Error", MB_OK | MB_ICONERROR);
		return -1;
	}
	
	int desk_wid = GetSystemMetrics(SM_CXSCREEN);
	int desk_hgt = GetSystemMetrics(SM_CYSCREEN);

	RECT rect = {};
	rect.left = (desk_wid - screenWidth) / 2;
	rect.right = rect.left + screenWidth;
	rect.top = (desk_hgt - screenHeight) / 2;
	rect.bottom = rect.top + screenHeight;

	AdjustWindowRect(&rect, windowStyle, FALSE);

	int winWidth = rect.right - rect.left;
	int winHeight = rect.bottom - rect.top;

	hWnd = CreateWindow(windowClassName.c_str(),
						windowTitle.c_str(),
						windowStyle,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						winWidth,
						winHeight,
						NULL,
						NULL,
						hInst,
						this);

	if(!hWnd)
	{
		MessageBox(NULL, "failed to create a window", "Error", NULL);
		return -1;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return 0;
}

void WindowsApp::DestroyAppWindow()
{
	if(hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = NULL;
	}
}

LRESULT CALLBACK WindowsApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_CREATE)
	{
		CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
		WindowsApp *pApp = (WindowsApp*)cs->lpCreateParams;
		pApp->hWnd = hWnd;

		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)pApp);
	}

	WindowsApp *app = (WindowsApp*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	if(app)
		return app->m_WndProc(hWnd, msg, wParam, lParam);
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsApp::m_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		Engine::Initialize(this);
		this->OnInitialize();
		break;

	case WM_DESTROY:
		this->OnTerminate();
		Engine::Terminate();
		break;
		
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if(wParam != VK_ESCAPE)
			Engine::OnKeyDown((KeyCode)tolower(wParam));
		break;

	case WM_KEYUP:
		if(wParam == VK_ESCAPE)
			Engine::OnBackPressed();
		else
			Engine::OnKeyUp((KeyCode)tolower(wParam));
		break;

	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		Engine::OnTouchDown((float)LOWORD(lParam), (float)HIWORD(lParam), 0);
		break;

	case WM_MOUSEMOVE:
		Engine::OnTouchMove((float)LOWORD(lParam), (float)HIWORD(lParam), 0);
		ReleaseCapture();
		break;

	case WM_LBUTTONUP:
		Engine::OnTouchUp((float)LOWORD(lParam), (float)HIWORD(lParam), 0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WindowsApp::Run()
{
	CreateAppWindow();

	Time::Reset();
	
	while(true)
	{
		if(!UpdateAppWindow())
			break;

		if(!Engine::Update())
			break;
	}

	DestroyAppWindow();

	return 0;
}

bool WindowsApp::UpdateAppWindow()
{
	MSG Msg;

	while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
	{
		if(Msg.message == WM_QUIT)
			return false;

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return true;
}
