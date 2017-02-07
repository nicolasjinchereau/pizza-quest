/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <string>
#include <ostream>
#include <istream>
#include <sys/stat.h>

class Path
{
public:

	class iterator
	{
	public:
		friend class Path;

		iterator()
			: pPath(0),
			  offset(0)
		{
		}

		iterator(const Path &Path, size_t offset)
			: pPath(&Path),
			  offset(offset)
		{
			get_val();
		}

		iterator(const iterator &other)
			: pPath(other.pPath),
			  offset(other.offset),
			  element(other.element)
		{
		}

		iterator(iterator &&other)
			: pPath(other.pPath),
			  offset(other.offset),
			  element(std::move(other.element))
		{
		}

		iterator &operator=(const iterator &other)
		{
			pPath = other.pPath;
			offset = other.offset;
			element = other.element;
			return *this;
		}

		iterator &operator=(iterator &&other)
		{
			pPath = other.pPath;
			offset = other.offset;
			element = std::move(other.element);
			return *this;
		}

		const std::string &operator*() const
		{
			return element;
		}

		const std::string *operator->() const
		{
			return &element;
		}

		iterator &operator++()
		{
			size_t sz = pPath->value.size();
		
			while(offset < sz && pPath->value[offset] != '\\' && pPath->value[offset] != '/')
				++offset;

			while(offset < sz && (pPath->value[offset] == '\\' || pPath->value[offset] == '/'))
				++offset;

			get_val();

			return *this;
		}

		iterator operator++(int)
		{
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		iterator& operator--()
		{
			if(offset > 0)
			{
				--offset;

				while(offset > 0 && (pPath->value[offset] == '\\' || pPath->value[offset] == '/'))
					--offset;
			
				while(offset > 0 && pPath->value[offset] != '\\' && pPath->value[offset] != '/')
					--offset;

				if(offset > 0)
					++offset;
			}

			get_val();

			return *this;
		}

		iterator operator--(int)
		{
			iterator tmp = *this;
			--(*this);
			return tmp;
		}

		bool operator==(const iterator& other) const
		{
			return (pPath == other.pPath && offset == other.offset);
		}

		bool operator!=(const iterator& other) const
		{
			return (pPath != other.pPath || offset != other.offset);
		}

	private:
		void get_val()
		{
			element.clear();

			const char *pStart = pPath->value.c_str() + offset;
			const char *pEnd = pStart;

			while(*pEnd != 0 && *pEnd != '\\' && *pEnd != '/')
				++pEnd;

			if(pEnd - pStart > 0)
				element.assign(pStart, pEnd);
		}

		const Path *pPath;
		size_t offset;
		std::string element;
	};
	

	Path() : slash('\\')
	{
	}

	Path(const Path &other)
		: value(other.value),
		  slash(other.slash)
	{
	}

	Path(Path &&other)
		: value(std::move(other.value)),
		  slash(other.slash)
	{
	}

	Path(const iterator &first, const iterator &last)
		: slash('\\')
	{
		append(first, last);
	}

	Path(const std::string &str)
		: slash('\\')
	{
		*this /= str;
	}

	Path(const char *str)
		: slash('\\')
	{
		*this /= str;
	}

	~Path()
	{
	}

	Path &operator=(const Path& other)
	{
		value = other.value;
		slash = other.slash;
		return *this;
	}

	Path &operator=(Path&& other)
	{
		value = move(other.value);
		slash = other.slash;
		return *this;
	}

	Path &operator=(const std::string &str)
	{
		clear();
		return (*this /= str);
	}

    Path& operator=(const char *ptr)
    {
        clear();
        return (*this /= ptr);
    }

	Path &operator/=(const Path &p)
	{
		return append(p.value.c_str(), p.value.c_str() + p.value.size());
	}

	Path &operator/=(const std::string &s)
	{
		return append(s.c_str(), s.c_str() + s.size());
	}

	Path &operator/=(const char *ptr)
	{
		const char *last = ptr;

		while(*last != 0)
			++last;
		
		return append(ptr, last);
	}

	Path operator/(const Path &other) const
	{
		Path ret(*this);
		ret /= other;
		return ret;
	}
	
	Path operator/(const std::string &other) const
	{
		Path ret(*this);
		ret /= other;
		return ret;
	}

	Path operator/(const char *other) const
	{
		Path ret(*this);
		ret /= other;
		return ret;
	}
	
	//friend Path operator+(const std::string &s, const Path &p)
	//{
	//	return s + p.str();
	//}

	//friend Path operator+(const char *s, const Path &p)
	//{
	//	return std::string(s) + p.str();
	//}

	const std::string &str() const
	{
		return value;
	}

	const char *c_str() const
	{
		return value.c_str();
	}

	operator const std::string&() const
	{
		return value;
	}

	//operator std::string() const
	//{
	//	return value;
	//}

	operator const char*() const
	{
		return value.c_str();
	}

	bool operator==(const Path &other) const
	{
		if(value.size() != other.value.size())
			return false;

		for(size_t i = 0, sz = value.size(); i < sz; ++i)
		{
			if(tolower(value[i]) != tolower(other.value[i]))
				return false;
		}

		return true;
	}

	bool operator!=(const Path &other) const
	{
		return !(*this == other);
	}

	friend std::ostream &operator<<(std::ostream &output, const Path &p)
	{
		return output << (const std::string&)p;
	}

	friend std::istream &operator>>(std::istream &input, Path &p)
	{
		std::string s;
		input >> s;
		p = s;
		return input;
	}

	Path &append(const iterator &first, const iterator &last)
	{
		for(auto it = first; it != last; ++it)
		{
			if(!value.empty())
				value.push_back(slash);

			value.append(*it);
		}

		return *this;
	}

