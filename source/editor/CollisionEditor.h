/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include <afxdialogex.h>
#include "definitions.h"
#include "resource.h"
#include "PQMapNode.h"
#include "PQB2Shapes.h"
#include <vector>
#include "list.h"

using namespace std;

class cGL_Screen : public GL_Screen
{
	DECLARE_DYNAMIC(cGL_Screen)

public:
	float halfWidth;
	float halfHeight;

	PQResImagePtr img;
	
	int shapeID;
	Vec2<float> dragStart;
	Vec2<float> dragTarget;
	Vec2<float> totalMove;

	bool shiftDown;
	bool moving;
	int selIndex;

	PQB2Shape	*newShape;
	PQB2Circle	newCircle;	//SHAPE_CIRCLE
	PQB2Polygon newPolygon; //SHAPE_POLYGON
	PQB2Box		newBox;		//SHAPE_BOX
	PQB2Edge	newEdge;	//SHAPE_EDGE
	
	bool changed;

	cGL_Screen(PQResImagePtr &image)
	{
		shiftDown =  false;
		moving = false;
		shapeID = -1;
		newShape = NULL;
		img = image;
		selIndex = -1;
		changed = false;
	}

	~cGL_Screen()
	{
		
	}

	void OnSize(UINT nType, int cx, int cy)
	{
		GL_Screen::OnSize(nType,cx, cy);

		halfWidth = (float)this->GetWidth() / 2.0f;
		halfHeight = (float)this->GetHeight() / 2.0f;
	}

	virtual void OnDraw()
	{
		Set2D();

		img->Draw(this, halfWidth, halfHeight, 0, 0, 1.0f, 0.0f);

		// draw all existing shapes
		for(int i = 0; i < (int)img->collision_shapes.size(); i++)
			img->collision_shapes[i]->Draw(this, halfWidth, halfHeight);

		// draw currently drawing shape
		if(newShape)
			newShape->Draw(this, halfWidth, halfHeight);
	}

	void SetShapeID(int ID)
	{
		shapeID = ID;
	}

	///////////////////////////////////////////////////

	bool IsConvex(Vec2<float> *vertices, int nVerts)
	{
		int a = nVerts - 1;
		for(int b = 0; b < nVerts; b++)
		{
			Vec2<float> edge = vertices[a] - vertices[b];

			// make sure this edge is longer than 0
			if(edge.LengthSq() < FLT_EPSILON * FLT_EPSILON)
				return false;

			for(int v = 0; v < nVerts; v++)
			{
				if(v == a || v == b)
					continue;
				
				Vec2<float> r = vertices[v] - vertices[b];

				// if there is an indentation in the shape,
				// or two edges are collinear,
				if(edge.Det(r) > 0.0f)
					return false;
			}

			a = b;
		}

		return true;
	}

	/////////////////////////////////////////////////

	virtual void OnLMouseDown(int x, int y)
	{
		float sX = (float)x - halfWidth;
		float sY = (float)y - halfHeight;

		dragStart.set(sX, sY);

		if(newShape == NULL || newShape->type == SHAPE_POLYGON)
		{
			switch(shapeID)
			{
			case SHAPE_CIRCLE:
				newCircle.center.set(sX, sY);
				newCircle.radius = 0.0f;
				newShape = &newCircle;
				break;

			case SHAPE_POLYGON:
			{
				if(newPolygon.nVerts == 0)
				{
					newPolygon.vertices[newPolygon.nVerts++] = Vec2<float>(sX, sY);
					newShape = &newPolygon;
				}

				newPolygon.vertices[newPolygon.nVerts++] = Vec2<float>(sX, sY);

				if(newPolygon.nVerts > 3
				&& IsConvex(newPolygon.vertices, newPolygon.nVerts - 1) == false)
				{
					MessageBeep(0);
					newPolygon.nVerts--;
				}
				
				break;
			}
			case SHAPE_BOX:
				newBox.center.set(sX, sY);
				newBox.halfWidth = 0.0f;
				newBox.halfHeight = 0.0f;
				newBox.angle = 0.0f;
				newShape = &newBox;
				break;

			case SHAPE_EDGE:
				newEdge.p1.set(sX, sY);
				newEdge.p2.set(sX, sY);
				newShape = &newEdge;
				break;

			case -1: // selection tool
				selIndex = -1;
				for(int i = 0; i < (int)img->collision_shapes.size(); i++)
				{
					if(selIndex == -1 && img->collision_shapes[i]->HitTest((int)sX, (int)sY) != -1)
					{
						selIndex = i;
						img->collision_shapes[i]->selected = true;
						moving = true;
						totalMove.set(0, 0);
					}
					else
					{
						img->collision_shapes[i]->selected = false;
					}
				}

				break;
			}
		}

		Redraw();
	}
	
	///////////////////////////////////////////////////

