/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_GL_DRAW_H
#define INC_GL_DRAW_H

#include "definitions.h"
#include "GL_Screen.h"
#include "GL_Sprite.h"

void GL_DrawRect(GL_Screen *dest, Rect<int> *rect, Color clr, float thickness = 1.0f);
void GL_DrawRect(GL_Screen *dest, Rect<float> *rect, Color clr, float thickness = 1.0f);
void GL_FillRect(GL_Screen *dest, Rect<int> *rect, Color clr);
void GL_FillRect(GL_Screen *dest, Rect<float> *rect, Color clr);
void GL_DrawLine(GL_Screen *dest, Vec2<int> &p1, Vec2<int> &p2, Color clr, float thickness = 1.0f);
void GL_DrawLine(GL_Screen *dest, Vec2<float> &p1, Vec2<float> &p2, Color clr, float thickness = 1.0f);
void GL_DrawCircle(GL_Screen *dest, Vec2<float> &center, float radius, Color clr, float thickness = 1.0f);
void GL_FillCircle(GL_Screen *dest, Vec2<float> &center, float radius, const Color &clr);

#endif /* INC_GL_DRAW_H */