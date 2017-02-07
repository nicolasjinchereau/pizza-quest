/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Singleton.h"
#include "EnumBitmask.h"
#include <vector>
#include <algorithm>
using namespace std;

class Object;

enum class DrawLayer : int
{
	Bottom = 0,
	Background = 1000,
	Tiles = 2000,
	Characters = 3000,
	Cars = 4000,
	Props = 5000,
	Structures = 6000,
	Overlay = 7000,
	UserInterface = 8000,
	Top = 9000
};

ENUM_BITMASK(DrawLayer)

class RenderQueue : public Singleton<RenderQueue>
{
	vector<Object*> _queue;
public:
	static void Submit(Object *pObject);
	static void Clear();
	static void Sort();
	static void Execute();
};
