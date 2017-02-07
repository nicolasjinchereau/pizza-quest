/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_PROPERTYGRID_H
#define INC_PROPERTYGRID_H

#define NOMINMAX

#ifndef max
#define max(a, b)            (((a) > (b)) ? (a) : (b))
#endif
 
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <afxpropertygridctrl.h>

#undef max
#undef min

#endif /* INC_PROPERTYGRID_H */