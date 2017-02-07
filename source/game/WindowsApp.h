/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <string>
#include "includes.h"
#include "bytestream.h"
#include "Engine.h"
#include "Graphics.h"
using namespace std;

class WindowsApp
{
public:
	HINSTANCE hInst;
	HWND hWnd;
	
	// cannot be changed after the game has started
	bool fullScreen;

	string windowTitle;
	string windowClassName;
	int screenWidth;
	int screenHeight;
	int windowStyle;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT m_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	WindowsApp(const string &windowTitle, int width, int height);
	~WindowsApp();
    
	int CreateAppWindow();
	void DestroyAppWindow();
	bool UpdateAppWindow();

	int Run();

	virtual void OnInitialize() = 0;
	virtual void OnTerminate() = 0;
};
