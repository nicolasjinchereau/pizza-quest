/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

class bytestream
{
	char* _first;
	char* _ptr;
	char* _last;
	char* _end;
public:

	enum class supported_types
	{
		unsuported_type = -1,
		bool_type,
		char_type,
		uchar_type,
		short_type,
		ushort_type,
		int_type,
		uint_type,
		long_type,
		ulong_type,
		longlong_type,
		ulonglong_type,
		float_type,
		double_type,
		std_string_type,
	};

	template<class T> struct is_supported : std::false_type{};
	
	bytestream() : _first(nullptr),
				   _ptr(nullptr),		   
				   _last(nullptr),
				   _end(nullptr){}
	
	bytestream(size_t sz)
	{
		size_t cap = 1;
		while(cap < sz)
			cap <<= 1;
		
		_first = (char*)::operator new(cap);
		_ptr = _first;
		_last = _first + sz;
		_end = _first + cap;
	}
	
	bytestream(size_t sz, char value)
	{
		size_t cap = 1;
		while(cap < sz)
			cap <<= 1;
		
		_first = (char*)::operator new(cap);
		_ptr = _first;
		_last = _first + sz;
		_end = _first + cap;

		memset(_first, value, sz);
	}

	bytestream(const bytestream &other)
	{
		if(other._first)
		{
			size_t cap = other.capacity();
			size_t sz = other.size();
			size_t ps = other.pos();

			_first = (char*)::operator new(cap);
			_ptr = _first + ps;
			_last = _first + sz;
			_end = _first + cap;
			
			if(sz)
				memcpy(_first, other._first, sz);
		}
		else
		{
			_first = nullptr;
			_ptr = nullptr;
			_last = nullptr;
			_end = nullptr;
		}
	}
	
	bytestream(bytestream &&other)
		: _first(other._first),
		  _ptr(other._ptr),
		  _last(other._last),
		  _end(other._end)
	{
		other._first = nullptr;
		other._ptr = nullptr;
		other._last = nullptr;
		other._end = nullptr;
	}
	
	~bytestream()
	{
		if(_first) ::operator delete(_first);
	}
	
	bytestream &operator=(const bytestream &other)
	{
		if(other._first)
		{
			size_t cap = other.capacity();
			size_t sz = other.size();
			size_t ps = other.pos();

			_first = (char*)::operator new(cap);
			_ptr = _first + ps;
			_last = _first + sz;
			_end = _first + cap;
			
			if(sz)
				memcpy(_first, other._first, sz);
		}
		else
		{
			_first = nullptr;
			_ptr = nullptr;
			_last = nullptr;
			_end = nullptr;
		}
		
		return *this;
	}
	
	bytestream &operator=(bytestream &&other)
	{
		if(_first) ::operator delete(_first);
		
		_first = other._first;
		_ptr = other._ptr;
		_last = other._last;
		_end = other._end;
		
		other._first = nullptr;
		other._ptr = nullptr;
		other._last = nullptr;
		other._end = nullptr;
		
		return *this;
	}

	void assign(const bytestream &other)
	{
		if(other._first)
		{
			size_t cap = other.capacity();
			size_t sz = other.size();
			size_t ps = other.pos();

			_first = (char*)::operator new(cap);
			_ptr = _first + ps;
			_last = _first + sz;
			_end = _first + cap;
			
			if(sz)
				memcpy(_first, other._first, sz);
		}
		else
		{
			_first = nullptr;
			_ptr = nullptr;
			_last = nullptr;
			_end = nullptr;
		}
	}
	
	void assign(bytestream &&other)
	{
		if(_first) ::operator delete(_first);
		
		_first = other._first;
		_ptr = other._ptr;
		_last = other._last;
		_end = other._end;
		
		other._first = nullptr;
		other._ptr = nullptr;
		other._last = nullptr;
		other._end = nullptr;
	}

	void reserve(size_t new_cap)
	{
		if(capacity() < new_cap)
		{
			size_t sz = size();
			size_t ps = pos();

			size_t cap = 1;
			while(cap < new_cap) cap <<= 1;
			
			char *tmp = (char*)::operator new(cap);

			if(sz)
				memcpy(tmp, _first, sz);
			
			if(_first)
				::operator delete(_first);
			
			_first = tmp;
			_ptr   = tmp + ps;
			_last  = tmp + sz;
			_end   = tmp + cap;
		}
	}
	
	void shrink_to_fit()
	{
		size_t sz = size();
		size_t ps = pos();
		
		if(sz == 0)
		{
			if(_first)
				::operator delete(_first);

			_first = nullptr;
			_ptr = nullptr;
			_last = nullptr;
			_end = nullptr;
		}
		else
		{
			size_t cap = 1;
			while(cap < sz)
				cap <<= 1;

			if(cap < capacity())
			{
				char *tmp = (char*)::operator new(cap);

				if(sz)
					memcpy(tmp, _first, sz);
			
				if(_first)
					::operator delete(_first);

				_first = tmp;
				_ptr   = tmp + ps;
				_last  = tmp + sz;
				_end   = tmp + cap;
			}
		}
	}

	void clear()
	{
		_ptr = _first;
		_last = _first;
	}
	
