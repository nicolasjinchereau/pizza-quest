/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Graphics.h"
#include "Shader.h"
#include "Texture.h"

Graphics::Graphics()
	: _viewPort(0, 0, 1, 1)
{
	alive = false;
	hWnd = NULL;
	hGLRC = NULL;
	hDC = NULL;
	_width = 0;
	_height = 0;
}

int Graphics::Initialize(HWND hWnd)
{
	try
	{
		that->hWnd = hWnd;
		that->hDC = GetDC(that->hWnd);

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32, // bit depth
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			24, // z-buffer depth
			0, // stencil-buffer depth
			0, 0, 0, 0, 0, 0,
		};

		int pixelFormat = ChoosePixelFormat(that->hDC, &pfd);
		SetPixelFormat(that->hDC, pixelFormat, &pfd);
		
		that->hGLRC = wglCreateContext(that->hDC);
		if(that->hGLRC == NULL)
			throw "couldn't create an OpenGL context";

		wglMakeCurrent(that->hDC, that->hGLRC);
		
		Trace( "opengl version:", (const char*)glGetString(GL_VERSION) );

		ReleaseDC(that->hWnd, that->hDC);

		if(glewInit() != GLEW_OK)
			throw "couldn't initialize OpenGL extensions";

		RECT rect;
		GetClientRect(that->hWnd, &rect);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		SetViewport(width, height);
	}
	catch(const char *error)
	{
		MessageBox(NULL, error, "Error", MB_OK | MB_ICONERROR);
		Trace( error, GetError() );
		return 0;
	}

	glDisable(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DITHER);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	that->_defaultShader = make_shared<Shader>();
	that->_defaultShader->Load("assets\\Shaders\\default.vert",
							   "assets\\Shaders\\default.frag");

	that->alive = true;

	return 1;
}

void Graphics::SetViewport(int vpWidth, int vpHeight)
{
	that->_width = vpWidth;
	that->_height = vpHeight;
	glViewport(0, 0, that->_width, that->_height);
	that->_viewPort.Set(0, 0, (float)that->_width, (float)that->_height);
}

void Graphics::Destroy()
{
	if(that->alive)
	{
		that->_defaultShader.reset();

		if(that->hGLRC)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(that->hGLRC);
		}

		that->hWnd = NULL;
		that->hGLRC = NULL;
		that->hDC = NULL;

		that->alive = false;
	}
}

bool Graphics::IsAlive()
{
	return that->alive;
}

int Graphics::width()
{
	return that->_width;
}

int Graphics::height()
{
	return that->_height;
}

Rect Graphics::viewport()
{
	return that->_viewPort;
}

void Graphics::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::Flip()
{
	SwapBuffers(that->hDC);
}

string Graphics::GetError()
{
	GLenum error = glGetError();

	switch(error)
	{
	case GL_NO_ERROR:
		return "GL_NO_ERROR";
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:
		return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:
		return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	}

	return "";
}

shared_ptr<Shader> Graphics::defaultShader()
{
	return that->_defaultShader;
}

void Graphics::DrawArray(uint32_t start, uint32_t count, DrawMode mode)
{
	GLenum modes[] =
	{
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP
	};

	auto f = glDrawArrays;
	glDrawArrays(modes[(int)mode], start, count);
}

void Graphics::DrawIndexed(uint32_t start, uint32_t count, DrawMode mode)
{
	GLenum modes[] =
	{
		GL_POINTS,
		GL_LINES,
		GL_LINE_STRIP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP
	};

	glDrawElements(modes[(int)mode], count, GL_UNSIGNED_INT, nullptr);
}
