/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <afxdisp.h>
#include <string>
using namespace std;

class PQString : public string
{
	char buffer[64];

public:
///////// constructors
	PQString(){}

	PQString(const char *str)
	{
		assign(str);
	}

	PQString(string str)
	{
		assign(str);
	}

	PQString(const int &i)
	{
		sprintf_s(buffer, "%i", i);
		assign(buffer);
	}

	PQString(const unsigned int &u)
	{
		sprintf_s(buffer, "%u", u);
		assign(buffer);
	}

	PQString(const float &f)
	{
		sprintf_s(buffer, "%f", f);
		assign(buffer);
	}

	PQString(const void *ptr)
	{
		sprintf_s(buffer, "%X", ptr);
		assign(buffer);
	}

	
	///////// +

	PQString operator+(const char *str)
	{
		append(str);
		return *this;
	}
	
	PQString operator+(string str)
	{
		append(str);
		return *this;
	}

	PQString operator+(const int &i)
	{
		sprintf_s(buffer, "%i", i);
		return string(*this) + buffer;
	}
	
	PQString operator+(const unsigned int &u)
	{
		sprintf_s(buffer, "%u", u);
		return string(*this) + buffer;
	}

	PQString operator+(const float &f)
	{
		sprintf_s(buffer, "%f", f);
		return string(*this) + buffer;
	}

	PQString operator+(const void *ptr)
	{
		sprintf_s(buffer, "%X", ptr);
		return string(*this) + buffer;
	}

	////// +=

	PQString &operator+=(const char *str)
	{
		append(str);
		return *this;
	}

	PQString &operator+=(string str)
	{
		append(str);
		return *this;
	}

	PQString &operator+=(const int &i)
	{
		sprintf_s(buffer, "%i", i);
		append(buffer);
		return *this;
	}
	
	PQString &operator+=(const unsigned int &u)
	{
		sprintf_s(buffer, "%u", u);
		append(buffer);
		return *this;
	}

	PQString &operator+=(const float &f)
	{
		sprintf_s(buffer, "%f", f);
		append(buffer);
		return *this;
	}

	PQString &operator+=(const void *ptr)
	{
		sprintf_s(buffer, "%X", ptr);
		append(buffer);
		return *this;
	}

	operator COleVariant()
	{
		return COleVariant(c_str());
	}

	operator void*()
	{
		return (void*)strtoul(c_str(), NULL, 10);
	}

	operator const char*()
	{
		return c_str();
	}

	operator char*()
	{
		return (char*)c_str();
	}

	operator int()
	{
		return atoi(c_str());
	}

	operator unsigned int()
	{
		return strtoul(c_str(), NULL, 10);
	}

	operator float()
	{
		return (float)atof(c_str());
	}

	operator double()
	{
		return atof(c_str());
	}

	operator bool()
	{
		return compare("false") == 1;
	}
};