	void seek(size_t offset)
	{
		_ptr = _first + offset;
	}
	
	void ignore(size_t count)
	{
		_ptr += count;
	}
	
	void rewind()
	{
		_ptr = _first;
	}

	void resize(size_t sz)
	{
		reserve(sz);

		_last = _first + sz;

		if(_ptr > _last)
			_ptr = _last;
	}
	
	void resize(size_t sz, char value)
	{
		reserve(sz);

		size_t old_sz = size();

		if(sz > old_sz)
			memset(_last, value, sz - old_sz);

		_last =  _first + sz;

		if(_ptr > _last)
			_ptr = _last;
	}

	void peek(void *out, size_t size)
	{
		assert((int)size <= available());
		memcpy(out, _ptr, size);
	}

	void read(void *out, size_t size)
	{
		assert((int)size <= available());
		memcpy(out, _ptr, size);
		_ptr += size;
	}

	void write(const void *in, size_t size)
	{
		reserve(pos() + size);

		memcpy(_ptr, in, size);
		_ptr += size;

		if(_last < _ptr)
			_last = _ptr;
	}

	void peek(std::string &out)
	{
		char* str_end = _ptr;

		while(*str_end != 0)
			++str_end;
		
		out.assign(_ptr, str_end);
	}

	void read(std::string &out)
	{
		char* str_end = _ptr;

		while(*str_end != 0)
			++str_end;

		out.assign(_ptr, str_end);
		
		_ptr = str_end + 1;
	}

	inline void write(const std::string &in)
	{
		write((void*)in.c_str(), in.length() + 1);
	}

	inline bytestream& operator<<(const std::string &in)
	{
		write(in);
		return *this;
	}

	inline bytestream& operator>>(std::string &out)
	{
		read(out);
		return *this;
	}

	template<class T>
	inline void peek(T &out)
	{
		static_assert(is_supported<T>::value, "a bytestream cannot automatically deserialize this type");
		peek((void*)&out, sizeof(T));
	}

	template<class T>
	inline void read(T &out)
	{
		static_assert(is_supported<T>::value, "a bytestream cannot automatically deserialize this type");
		read((void*)&out, sizeof(T));
	}

	template<class T>
	inline void write(const T &in)
	{
		static_assert(is_supported<T>::value, "a bytestream cannot automatically serialize this type");
		write((void*)&in, sizeof(T));
	}

	template<class T>
	inline bytestream& operator<<(const T &in)
	{
		static_assert(is_supported<T>::value, "a bytestream cannot automatically serialize this type");
		write(in);
		return *this;
	}
	
	template<class T>
	inline bytestream& operator>>(T &out)
	{
		static_assert(is_supported<T>::value, "a bytestream cannot automatically deserialize this type");
		read(out);
		return *this;
	}

	inline size_t capacity() const					{ return _end - _first; }
	inline size_t size() const						{ return _last - _first; }
	inline size_t pos() const						{ return _ptr - _first; }
	inline size_t available() const					{ return _last - _ptr; }
	inline bool empty() const						{ return _first == _last; }

	inline char* begin()							{ return _first; }
	inline const char* begin() const				{ return _first; }
	inline const char* cbegin() const				{ return _first; }

	inline char* end()								{ return _last; }
	inline const char* end() const					{ return _last; }
	inline const char* cend() const					{ return _last; }

	inline char* rbegin()							{ return _last; }
	inline const char* rbegin() const				{ return _last; }
	inline const char* crbegin() const				{ return _last; }

	inline char* rend()								{ return _first - 1; }
	inline const char* rend() const					{ return _first - 1; }
	inline const char* crend() const				{ return _first - 1; }

	char* ptr()										{ return _ptr; }
	const char* ptr() const							{ return _ptr; }
	const char* cptr() const						{ return _ptr; }
	
	inline char *data()								{ return _first; }
	inline const char *data() const					{ return _first; }

	inline char& at(size_t i)						{ return *(_first + i); }
	inline const char& at(size_t i) const			{ return *(_first + i); }

	inline char& operator[](size_t i)				{ return *(_first + i); }
	inline const char& operator[](size_t i) const	{ return *(_first + i); }
};

template<> struct bytestream::is_supported<bool>               : std::true_type {};
template<> struct bytestream::is_supported<char>               : std::true_type {};
template<> struct bytestream::is_supported<unsigned char>      : std::true_type {};
template<> struct bytestream::is_supported<short>              : std::true_type {};
template<> struct bytestream::is_supported<unsigned short>     : std::true_type {};
template<> struct bytestream::is_supported<int>                : std::true_type {};
template<> struct bytestream::is_supported<unsigned int>       : std::true_type {};
template<> struct bytestream::is_supported<long>               : std::true_type {};
template<> struct bytestream::is_supported<unsigned long>      : std::true_type {};
template<> struct bytestream::is_supported<long long>          : std::true_type {};
template<> struct bytestream::is_supported<unsigned long long> : std::true_type {};
template<> struct bytestream::is_supported<float>              : std::true_type {};
template<> struct bytestream::is_supported<double>             : std::true_type {};
template<> struct bytestream::is_supported<std::string>        : std::true_type {};
