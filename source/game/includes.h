/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "NPng.lib")
#pragma comment (lib, "Box2D.lib")
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "LibMad.lib")
#pragma comment(lib, "coroutine.lib")
#include <glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <Windows.h>
#include "Math.h"
#include <mutex>
#include "Trace.h"
#include "Time.h"
#include <list>
#include "Path.h"
#include <string>
#include <stack>
#include <fstream>
#include "bytestream.h"
#include <coroutine.h>
using namespace std;
using namespace coroutines;
