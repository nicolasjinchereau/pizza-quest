/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_FILENAME_H
#define INC_FILENAME_H

#include <sys/stat.h>
#include <string>

using std::string;

class Filename : public string
{
public:
	Filename();
	Filename(string &filename);
	Filename(const char *filename);
	~Filename();

	void MakeRelativeTo(string file);
	void MakeAbsoluteFrom(string file);
	void UseBackSlash();
	void UseForwardSlash();
	void EraseLeadingSlash();
	void AddLeadingSlash();

	string GetName() const;
	string GetPath() const;
	string GetExtension() const;
	unsigned long FileSize() const;
	bool Exists() const;
	int CopyTo(char *out, int bufferLen) const;

	static string MakeRelative(string filename, string to_parent);
	static string MakeAbsolute(string filename, string from_parent);
	static string GetNameFrom(string filename);
	static string GetPathFrom(string filename);
	static string GetExtensionFrom(string filename);
	static long GetFileSizeOf(string filename);
	static string LowerCase(string filename);
	static bool Exists(string filename);
	static string UseBackSlash(string filename);
	static string UseForwardSlash(string filename);
	static string EraseLeadingSlash(string filename);
	static string AddLeadingSlash(string filename);

	Filename &operator=(string &str);
	Filename &operator=(const char *str);
};

#endif /* INC_FILENAME_H */