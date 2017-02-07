/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "includes.h"
#include "Singleton.h"
#include "Texture.h"
#include "Shader.h"
#include "DrawBuffer.h"
#include "RenderQueue.h"

enum class DrawMode
{
	Points,
	Lines,
	LineStrip,
	Triangles,
	TriangleStrip
};

class Graphics : public Singleton<Graphics>
{
public:
	Graphics();
	
	static int Initialize(HWND window);
	static void Destroy();
	static bool IsAlive();
	static void Clear();
	static void Flip();
	static void SetViewport(int vpWidth, int vpHeight);
	static int width();
	static int height();
	static Rect viewport();
	static shared_ptr<Shader> defaultShader();
	static string GetError();
	static void DrawArray(uint32_t start, uint32_t count, DrawMode mode);
	static void DrawIndexed(uint32_t start, uint32_t count, DrawMode mode);
private:
	Rect _viewPort;
	bool alive;
	HGLRC hGLRC;
	HWND hWnd;
	HDC hDC;
	int _width;
	int _height;
	shared_ptr<Shader> _defaultShader;
};
