/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include "fcontext.h"
#include <cstdlib>
#include <memory>
#include <functional>
#include <cassert>
#include <chrono>
#include <vector>
using namespace std;

namespace coroutines
{

template<class T>
class yield_token
{
	template<class TT>
	friend class coroutine;

	coroutine<T> *_c;
	yield_token(coroutine<T> *c) : _c(c){}

public:
	yield_token() : _c(nullptr) {}
	void operator()(const T& return_value);
};

template<>
class yield_token<void>
{
	template<class TT>
	friend class coroutine;

	coroutine<void> *_c;
	yield_token(coroutine<void> *c) : _c(c){}

public:
	yield_token() : _c(nullptr) {}
	void operator()();
};

template<class T, class CORO>
class basic_coroutine
{
public:
	typedef function<void(yield_token<T> yield)> entry_point_t;
	const static size_t default_stack_size = 64U * 1024U;

protected:
	basic_coroutine(const basic_coroutine &other){}
	basic_coroutine &operator=(const basic_coroutine &other) { return *this; }

	entry_point_t _entry_point;
	fcontext_t _main;
	fcontext_t *_routine;
	void *_stack;
	void *_stacktop;
	bool _finished;

	struct abort_exception
	{
		int unused;
	};

public:

	class stack_allocator
	{
		vector<void*> _stacks;

		stack_allocator()
		{
			for(size_t i = 0; i < 16; ++i)
				_stacks.push_back(::operator new(default_stack_size));
		}

		~stack_allocator()
		{
			for(void *p : _stacks)
				::operator delete(p);
		}
	public:
		static stack_allocator& instance()
		{
			static stack_allocator _stack_allocator;
			return _stack_allocator;
		}

		static void* allocate()
		{
			void *ret;

			stack_allocator &inst = instance();
			if(!inst._stacks.empty())
			{
				ret = inst._stacks.back();
				inst._stacks.pop_back();
			}
			else
			{
				ret = ::operator new(default_stack_size);
			}

			return ret;
		}

		static void deallocate(void *p)
		{
			assert(p);
			instance()._stacks.push_back(p);
		}
	};

	basic_coroutine()
	{
		_stack = nullptr;
		_stacktop = nullptr;
		_routine = nullptr;
		_finished = true;
	}

	template<class FN>
	basic_coroutine(FN &&fn)
	{
		_entry_point = std::bind(std::forward<FN>(fn), std::placeholders::_1);
		_stack = stack_allocator::allocate();
		_stacktop = (char*)_stack + default_stack_size;
		_routine = make_fcontext(_stacktop, default_stack_size, &basic_coroutine::_run);
		_finished = false;
	}

	template<class FN, class V1>
	basic_coroutine(FN &&fn, V1 &&v1)
	{
		_entry_point = std::bind(std::forward<FN>(fn), std::forward<V1>(v1), std::placeholders::_1);
		_stack = stack_allocator::allocate();
		_stacktop = (char*)_stack + default_stack_size;
		_routine = make_fcontext(_stacktop, default_stack_size, &basic_coroutine::_run);
		_finished = false;
	}

	~basic_coroutine()
	{
		reset();
	}

	bool finished() const { return _finished; }
	operator bool() const { return !_finished; }
	bool operator!() const { return _finished; }

	basic_coroutine &operator=(basic_coroutine &&other)
	{
		if(_stack) stack_allocator::deallocate(_stack);
		
		_entry_point = move(other._entry_point);
		_stack       = other._stack;
		_stacktop    = other._stacktop;
		_routine     = other._routine;
		_finished    = other._finished;
		
		other._routine = nullptr;
		other._stack = nullptr;
		other._stacktop = nullptr;
		other._finished = true;
		
		return *this;
	}

	void reset()
	{
		if(!_finished)
		{
			_finished = true;
			jump_fcontext(&_main, _routine, (intptr_t)this);

			if(_stack) stack_allocator::deallocate(_stack);

			_routine = nullptr;
			_stack = nullptr;
			_stacktop = nullptr;
			_entry_point = nullptr;
		}
	}

	template<class FN>
	void reset(FN &&fn)
	{
		if(!_finished)
		{
			_finished = true;
			jump_fcontext(&_main, _routine, (intptr_t)this);
		}

		if(!_stack)
		{
			_stack = stack_allocator::allocate();
			_stacktop = (char*)_stack + default_stack_size;
		}

		_entry_point = bind(forward<FN>(fn), std::placeholders::_1);
		_routine = make_fcontext(_stacktop, default_stack_size, &basic_coroutine::_run);
		_finished = false;
	}

	template<class FN, class V1>
	void reset(FN &&fn, V1 &&v1)
	{
		if(!_finished)
		{
			_finished = true;
			jump_fcontext(&_main, _routine, (intptr_t)this);
		}

		if(!_stack)
		{
			_stack = stack_allocator::allocate();
			_stacktop = (char*)_stack + default_stack_size;
		}

		_entry_point = bind(forward<FN>(fn), forward<V1>(v1), std::placeholders::_1);
		_routine = make_fcontext(_stacktop, default_stack_size, &basic_coroutine::_run);
		_finished = false;
	}

