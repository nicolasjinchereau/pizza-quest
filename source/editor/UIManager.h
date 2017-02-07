/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <afx.h>
#include <string>
#include <map>
#include <assert.h>
#include "delegate.h"
#include <functional>

using namespace std;

class UI_Manager
{
public:
	UI_Manager()
	{

	}

	virtual ~UI_Manager()
	{
		while(!items.empty())
		{
			delete items.begin()->second;
			items.erase(items.begin());
		}
	}

	template<class T, int (T::*pMethod)(void *data)>
	void AddItem(string name, T *object)
	{
		items.insert( pair<string, delegate*>(name, new Delegate<T, pMethod>(object)) );
	}

	template<class T>
	T *GetItem(string itemName)
	{
		map<string, delegate*>::iterator it = items.find(itemName);
		assert(it != items.end());
		Delegate<T, NULL> *dlgUpdate = (Delegate<T, NULL>*)&(*it->second);
		return dlgUpdate->obj;
	}

	int UpdateItem(string itemName)
	{
		map<string, delegate*>::iterator it = items.find(itemName);
		assert(it != items.end());
		delegate &fnUpdate = (*it->second);
		return fnUpdate(NULL);
	}

private:
	void NullFunc(void*){}

	map<string, delegate*> items;

};

extern UI_Manager UIManager;

/*
class SomeClass
{
public:
	int SomeUpdate(void *data)
	{
		return 0;
	}
};

void SomeFunction()
{
	SomeClass someClass;

	UIManager.AddItem<SomeClass, &SomeClass::SomeUpdate>("some update", &someClass);
	
	UIManager.UpdateItem("some update", NULL);

	SomeClass *cls = UIManager.GetItem<SomeClass>("some update");
}

*/