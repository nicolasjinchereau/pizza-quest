/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <string>
using std::string;

void Trace(string message);
void Trace(string message, string arg);
void Trace(string message, int arg);
void Trace(string message, unsigned int arg);
void Trace(string message, float arg);
