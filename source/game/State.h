/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <stdlib.h>
#include <vector>
#include <memory>
#include "Keycodes.h"
#include "Trace.h"
#include "Object.h"
#include "RenderQueue.h"
#include "Physics.h"

using namespace std;

class State : public Object
{
public:
	shared_ptr<Physics> physics;

	State() : physics(make_shared<Physics>()) {}
	virtual ~State(){}

	virtual void StateDraw()
	{
		RenderQueue::Clear();
		
		SubmitDrawCalls_R(0xFFFFFFFF);

		RenderQueue::Sort();
		RenderQueue::Execute();
	}

	virtual void OnTouchDown(float x, float y, int id){}
	virtual void OnTouchMove(float x, float y, int id){}
	virtual void OnTouchUp(float x, float y, int id){}
	virtual void OnKeyDown(KeyCode keyCode){}
	virtual void OnKeyUp(KeyCode keyCode){}
	virtual void OnBackPressed(){}
	
	virtual shared_ptr<State> state() {
		return static_pointer_cast<State>(shared_from_this());
	}
};
