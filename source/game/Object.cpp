/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Object.h"
#include "State.h"
#include "Engine.h"

bool Object::isRootTopState()
{
	return root() == Engine::GetState();
}

weak_ptr<Task> Object::RunAfterUpdate(const function<void()> &fx)
{
	auto ret = make_shared<InvokeTask>(fx);
	tasks.push_back(ret);
	return ret;
}

weak_ptr<Task> Object::RunAfterDelay(const function<void()> &fx, float delay)
{
	auto ret = make_shared<DelayedTask>(fx, Time::exactTime() + delay);
	tasks.push_back(ret);
	return ret;
}

weak_ptr<Task> Object::RunCoroutine(const function<void(yield_token<float>)> &fx)
{
	shared_ptr<Task> ret = make_shared<CoroutineTask>(fx);

	if(ret->Execute())
		tasks.push_back(ret);
	else
		ret.reset();

	return ret;
}

void Object::CancelTask(const weak_ptr<Task> &stub)
{
	auto it = std::find(tasks.begin(), tasks.end(), stub.lock());

	if(it != tasks.end())
		tasks.erase(it);
}