	bool operator()()
	{
		jump_fcontext(&_main, _routine, (intptr_t)this);

		if(_finished)
		{
			if(_stack) stack_allocator::deallocate(_stack);

			_routine = nullptr;
			_stack = nullptr;
			_stacktop = nullptr;
			_entry_point = nullptr;
		}

		return !_finished;
	}

	class basic_iterator
	{
	protected:
		basic_coroutine<T, CORO> *_c;
	public:
		basic_iterator() : _c(nullptr)
		{
		}

		basic_iterator(basic_coroutine<T, CORO> *c) : _c(c)
		{
			if(!(*_c)())
				_c = nullptr;
		}

		basic_iterator& operator++()
		{
			if(!(*_c)())
				_c = nullptr;

			return *this;
		}
		
		bool operator==(const basic_iterator &other) { return _c == other._c; }
		bool operator!=(const basic_iterator &other) { return _c != other._c; }
	};

private:
	
	static void _run(intptr_t param)
	{
		basic_coroutine *c = (basic_coroutine*)param;

		try
		{
			if(!c->_finished)
			{
				c->_entry_point(static_cast<CORO*>(c)->_get_yield_token());
				c->_finished = true;
			}
		}
		catch(abort_exception&)
		{
		}

		jump_fcontext(c->_routine, &c->_main, 0);
	}
};

template<class T>
class coroutine : public basic_coroutine<T, coroutine<T>>
{
	const T *_value;

	coroutine(const coroutine &other){}
	coroutine &operator=(const coroutine &other) { return *this; }

public:
	
	template<class S>
	friend class yield_token;

	coroutine()
	{
		_value = nullptr;
	}

	template<class FN>
	coroutine(FN &&fn) : basic_coroutine<T, coroutine<T>>(fn)
	{
		_value = nullptr;
	}

	template<class FN, class V1>
	coroutine(FN &&fn, V1 &&v1) : basic_coroutine<T, coroutine<T>>(fn, v1)
	{
		_value = nullptr;
	}

	coroutine(coroutine &&other)
	{
		*this = move(other);
	}

	coroutine& operator=(coroutine &&other)
	{
		basic_coroutine<T, coroutine<T>>::operator=(std::move(other));
		
		_value = other._value;
		other._value = nullptr;

		return *this;
	}

	const T& get() const
	{
		assert(_value);
		return *_value;
	}

	yield_token<T> _get_yield_token()
	{
		return yield_token<T>(this);
	}

protected:
	
	void _yield(const T &return_value)
	{
		_value = &return_value;

		jump_fcontext(_routine, &_main, 0);
		
		_value = nullptr;

		if(_finished)
			throw abort_exception();
	}

public:
	class iterator : public basic_coroutine<T, coroutine<T>>::basic_iterator
	{
	public:
		iterator(){}
		iterator(coroutine<T> *c) : basic_coroutine<T, coroutine<T>>::basic_iterator(c){}

		T operator*()
		{
			return ((coroutine<T>*)_c)->get();
		}
	};

	iterator begin() { return iterator(this); }
	iterator end() { return iterator(); }
};

///////////////////////// VOID

template<>
class coroutine<void> : public basic_coroutine<void, coroutine<void>>
{
	coroutine(const coroutine &other){}
	coroutine &operator=(const coroutine &other) { return *this; }
public:

	template<class S>
	friend class yield_token;

	coroutine()
	{
	}

	template<class FN>
	coroutine(FN &&fn) : basic_coroutine<void, coroutine<void>>(fn)
	{
	}

	template<class FN, class V1>
	coroutine(FN &&fn, V1 &&v1) : basic_coroutine<void, coroutine<void>>(fn, v1)
	{
	}

	coroutine(coroutine &&other)
	{
		*this = move(other);
	}

	coroutine &operator=(coroutine &&other)
	{
		basic_coroutine<void, coroutine<void>>::operator=(std::move(other));
		return *this;
	}

	yield_token<void> _get_yield_token()
	{
		return yield_token<void>(this);
	}

protected:
	void _yield()
	{
		jump_fcontext(_routine, &_main, 0);
		
		if(_finished)
			throw abort_exception();
	}

public:

	class iterator : public basic_coroutine<void, coroutine<void>>::basic_iterator
	{
	public:
		iterator(){}
		iterator(coroutine<void> *c) : basic_coroutine<void, coroutine<void>>::basic_iterator(c){}
	};

	iterator begin() { return iterator(this); }
	iterator end() { return iterator(); }
};

template<class T>
inline void yield_token<T>::operator()(const T& return_value)
{
	_c->_yield(return_value);
}

inline void yield_token<void>::operator()()
{
	_c->_yield();
}

}
