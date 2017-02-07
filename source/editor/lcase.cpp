/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "lcase.h"

std::string LCASE(std::string &text)
{
	std::string tmp = text;

	for(unsigned int i = 0; i < tmp.length(); i++)
	{
		if(tmp[i] >= 'A' && tmp[i] <= 'Z')
		{
			tmp[i] |= 0x20;
		}
	}

	return tmp;
}