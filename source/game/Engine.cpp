/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Engine.h"
#include "Object.h"
#include "State.h"
#include "Camera.h"
#include "Shader.h"
#include "Graphics.h"
#include "Audio.h"
#include "WindowsApp.h"
#include <time.h>
#include "RenderQueue.h"

Engine::Engine()
{
	quit = false;
	pApp = nullptr;
	srand((unsigned int)time(NULL));
}

Engine::~Engine()
{
	
}

void Engine::Initialize(WindowsApp *pApp)
{
	that->pApp = pApp;
	Audio::Initialize();
	Graphics::Initialize(pApp->hWnd);
}

void Engine::Terminate()
{
	that->tasks.clear();
	
	while(!that->states.empty())
		that->states.pop();

	Graphics::Destroy();
	Audio::Terminate();

	that->quit = false;
	that->pApp = nullptr;
}

bool Engine::Update()
{
	return that->_update();
}

bool Engine::_update()
{
	Time::Step();

	for(auto it = tasks.begin(); it != tasks.end(); )
	{
		if((*it)->Execute())
			++it;
		else
			it = tasks.erase(it);
	}

	if(states.empty())
		quit = true;

	if(quit) return false;

	auto& topState = states.top();

	topState->RunTasks_R();
	topState->Update_R();
	topState->physics->Update();
	topState->LateUpdate_R();
	topState->PreDrawUpdate_R();

	if(quit) return false;

	Graphics::Clear();
	topState->StateDraw();
	Graphics::Flip();
	
	return true;
}

void Engine::_setState(shared_ptr<State> state)
{
	while(!states.empty())
		states.pop();

	states.push(state);
	state->Start();
}

void Engine::_pushState(shared_ptr<State> state)
{
	states.push(state);
	state->Start();
}

void Engine::_popState()
{
	if(!states.empty())
		states.pop();
}

void Engine::SetState(shared_ptr<State> state)
{
	RunAfterUpdate( [state]{ that->_setState(state); } );
}

void Engine::PushState(shared_ptr<State> state)
{
	RunAfterUpdate( [state]{ that->_pushState(state); } );
}

void Engine::PopState()
{
	RunAfterUpdate( []{ that->_popState(); } );
}

void Engine::QuitGame()
{
	that->quit =  true;
}

void Engine::RunAfterUpdate(const function<void()> &fx)
{
	that->tasks.emplace_back(new InvokeTask(fx));
}

void Engine::RunAfterDelay(const function<void()> &fx, float delay)
{
	that->tasks.emplace_back(new DelayedTask(fx, Time::exactTime() + delay));
}

void Engine::RunCoroutine(const function<void(yield_token<float>)> &fx)
{
	unique_ptr<Task> task(new CoroutineTask(fx));

	if(task->Execute())
		that->tasks.emplace_back(move(task));
}

void Engine::OnTouchDown(float x, float y, int id)
{
	if( !that->states.empty() )
		that->states.top()->OnTouchDown(x, y, id);
}

void Engine::OnTouchMove(float x, float y, int id)
{
	if( !that->states.empty() )
		that->states.top()->OnTouchMove(x, y, id);
}

void Engine::OnTouchUp(float x, float y, int id)
{
	if( !that->states.empty() )
		that->states.top()->OnTouchUp(x, y, id);
}

void Engine::OnKeyDown(KeyCode keyCode)
{
	if( !that->states.empty() )
		that->states.top()->OnKeyDown(keyCode);
}

void Engine::OnKeyUp(KeyCode keyCode)
{
	if( !that->states.empty() )
		that->states.top()->OnKeyUp(keyCode);
}

void Engine::OnBackPressed()
{
	if( !that->states.empty() )
		that->states.top()->OnBackPressed();
}
