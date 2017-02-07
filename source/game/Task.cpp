/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Task.h"
#include "Object.h"

////////////////////////

InvokeTask::InvokeTask(const function<void()> &fx)
	: fx(fx)
{

}

bool InvokeTask::Execute()
{
	fx();
	return false;
}

////////////////////////

ObjectStartTask::ObjectStartTask(shared_ptr<Object> object)
	: object(object)
{

}

bool ObjectStartTask::Execute()
{
	object->Start();
	return false;
}

////////////////////////

DelayedTask::DelayedTask(const function<void()> &fx, float runAt)
	: fx(fx), runAt(runAt)
{

}

bool DelayedTask::Execute()
{
	if(Time::exactTime() < runAt)
		return true;

	fx();
	return false;
}

////////////////////////

CoroutineTask::CoroutineTask(const function<void(yield_token<float> yield)> &fx)
	: routine(fx), runAt(0)
{

}

bool CoroutineTask::Execute()
{
	if(Time::exactTime() < runAt)
	{
		return true;
	}

	if(routine())
	{
		runAt = Time::exactTime() + routine.get();
		return true;
	}

	return false;
}
