/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_IMAGERESOURCE_H
#define INC_IMAGERESOURCE_H

#include "GL_Image.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

class ImageResource
{
public:
	ImageResource();
	ImageResource(GL_Image *image, int id, string tag, string filename);
	~ImageResource();

	GL_Image *image;
	int id;
	string tag;
	string filename;
};


#endif /* INC_IMAGERESOURCE_H */