/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

struct delegate
{
	virtual ~delegate(){}
	virtual int operator()(void *data) = 0;
};

template<class T, int (T::*pMethod)(void *data)>
struct Delegate : public delegate
{
	T *obj;

	Delegate(T *object = NULL) : obj(object){}

	virtual int operator()(void *data)
	{
		return obj ? (obj->*pMethod)(data) : -1;
	}
};