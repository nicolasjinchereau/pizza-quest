/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <cstdlib>

template<class T>
class Singleton
{
public:
	virtual ~Singleton(){}

	static struct That
	{
		T& get() {
			static T inst;
			return inst;
		}

		const T& get() const { return get(); }
		inline T *operator->()				{ return  &get(); }
		inline const T *operator->() const	{ return  &get(); }
		inline operator T&()				{ return   get(); }
		inline operator const T&() const	{ return   get(); }
		inline operator T*()				{ return  &get(); }
		inline operator const T*() const	{ return  &get(); }

	} that;
};

template<class T>
typename Singleton<T>::That Singleton<T>::that;