	virtual void OnLMouseUp(int x, int y)
	{
		float sX = (float)x - halfWidth;
		float sY = (float)y - halfHeight;

		if(newShape != NULL)
		{
			switch(newShape->type)
			{
			case SHAPE_CIRCLE:
				if(newCircle.radius > 2.0f)
					img->collision_shapes.push_back( PQB2ShapePtr(new PQB2Circle(newCircle)) );

				newShape = NULL;
				changed = true;
				break;

			case SHAPE_POLYGON:
				if(newPolygon.nVerts == 9)
				{
					newPolygon.nVerts--;
					img->collision_shapes.push_back( PQB2ShapePtr(new PQB2Polygon(newPolygon)) );
					newPolygon.nVerts = 0;
					newShape = NULL;
					changed = true;
				}
				break;

			case SHAPE_BOX:
				newBox.halfWidth = fabs(newBox.halfWidth);
				newBox.halfHeight = fabs(newBox.halfHeight);

				if(newBox.halfWidth > 2.0f && newBox.halfHeight > 2.0f)
					img->collision_shapes.push_back( PQB2ShapePtr(new PQB2Box(newBox)) );

				newShape = NULL;
				changed = true;
				break;

			case SHAPE_EDGE:
				if((newEdge.p2 - newEdge.p1).Length() > 3.0f)
					img->collision_shapes.push_back( PQB2ShapePtr(new PQB2Edge(newEdge)) );

				newShape = NULL;
				changed = true;
				break;
			}
		}
		else if(moving == true)
		{
			moving = false;

			// if the move was too small, assume that is was a miss-click
			if(totalMove.Length() > 2.0f)
				changed = true;
			else
				img->collision_shapes[selIndex]->Move(-totalMove.x, -totalMove.y);
				
		}

		Redraw();
	}

	///////////////////////////////////////////////////

	virtual void OnMouseMove(int x, int y)
	{
		float sX = (float)x - halfWidth;
		float sY = (float)y - halfHeight;

		if(newShape != NULL)
		{
			switch(newShape->type)
			{
			case SHAPE_CIRCLE:
				dragTarget.set(sX, sY);
				dragTarget -= newCircle.center;
				newCircle.radius = dragTarget.Length();
				Redraw();
				break;

			case SHAPE_POLYGON:
				dragTarget.set(sX, sY);
				if(newPolygon.nVerts > 0)
				{
					newPolygon.vertices[newPolygon.nVerts - 1].set(sX, sY);
					Redraw();
				}
				break;

			case SHAPE_BOX:
				// top right corner of box is target
				dragTarget.set(sX, sY);
				dragTarget -= newBox.center;

				if(shiftDown == true) // rotation
				{
					float oldAngle = atan2(dragStart.y, dragStart.x);
					float newAngle = atan2(dragTarget.y, dragTarget.x);

					newBox.angle += newAngle - oldAngle;

					if(newBox.angle < 0.0f)
						newBox.angle += TWOPI;
				}
				else // sizing
				{
					Vec2<float> rcVec;
					
					rcVec.set(cosf(newBox.angle),
							  sinf(newBox.angle));

					newBox.halfWidth = dragTarget * rcVec;
					newBox.halfHeight = dragTarget * rcVec.Perp();
				}

				dragStart = dragTarget;

				Redraw();

				break;

			case SHAPE_EDGE:
				newEdge.p2.set(sX, sY);
				Redraw();
				break;
			}
		}
		else if(moving == true)
		{
			dragTarget.set(sX, sY);

			Vec2<float> mov = dragTarget - dragStart;
			totalMove += mov;

			img->collision_shapes[selIndex]->Move(mov.x, mov.y);

			dragStart = dragTarget;

			Redraw();
		}
	}

	///////////////////////////////////////////////////

