/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <fstream>
#include "bytestream.h"
using namespace std;

inline bytestream bytestream_from_file(const string &filename)
{
	bytestream ret;

	ifstream fin(filename, ios::in | ios::binary);
	
	if(fin.is_open())
	{
		fin.seekg(0, ios::end);
		size_t sz = (size_t)fin.tellg();
		fin.seekg(0, ios::beg);
		
		if(sz > 0)
		{
			ret.resize(sz);
			fin.read((char*)ret.data(), sz);
		}
	}

	return ret;
}

inline bool bytestream_to_file(const string &filename, const bytestream &stream)
{
	ofstream fout(filename, ios::out | ios::binary);
	if(!fout.is_open())
		return false;

	fout.write(stream.data(), stream.size());
	if(fout.fail())
		return false;

	return true;
}
