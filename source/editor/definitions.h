/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_DEFINITIONS_H
#define INC_DEFINITIONS_H

#include "vecmath.h"
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>

// find the minimum, maximum, or difference of two values
#define MIN(a, b) (a < b ? (a) : (b))
#define MAX(a, b) (a > b ? (a) : (b))
#define DIFF(a, b) ((a < b) ? (b - a) : (a - b))

// make an RGB color into SDL_gfx format
#define RGBACOLOR(r, g, b) ((unsigned int)(r << 24 | g << 16 | b << 8 | 0xFF))
#define ARGBCOLOR(r, g, b) ((unsigned int)(0xFF << 24 | r << 16 | g << 8 | b))

#define CREATINGCOLOR	Color(255, 0, 255)
#define DISABLEDCOLOR	Color(90, 90, 90)
#define SELECTEDCOLOR	Color(255, 255, 255)
#define UNSELECTEDCOLOR Color(127, 127, 127)
#define YELLOWCOLOR		Color(255, 255, 0)
#define REDCOLOR		Color(255, 0, 0)
#define BLUECOLOR		Color(0, 0, 255)
#define GREENCOLOR		Color(0, 255, 0)

typedef char			int8;
typedef short			Int16;
typedef int				int32;
typedef unsigned char	Uint8;
typedef unsigned short	Uint16;
typedef unsigned int	Uint32;

#endif /* INC_DEFINITIONS_H */