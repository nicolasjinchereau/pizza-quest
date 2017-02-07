/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <functional>
#include <stack>
#include <memory>
#include <vector>
#include "Singleton.h"
#include "Keycodes.h"
#include "Task.h"

using namespace std;

class State;
class Camera;
class Shader;
class WindowsApp;

class Engine : public Singleton<Engine>
{
	void _setState(shared_ptr<State> state);
	void _pushState(shared_ptr<State> state);
	void _popState();
	bool _update();

	static bool Update();
	static void OnTouchDown(float x, float y, int id);
	static void OnTouchMove(float x, float y, int id);
	static void OnTouchUp(float x, float y, int id);
	static void OnKeyDown(KeyCode keyCode);
	static void OnKeyUp(KeyCode keyCode);
	static void OnBackPressed();

public:
	stack<shared_ptr<State>> states;
	vector<unique_ptr<Task>> tasks;
	WindowsApp *pApp;
	bool quit;
	
	Engine();
	~Engine();

	static void Initialize(WindowsApp *pApp);
	static void Terminate();
	static void SetState(shared_ptr<State> state);
	static void PushState(shared_ptr<State> state);
	static void PopState();
	
	static void QuitGame();
	static void RunAfterUpdate(const function<void()> &fx);
	static void RunAfterDelay(const function<void()> &fx, float delay);
	static void RunCoroutine(const function<void(yield_token<float>)> &fx);

	static shared_ptr<State> GetState() {
		return that->states.top();
	}

	template<class T>
	static shared_ptr<T> GetState() {
		return static_pointer_cast<T>(that->states.top());
	}

	friend class WindowsApp;
	friend class Wave;
	friend class Sound;
	friend class Stream;
	friend class Sprite;
	friend class PQGame;
	friend class PQGameLoader;
};
