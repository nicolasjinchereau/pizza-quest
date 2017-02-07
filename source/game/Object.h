/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <vector>
#include <type_traits>
#include <cstdint>
#include "Physics.h"
#include "RenderQueue.h"
#include "Task.h"
#include <functional>
#include <coroutine.h>
using namespace coroutines;
using namespace std;
class Engine;
class State;

class Object : public enable_shared_from_this<Object>
{
	friend class Engine;
protected:
	weak_ptr<Object> _parent;
	vector<shared_ptr<Object>> _children;
	vector<shared_ptr<Task>> tasks;
	
public:
	unsigned char type; // only used for map loading
	DrawLayer layer; // render sorting layer
	uint32_t category; // render pass category

	Object() : type(0), layer(DrawLayer::Bottom), category(0){}
	virtual ~Object(){}
	
	virtual void Start(){}
	virtual void Update(){}
	virtual void LateUpdate(){}
	virtual void PreDrawUpdate(){}
	virtual void Draw(){}
	virtual void OnCollisionEnter(Contact contact){}
	virtual void OnCollisionExit(Contact contact){}
	
	virtual string tag() const {
		return "Object";
	}
	
	bool isRootTopState();

	shared_ptr<Object> root() {
		auto p = _parent.lock();
		return p ? p->root() : shared_from_this();
	}

	shared_ptr<Object> parent() {
		return _parent.lock();
	}

	void parent(const shared_ptr<Object> &setParent) {
		if(setParent)
		{
			setParent->AddChild(shared_from_this());
		}
		else
		{
			auto p = _parent.lock();
			if(p) p->RemoveChild(this);
		}
	}

	virtual shared_ptr<State> state() {
		auto p = _parent.lock();
		return p ? p->state() : shared_ptr<State>();
	}

	const vector<shared_ptr<Object>>& children() const {
		return _children;
	}

	template<class T>
	shared_ptr<T> as()
	{
		return static_pointer_cast<T>(shared_from_this());
	}

	template<class T>
	shared_ptr<T> AddChild(shared_ptr<T> child)
	{
		static_assert(is_convertible<T, Object>::value,
			"Only instances of \'Object\' can be added as children.");
		
		_children.push_back(child);
		child->_parent = shared_from_this();
		
		if(isRootTopState())
			child->Start();
		else
			tasks.emplace_back(new ObjectStartTask(child));

		return child;
	}

	void RemoveChild(const shared_ptr<Object> &child)
	{
		for(auto it = _children.begin();
			it != _children.end();
			++it)
		{
			if(*it == child)
			{
				child->_parent.reset();
				_children.erase(it);
				break;
			}
		}
	}

	void RemoveChild(const Object *child)
	{
		for(auto it = _children.begin();
			it != _children.end();
			++it)
		{
			if(it->get() == child)
			{
				(*it)->_parent.reset();
				_children.erase(it);
				break;
			}
		}
	}
	
	void RunTasks_R()
	{
		for(auto it = tasks.begin(); it != tasks.end(); )
		{
			if((*it)->Execute())
				++it;
			else
				it = tasks.erase(it);
		}

		for(size_t i = 0; i < _children.size(); ++i)
			_children[i]->RunTasks_R();
	}

	void Update_R()
	{
		Update();

		for(size_t i = 0; i < _children.size(); ++i)
			_children[i]->Update_R();
	}

	void LateUpdate_R()
	{
		LateUpdate();

		for(size_t i = 0; i < _children.size(); ++i)
			_children[i]->LateUpdate_R();
	}

	void PreDrawUpdate_R()
	{
		PreDrawUpdate();
		
		for(auto& c : _children)
			c->PreDrawUpdate_R();
	}

	void SubmitDrawCalls_R(uint32_t mask /* = 0xFFFFFFFF */)
	{
		if(category & mask)
			RenderQueue::Submit(this);

		for(auto& c : _children)
			c->SubmitDrawCalls_R(mask);
	}
	
	template<class FN>
	void RecursiveTransform(FN visitor)
	{
		visitor(this);
	}

	template<class FN>
	void RecursiveTransform_R(FN visitor)
	{
		visitor(this);

		for(size_t i = 0; i < children.size(); ++i)
			_children[i]->RecursiveTransform_R(visitor);
	}

	weak_ptr<Task> RunAfterUpdate(const function<void()> &fx);
	weak_ptr<Task> RunAfterDelay(const function<void()> &fx, float delay);
	weak_ptr<Task> RunCoroutine(const function<void(yield_token<float>)> &fx);
	void CancelTask(const weak_ptr<Task> &task);
};