	virtual void OnRMouseDown(int x, int y)
	{
		float sX = (float)x - halfWidth;
		float sY = (float)y - halfHeight;
		
		if(newShape != NULL) // if creating, cancel creation, or close polygon
		{
			switch(newShape->type)
			{
			case SHAPE_CIRCLE:
				newShape = NULL;
				break;

			case SHAPE_POLYGON:
				if(newPolygon.nVerts >= 4)
				{
					newPolygon.nVerts--;
					img->collision_shapes.push_back( PQB2ShapePtr(new PQB2Polygon(newPolygon)) );
					changed = true;
				}
				newPolygon.nVerts = 0;
				newShape = NULL;
				break;

			case SHAPE_BOX:
				newShape = NULL;
				break;

			case SHAPE_EDGE:
				newShape = NULL;
				break;
			}
		}
		else if(moving == true) // if moving, cancel move
		{
			img->collision_shapes[selIndex]->Move(-totalMove.x, -totalMove.y);
			moving = false;
		}
		else // try to open a shape for editing
		{
			bool foundShape = false;

			for(int i = 0; i < (int)img->collision_shapes.size(); i++)
			{
				switch(img->collision_shapes[i]->type)
				{
					case SHAPE_CIRCLE:
					{
						if(img->collision_shapes[i]->HitTest((int)sX, (int)sY) != -1)
						{
							PQB2CirclePtr circle = dynamic_pointer_cast<PQB2Circle>(img->collision_shapes[i]);
							
							newCircle = PQB2Circle(*circle);
							newShape = &newCircle;

							img->collision_shapes.erase(img->collision_shapes.begin() + i);

							foundShape = true;
						}
						break;
					}
					case SHAPE_POLYGON:
					{
						int idx = img->collision_shapes[i]->HitTest((int)sX, (int)sY);
						if(idx != -1)
						{
							PQB2PolygonPtr poly = dynamic_pointer_cast<PQB2Polygon>(img->collision_shapes[i]);
							
							// shuffle vertices so polygon opens at idx
							Vec2<float> vTmp[8];
							memcpy(vTmp, poly->vertices, 8 * sizeof(Vec2<float>));

							idx = (idx > (poly->nVerts - 1) ? 0 : idx + 1);

							int uStart = (poly->nVerts - idx);
							memcpy(poly->vertices, vTmp + idx, uStart * sizeof(Vec2<float>));
							memcpy(poly->vertices + uStart, vTmp, idx * sizeof(Vec2<float>));

							if(poly->nVerts < 8)
							{
								newPolygon = PQB2Polygon(*poly);
								newPolygon.vertices[newPolygon.nVerts++] = Vec2<float>(sX, sY);
								newShape = &newPolygon;

								img->collision_shapes.erase(img->collision_shapes.begin() + i);
								
								foundShape = true;
							}
						}
						break;
					}
					case SHAPE_BOX:
					{
						if(img->collision_shapes[i]->HitTest((int)sX, (int)sY) != -1)
						{
							PQB2BoxPtr box = dynamic_pointer_cast<PQB2Box>(img->collision_shapes[i]);
							newBox = PQB2Box(*box);
							newShape = &newBox;

							img->collision_shapes.erase(img->collision_shapes.begin() + i);

							foundShape = true;
						}

						break;
					}
					case SHAPE_EDGE:
					{
						int idx = img->collision_shapes[i]->HitTest((int)sX, (int)sY);
						if(idx != -1)
						{
							PQB2EdgePtr edge = dynamic_pointer_cast<PQB2Edge>(img->collision_shapes[i]);

							if(idx == 0)
							{
								Vec2<float> tmp = edge->p1;
								edge->p1 = edge->p2;
								edge->p2 = tmp;
							}

							newEdge = PQB2Edge(*edge);
							newShape = &newEdge;

							img->collision_shapes.erase(img->collision_shapes.begin() + i);
							
							foundShape = true;
						}
						break;
					}
				}

				if(foundShape == true)
					break;
			}
		}

		Redraw();
	}

	///////////////////////////////////////////////////

	virtual void OnRMouseUp(int x, int y)
	{
		// if creating or editing, finish the process
		if(newShape != NULL)
			OnLMouseUp(x, y);
	}

	///////////////////////////////////////////////////

	virtual void OnKeyDown(UINT nChar)
	{
		switch(nChar)
		{
		case VK_SHIFT:
			shiftDown =  true;
			break;
		case VK_DELETE:
			if(selIndex != -1)
			{
				img->collision_shapes.erase(img->collision_shapes.begin() + selIndex);
				selIndex = -1;
				changed = true;
				Redraw();
			}
			break;
		case VK_BACK:
			if(newShape != NULL)
			{
				if(newShape->type == SHAPE_POLYGON)
				{
					if(newPolygon.nVerts > 2)
					{
						newPolygon.nVerts--;
						newPolygon.vertices[newPolygon.nVerts - 1] = dragTarget;
					}
					else
					{
						newPolygon.nVerts = 0;
						newShape = NULL;
					}

					Redraw();
				}
			}
			break;
		}
	}

	virtual void OnKeyUp(UINT nChar)
	{
		switch(nChar)
		{
		case VK_SHIFT:
			shiftDown =  false;
			break;
		}
	}

	DECLARE_MESSAGE_MAP()
};

#define IDCTB_MOVE		0x7780
#define IDCTB_CIRCLE	0x7781
#define IDCTB_BOX		0x7782
#define IDCTB_POLYGON	0x7783
#define IDCTB_EDGE		0x7784

class CCollisionEditor : public CDialog
{
	DECLARE_DYNAMIC(CCollisionEditor)

public:

	PQResImagePtr img;

	cGL_Screen *collisionEditor;

	CToolBar toolbar;
	
	CCollisionEditor(CWnd* pParent = NULL);
	virtual ~CCollisionEditor();
	int CreateToolBar();
	int ShowDialog(PQResImagePtr &image);

	afx_msg void OnClose();
	afx_msg void OnBtnMove();
	afx_msg void OnBtnCircle();
	afx_msg void OnBtnBox();
	afx_msg void OnBtnPolygon();
	afx_msg void OnBtnEdge();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LONG OnInitDialog(UINT wParam, LONG lParam);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	
};