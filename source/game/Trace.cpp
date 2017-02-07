/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Trace.h"

#include <Windows.h>
#define PRINTFN(s)	   char tmp[256]; sprintf_s(tmp, "Engine: %s", s); OutputDebugStringA(tmp)
#define PRINTFNA(s, a) char tmp[256]; OutputDebugStringA("Engine: "); sprintf_s(tmp, s, a); OutputDebugStringA(tmp)

void Trace(string message)
{
	string msg = message + "\n";
	PRINTFN(msg.c_str());
}

void Trace(string message, string arg)
{
	string msg = message + ": %s\n";
	PRINTFNA(msg.c_str(), arg.c_str());
}

void Trace(string message, int arg)
{
	string msg = message + ": %i\n";
	PRINTFNA(msg.c_str(), arg);
}

void Trace(string message, unsigned int arg)
{
	string msg = message + ": %u\n";
	PRINTFNA(msg.c_str(), arg);
}

void Trace(string message, float arg)
{
	string msg = message + ": %f\n";
	PRINTFNA(msg.c_str(), arg);
}