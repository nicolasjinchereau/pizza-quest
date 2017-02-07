/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once

#include "includes.h"
#include "Graphics.h"
#include "Object.h"
#include "Engine.h"

using namespace std;

class Camera : public Object
{
public:
	Camera();
	Camera(int height);
	~Camera();

	Rect viewRect; // the translated world space camera rectangle

	void SetSize(float height); // width determined automatically to maintain aspect ratio
	float GetSize() const;

	// move the camera so that the point (wx, wy) is inside the view rectangle
	void LookAt(float wx, float wy, bool immediate = false);

	// center the view rectangle around (wx, wy)
	void CenterAt(float wx, float wy, bool immediate = false);

	// reset the top left corner of the view rectangle to (0, 0)
	void ResetView();

	// convert a point on the screen to the world coordinate
	// viewed through this camera
	float ScreenToWorldX(float x);
	float ScreenToWorldY(float y);
	float NormalizedToWorldX(float x);
	float NormalizedToWorldY(float y);
	vec2f NormalizedToWorld(vec2f point);

	vec2f WorldToScreen(const vec2f &point) const;
	vec2f ScreenToWorld(const vec2f &point) const;

	virtual void PreDrawUpdate();
	
	const mat3f &matrix() const;

	static shared_ptr<Camera> activeCamera();
	static void activeCamera(const shared_ptr<Camera> &camera);

	float viewBorder() const;
	void viewBorder(float setViewBorder);
private:
	static weak_ptr<Camera> _activeCamera;

	void Init();
	
	float camSpeed; // make sure this is faster than max player/car speed
	float _viewBorder;

	vec2f position;
	vec2f target;
	vec2f velocity;

	vec2f delta;

	mat3f identity;
	mat3f mtxCamera;
	mat3f mtxProject;
	mat3f mtxViewProjection;

	float viewPanEdgeDistX;
	float viewPanEdgeDistY;
	
	float screenToWorldXFactor;
	float screenToWorldYFactor;

	friend class Sprite;
	friend class Engine;
	friend class Shader;
};
