/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "filename.h"

Filename::Filename()
{

}

Filename::Filename(string &filename)
{
	assign(filename);
}

Filename::Filename(const char *filename)
{
	assign(filename);
}

Filename::~Filename()
{

}

void Filename::MakeRelativeTo(string file)
{
	// exit if either string is empty
	if(*this == "" || file == "")
		return;
	
	// return if the parent filename was passed in without a path
	size_t found = file.find_last_of("/\\");
	if(found == string::npos)
		return;
	
	// return if the file is not in a sub-folder
	// below the parent file
	std::string parentDir = file.substr(0, found);
	std::string fileDir = substr(0, found);

	if(parentDir != fileDir)
		return;

	// return the relative portion of the filename
	// relative filename contains leading slash
	assign(this->substr(found + 1));
}

void Filename::MakeAbsoluteFrom(string file)
{
	if(*this == "")
		return;

	// if the filename contains a colon, then it is already
	// an absolute pathname, so quit.
	if(find_first_of(":") != string::npos)
		return;

	// add a leading slash if necessary
	AddLeadingSlash();
	
	// return the absolute path of the file.
	assign(GetPathFrom(file) + *this);
}

string Filename::MakeRelative(string filename, string to_parent)
{
	// exit if either string is empty
	if(filename == "" || to_parent == "")
		return filename;
	
	// return if the parent filename was passed in without a path
	size_t found = to_parent.find_last_of("/\\");
	if(found == std::string::npos)
		return filename;
	
	// return if the file is not in a sub-folder
	// below the parent file
	std::string parentDir = to_parent.substr(0, found);
	std::string fileDir = filename.substr(0, found);

	if(parentDir != fileDir)
		return filename;

	// return the relative portion of the file_name
	return filename.substr(found + 1);
}

string Filename::MakeAbsolute(string filename, string from_parent)
{
	if(filename == "")
		return "";

	// if the filename contains a colon, then it is already
	// an absolute pathname, so quit.
	size_t found = filename.find_first_of(":");

	if(found != std::string::npos)
		return filename;
	
	// add a leading slach if necessary
	filename = AddLeadingSlash(filename);

	// return the absolute path of the file.
	return GetPathFrom(from_parent) + filename;
}

string Filename::GetName() const
{
	size_t found = find_last_of("/\\");
	
	if(found != string::npos)
		return substr(found + 1);

	// if no slash was found, assume this is already just a filename
	// does not contain leading slash
	return *this;
}

string Filename::GetNameFrom(string filename)
{
	size_t found = filename.find_last_of("/\\");
	
	if(found != string::npos)
		return filename.substr(found + 1);

	// if no slash was found, assume this is already just a filename
	// does not contain leading slash
	return filename;
}

string Filename::GetPath() const
{
	size_t found = find_last_of("/\\");

	if(found != string::npos)
		return this->substr(0, found);

	return "";
	//result does not contain trailing slash
}

string Filename::GetPathFrom(string filename)
{
	size_t found = filename.find_last_of("/\\");

	if(found != string::npos)
		return filename.substr(0, found);

	return "";
	//result does not contain trailing slash
}

Filename &Filename::operator=(string &str)
{
	assign(str);
	return *this;
}

Filename &Filename::operator=(const char *str)
{
	assign(str);
	return *this;
}

string Filename::GetExtension() const
{
	string strName = LowerCase(*this);
	size_t found = strName.find_last_of(".");

	if(found != std::string::npos)
		return strName.substr(found + 1);

	return "";
}

string Filename::GetExtensionFrom(string filename)
{
	string strName = LowerCase(filename);
	size_t found = strName.find_last_of(".");

	if(found != std::string::npos)
		return strName.substr(found + 1);

	return "";
}

unsigned long Filename::FileSize() const
{
	struct stat stFileInfo;

	if(stat(c_str(), &stFileInfo) != 0)
		return 0; // file not found

	return stFileInfo.st_size;
}

long Filename::GetFileSizeOf(string filename)
{
	struct stat stFileInfo;

	if(stat(filename.c_str(), &stFileInfo) != 0)
		return 0; // file not found

	return stFileInfo.st_size;
}

string Filename::LowerCase(string filename)
{
	std::string tmp = filename;

	for(unsigned int i = 0; i < tmp.length(); i++)
	{
		if(tmp[i] >= 'A' && tmp[i] <= 'Z')
		{
			tmp[i] |= 0x20;
		}
	}

	return tmp;
}

bool Filename::Exists() const
{
	struct stat stFileInfo;

	int ret = stat(c_str(), &stFileInfo);

	return(ret == 0);
}

bool Filename::Exists(string filename)
{
	struct stat stFileInfo;

	int ret = stat(filename.c_str(), &stFileInfo);

	return(ret == 0);
}

int Filename::CopyTo(char *out, int bufferLen) const
{
	if((int)this->size() + 1 > bufferLen)
		return -1;

	memset(out, 0, bufferLen);
	memcpy(out, c_str(), size() + 1);

	return 0;
}

void Filename::UseBackSlash()
{
	size_t pos = find_first_of("/");
	while(pos != npos)
	{
		replace(pos, 1, "\\");
		pos = find_first_of("/");
	}
}

void Filename::UseForwardSlash()
{
	size_t pos = find_first_of("\\");
	while(pos != npos)
	{
		replace(pos, 1, "/");
		pos = find_first_of("\\");
	}
}

string Filename::UseBackSlash(string filename)
{
	size_t pos = filename.find_first_of("/");
	while(pos != std::string::npos)
	{
		filename.replace(pos, 1, "\\");
		pos = filename.find_first_of("/");
	}

	return filename;
}

string Filename::UseForwardSlash(string filename)
{
	size_t pos = filename.find_first_of("\\");
	while(pos != std::string::npos)
	{
		filename.replace(pos, 1, "/");
		pos = filename.find_first_of("\\");
	}

	return filename;
}

void Filename::EraseLeadingSlash()
{
	// uses forward slashes by default
	while(size() > 0 && (at(0) == '\\' || at(0) == '/'))
		erase(0, 1);
}

void Filename::AddLeadingSlash()
{
	if(size() == 0)
		return;

	// return if leading slash is already present, return
	if(at(0) == '\\' || at(0) == '/')
		return;

	size_t pos = find_first_of("/");
	if(pos != npos)
		insert(0, "/");
	else
		insert(0, "\\");
}

string Filename::EraseLeadingSlash(string filename)
{
	// uses forward slashes by default
	while(filename.size() > 0 && (filename.at(0) == '\\' || filename.at(0) == '/'))
		filename.erase(0, 1);

	return filename;
}

string Filename::AddLeadingSlash(string filename)
{
	if(filename.size() == 0)
		return "";

	if(filename.at(0) == '\\' || filename.at(0) == '/')
		return filename;

	size_t pos = filename.find_first_of("/");
	if(pos != npos)
		filename.insert(0, "/");
	else
		filename.insert(0, "\\");

	return filename;
}
