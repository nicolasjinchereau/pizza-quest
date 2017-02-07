/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "Time.h"
#include <coroutine.h>
using namespace coroutines;

class Task
{
public:
	virtual ~Task(){}
	virtual bool Execute() = 0;
};

class InvokeTask : public Task
{
	function<void()> fx;
public:
	InvokeTask(const function<void()> &fx);
	virtual bool Execute() override;
};

class Object;

class ObjectStartTask : public Task
{
	shared_ptr<Object> object;
public:
	ObjectStartTask(shared_ptr<Object> object);
	virtual bool Execute() override;
};

class DelayedTask : public Task
{
	function<void()> fx;
	float runAt;
public:
	DelayedTask(const function<void()> &fx, float runAt);
	virtual bool Execute() override;
};

class CoroutineTask : public Task
{
	coroutine<float> routine;
	float runAt;
public:
	CoroutineTask(const function<void(yield_token<float> yield)> &fx);
	virtual bool Execute() override;
};
