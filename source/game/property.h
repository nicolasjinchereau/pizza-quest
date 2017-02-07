/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <cstdlib>
#include <cassert>
#include <type_traits>
#include <ostream>
#include <functional>

using namespace std;

template<class T>
class property
{
	property &operator=(const property &other) { return *this }
public:

	template<class T, bool byval = (is_fundamental<T>::value || is_pointer<T>::value)>
	struct parameter {
		typedef typename T type;
	};

	template<class T>
	struct parameter<T, false> {
		typedef typename const std::remove_const<typename std::remove_reference<T>::type>::type& type;
	};

	template<typename X, typename Y>
	struct promote {
		typedef decltype(X(0) + Y(0)) type;
	};

	typedef property<T> my_type;
	typedef T return_type;
	typedef typename parameter<T>::type assign_type;
	typedef return_type(my_type::*getter_type)();
	typedef void(my_type::*setter_type)(assign_type);

	template<class PROPERTY>
	property(PROPERTY &prop,
		typename enable_if<is_member_function_pointer<decltype(&PROPERTY::get)>::value>::type* = 0)
		: _object(*(void**)&prop),
		  _getter((getter_type)&PROPERTY::get),
		  _setter((setter_type)&PROPERTY::set)
	{
		static_assert(std::is_pod<PROPERTY>::value,
			"Invalid property. A property should be a POD struct");

		static_assert(sizeof(PROPERTY) == sizeof(void*),
			"Invalid property. A property must only contain a pointer to an object with the type of the target class.");
		
		static_assert(
			is_same<decltype(&PROPERTY::get), return_type(PROPERTY::*)()>::value,
			"Invalid property. Incorrect method signature for property getter");

		static_assert(
			is_same<decltype(&PROPERTY::set), void(PROPERTY::*)(assign_type)>::value,
			"Invalid property. Incorrect method signature for property setter");
	}

	inline property &operator=(assign_type value) {
		(this->*_setter)(value);
		return *this;
	}

	inline operator return_type() {
		return (this->*_getter)();
	}

	inline operator const return_type() const {
		return (this->*_getter)();
	}

	inline friend std::ostream &operator<<(std::ostream &stream, my_type &in) {
		stream << (return_type)in;
		return stream;
	}

	template<class S>
	inline property& operator+=(const S &right) {
		return (*this = ((T)*this + right));
	}

	template<class S>
	inline property& operator-=(const S &right) {
		return (*this = ((T)*this - right));
	}

	template<class S>
	inline property& operator*=(const S &right) {
		return (*this = ((T)*this * right));
	}

	template<class S>
	inline property& operator/=(const S &right) {
		return (*this = ((T)*this / right));
	}

	template<class S>
	inline friend typename promote<T, S>::type operator+(const my_type &left, const S &right) {
		return (T)(my_type&)left + right;
	}

	template<class S>
	inline friend typename promote<T, S>::type operator-(const my_type &left, const S &right) {
		return (T)(my_type&)left - right;
	}

	template<class S>
	inline friend typename promote<T, S>::type operator*(const my_type &left, const S &right) {
		return (T)(my_type&)left * right;
	}

	template<class S>
	inline friend typename promote<T, S>::type operator/(const my_type &left, const S &right) {
		return (T)(my_type&)left / right;
	}

	template<class S>
	inline friend bool operator>(const my_type &left, const S &right) {
		return (T)(my_type&)left > right;
	}

	template<class S>
	inline friend bool operator>=(const my_type &left, const S &right) {
		return (T)(my_type&)left >= right;
	}

	template<class S>
	inline friend bool operator<(const my_type &left, const S &right) {
		return (T)(my_type&)left < right;
	}

	template<class S>
	inline friend bool operator<=(const my_type &left, const S &right) {
		return (T)(my_type&)left <= right;
	}

private:
	void *_object;
	getter_type _getter;
	setter_type _setter;
};