	Path &append(const char *first, const char *last)
	{
		if(first != last)
		{
			const char *pStart = first;
			const char *pEnd = first;

			while(*pStart != 0)
			{
				while(*pEnd != 0 && *pEnd != '\\' && *pEnd != '/')
					++pEnd;

				if(pEnd - pStart > 0)
				{
					if(!value.empty())
						value.push_back(slash);

					value.append(pStart, pEnd);
					pStart = pEnd;
				}

				while(*pEnd != 0 && (*pEnd == '\\' || *pEnd == '/'))
					pEnd++;

				pStart = pEnd;
			}
		}

		return *this;
	}

	void clear()
	{
		value.clear();
	}

	bool empty() const
	{
		return value.empty();
	}

	iterator begin() const
	{
		return iterator(*this, (size_t)0);
	}

	iterator end() const
	{
		return iterator(*this, value.size());
	}
	
	void use_forward_slash()
	{
		slash = '/';

		for(size_t i = 0, sz = value.size(); i < sz; ++i)
		{
			if(value[i] == '\\')
				value[i] = '/';
		}
	}

	void use_back_slash()
	{
		slash = '\\';

		for(size_t i = 0, sz = value.size(); i < sz; ++i)
		{
			if(value[i] == '/')
				value[i] = '\\';
		}
	}

	void remove_front()
	{
		if(!empty())
			*this = Path(++begin(), end());
	}

	void remove_back()
	{
		size_t off = (--end()).offset;

		if(off > 0)
			value.erase(off - 1);
		else
			value.clear();
	}

	void make_relative(const Path &base_path)
	{
		if(empty() || base_path.empty())
			return;

		iterator it_path = begin();
		iterator it_base = base_path.begin();

		if(*it_path != *it_base)
			return;

		++it_path;
		++it_base;

		Path newPath;
		bool different = false;
		
		while(it_path != end() && it_base != base_path.end())
		{
			if(*it_path != *it_base || different)
			{
				different = true;
				newPath = Path("..") / newPath / *it_path;
			}

			++it_path;
			++it_base;
		}

		while(it_path != end())
		{
			newPath /= *it_path;
			++it_path;
		}
		
		while(it_base != base_path.end())
		{
			newPath = Path("..") / newPath;
			++it_base;
		}

		*this = std::move(newPath);
	}

	void make_absolute(const Path &base_path)
	{
		Path base = base_path;

		while(!empty() && *begin() == "..")
		{
			remove_front();
			
			if(!base.empty())
				base.remove_back();
		}

		*this = (base / *this);
	}
	
	void set_extension(const std::string &extension)
	{
		size_t sz = value.size();

		if(sz == 0)
			return;

		size_t off = sz - 1;

		while(off > 0 && value[off] != '.' && value[off] != '\\' && value[off] != '/')
			--off;

		if(value[off] == '.')
		{
			value.erase(off);
			
			if(extension.size() > 0)
				value += "." + extension;
		}
	}

	//bool is_inside(const Path &Path) const
	//{
	//	if(parts.size() <= Path.parts.size())
	//		return false;

	//	const_iterator it_file = begin();
	//	const_iterator it_path = Path.begin();

	//	while(it_file != end() && it_path != Path.end())
	//	{
	//		if(*it_file != *it_path)
	//			return false;

	//		++it_file;
	//		++it_path;
	//	}

	//	return true;
	//}
	//
	//bool is_relative() const
	//{
	//	return value.find(':') == std::string::npos;
	//}

	//bool is_absolute() const
	//{
	//	return value.find(':') != std::string::npos;
	//}

	bool is_dots() const
	{
		iterator it = --end();
		return *it == "." || *it == "..";
	}

	std::string file_name() const
	{
		return empty() ? "" : (*--end());
	}

	std::string file_title() const
	{
		std::string ret = empty() ? "" : (*--end());

		size_t dot = ret.find(".");

		if(dot != std::string::npos)
			ret.erase(dot);

		return ret;
	}

	std::string file_extension() const
	{
		std::string ret = empty() ? "" : (*--end());
		
		size_t dot = ret.find(".");
		
		if(dot != std::string::npos)
			ret.erase(0, dot + 1);

		return ret;
	}

	std::string file_path() const
	{
		return value.substr(0, (--end()).offset);
	}

	bool exists() const
	{
		struct stat stFileInfo;
		return stat(c_str(), &stFileInfo) == 0;
	}

	bool is_folder() const
	{
		struct stat stFileInfo;
		
		if(stat(c_str(), &stFileInfo) == 0)
		{
			if(stFileInfo.st_mode & S_IFDIR)
				return true;
		}

		return false;
	}

	bool is_file() const
	{
		struct stat stFileInfo;
		
		if(stat(c_str(), &stFileInfo) == 0)
		{
			if(stFileInfo.st_mode & S_IFREG)
				return true;
		}

		return false;
	}

	unsigned long file_size() const
	{
		struct stat stFileInfo;
		if(stat(c_str(), &stFileInfo) != 0)
			return 0;

		return stFileInfo.st_size;
	}

	long long last_accessed()
	{
		struct stat stFileInfo;
		if(stat(c_str(), &stFileInfo) == 0)
			return stFileInfo.st_atime;

		return 0;
	}

	long long last_modified()
	{
		struct stat stFileInfo;
		if(stat(c_str(), &stFileInfo) == 0)
			return stFileInfo.st_mtime;

		return 0;
	}

	long long time_created()
	{
		struct stat stFileInfo;
		if(stat(c_str(), &stFileInfo) == 0)
			return stFileInfo.st_ctime;

		return 0;
	}

private:
	std::string value;
	char slash;
};
